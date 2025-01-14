/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "DispenseController.h"
#include "DispensingRequest.h"
#include "DispenseStatus.h"
#include "DispenseSelectionRequestHandler.h"
#include "DataModelErdPointerAccess.h"
#include "WaterValveVotedState.h"
#include "AugerMotorVotedIceType.h"
#include "Constants_Time.h"
#include "utils.h"
#include "DispensingInhibitedReasonBitmap.h"

#define InstanceFromHsm(hsm) CONTAINER_OF(DispenseController_t, _private.hsm, hsm)

enum
{
   Signal_StopDispensingRequested = Hsm_UserSignalStart,
   Signal_StartDispensingRequested,
   Signal_MaxDispenseTimerExpired,
   Signal_DispensingDisabled,
   Signal_DispensingInhibitedByRfid,
   Signal_DispensingInhibitedByDoor,
   Signal_AutoFillSensorError,
};

static bool State_Global(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_Dispensing(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_NotDispensing(Hsm_t *hsm, HsmSignal_t signal, const void *data);

static const HsmStateHierarchyDescriptor_t stateList[] = {
   { State_Global, HSM_NO_PARENT },
   { State_Dispensing, State_Global },
   { State_NotDispensing, State_Global },
};

static const HsmConfiguration_t hsmConfiguration = {
   stateList,
   NUM_ELEMENTS(stateList)
};

static void SetIsolationValveVoteTo(DispenseController_t *instance, WaterValveState_t state, Vote_t care)
{
   WaterValveVotedState_t votedState = {
      .state = state,
      .care = care
   };

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->isolationValveDispensingVoteErd,
      &votedState);
}

static void SetDispensingValveVoteTo(DispenseController_t *instance, WaterValveState_t state, Vote_t care)
{
   WaterValveVotedState_t votedState = {
      .state = state,
      .care = care
   };

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->dispensingValveDispensingVoteErd,
      &votedState);
}

static void SetAugerMotorVoteTo(DispenseController_t *instance, AugerMotorIceType_t iceType, Vote_t care)
{
   AugerMotorVotedIceType_t votedState = {
      .iceType = iceType,
      .care = care
   };

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->augerMotorDispensingVoteErd,
      &votedState);
}

static void SetDispensingResultStatusTo(DispenseController_t *instance, DispenseStatus_t selection)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->privateDispensingResultStatusErd,
      &selection);
}

static void MaxDispenseTimerExpired(void *context)
{
   DispenseController_t *instance = context;
   Hsm_SendSignal(&instance->_private.hsm, Signal_MaxDispenseTimerExpired, NULL);
}

static void StartMaxDispenseTimer(DispenseController_t *instance)
{
   TimerModule_StartOneShot(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         Erd_TimerModule),
      &instance->_private.maxDispenseTimer,
      instance->_private.dispenserData->maximumDispenseTimeInSeconds * MSEC_PER_SEC,
      MaxDispenseTimerExpired,
      instance);
}

static void StopMaxDispenseTimer(DispenseController_t *instance)
{
   TimerModule_Stop(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         Erd_TimerModule),
      &instance->_private.maxDispenseTimer);
}

static bool DispenseEnabled(DispenseController_t *instance)
{
   bool disabled;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->dispensingDisabledErd,
      &disabled);

   return !disabled;
}

static bool DispensingInhibitedBy(DispenseController_t *instance, DispensingInhibitedReason_t reason)
{
   DispensingInhibitedReasonBitmap_t dispensingInhibitedBitmap;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->dispensingInhibitedErd,
      &dispensingInhibitedBitmap);

   return BITMAP_STATE(dispensingInhibitedBitmap.bitmap, reason);
}

static bool WaterDispenseRequestInhibitedByDoorOpen(DispenseController_t *instance, DispensingRequestSelection_t requestSelection)
{
   return (requestSelection == DispensingRequestSelection_Autofill || requestSelection == DispensingRequestSelection_Water) && DispensingInhibitedBy(instance, DispensingInhibitedReason_WaterDueToDoorOpen);
}

static bool IceDispenseRequestInhibitedByDoorOpen(DispenseController_t *instance, DispensingRequestSelection_t requestSelection)
{
   return (requestSelection == DispensingRequestSelection_CubedIce || requestSelection == DispensingRequestSelection_CrushedIce) && DispensingInhibitedBy(instance, DispensingInhibitedReason_IceDueToDoorOpen);
}

