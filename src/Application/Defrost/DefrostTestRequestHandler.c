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

static void SetNextDefrostTypeOverride(DefrostTestRequestHandler_t *instance, DefrostType_t defrostType)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->nextDefrostTypeOverrideErd,
      &defrostType);
}

static void SetUseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsTo(DefrostTestRequestHandler_t *instance, bool useAhamPrechillReadyToDefrostTimeAndResetDefrostCounts)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->useAhamPrechillReadyToDefrostTimeAndResetDefrostCountsErd,
      &useAhamPrechillReadyToDefrostTimeAndResetDefrostCounts);
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

static void SetDontSkipDefrostPrechillToTrue(DefrostTestRequestHandler_t *instance)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->dontSkipDefrostPrechillErd,
      set);
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

         case DefrostTestRequest_ExitDefrostHeaterOnState:
            UpdateDefrostTestRequestStatus(instance, DefrostTestRequest_ExitDefrostHeaterOnState);
            RequestDefrostTestState(instance, DefrostTestStateRequest_ExitDefrostHeaterOnState);
            break;

         case DefrostTestRequest_SecondaryOnlyDefrost:
            UpdateDefrostTestRequestStatus(instance, DefrostTestRequest_SecondaryOnlyDefrost);
            SetNextDefrostTypeOverride(instance, DefrostType_SecondaryOnly);
            RequestDefrostTestState(instance, DefrostTestStateRequest_Defrost);
            break;

         case DefrostTestRequest_Defrost:
            UpdateDefrostTestRequestStatus(instance, DefrostTestRequest_Defrost);
            SetNextDefrostTypeOverride(instance, DefrostType_Full);
            RequestDefrostTestState(instance, DefrostTestStateRequest_Defrost);
            break;

         case DefrostTestRequest_SecondaryOnlyPrechill:
            UpdateDefrostTestRequestStatus(instance, DefrostTestRequest_SecondaryOnlyPrechill);
            SetNextDefrostTypeOverride(instance, DefrostType_SecondaryOnly);
            RequestDefrostTestState(instance, DefrostTestStateRequest_Prechill);
            break;

         case DefrostTestRequest_Prechill:
            UpdateDefrostTestRequestStatus(instance, DefrostTestRequest_Prechill);
            SetNextDefrostTypeOverride(instance, DefrostType_Full);
            RequestDefrostTestState(instance, DefrostTestStateRequest_Prechill);
            break;

         case DefrostTestRequest_AhamSecondaryOnlyPrechill:
            SetDontSkipDefrostPrechillToTrue(instance);
            UpdateDefrostTestRequestStatus(instance, DefrostTestRequest_AhamSecondaryOnlyPrechill);
            SetNextDefrostTypeOverride(instance, DefrostType_SecondaryOnly);
            SetUseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsTo(instance, true);
            break;

         case DefrostTestRequest_AhamPrechill:
            SetDontSkipDefrostPrechillToTrue(instance);
            UpdateDefrostTestRequestStatus(instance, DefrostTestRequest_AhamPrechill);
            SetNextDefrostTypeOverride(instance, DefrostType_Full);
            SetUseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsTo(instance, true);
            break;

         default:
            break;
      }
   }
   else if(*defrostTestRequest == DefrostTestRequest_Enable)
   {
      UpdateDefrostTestRequestStatus(instance, DefrostTestRequest_Enable);
      SetNextDefrostTypeOverride(instance, DefrostType_Full);
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
