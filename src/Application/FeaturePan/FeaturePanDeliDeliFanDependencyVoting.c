/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "FeaturePanDeliDeliFanDependencyVoting.h"
#include "utils.h"
#include "FanControl.h"
#include "PercentageDutyCycleVote.h"

static void Vote(FeaturePanDeliDeliFanDependencyVoting_t *instance, Vote_t vote)
{
   PercentageDutyCycleVote_t deliHeaterVote = { .percentageDutyCycle = PercentageDutyCycle_Min, .care = vote };

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->deliHeaterVotedErd,
      &deliHeaterVote);
};

static FanRpm_t DeliFanRpm(FeaturePanDeliDeliFanDependencyVoting_t *instance)
{
   FanRpm_t fanRpm;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->deliFanActualRpmErd,
      &fanRpm);

   return fanRpm;
}

static void OnDataModelChange(void *context, const void *_args)
{
   FeaturePanDeliDeliFanDependencyVoting_t *instance = context;

   const FanRpm_t *deliFanRpm = _args;
   Vote(instance, *deliFanRpm == 0 ? Vote_Care : Vote_DontCare);
}

void FeaturePanDeliDeliFanDependencyVoting_Init(
   FeaturePanDeliDeliFanDependencyVoting_t *instance,
   I_DataModel_t *dataModel,
   const FeaturePanDeliDeliFanDependencyVotingConfig_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;

   Vote(instance, DeliFanRpm(instance) == 0 ? Vote_Care : Vote_DontCare);

   EventSubscription_Init(
      &instance->_private.onDataModelChangeSubscription,
      instance,
      OnDataModelChange);
   DataModel_Subscribe(
      dataModel,
      instance->_private.config->deliFanActualRpmErd,
      &instance->_private.onDataModelChangeSubscription);
}