static bool WaterDispenseInhibitedByRfid(DispenseController_t *instance, DispensingRequestSelection_t requestSelection)
{
   return (requestSelection == DispensingRequestSelection_Autofill || requestSelection == DispensingRequestSelection_Water) && DispensingInhibitedBy(instance, DispensingInhibitedReason_WaterDueToRfidFilter);
}

static bool AutoFillSensorError(DispenseController_t *instance)
{
   if(instance->_private.config->autofillSensorErrorErd != Erd_Invalid)
   {
      bool state;
      DataModel_Read(
         instance->_private.dataModel,
         instance->_private.config->autofillSensorErrorErd,
         &state);

      return state;
   }

   return false;
}

static bool AutofillDispenseInhibitedByAutofillSensorError(DispenseController_t *instance, DispensingRequestSelection_t requestSelection)
{
   return (requestSelection == DispensingRequestSelection_Autofill) && AutoFillSensorError(instance);
}

static DispensingRequestSelection_t RequestSelection(DispenseController_t *instance)
{
   DispensingRequest_t request;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->privateDispensingRequestErd,
      &request);

   return request.selection;
}

static void UpdateDispensingRequestStatus(DispenseController_t *instance, DispenseStatus_t status)
{
   DispensingRequest_t privateDispensingRequest;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->privateDispensingRequestErd,
      &privateDispensingRequest);

   DispensingRequestStatus_t dispensingRequestStatus = {
      .action = privateDispensingRequest.action,
      .selection = privateDispensingRequest.selection,
      .specialOptions = privateDispensingRequest.specialOptions,
      .status = status,
      .preciseFillOuncesx100 = privateDispensingRequest.preciseFillOuncesx100
   };
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->dispensingRequestStatusErd,
      &dispensingRequestStatus);
}

static void SetIceWaterStopsDispensingBasedOnTimeFault(DispenseController_t *instance, bool state)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->iceWaterStopsDispensingBasedOnTimeFaultErd,
      &state);
}

static bool State_Global(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   DispenseController_t *instance = InstanceFromHsm(hsm);
   IGNORE(data);
   DispensingRequestSelection_t requestSelection;

   switch(signal)
   {
      case Hsm_Entry:
         break;

      case Signal_DispensingDisabled:
         SetDispensingResultStatusTo(instance, DispenseStatus_DisabledOrBlocked);
         break;

      case Signal_DispensingInhibitedByRfid:
         SetDispensingResultStatusTo(instance, DispenseStatus_DispenseInhibitedDueToRfidErrorOrLeak);
         break;

      case Signal_DispensingInhibitedByDoor:
         requestSelection = RequestSelection(instance);
         if(WaterDispenseRequestInhibitedByDoorOpen(instance, requestSelection) || IceDispenseRequestInhibitedByDoorOpen(instance, requestSelection))
         {
            SetDispensingResultStatusTo(instance, DispenseStatus_DispenseInhibitedDueToDoorOpen);
         }
         break;

      case Signal_AutoFillSensorError:
         SetDispensingResultStatusTo(instance, DispenseStatus_SensorError);
         break;

      case Hsm_Exit:
         break;

      default:
         return HsmSignalDeferred;
   }
   return HsmSignalConsumed;
}

