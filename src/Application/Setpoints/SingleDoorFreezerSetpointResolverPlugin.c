/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SingleDoorFreezerSetpointResolverPlugin.h"
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

static const ErdResolverConfiguration_t freezerResolverConfiguration = {
   .votingErdCare = VotingErdCareDelegate,
   .defaultData = &defaultData,
   .winningVoterErd = Erd_FreezerSetpoint_WinningVoteErd,
   .resolvedStateErd = Erd_FreezerSetpoint_ResolvedVote,
   .numberOfVotingErds = (Erd_FreezerSetpoint_ColdestSetpointVote - Erd_FreezerSetpoint_WinningVoteErd)
};

static const Erd_t freezerSetpointVoteErds[] = {
   Erd_FreezerSetpoint_TurboFreezeVote,
   Erd_FreezerSetpoint_FreezerIceMakerVote,
   Erd_FreezerSetpoint_FreezerIceRateVote,
   Erd_FreezerSetpoint_UserVote
};

static const ErdList_t erdListOfFreezerSetpointVoteErds = {
   .erds = freezerSetpointVoteErds,
   .numberOfErds = NUM_ELEMENTS(freezerSetpointVoteErds)
};

static const ColdestOfSetpointVotesConfiguration_t coldestOfFreezerSetpointsConfiguration = {
   .listOfSetpointVoteErds = erdListOfFreezerSetpointVoteErds,
   .coldestSetpointVoteErd = Erd_FreezerSetpoint_ColdestSetpointVote,
   .coldestSetpointWinningVoteErd = Erd_FreezerSetpoint_ColdestSetpointWinningVote
};

void SingleDoorFreezerSetpointResolverPlugin_Init(
   SingleDoorFreezerSetpointResolverPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   ColdestOfSetpointVotes_Init(
      &instance->_private.coldestOfFreezerSetpoints,
      dataModel,
      &coldestOfFreezerSetpointsConfiguration);
   ErdResolver_Init(
      &instance->_private.freezerSetpointResolver,
      DataModel_AsDataSource(dataModel),
      &freezerResolverConfiguration);

   DataModel_Write(
      dataModel,
      Erd_SetpointResolverReady,
      set);
}
