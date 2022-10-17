/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "PostDefrostShiftSeedingRequester.h"
#include "Signal.h"

static void DefrostHsmStateUpdated(void *context, const void *args)
{
   PostDefrostShiftSeedingRequester_t *instance = context;
   const DefrostHsmState_t *newDefrostHsmState = args;

   if((instance->_private.oldDefrostHsmState == DefrostHsmState_PostDwell) &&
      (*newDefrostHsmState == DefrostHsmState_Idle))
   {
      Signal_SendViaErd(
         DataModel_AsDataSource(instance->_private.dataModel),
         instance->_private.config->RequestPostDefrostShiftSeedingErd);
   }

   instance->_private.oldDefrostHsmState = *newDefrostHsmState;
}

void PostDefrostShiftSeedingRequester_Init(
   PostDefrostShiftSeedingRequester_t *instance,
   I_DataModel_t *dataModel,
   const PostDefrostShiftSeedingRequesterConfig_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;

   DataModel_Read(
      instance->_private.dataModel,
      config->DefrostHsmStateErd,
      &instance->_private.oldDefrostHsmState);

   EventSubscription_Init(
      &instance->_private.defrostHsmStateSubscription,
      instance,
      DefrostHsmStateUpdated);

   DataModel_Subscribe(
      dataModel,
      config->DefrostHsmStateErd,
      &instance->_private.defrostHsmStateSubscription);
}
