/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "RfidCommunicationController.h"
#include "utils.h"
#include "RfidTypes.h"
#include "RfidCommunicationControllerState.h"
#include "SystemErds.h"
#include "Constants_Time.h"
#include "Constants_Binary.h"
#include "DataModelErdPointerAccess.h"

enum
{
   Signal_DemoModeEnabled = Hsm_UserSignalStart,
   Signal_ReadWriteResultSuccessful,
   Signal_ReadFailure,
   Signal_ReadWriteResultHardwareFailure,
   Signal_WriteFailure,
   Signal_AllFreshFoodDoorsAreClosed,
   Signal_AFreshFoodDoorsIsOpen,
   Signal_FreshFoodDoorsTimeoutTimerExpired,
   Signal_WriteTimeoutTimerExpired,
   Signal_ReadRequestTimerExpired
};

static const uint8_t BypassFilterIdentifier[FilterIdentifierSizeInBytes] = "BPXWFE";

#define HSM_STATE_EXPAND_AS_FUNCTION_DEFINITION(Name) \
   static bool Name(Hsm_t *hsm, HsmSignal_t signal, const void *data);

#define HSM_STATE_TABLE(ENTRY)              \
   ENTRY(State_Active)                      \
   ENTRY(State_AllFreshFoodDoorsJustClosed) \
   ENTRY(State_FreshFoodDoorOpen)           \
   ENTRY(State_DemoMode)                    \
   ENTRY(State_AllFreshFoodDoorsClosed)     \
   ENTRY(State_AllDoorsClosedRead)          \
   ENTRY(State_AllDoorsClosedWrite)

HSM_STATE_TABLE(HSM_STATE_EXPAND_AS_FUNCTION_DEFINITION)

static const HsmStateHierarchyDescriptor_t stateList[] = {
   { State_Active, HSM_NO_PARENT },
   { State_FreshFoodDoorOpen, State_Active },
   { State_DemoMode, HSM_NO_PARENT },
   { State_AllFreshFoodDoorsClosed, State_Active },
   { State_AllFreshFoodDoorsJustClosed, State_AllFreshFoodDoorsClosed },
   { State_AllDoorsClosedRead, State_AllFreshFoodDoorsClosed },
   { State_AllDoorsClosedWrite, State_AllFreshFoodDoorsClosed }
};

static const HsmConfiguration_t hsmConfiguration = {
   stateList,
   NUM_ELEMENTS(stateList)
};

static RfidCommunicationController_t *InstanceFromHsm(Hsm_t *hsm)
{
   return CONTAINER_OF(RfidCommunicationController_t, _private.hsm, hsm);
}

static void UpdateHsmStateTo(RfidCommunicationController_t *instance, RfidCommunicationControllerState_t state)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->rfidCommunicationControllerStateErd,
      &state);
}

static void SetFilterTypeTo(RfidCommunicationController_t *instance, WaterFilterType_t waterFilterType)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->waterFilterTypeErd,
      &waterFilterType);
}

static void RequestRfidFault(RfidCommunicationController_t *instance, Erd_t erd, bool status)
{
   RfidFaultRequest_t rfidFaultRequest;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->rfidFaultRequestErd,
      &rfidFaultRequest);

   rfidFaultRequest.faultErd = erd;
   rfidFaultRequest.requestStatus = status;
   rfidFaultRequest.signal++;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->rfidFaultRequestErd,
      &rfidFaultRequest);
}

static void RequestLeakDetectedFault(RfidCommunicationController_t *instance, bool request)
{
   RequestRfidFault(
      instance,
      instance->_private.config->rfidBoardLeakDetectedFaultErd,
      request);
}

static void RequestBlockedTagFault(RfidCommunicationController_t *instance, bool request)
{
   RequestRfidFault(
      instance,
      instance->_private.config->rfidBoardBlockedTagFaultErd,
      request);
}

static void RequestTagAuthenticationFailedFault(RfidCommunicationController_t *instance, bool request)
{
   RequestRfidFault(
      instance,
      instance->_private.config->rfidBoardTagAuthenticationFailedFault,
      request);
}

static void RequestHardwareFailureFault(RfidCommunicationController_t *instance, bool request)
{
   RequestRfidFault(
      instance,
      instance->_private.config->rfidBoardHardwareFailureFaultErd,
      request);
}

static void SendNewFilterInstalledSignal(RfidCommunicationController_t *instance)
{
   Signal_SendViaErd(DataModel_AsDataSource(instance->_private.dataModel), instance->_private.config->newFilterInstalledSignalErd);
}

