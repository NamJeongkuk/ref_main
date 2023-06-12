/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ResolvedVotePercentageDutyCycleConnector.h"
#include "PercentageDutyCycleVote.h"

static void VoteForOutput(
   ResolvedVotePercentageDutyCycleConnector_t *instance,
   PercentageDutyCycle_t dutyCycle)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->percentageDutyCycleOutputErd,
      &dutyCycle);
}

static void ResolvedVoteChanged(void *context, const void *args)
{
   ResolvedVotePercentageDutyCycleConnector_t *instance = context;
   const PercentageDutyCycleVote_t *vote = args;

   VoteForOutput(instance, vote->percentageDutyCycle);
}

void ResolvedVotePercentageDutyCycleConnector_Init(
   ResolvedVotePercentageDutyCycleConnector_t *instance,
   I_DataModel_t *dataModel,
   const ResolvedVotePercentageDutyCycleConnectorConfiguration_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;

   PercentageDutyCycleVote_t resolvedVote;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->resolvedPercentageDutyCycleVoteErd,
      &resolvedVote);
   VoteForOutput(instance, resolvedVote.percentageDutyCycle);

   EventSubscription_Init(
      &instance->_private.dataModelSubscription,
      instance,
      ResolvedVoteChanged);
   DataModel_Subscribe(
      instance->_private.dataModel,
      instance->_private.config->resolvedPercentageDutyCycleVoteErd,
      &instance->_private.dataModelSubscription);
}
