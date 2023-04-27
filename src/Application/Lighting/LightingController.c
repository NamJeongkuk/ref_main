/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "LightingController.h"
#include "utils.h"
#include "VotedPwmDutyCyclePair.h"
#include "PwmVotedDutyCycle.h"

static void OnDataModelChange(void *context, const void *_args)
{
   LightingController_t *instance = context;
   const DataModelOnDataChangeArgs_t *args = _args;

   uint16_t foundIndex;
   VotedPwmDutyCyclePair_t foundPwmDutyCyclePair;
   if(ConstArrayMap_Find(instance->_private.votedPwmToPwmMap, &args->erd, &foundIndex, &foundPwmDutyCyclePair))
   {
      const PwmVotedDutyCycle_t *pwmVotedDutyCycle = args->data;
      DataModel_Write(
         instance->_private.dataModel,
         foundPwmDutyCyclePair.pwmDutyCycleErd,
         pwmVotedDutyCycle);
   }
}

void LightingController_Init(
   LightingController_t *instance,
   I_DataModel_t *dataModel,
   I_ConstArrayMap_t *votedPwmToPwmMap)
{
   instance->_private.dataModel = dataModel,
   instance->_private.votedPwmToPwmMap = votedPwmToPwmMap,

   EventSubscription_Init(
      &instance->_private.onDataModelChangeSubscription,
      instance,
      OnDataModelChange);
   DataModel_SubscribeAll(
      dataModel,
      &instance->_private.onDataModelChangeSubscription);
}