static bool UidsMatch(RfidUid_t uidOne, RfidUid_t uidTwo)
{
   return memcmp(&uidOne, &uidTwo, sizeof(RfidUid_t)) == 0;
}

static bool NewFilterInstalled(RfidCommunicationController_t *instance)
{
   RfidUid_t rfidFilterUidRfidBoard;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->rfidFilterUidRfidBoardErd,
      &rfidFilterUidRfidBoard);

   RfidUid_t rfidFilterUid;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->rfidFilterUidErd,
      &rfidFilterUid);

   return !UidsMatch(rfidFilterUidRfidBoard, rfidFilterUid);
}

static bool AllFreshFoodDoorsAreClosed(RfidCommunicationController_t *instance)
{
   bool allFreshFoodDoorsAreClosed;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->allFreshFoodDoorsAreClosedErd,
      &allFreshFoodDoorsAreClosed);
   return allFreshFoodDoorsAreClosed;
}

static void SendRfidFilterDataRequest(RfidCommunicationController_t *instance, ReadWriteRequest_t request)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->rfidFilterDataRequestErd,
      &request);
}

static void ReadRequestTimerExpired(void *context)
{
   RfidCommunicationController_t *instance = context;
   Hsm_SendSignal(&instance->_private.hsm, Signal_ReadRequestTimerExpired, NULL);
}

static void StartReadRequestTimer(RfidCommunicationController_t *instance, TimerTicks_t timeInMsec)
{
   TimerModule_StartPeriodic(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         Erd_TimerModule),
      &instance->_private.readRequestTimer,
      timeInMsec,
      ReadRequestTimerExpired,
      instance);
}

static void StopReadRequestTimer(RfidCommunicationController_t *instance)
{
   TimerModule_Stop(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         Erd_TimerModule),
      &instance->_private.readRequestTimer);
}

static void StartReadWriteRequestTimer(RfidCommunicationController_t *instance, TimerTicks_t timeInMsec)
{
   TimerModule_StartOneShot(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         Erd_TimerModule),
      &instance->_private.readWriteRequestTimer,
      timeInMsec,
      ReadRequestTimerExpired,
      instance);
}

static bool ReadWriteTimerIsRunning(RfidCommunicationController_t *instance)
{
   return TimerModule_IsRunning(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         Erd_TimerModule),
      &instance->_private.readWriteRequestTimer);
}

static void StopReadWriteRequestTimer(RfidCommunicationController_t *instance)
{
   TimerModule_Stop(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         Erd_TimerModule),
      &instance->_private.readWriteRequestTimer);
}

static void SendReadRequest(RfidCommunicationController_t *instance)
{
   SendRfidFilterDataRequest(instance, ReadWriteRequest_Read);
}

static void StartFastReadRequestTimer(RfidCommunicationController_t *instance, TimerTicks_t timeInMsec)
{
   StartReadRequestTimer(instance, timeInMsec);
}

static void StopFastReadRequestTimer(RfidCommunicationController_t *instance)
{
   StopReadRequestTimer(instance);
}

static void WriteTimeoutTimerExpired(void *context)
{
   RfidCommunicationController_t *instance = context;
   Hsm_SendSignal(&instance->_private.hsm, Signal_WriteTimeoutTimerExpired, NULL);
}

static void FreshFoodDoorsTimeoutTimerExpired(void *context)
{
   RfidCommunicationController_t *instance = context;
   Hsm_SendSignal(&instance->_private.hsm, Signal_FreshFoodDoorsTimeoutTimerExpired, NULL);
}

static void StartTimeoutTimer(RfidCommunicationController_t *instance, TimerTicks_t timeInMsec, TimerCallback_t callback)
{
   TimerModule_StartOneShot(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         Erd_TimerModule),
      &instance->_private.timeoutTimer,
      timeInMsec,
      callback,
      instance);
}

static void StopTimeoutTimer(RfidCommunicationController_t *instance)
{
   TimerModule_Stop(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         Erd_TimerModule),
      &instance->_private.timeoutTimer);
}

static bool FilterIsBypassFilter(RfidCommunicationController_t *instance)
{
   RfidFilterIdentifier_t filterId;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->rfidFilterIdentifierErd,
      &filterId);
   return memcmp(&filterId, &BypassFilterIdentifier, sizeof(RfidFilterIdentifier_t)) == 0;
}

