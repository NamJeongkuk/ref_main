/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "DutyCycleVotePercentageToPwmDutyCycleConverter.h"
#include "DutyCyclePercentageVote.h"
#include "PwmDutyCycle.h"

static void OnDutyCyclePercentageVoteChange(void *context, const void *args)
{
   DutyCycleVotePercentageToPwmDutyCycleConverter_t *instance = context;
   const DutyCyclePercentageVote_t *dutyCyclePercentageVote = args;

   if(dutyCyclePercentageVote->care)
   {
      PwmDutyCycle_t convertedPwmDutyCycle = (PwmDutyCycle_Max * dutyCyclePercentageVote->dutyCyclePercentage) / 100;

      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->outputPwmDutyCycleErd,
         &convertedPwmDutyCycle);
   }
}

void DutyCycleVotePercentageToPwmDutyCycleConverter_Init(
   DutyCycleVotePercentageToPwmDutyCycleConverter_t *instance,
   I_DataModel_t *dataModel,
   const DutyCycleVotePercentageToPwmDutyCycleConverterConfig_t *config)
{
   instance->_private.config = config;
   instance->_private.dataModel = dataModel;

   EventSubscription_Init(
      &instance->_private.dutyCyclePercentageOnChangeSubscription,
      instance,
      OnDutyCyclePercentageVoteChange);
   DataModel_Subscribe(
      instance->_private.dataModel,
      instance->_private.config->inputDutyCyclePercentageVoteErd,
      &instance->_private.dutyCyclePercentageOnChangeSubscription);
}
