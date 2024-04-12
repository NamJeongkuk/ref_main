/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "FeaturePanDeliDependencyVoting.h"
#include "FanSpeed.h"
#include "DamperVotedPosition.h"
#include "Vote.h"

static void Vote(FeaturePanDeliDependencyVoting_t *instance, Vote_t vote)
{
   DamperVotedPosition_t damperVote = { .position = DamperPosition_Closed, .care = vote };
   FanVotedSpeed_t deliFanVote = { . speed = FanSpeed_Off, .care = vote };

   DataModel_Write(
      instance->_private.dataModel, 
      instance->_private.config->fanVoteErd, 
      &deliFanVote);

   DataModel_Write(
      instance->_private.dataModel, 
      instance->_private.config->damperVoteErd, 
      &damperVote);
};

static FanVotedSpeed_t EvapFanVotedSpeed(FeaturePanDeliDependencyVoting_t *instance)
{
   FanVotedSpeed_t fanVotedSpeed;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->evapFanResolvedVoteErd,
      &fanVotedSpeed);

   return fanVotedSpeed;
}

static void VoteForDamperAndDeliFan(FeaturePanDeliDependencyVoting_t *instance, FanVotedSpeed_t fanVotedSpeed)
{
   if(fanVotedSpeed.speed == FanSpeed_Off && fanVotedSpeed.care == Vote_Care)
   {
      Vote(instance, Vote_Care);
   }
   else
   {
      Vote(instance, Vote_DontCare);
   }
}

static void OnEvapFanResolvedVoteChange(void *context, const void *_args)
{
   FeaturePanDeliDependencyVoting_t *instance = context;
   const FanVotedSpeed_t *fanVotedSpeed = _args;

   VoteForDamperAndDeliFan(instance, *fanVotedSpeed);
}

void FeaturePanDeliDependencyVoting_Init(
   FeaturePanDeliDependencyVoting_t *instance,
   I_DataModel_t *dataModel,
   const FeaturePanDeliDependencyVotingConfiguration_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;

   VoteForDamperAndDeliFan(instance, EvapFanVotedSpeed(instance));

   EventSubscription_Init(
      &instance->_private.subscription,
      instance,
      OnEvapFanResolvedVoteChange);

   DataModel_Subscribe(
      dataModel,
      instance->_private.config->evapFanResolvedVoteErd,
      &instance->_private.subscription);
}