static bool DemoModeIsEnabled(RfidCommunicationController_t *instance)
{
   bool demoModeEnabled;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->demoModeEnableErd,
      &demoModeEnabled);
   return demoModeEnabled;
}

static bool RfidFilterStatusLeakDetected(RfidCommunicationController_t *instance)
{
   FilterStatusRfidBoard_t rfidFilterStatus;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->rfidFilterStatusRfidBoardErd,
      &rfidFilterStatus);
   return BIT_STATE(rfidFilterStatus, RfidFilterStatusBits_LeakDetected);
}

static bool RfidFilterStatusRfidBlocked(RfidCommunicationController_t *instance)
{
   FilterStatusRfidBoard_t rfidFilterStatus;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->rfidFilterStatusRfidBoardErd,
      &rfidFilterStatus);
   return BIT_STATE(rfidFilterStatus, RfidFilterStatusBits_Blocked_RfidBoard);
}

static bool ReadWriteResultIsAReadFailure(const RfidFilterReadWriteResult_t *readWriteResult)
{
   return (readWriteResult->result == ReadWriteResult_ReadFailure ||
      readWriteResult->result == ReadWriteResult_ReadBeforeWriteFailure ||
      readWriteResult->result == ReadWriteResult_ReadAfterWriteFailure);
}

static bool ReadWriteResultIsAWriteFailure(const RfidFilterReadWriteResult_t *readWriteResult)
{
   return (readWriteResult->result == ReadWriteResult_EepromWriteFailure ||
      readWriteResult->result == ReadWriteResult_UidMismatch ||
      readWriteResult->result == ReadWriteResult_TagUidIsInvalid);
}

static bool ReadWriteResultIsAFailure(const RfidFilterReadWriteResult_t *readWriteResult)
{
   return (ReadWriteResultIsAReadFailure(readWriteResult) ||
      ReadWriteResultIsAWriteFailure(readWriteResult));
}

static bool LastReadWriteResultWasAFailure(RfidCommunicationController_t *instance)
{
   RfidFilterReadWriteResult_t readWriteResult;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->rfidFilterReadWriteResultErd,
      &readWriteResult);

   return (ReadWriteResultIsAReadFailure(&readWriteResult)) || (readWriteResult.result == ReadWriteResult_HardwareFailure);
}

static void ClearRfidFilterTagAuthenticationFailedAndHardwareFailureFaults(RfidCommunicationController_t *instance)
{
   RequestTagAuthenticationFailedFault(instance, CLEAR);
   RequestHardwareFailureFault(instance, CLEAR);
}

static bool State_Active(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   IGNORE(data);
   RfidCommunicationController_t *instance = InstanceFromHsm(hsm);

   switch(signal)
   {
      case Hsm_Entry:
         UpdateHsmStateTo(instance, RfidCommunicationControllerHsmState_Active);
         SetFilterTypeTo(instance, WaterFilterType_XWFE);
         break;

      case Signal_DemoModeEnabled:
         Hsm_Transition(&instance->_private.hsm, State_DemoMode);
         break;

      case Signal_ReadRequestTimerExpired:
         SendReadRequest(instance);
         break;

      default:
         return HsmSignalDeferred;
   }
   return HsmSignalConsumed;
}

static bool State_AllFreshFoodDoorsJustClosed(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   IGNORE(data);
   RfidCommunicationController_t *instance = InstanceFromHsm(hsm);

   switch(signal)
   {
      case Hsm_Entry:
         UpdateHsmStateTo(instance, RfidCommunicationControllerHsmState_AllFreshFoodDoorsJustClosed);
         SendReadRequest(instance);
         StartReadRequestTimer(
            instance,
            instance->_private.rfidFilterUpdateRateData->doorJustClosedFilterReadFrequencyInSeconds * MSEC_PER_SEC);
         StartTimeoutTimer(
            instance,
            instance->_private.rfidFilterUpdateRateData->doorJustClosedFilterReadTimeoutInSeconds * MSEC_PER_SEC,
            FreshFoodDoorsTimeoutTimerExpired);
         break;

      case Signal_FreshFoodDoorsTimeoutTimerExpired:
         Hsm_Transition(&instance->_private.hsm, State_AllDoorsClosedRead);
         break;

      case Signal_ReadWriteResultSuccessful:
         ClearRfidFilterTagAuthenticationFailedAndHardwareFailureFaults(instance);
         if(NewFilterInstalled(instance))
         {
            SendNewFilterInstalledSignal(instance);
            Hsm_Transition(&instance->_private.hsm, State_AllDoorsClosedRead);
         }
         break;

      case Hsm_Exit:
         StopReadRequestTimer(instance);
         StopTimeoutTimer(instance);
         break;

      default:
         return HsmSignalDeferred;
   }
   return HsmSignalConsumed;
}

