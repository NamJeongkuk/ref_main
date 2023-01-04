/*!
 * @file
 * @brief Implementation of DefrostTestRequestHandler
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "DefrostTestRequestHandler.h"
#include "DefrostTestRequest.h"
#include "DefrostTestStateRequestMessage.h"
#include "DefrostState.h"
#include "DefrostType.h"
#include "Constants_Binary.h"

static DefrostState_t DefrostState(DefrostTestRequestHandler_t *instance)
{
   DefrostState_t defrostState;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->defrostStateErd,
      &defrostState);

   return defrostState;
}

static void RequestDefrostTestState(DefrostTestRequestHandler_t *instance, DefrostTestStateRequest_t request)
{
   DefrostTestStateRequestMessage_t defrostTestStateRequestMessage;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->defrostTestStateRequestErd,
      &defrostTestStateRequestMessage);

   defrostTestStateRequestMessage.request = request;
   defrostTestStateRequestMessage.requestId++;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->defrostTestStateRequestErd,
      &defrostTestStateRequestMessage);
}

static void UpdateDefrostTestRequestStatus(DefrostTestRequestHandler_t *instance, DefrostTestRequest_t status)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->defrostTestRequestStatusErd,
      &status);
}

static void SetNextDefrostType(DefrostTestRequestHandler_t *instance, DefrostType_t defrostType)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->nextDefrostTypeErd,
      &defrostType);
}

static void SetUseMinimumReadyToDefrostTime(DefrostTestRequestHandler_t *instance, bool useMinimumReadyToDefrostTime)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->useMinimumReadyToDefrostTimeErd,
      &useMinimumReadyToDefrostTime);
}

static void IncrementResetAndCountDefrostCompressorOnTimeCountsAndDoorAccelerationsRequest(DefrostTestRequestHandler_t *instance)
{
   Signal_SendViaErd(
      DataModel_AsDataSource(instance->_private.dataModel),
      instance->_private.config->resetAndCountDefrostCompressorOnTimeCountsAndDoorAccelerationsRequestErd);
}

static void SetDisableDefrost(DefrostTestRequestHandler_t *instance, bool disableDefrost)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->disableDefrostErd,
      &disableDefrost);
}

static void ResetDefrostTestRequest(DefrostTestRequestHandler_t *instance)
{
   DefrostTestRequest_t request = DefrostTestRequest_None;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->defrostTestRequestErd,
      &request);
}

static void HandleDefrostTestRequest(void *context, const void *args)
{
   DefrostTestRequestHandler_t *instance = context;
   const DefrostTestRequest_t *defrostTestRequest = args;

   if(*defrostTestRequest == DefrostTestRequest_Disable)
   {
      UpdateDefrostTestRequestStatus(instance, DefrostTestRequest_Disable);
      SetDisableDefrost(instance, true);
   }
   else if(DefrostState(instance) != DefrostState_Disabled)
   {
      switch(*defrostTestRequest)
      {
         case DefrostTestRequest_Idle:
            UpdateDefrostTestRequestStatus(instance, DefrostTestRequest_Idle);
            RequestDefrostTestState(instance, DefrostTestStateRequest_Idle);
            break;

         case DefrostTestRequest_FreshFoodOnlyDefrost:
            UpdateDefrostTestRequestStatus(instance, DefrostTestRequest_FreshFoodOnlyDefrost);
            SetNextDefrostType(instance, DefrostType_FreshFood);
            RequestDefrostTestState(instance, DefrostTestStateRequest_Defrost);
            break;

         case DefrostTestRequest_Defrost:
            UpdateDefrostTestRequestStatus(instance, DefrostTestRequest_Defrost);
            SetNextDefrostType(instance, DefrostType_Full);
            RequestDefrostTestState(instance, DefrostTestStateRequest_Defrost);
            break;

         case DefrostTestRequest_FreshFoodOnlyPrechill:
            UpdateDefrostTestRequestStatus(instance, DefrostTestRequest_FreshFoodOnlyPrechill);
            SetNextDefrostType(instance, DefrostType_FreshFood);
            RequestDefrostTestState(instance, DefrostTestStateRequest_Prechill);
            break;

         case DefrostTestRequest_Prechill:
            UpdateDefrostTestRequestStatus(instance, DefrostTestRequest_Prechill);
            SetNextDefrostType(instance, DefrostType_Full);
            RequestDefrostTestState(instance, DefrostTestStateRequest_Prechill);
            break;

         case DefrostTestRequest_AhamFreshFoodOnlyPrechill:
            UpdateDefrostTestRequestStatus(instance, DefrostTestRequest_AhamFreshFoodOnlyPrechill);
            SetNextDefrostType(instance, DefrostType_FreshFood);
            SetUseMinimumReadyToDefrostTime(instance, true);
            IncrementResetAndCountDefrostCompressorOnTimeCountsAndDoorAccelerationsRequest(instance);
            break;

         case DefrostTestRequest_AhamPrechill:
            UpdateDefrostTestRequestStatus(instance, DefrostTestRequest_AhamPrechill);
            SetNextDefrostType(instance, DefrostType_Full);
            SetUseMinimumReadyToDefrostTime(instance, true);
            IncrementResetAndCountDefrostCompressorOnTimeCountsAndDoorAccelerationsRequest(instance);
            break;

         default:
            break;
      }
   }
   else if(*defrostTestRequest == DefrostTestRequest_Enable)
   {
      UpdateDefrostTestRequestStatus(instance, DefrostTestRequest_Enable);
      SetDisableDefrost(instance, false);
   }

   ResetDefrostTestRequest(instance);
}

void DefrostTestRequestHandler_Init(
   DefrostTestRequestHandler_t *instance,
   I_DataModel_t *dataModel,
   const DefrostTestRequestHandlerConfiguration_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;

   EventSubscription_Init(
      &instance->_private.defrostTestRequestSubscription,
      instance,
      HandleDefrostTestRequest);
   DataModel_Subscribe(
      instance->_private.dataModel,
      instance->_private.config->defrostTestRequestErd,
      &instance->_private.defrostTestRequestSubscription);
}