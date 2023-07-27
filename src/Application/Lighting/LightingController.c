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
#include "RampingPwmDutyCyclePercentageVote.h"
#include "RampingPwmDutyCycle.h"
#include "PersonalityParametricData.h"

static void OnDataModelChange(void *context, const void *_args)
{
   LightingController_t *instance = context;
   const DataModelOnDataChangeArgs_t *args = _args;

   uint16_t foundIndex;
   VotedPwmDutyCyclePair_t foundPwmDutyCyclePair;
   if(ConstArrayMap_Find(instance->_private.votedPwmToPwmMap, &args->erd, &foundIndex, &foundPwmDutyCyclePair))
   {
      const RampingPwmDutyCyclePercentageVote_t *pwmVotedDutyCyclePercentage = args->data;
      RampingPwmDutyCycle_t rampingDutyCycle;
      PercentageDutyCycle_t maximumLightDutyCyclePercentage = instance->_private.lightingData->maximumLightDutyCyclePercentage;

      rampingDutyCycle.pwmDutyCycle = (pwmVotedDutyCyclePercentage->rampingPwmDutyCyclePercentage.pwmDutyCyclePercentage * UINT16_MAX * maximumLightDutyCyclePercentage) / (100 * 100);
      rampingDutyCycle.rampingUpCountPerMsec = pwmVotedDutyCyclePercentage->rampingPwmDutyCyclePercentage.rampingUpCountInMsec;
      rampingDutyCycle.rampingDownCountPerMsec = pwmVotedDutyCyclePercentage->rampingPwmDutyCyclePercentage.rampingDownCountInMsec;

      DataModel_Write(
         instance->_private.dataModel,
         foundPwmDutyCyclePair.pwmDutyCycleErd,
         &rampingDutyCycle);
   }
}

void LightingController_Init(
   LightingController_t *instance,
   I_DataModel_t *dataModel,
   I_ConstArrayMap_t *votedPwmToPwmMap,
   const LightingData_t *lightingData)
{
   instance->_private.dataModel = dataModel,
   instance->_private.votedPwmToPwmMap = votedPwmToPwmMap,
   instance->_private.lightingData = lightingData,

   EventSubscription_Init(
      &instance->_private.onDataModelChangeSubscription,
      instance,
      OnDataModelChange);
   DataModel_SubscribeAll(
      dataModel,
      &instance->_private.onDataModelChangeSubscription);
}