static bool State_FreshFoodDoorOpen(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   IGNORE(data);
   RfidCommunicationController_t *instance = InstanceFromHsm(hsm);

   switch(signal)
   {
      case Hsm_Entry:
         UpdateHsmStateTo(instance, RfidCommunicationControllerHsmState_FreshFoodDoorOpen);
         SendReadRequest(instance);
         StartReadRequestTimer(
            instance,
            instance->_private.rfidFilterUpdateRateData->doorOpenFilterReadFrequencyInSeconds * MSEC_PER_SEC);
         break;

      case Signal_ReadWriteResultSuccessful:
         ClearRfidFilterTagAuthenticationFailedAndHardwareFailureFaults(instance);
         if(NewFilterInstalled(instance))
         {
            SendNewFilterInstalledSignal(instance);
         }
         break;

      case Signal_AllFreshFoodDoorsAreClosed:
         Hsm_Transition(&instance->_private.hsm, State_AllFreshFoodDoorsJustClosed);
         break;

      case Hsm_Exit:
         StopReadRequestTimer(instance);
         break;

      default:
         return HsmSignalDeferred;
   }
   return HsmSignalConsumed;
}

static bool State_AllFreshFoodDoorsClosed(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   IGNORE(data);
   RfidCommunicationController_t *instance = InstanceFromHsm(hsm);

   switch(signal)
   {
      case Hsm_Entry:
         UpdateHsmStateTo(instance, RfidCommunicationControllerHsmState_AllFreshFoodDoorsClosed);
         break;

      case Signal_AFreshFoodDoorsIsOpen:
         Hsm_Transition(&instance->_private.hsm, State_FreshFoodDoorOpen);
         break;

      default:
         return HsmSignalDeferred;
   }
   return HsmSignalConsumed;
}

static bool State_AllDoorsClosedRead(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   IGNORE(data);
   RfidCommunicationController_t *instance = InstanceFromHsm(hsm);

   switch(signal)
   {
      case Hsm_Entry:
         UpdateHsmStateTo(instance, RfidCommunicationControllerHsmState_AllDoorsClosedRead);
         StartReadWriteRequestTimer(
            instance,
            instance->_private.rfidFilterUpdateRateData->doorClosedFilterUpdateTimeInMinutes * MSEC_PER_MIN);

         if(RfidFilterStatusLeakDetected(instance) || LastReadWriteResultWasAFailure(instance))
         {
            StartFastReadRequestTimer(
               instance,
               instance->_private.rfidFilterUpdateRateData->doorClosedRfidErrorDetectedReadFrequencySeconds * MSEC_PER_SEC);
         }
         break;

      case Signal_ReadWriteResultSuccessful:
         ClearRfidFilterTagAuthenticationFailedAndHardwareFailureFaults(instance);
         if(NewFilterInstalled(instance))
         {
            SendNewFilterInstalledSignal(instance);
         }

         if(!ReadWriteTimerIsRunning(instance))
         {
            if(!FilterIsBypassFilter(instance))
            {
               Hsm_Transition(&instance->_private.hsm, State_AllDoorsClosedWrite);
            }
            else
            {
               StartReadWriteRequestTimer(
                  instance,
                  instance->_private.rfidFilterUpdateRateData->doorClosedFilterUpdateTimeInMinutes * MSEC_PER_MIN);
            }
         }
         break;

      case Signal_ReadFailure:
      case Signal_ReadWriteResultHardwareFailure:
         StartFastReadRequestTimer(
            instance,
            instance->_private.rfidFilterUpdateRateData->doorClosedRfidErrorDetectedReadFrequencySeconds * MSEC_PER_SEC);
         break;

      case Hsm_Exit:
         StopFastReadRequestTimer(instance);
         StopReadWriteRequestTimer(instance);
         break;

      default:
         return HsmSignalDeferred;
   }
   return HsmSignalConsumed;
}

