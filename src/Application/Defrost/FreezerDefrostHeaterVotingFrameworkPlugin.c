/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SystemErds.h"
#include "FreezerDefrostHeaterVotingFrameworkPlugin.h"

static const HeaterVotedState_t defaultData = {
   .state = HeaterState_Off,
   .care = false
};

static bool VotingErdCareDelegate(const void *votingErdData)
{
   const HeaterVotedState_t *data = votingErdData;
   return (data->care);
}

static const ErdResolverConfiguration_t freezerDefrostHeaterErdResolverConfiguration = {
   .votingErdCare = VotingErdCareDelegate,
   .defaultData = &defaultData,
   .winningVoterErd = Erd_FreezerDefrostHeater_WinningVoteErd,
   .resolvedStateErd = Erd_FreezerDefrostHeater_ResolvedVote,
   .numberOfVotingErds = (Erd_FreezerDefrostHeater_DefrostVote - Erd_FreezerDefrostHeater_WinningVoteErd)
};

static const ResolvedVoteRelayConnectorConfiguration_t freezerDefrostHeaterRelayConnectorConfiguration = {
   .resolvedRelayVoteErd = Erd_FreezerDefrostHeater_ResolvedVote,
   .relayOutputErd = Erd_FreezerDefrostHeaterRelay
};

void FreezerDefrostHeaterVotingFrameworkPlugin_Init(FreezerDefrostHeaterVotingFrameworkPlugin_t *instance, I_DataModel_t *dataModel)
{
   ErdResolver_Init(
      &instance->_private.freezerDefrostHeaterErdResolver,
      DataModel_AsDataSource(dataModel),
      &freezerDefrostHeaterErdResolverConfiguration);

   ResolvedVoteRelayConnector_Init(
      &instance->_private.freezerDefrostHeaterRelayConnector,
      dataModel,
      &freezerDefrostHeaterRelayConnectorConfiguration);
}
