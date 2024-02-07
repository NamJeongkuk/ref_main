/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SystemErds.h"
#include "FreshFoodDefrostHeaterVotingFrameworkPlugin.h"

static const HeaterVotedState_t defaultData = {
   .state = HeaterState_Off,
   .care = false
};

static bool VotingErdCareDelegate(const void *votingErdData)
{
   const HeaterVotedState_t *data = votingErdData;
   return (data->care);
}

static const ErdResolverConfiguration_t freshFoodDefrostHeaterErdResolverConfiguration = {
   .votingErdCare = VotingErdCareDelegate,
   .defaultData = &defaultData,
   .winningVoterErd = Erd_FreshFoodDefrostHeater_WinningVoteErd,
   .resolvedStateErd = Erd_FreshFoodDefrostHeater_ResolvedVote,
   .numberOfVotingErds = (Erd_FreshFoodDefrostHeater_DefrostVote - Erd_FreshFoodDefrostHeater_WinningVoteErd)
};

static const ResolvedVoteRelayConnectorConfiguration_t freshFoodDefrostHeaterRelayConnectorConfiguration = {
   .resolvedRelayVoteErd = Erd_FreshFoodDefrostHeater_ResolvedVote,
   .relayOutputErd = Erd_FreshFoodDefrostHeaterRelay
};

void FreshFoodDefrostHeaterVotingFrameworkPlugin_Init(FreshFoodDefrostHeaterVotingFrameworkPlugin_t *instance, I_DataModel_t *dataModel)
{
   ErdResolver_Init(
      &instance->_private.freshFoodDefrostHeaterErdResolver,
      DataModel_AsDataSource(dataModel),
      &freshFoodDefrostHeaterErdResolverConfiguration);

   ResolvedVoteRelayConnector_Init(
      &instance->_private.freshFoodDefrostHeaterRelayConnector,
      dataModel,
      &freshFoodDefrostHeaterRelayConnectorConfiguration);
}