static bool State_AllDoorsClosedWrite(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   IGNORE(data);
   RfidCommunicationController_t *instance = InstanceFromHsm(hsm);

   switch(signal)
   {
      case Hsm_Entry:
         UpdateHsmStateTo(instance, RfidCommunicationControllerHsmState_AllDoorsClosedWrite);
         SendRfidFilterDataRequest(instance, ReadWriteRequest_Write);
         StartTimeoutTimer(
            instance,
            instance->_private.rfidFilterUpdateRateData->rfidFilterWriteTimeoutInMinutes * MSEC_PER_MIN,
            WriteTimeoutTimerExpired);
         break;

      case Signal_WriteFailure:
      case Signal_WriteTimeoutTimerExpired:
      case Signal_ReadWriteResultSuccessful:
         Hsm_Transition(&instance->_private.hsm, State_AllDoorsClosedRead);
         break;

      case Hsm_Exit:
         StopTimeoutTimer(instance);
         break;

      default:
         return HsmSignalDeferred;
   }
   return HsmSignalConsumed;
}

static bool State_DemoMode(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   IGNORE(data);
   RfidCommunicationController_t *instance = InstanceFromHsm(hsm);

   switch(signal)
   {
      case Hsm_Entry:
         UpdateHsmStateTo(instance, RfidCommunicationControllerHsmState_DemoMode);
         break;

      default:
         return HsmSignalDeferred;
   }
   return HsmSignalConsumed;
}

static void OnDataModelChange(void *context, const void *_args)
{
   RfidCommunicationController_t *instance = context;
   const DataModelOnDataChangeArgs_t *args = _args;

   Erd_t erd = args->erd;
   if(erd == instance->_private.config->rfidFilterReadWriteResultErd)
   {
      const RfidFilterReadWriteResult_t *readWriteResult = args->data;

      if(readWriteResult->result == ReadWriteResult_Success)
      {
         if(!FilterIsBypassFilter(instance))
         {
            RfidFilterStatusLeakDetected(instance) ? RequestLeakDetectedFault(instance, SET) : RequestLeakDetectedFault(instance, CLEAR);
            RfidFilterStatusRfidBlocked(instance) ? RequestBlockedTagFault(instance, SET) : RequestBlockedTagFault(instance, CLEAR);
         }

         Hsm_SendSignal(&instance->_private.hsm, Signal_ReadWriteResultSuccessful, NULL);
      }
      else if(ReadWriteResultIsAFailure(readWriteResult))
      {
         RequestTagAuthenticationFailedFault(instance, SET);

         ReadWriteResultIsAReadFailure(readWriteResult) ? 
            Hsm_SendSignal(&instance->_private.hsm, Signal_ReadFailure, NULL) : 
            Hsm_SendSignal(&instance->_private.hsm, Signal_WriteFailure, NULL);
      }
      else if(readWriteResult->result == ReadWriteResult_HardwareFailure)
      {
         RequestHardwareFailureFault(instance, SET);
         Hsm_SendSignal(&instance->_private.hsm, Signal_ReadWriteResultHardwareFailure, NULL);
      }
   }
   else if(erd == instance->_private.config->allFreshFoodDoorsAreClosedErd)
   {
      const bool *allFreshFoodDoorsAreClosed = args->data;
      if(*allFreshFoodDoorsAreClosed)
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_AllFreshFoodDoorsAreClosed, NULL);
      }
      else
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_AFreshFoodDoorsIsOpen, NULL);
      }
   }
   else if(erd == instance->_private.config->demoModeEnableErd)
   {
      const bool *demoModeEnabled = args->data;
      if(*demoModeEnabled)
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_DemoModeEnabled, NULL);
      }
   }
}

void RfidCommunicationController_Init(
   RfidCommunicationController_t *instance,
   I_DataModel_t *dataModel,
   const RfidCommunicationControllerConfig_t *config,
   const RfidFilterUpdateRateData_t *rfidFilterUpdateRateData)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;
   instance->_private.rfidFilterUpdateRateData = rfidFilterUpdateRateData;

   if(DemoModeIsEnabled(instance))
   {
      Hsm_Init(&instance->_private.hsm, &hsmConfiguration, State_DemoMode);
   }
   else if(AllFreshFoodDoorsAreClosed(instance))
   {
      Hsm_Init(&instance->_private.hsm, &hsmConfiguration, State_AllFreshFoodDoorsJustClosed);
   }
   else
   {
      Hsm_Init(&instance->_private.hsm, &hsmConfiguration, State_FreshFoodDoorOpen);
   }

   EventSubscription_Init(
      &instance->_private.onDataModelChangeSubscription,
      instance,
      OnDataModelChange);
   DataModel_SubscribeAll(
      dataModel,
      &instance->_private.onDataModelChangeSubscription);
}
