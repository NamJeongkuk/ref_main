/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "IceCabinetFreezerEvapFanDependencyVoting.h"
#include "SystemErds.h"

static void UpdateIceMakerFreezerDependencyVote(IceCabinetFreezerEvapFanDependencyVoting_t *instance, const FanVotedSpeed_t freezerEvapFanVotedSpeed)
{
   FanVotedSpeed_t vote = { .speed = FanSpeed_Off, .care = Vote_DontCare };

   if(freezerEvapFanVotedSpeed.care == Vote_Care &&
      freezerEvapFanVotedSpeed.speed == FanSpeed_Off)
   {
      vote.care = Vote_Care;
   }

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->iceCabinetDependencyOnFreezerFanVoteErd,
      &vote);
}

static void OnFreezerEvapResolvedVoteChange(void *context, const void *args)
{
   IceCabinetFreezerEvapFanDependencyVoting_t *instance = context;
   const FanVotedSpeed_t *resolvedFreezerEvapFanSpeed = args;

   UpdateIceMakerFreezerDependencyVote(instance, *resolvedFreezerEvapFanSpeed);
}

void IceCabinetFreezerEvapFanDependencyVoting_Init(
   IceCabinetFreezerEvapFanDependencyVoting_t *instance,
   I_DataModel_t *dataModel,
   const IceCabinetFreezerEvapFanDependencyVotingConfiguration_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;

   FanVotedSpeed_t resolvedFreezerEvapFanSpeed;
   DataModel_Read(
      dataModel,
      instance->_private.config->freezerEvapFanSpeedResolvedVoteErd,
      &resolvedFreezerEvapFanSpeed);

   UpdateIceMakerFreezerDependencyVote(instance, resolvedFreezerEvapFanSpeed);

   EventSubscription_Init(
      &instance->_private.subscription,
      instance,
      OnFreezerEvapResolvedVoteChange);

   DataModel_Subscribe(
      dataModel,
      config->freezerEvapFanSpeedResolvedVoteErd,
      &instance->_private.subscription);
}
