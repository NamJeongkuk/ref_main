/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "UserAllowableInteriorLightingRequestHandler.h"
#include "utils.h"

static void SetStatusTo(UserAllowableInteriorLightingRequestHandler_t *instance, PercentageDutyCycle_t percentageDutyCycle)
{
   percentageDutyCycle = CLAMP(percentageDutyCycle, 0, 100);

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->userAllowableInteriorLightingStateErd,
      &percentageDutyCycle);
}

static void RequestChanged(void *context, const void *args)
{
   UserAllowableInteriorLightingRequestHandler_t *instance = context;
   const PercentageDutyCycle_t *percentageDutyCycle = args;

   SetStatusTo(instance, *percentageDutyCycle);
}

static void CopyStateToRequest(UserAllowableInteriorLightingRequestHandler_t *instance)
{
   PercentageDutyCycle_t statePercentageDutyCycle;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->userAllowableInteriorLightingStateErd,
      &statePercentageDutyCycle);
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->userAllowableInteriorLightingRequestErd,
      &statePercentageDutyCycle);
}

void UserAllowableInteriorLightingRequestHandler_Init(
   UserAllowableInteriorLightingRequestHandler_t *instance,
   I_DataModel_t *dataModel,
   const UserAllowableInteriorLightingBitmap_t lightingBitmap,
   const UserAllowableLightingConfiguration_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->userAllowableInteriorLightingConfigurationErd,
      &lightingBitmap);

   CopyStateToRequest(instance);

   EventSubscription_Init(
      &instance->_private.dataModelSubscription,
      instance,
      RequestChanged);
   DataModel_Subscribe(
      dataModel,
      instance->_private.config->userAllowableInteriorLightingRequestErd,
      &instance->_private.dataModelSubscription);
}
