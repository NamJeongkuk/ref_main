/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "PostDwellCompletionSignalRequester.h"
#include "Signal.h"

static void DefrostHsmStateUpdated(void *context, const void *args)
{
   PostDwellCompletionSignalRequester_t *instance = context;
   const DefrostHsmState_t *newDefrostHsmState = args;

   if((instance->_private.oldDefrostHsmState == DefrostHsmState_PostDwell) &&
      (*newDefrostHsmState == DefrostHsmState_Idle))
   {
      Signal_SendViaErd(
         DataModel_AsDataSource(instance->_private.dataModel),
         instance->_private.config->postDwellCompletionSignalErd);
   }

   instance->_private.oldDefrostHsmState = *newDefrostHsmState;
}

void PostDwellCompletionSignalRequester_Init(
   PostDwellCompletionSignalRequester_t *instance,
   I_DataModel_t *dataModel,
   const PostDwellCompletionSignalRequesterConfig_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;

   DataModel_Read(
      instance->_private.dataModel,
      config->defrostHsmStateErd,
      &instance->_private.oldDefrostHsmState);

   EventSubscription_Init(
      &instance->_private.defrostHsmStateSubscription,
      instance,
      DefrostHsmStateUpdated);

   DataModel_Subscribe(
      dataModel,
      config->defrostHsmStateErd,
      &instance->_private.defrostHsmStateSubscription);
}
