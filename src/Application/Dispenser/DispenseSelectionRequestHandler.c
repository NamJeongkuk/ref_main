/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "DispenseSelectionRequestHandler.h"

static void ResetUserDispenseRequest(DispenseSelectionRequestHandler_t *instance)
{
   DispenseSelection_t resetDispenseSelectionRequest = DispenseSelection_Reset;
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->dispenseSelectionRequestErd,
      &resetDispenseSelectionRequest);
}

static void UpdateDispenseSelectionStatus(DispenseSelectionRequestHandler_t *instance, const DispenseSelection_t *dispenseSelectionStatus)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->dispenseSelectionStatusErd,
      dispenseSelectionStatus);
}

static void OnDataModelChange(void *context, const void *_args)
{
   DispenseSelectionRequestHandler_t *instance = context;
   const DispenseSelection_t *dispenseSelection = _args;

   if(*dispenseSelection != DispenseSelection_Reset)
   {
      UpdateDispenseSelectionStatus(instance, dispenseSelection);
      ResetUserDispenseRequest(instance);
   }
}

void DispenseSelectionRequestHandler_Init(
   DispenseSelectionRequestHandler_t *instance,
   const DispenseSelectionRequestHandlerConfig_t *config,
   I_DataModel_t *dataModel)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;

   ResetUserDispenseRequest(instance);

   EventSubscription_Init(
      &instance->_private.onDataModelChangeSubscription,
      instance,
      OnDataModelChange);
   DataModel_Subscribe(
      instance->_private.dataModel,
      instance->_private.config->dispenseSelectionRequestErd,
      &instance->_private.onDataModelChangeSubscription);
}
