/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SingleDoorFreshFoodSetpointResolverPlugin.h"
#include "SystemErds.h"
#include "ErdResolver.h"
#include "Constants_Binary.h"

static const SetpointVotedTemperature_t defaultData = {
   .temperatureInDegFx100 = INT16_MAX,
   .care = false
};

static bool VotingErdCareDelegate(const void *votingErdData)
{
   const SetpointVotedTemperature_t *data = votingErdData;
   return (data->care);
}

static const ErdResolverConfiguration_t freshFoodResolverConfiguration = {
   .votingErdCare = VotingErdCareDelegate,
   .defaultData = &defaultData,
   .winningVoterErd = Erd_FreshFoodSetpoint_WinningVoteErd,
   .resolvedStateErd = Erd_FreshFoodSetpoint_ResolvedVote,
   .numberOfVotingErds = (Erd_FreshFoodSetpoint_ColdestSetpointVote - Erd_FreshFoodSetpoint_WinningVoteErd)
};

static const Erd_t freshFoodSetpointVoteErds[] = {
   Erd_FreshFoodSetpoint_TurboCoolVote,
   Erd_FreshFoodSetpoint_UserVote
};

static const ErdList_t erdListOfFreshFoodSetpointVoteErds = {
   .erds = freshFoodSetpointVoteErds,
   .numberOfErds = NUM_ELEMENTS(freshFoodSetpointVoteErds)
};

static const ColdestOfSetpointVotesConfiguration_t coldestOfFreshFoodSetpointsConfiguration = {
   .listOfSetpointVoteErds = erdListOfFreshFoodSetpointVoteErds,
   .coldestSetpointVoteErd = Erd_FreshFoodSetpoint_ColdestSetpointVote,
   .coldestSetpointWinningVoteErd = Erd_FreshFoodSetpoint_ColdestSetpointWinningVote
};

void SingleDoorFreshFoodSetpointResolverPlugin_Init(
   SingleDoorFreshFoodSetpointResolverPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   ColdestOfSetpointVotes_Init(
      &instance->_private.coldestOfFreshFoodSetpoints,
      dataModel,
      &coldestOfFreshFoodSetpointsConfiguration);

   ErdResolver_Init(
      &instance->_private.freshFoodSetpointResolver,
      DataModel_AsDataSource(dataModel),
      &freshFoodResolverConfiguration);

   DataModel_Write(
      dataModel,
      Erd_SetpointResolverReady,
      set);
}
