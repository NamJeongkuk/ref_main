/*!
 * @file
 * @brief Implementation of Dispensing Request Handler
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "DispensingRequestHandler.h"
#include "DataModelErdPointerAccess.h"
#include "SystemErds.h"
#include "utils.h"
#include "Constants_Binary.h"

enum
{
   Signal_StartRequest = Fsm_UserSignalStart,
   Signal_StopRequest,
   Signal_InvalidRequest,
   Signal_ContinueRequest,
   Signal_DispensingContinueRequestWaitTimeExpired,
   Signal_PrivateDispensingResultStatusChanged
};

static void State_Idle(Fsm_t *fsm, const FsmSignal_t signal, const void *data);
static void State_RequestingDispense(Fsm_t *fsm, const FsmSignal_t signal, const void *data);

static DispensingRequestHandler_t *InstanceFromFsm(Fsm_t *fsm)
{
   return CONTAINER_OF(DispensingRequestHandler_t, _private.fsm, fsm);
}

static void ClearDispensingRequest(DispensingRequestHandler_t *instance)
{
   DispensingRequest_t dispensingRequest = {
      .action = DispensingAction_None,
      .selection = DispensingRequestSelection_None,
      .specialOptions = DispensingSpecialOptions_EmptyRequest,
      .padding = UINT8_MAX,
      .preciseFillOuncesx100 = UINT16_MAX
   };
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->dispensingRequestErd,
      &dispensingRequest);
}

static void ClearPrivateDispenseStatus(DispensingRequestHandler_t *instance)
{
   DispenseStatus_t resetValue = DispenseStatus_PrivateDispenseStatusResetValue;
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->privateDispensingResultStatusErd,
      &resetValue);
}

static void OnDataModelChange(void *context, const void *args)
{
   DispensingRequestHandler_t *instance = context;
   const DataModelOnDataChangeArgs_t *onChangeData = args;
   Erd_t erd = onChangeData->erd;

   if(erd == instance->_private.config->dispensingRequestErd)
   {
      const DispensingRequest_t *dispensingRequest = onChangeData->data;

      if(dispensingRequest->action != DispensingAction_None)
      {
         instance->_private.currentPrivateDispensingRequest = *dispensingRequest;

         switch(dispensingRequest->action)
         {
            case DispensingAction_Continue:
               Fsm_SendSignal(&instance->_private.fsm, Signal_ContinueRequest, NULL);
               break;

            case DispensingAction_Stop:
               Fsm_SendSignal(&instance->_private.fsm, Signal_StopRequest, NULL);
               break;

            case DispensingAction_Start:
               Fsm_SendSignal(&instance->_private.fsm, Signal_StartRequest, NULL);
               break;

            default:
               Fsm_SendSignal(&instance->_private.fsm, Signal_InvalidRequest, NULL);
               break;
         }
      }

      if((dispensingRequest->selection > DispensingRequestSelection_Autofill) && (dispensingRequest->selection != DispensingRequestSelection_None))
      {
         Fsm_SendSignal(&instance->_private.fsm, Signal_InvalidRequest, NULL);
      }

      ClearDispensingRequest(instance);
   }
   else if(erd == instance->_private.config->privateDispensingResultStatusErd)
   {
      const DispenseStatus_t *status = onChangeData->data;
      if(*status != DispenseStatus_PrivateDispenseStatusResetValue)
      {
         Fsm_SendSignal(&instance->_private.fsm, Signal_PrivateDispensingResultStatusChanged, NULL);
         ClearPrivateDispenseStatus(instance);
      }
   }
}

static void UpdateDispensingRequestStatus(DispensingRequestHandler_t *instance, uint8_t status)
{
   DispensingRequestStatus_t dispensingRequestStatus = {
      .action = instance->_private.currentPrivateDispensingRequest.action,
      .selection = instance->_private.currentPrivateDispensingRequest.selection,
      .specialOptions = instance->_private.currentPrivateDispensingRequest.specialOptions,
      .status = status,
      .preciseFillOuncesx100 = instance->_private.currentPrivateDispensingRequest.preciseFillOuncesx100
   };

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->dispensingRequestStatusErd,
      &dispensingRequestStatus);
}

static void UpdatePrivateDispensingRequestAction(DispensingRequestHandler_t *instance, uint8_t action)
{
   instance->_private.currentPrivateDispensingRequest.action = action;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->privateDispensingRequestErd,
      &instance->_private.currentPrivateDispensingRequest);
}

static void CopyPrivateDispensingRequestStatusToThePublicStatus(DispensingRequestHandler_t *instance)
{
   DispenseStatus_t dispensingResultStatus;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->privateDispensingResultStatusErd,
      &dispensingResultStatus);

   DispensingRequest_t dispensingRequest;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->privateDispensingRequestErd,
      &dispensingRequest);

   DispensingRequestStatus_t dispensingRequestStatus = {
      .action = dispensingRequest.action,
      .selection = dispensingRequest.selection,
      .specialOptions = dispensingRequest.specialOptions,
      .status = dispensingResultStatus,
      .preciseFillOuncesx100 = dispensingRequest.preciseFillOuncesx100
   };

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->dispensingRequestStatusErd,
      &dispensingRequestStatus);
}

static void OnDispensingContinueRequestWaitTimeExpired(void *context)
{
   DispensingRequestHandler_t *instance = context;
   Fsm_SendSignal(&instance->_private.fsm, Signal_DispensingContinueRequestWaitTimeExpired, NULL);
}

static void StartDispensingContinueRequestWaitTimer(DispensingRequestHandler_t *instance)
{
   TimerModule_StartOneShot(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         Erd_TimerModule),
      &instance->_private.dispensingRequestContinueWaitTimer,
      DispensingRequestContinueWaitMaxTimeInMsec,
      OnDispensingContinueRequestWaitTimeExpired,
      instance);
}

static void StopDispensingContinueRequestWaitTimer(DispensingRequestHandler_t *instance)
{
   TimerModule_Stop(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         Erd_TimerModule),
      &instance->_private.dispensingRequestContinueWaitTimer);
}

static void State_RequestingDispense(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   DispensingRequestHandler_t *instance = InstanceFromFsm(fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         StartDispensingContinueRequestWaitTimer(instance);
         UpdatePrivateDispensingRequestAction(instance, DispensingAction_Start);
         break;

      case Signal_ContinueRequest:
         StartDispensingContinueRequestWaitTimer(instance);
         break;

      case Signal_StopRequest:
         UpdateDispensingRequestStatus(instance, DispenseStatus_CompletedSuccessfully);
         Fsm_Transition(fsm, State_Idle);
         break;

      case Signal_DispensingContinueRequestWaitTimeExpired:
         UpdateDispensingRequestStatus(instance, DispenseStatus_DidNotReceiveDispenseRequestAfterHeartbeat);
         Fsm_Transition(fsm, State_Idle);
         break;

      case Signal_PrivateDispensingResultStatusChanged:
         CopyPrivateDispensingRequestStatusToThePublicStatus(instance);
         Fsm_Transition(fsm, State_Idle);
         break;

      case Signal_InvalidRequest:
         UpdateDispensingRequestStatus(instance, DispenseStatus_BadCommand);
         break;

      case Fsm_Exit:
         StopDispensingContinueRequestWaitTimer(instance);
         UpdatePrivateDispensingRequestAction(instance, DispensingAction_Stop);
         break;
   }
}

static void State_Idle(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   DispensingRequestHandler_t *instance = InstanceFromFsm(fsm);
   IGNORE(data);

   switch(signal)
   {
      case Signal_StartRequest:
         Fsm_Transition(fsm, State_RequestingDispense);
         break;

      case Signal_StopRequest:
         UpdateDispensingRequestStatus(instance, DispenseStatus_CompletedSuccessfully);
         break;

      case Signal_InvalidRequest:
         UpdateDispensingRequestStatus(instance, DispenseStatus_BadCommand);
         break;
   }
}

void DispensingRequestHandler_Init(
   DispensingRequestHandler_t *instance,
   I_DataModel_t *dataModel,
   const DispensingRequestHandlerConfig_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;

   ClearDispensingRequest(instance);

   Fsm_Init(&instance->_private.fsm, State_Idle);

   EventSubscription_Init(
      &instance->_private.dataModelSubscription,
      instance,
      OnDataModelChange);

   DataModel_SubscribeAll(
      dataModel,
      &instance->_private.dataModelSubscription);
}
