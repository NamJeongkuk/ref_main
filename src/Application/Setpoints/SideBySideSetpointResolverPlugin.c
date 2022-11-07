/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SideBySideSetpointResolverPlugin.h"
#include "SystemErds.h"
#include "ErdResolver.h"
#include "Constants_Binary.h"

static const SetpointVotedTemperature_t defaultData = {
   .temperature = INT16_MAX,
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
   .numberOfVotingErds = (Erd_FreshFoodSetpoint_UserVote - Erd_FreshFoodSetpoint_WinningVoteErd)
};

static const ErdResolverConfiguration_t freezerFoodResolverConfiguration = {
   .votingErdCare = VotingErdCareDelegate,
   .defaultData = &defaultData,
   .winningVoterErd = Erd_FreezerSetpoint_WinningVoteErd,
   .resolvedStateErd = Erd_FreezerSetpoint_ResolvedVote,
   .numberOfVotingErds = (Erd_FreezerSetpoint_UserVote - Erd_FreezerSetpoint_WinningVoteErd)
};

void SideBySideSetpointResolverPlugin_Init(
   SideBySideSetpointResolverPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   ErdResolver_Init(
      &instance->_private.freshFoodSetpointResolver,
      DataModel_AsDataSource(dataModel),
      &freshFoodResolverConfiguration);
   ErdResolver_Init(
      &instance->_private.freezeSetpointResolver,
      DataModel_AsDataSource(dataModel),
      &freezerFoodResolverConfiguration);

   DataModel_Write(
      dataModel,
      Erd_SetpointResolverReady,
      set);
}