static bool State_Dispensing(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   DispenseController_t *instance = InstanceFromHsm(hsm);
   IGNORE(data);

   switch(signal)
   {
      case Hsm_Entry:
         SetIceWaterStopsDispensingBasedOnTimeFault(instance, false);
         UpdateDispensingRequestStatus(instance, DispenseStatus_Dispensing);
         StartMaxDispenseTimer(instance);
         DispensingRequestSelection_t requestSelection = RequestSelection(instance);

         if(requestSelection == DispensingRequestSelection_Water || requestSelection == DispensingRequestSelection_Autofill)
         {
            SetDispensingValveVoteTo(instance, WaterValveState_On, Vote_Care);
            SetIsolationValveVoteTo(instance, WaterValveState_On, Vote_Care);
         }
         else if(requestSelection == DispensingRequestSelection_CubedIce)
         {
            SetAugerMotorVoteTo(instance, AugerMotorIceType_Cubed, Vote_Care);
         }
         else if(requestSelection == DispensingRequestSelection_CrushedIce)
         {
            SetAugerMotorVoteTo(instance, AugerMotorIceType_Crushed, Vote_Care);
         }
         break;

      case Signal_StopDispensingRequested:
         Hsm_Transition(hsm, State_NotDispensing);
         break;

      case Signal_MaxDispenseTimerExpired:
         SetDispensingResultStatusTo(instance, DispenseStatus_HitMaxTime);
         SetIceWaterStopsDispensingBasedOnTimeFault(instance, true);
         break;

      case Hsm_Exit:
         StopMaxDispenseTimer(instance);
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

static bool State_NotDispensing(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   DispenseController_t *instance = InstanceFromHsm(hsm);
   IGNORE(data);
   DispensingRequestSelection_t requestSelection;

   switch(signal)
   {
      case Hsm_Entry:
         SetDispensingValveVoteTo(instance, WaterValveState_Off, Vote_DontCare);
         SetIsolationValveVoteTo(instance, WaterValveState_Off, Vote_DontCare);
         SetAugerMotorVoteTo(instance, AugerMotorIceType_Off, Vote_DontCare);
         break;

      case Signal_StartDispensingRequested: {
         requestSelection = RequestSelection(instance);

         if(WaterDispenseRequestInhibitedByDoorOpen(instance, requestSelection) || IceDispenseRequestInhibitedByDoorOpen(instance, requestSelection))
         {
            SetDispensingResultStatusTo(instance, DispenseStatus_DispenseInhibitedDueToDoorOpen);
         }
         else if(WaterDispenseInhibitedByRfid(instance, requestSelection))
         {
            SetDispensingResultStatusTo(instance, DispenseStatus_DispenseInhibitedDueToRfidErrorOrLeak);
         }
         else if(AutofillDispenseInhibitedByAutofillSensorError(instance, requestSelection))
         {
            SetDispensingResultStatusTo(instance, DispenseStatus_SensorError);
         }
         else if(!DispenseEnabled(instance))
         {
            SetDispensingResultStatusTo(instance, DispenseStatus_DisabledOrBlocked);
         }
         else
         {
            Hsm_Transition(hsm, State_Dispensing);
         }
      }
      case Hsm_Exit:
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

static void OnDataModelChange(void *context, const void *args)
{
   DispenseController_t *instance = context;
   const DataModelOnDataChangeArgs_t *onChangeData = args;
   Erd_t erd = onChangeData->erd;

   if(erd == instance->_private.config->privateDispensingRequestErd)
   {
      const DispensingRequest_t *request = onChangeData->data;
      if(request->action == DispensingAction_Start)
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_StartDispensingRequested, NULL);
      }
      else if(request->action == DispensingAction_Stop)
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_StopDispensingRequested, NULL);
      }
   }
   else if(erd == instance->_private.config->dispensingDisabledErd)
   {
      const bool *state = onChangeData->data;
      if(*state)
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_DispensingDisabled, NULL);
      }
   }
   else if(erd == instance->_private.config->dispensingInhibitedErd)
   {
      const DispensingInhibitedReasonBitmap_t *dispensingInhibitedBitmap = onChangeData->data;
      if(BITMAP_STATE(dispensingInhibitedBitmap->bitmap, DispensingInhibitedReason_WaterDueToRfidFilter))
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_DispensingInhibitedByRfid, NULL);
      }
      else if(BITMAP_STATE(dispensingInhibitedBitmap->bitmap, DispensingInhibitedReason_WaterDueToDoorOpen) ||
         BITMAP_STATE(dispensingInhibitedBitmap->bitmap, DispensingInhibitedReason_IceDueToDoorOpen))
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_DispensingInhibitedByDoor, NULL);
      }
   }
   else if(erd == instance->_private.config->autofillSensorErrorErd)
   {
      if(erd != Erd_Invalid)
      {
         const bool *state = onChangeData->data;
         if(*state)
         {
            Hsm_SendSignal(&instance->_private.hsm, Signal_AutoFillSensorError, NULL);
         }
      }
   }
}

void DispenseController_Init(
   DispenseController_t *instance,
   I_DataModel_t *dataModel,
   const DispenserData_t *data,
   const DispenseControllerConfig_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;
   instance->_private.dispenserData = data;

   Hsm_Init(&instance->_private.hsm, &hsmConfiguration, State_NotDispensing);

   EventSubscription_Init(
      &instance->_private.onDataModelChangeSubscription,
      instance,
      OnDataModelChange);

   DataModel_SubscribeAll(
      instance->_private.dataModel,
      &instance->_private.onDataModelChangeSubscription);
}
