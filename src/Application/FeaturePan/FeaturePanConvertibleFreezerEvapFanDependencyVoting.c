/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "FeaturePanConvertibleFreezerEvapFanDependencyVoting.h"
#include "FanSpeed.h"
#include "DamperVotedPosition.h"
#include "Vote.h"

static void Vote(FeaturePanConvertibleFreezerEvapFanDependencyVoting_t *instance, Vote_t care)
{
   DamperVotedPosition_t damperVote = { .position = DamperPosition_Closed, .care = care };

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->convertibleCompartmentDamperVoteErd,
      &damperVote);
};

static FanVotedSpeed_t FreezerEvapFanVotedSpeed(FeaturePanConvertibleFreezerEvapFanDependencyVoting_t *instance)
{
   FanVotedSpeed_t fanVotedSpeed;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerEvapFanResolvedVoteErd,
      &fanVotedSpeed);

   return fanVotedSpeed;
}

static void VoteForConvertibleCompartmentDamper(
   FeaturePanConvertibleFreezerEvapFanDependencyVoting_t *instance,
   FanVotedSpeed_t fanVotedSpeed)
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
   FeaturePanConvertibleFreezerEvapFanDependencyVoting_t *instance = context;
   const FanVotedSpeed_t *fanVotedSpeed = _args;

   VoteForConvertibleCompartmentDamper(instance, *fanVotedSpeed);
}

void FeaturePanConvertibleFreezerEvapFanDependencyVoting_Init(
   FeaturePanConvertibleFreezerEvapFanDependencyVoting_t *instance,
   I_DataModel_t *dataModel,
   const FeaturePanConvertibleFreezerEvapFanDependencyVotingConfiguration_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;

   VoteForConvertibleCompartmentDamper(instance, FreezerEvapFanVotedSpeed(instance));

   EventSubscription_Init(
      &instance->_private.subscription,
      instance,
      OnEvapFanResolvedVoteChange);

   DataModel_Subscribe(
      dataModel,
      instance->_private.config->freezerEvapFanResolvedVoteErd,
      &instance->_private.subscription);
}
