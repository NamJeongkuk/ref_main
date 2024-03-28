/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "FourDoorSetpointResolverPlugin.h"
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

static const ErdResolverConfiguration_t freezerResolverConfiguration = {
   .votingErdCare = VotingErdCareDelegate,
   .defaultData = &defaultData,
   .winningVoterErd = Erd_FreezerSetpoint_WinningVoteErd,
   .resolvedStateErd = Erd_FreezerSetpoint_ResolvedVote,
   .numberOfVotingErds = (Erd_FreezerSetpoint_ColdestSetpointVote - Erd_FreezerSetpoint_WinningVoteErd)
};

static const ErdResolverConfiguration_t convertibleCompartmentResolverConfiguration = {
   .votingErdCare = VotingErdCareDelegate,
   .defaultData = &defaultData,
   .winningVoterErd = Erd_ConvertibleCompartmentSetpoint_WinningVoteErd,
   .resolvedStateErd = Erd_ConvertibleCompartmentSetpoint_ResolvedVote,
   .numberOfVotingErds = (Erd_ConvertibleCompartmentSetpoint_UserVote - Erd_ConvertibleCompartmentSetpoint_WinningVoteErd)
};

static const Erd_t freezerSetpointVoteErds[] = {
   Erd_FreezerSetpoint_TurboFreezeVote,
   Erd_FreezerSetpoint_IceInDoorVote,
   Erd_FreezerSetpoint_FreezerIceMakerVote,
   Erd_FreezerSetpoint_FreezerIceRateVote,
   Erd_FreezerSetpoint_UserVote
};

static const ColdestOfSetpointVotesConfiguration_t coldestOfFreezerSetpointsConfiguration = {
   .listOfSetpointVoteErds = {
      .erds = freezerSetpointVoteErds,
      .numberOfErds = NUM_ELEMENTS(freezerSetpointVoteErds),
   },
   .coldestSetpointVoteErd = Erd_FreezerSetpoint_ColdestSetpointVote,
   .coldestSetpointWinningVoteErd = Erd_FreezerSetpoint_ColdestSetpointWinningVote
};

static const Erd_t freshFoodSetpointVoteErds[] = {
   Erd_FreshFoodSetpoint_TurboCoolVote,
   Erd_FreshFoodSetpoint_UserVote
};

static const ColdestOfSetpointVotesConfiguration_t coldestOfFreshFoodSetpointsConfiguration = {
   .listOfSetpointVoteErds = {
      .erds = freshFoodSetpointVoteErds,
      .numberOfErds = NUM_ELEMENTS(freshFoodSetpointVoteErds),
   },
   .coldestSetpointVoteErd = Erd_FreshFoodSetpoint_ColdestSetpointVote,
   .coldestSetpointWinningVoteErd = Erd_FreshFoodSetpoint_ColdestSetpointWinningVote
};

void FourDoorSetpointResolverPlugin_Init(
   FourDoorSetpointResolverPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   ColdestOfSetpointVotes_Init(
      &instance->_private.coldestOfFreshFoodSetpoints,
      dataModel,
      &coldestOfFreshFoodSetpointsConfiguration);

   ColdestOfSetpointVotes_Init(
      &instance->_private.coldestOfFreezerSetpoints,
      dataModel,
      &coldestOfFreezerSetpointsConfiguration);

   ErdResolver_Init(
      &instance->_private.freshFoodSetpointResolver,
      DataModel_AsDataSource(dataModel),
      &freshFoodResolverConfiguration);
   ErdResolver_Init(
      &instance->_private.freezerSetpointResolver,
      DataModel_AsDataSource(dataModel),
      &freezerResolverConfiguration);
   ErdResolver_Init(
      &instance->_private.convertibleCompartmentSetpointResolver,
      DataModel_AsDataSource(dataModel),
      &convertibleCompartmentResolverConfiguration);

   DataModel_Write(
      dataModel,
      Erd_SetpointResolverReady,
      set);
}
