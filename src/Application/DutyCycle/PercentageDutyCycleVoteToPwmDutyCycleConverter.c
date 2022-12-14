/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "PercentageDutyCycleVoteToPwmDutyCycleConverter.h"
#include "PercentageDutyCycleVote.h"
#include "PwmDutyCycle.h"

static void OnPercentageDutyCycleVoteChange(void *context, const void *args)

{
   PercentageDutyCycleVoteToPwmDutyCycleConverter_t *instance = context;
   const PercentageDutyCycleVote_t *percentageDutyCycleVote = args;

   if(percentageDutyCycleVote->care)
   {
      PwmDutyCycle_t convertedPwmDutyCycle = (PwmDutyCycle_Max * percentageDutyCycleVote->percentageDutyCycle) / 100;

      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->outputPwmDutyCycleErd,
         &convertedPwmDutyCycle);
   }
}

void PercentageDutyCycleVoteToPwmDutyCycleConverter_Init(
   PercentageDutyCycleVoteToPwmDutyCycleConverter_t *instance,
   I_DataModel_t *dataModel,
   const PercentageDutyCycleVoteToPwmDutyCycleConverterConfig_t *config)
{
   instance->_private.config = config;
   instance->_private.dataModel = dataModel;

   EventSubscription_Init(
      &instance->_private.percentageDutyCycleOnChangeSubscription,
      instance,
      OnPercentageDutyCycleVoteChange);
   DataModel_Subscribe(
      instance->_private.dataModel,
      instance->_private.config->inputPercentageDutyCycleVoteErd,
      &instance->_private.percentageDutyCycleOnChangeSubscription);
}
