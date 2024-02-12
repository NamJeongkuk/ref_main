/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ConvertibleCompartmentDefrostHeaterVotingFrameworkPlugin.h"
#include "SystemErds.h"

enum
{
   WinningVoteErd = Erd_ConvertibleCompartmentDefrostHeater_WinningVoteErd,
   ResolvedVoteErd = Erd_ConvertibleCompartmentDefrostHeater_ResolvedVote,
   LastVoteErd = Erd_ConvertibleCompartmentDefrostHeater_DefrostVote,
   RelayOutputErd = Erd_ConvertibleCompartmentDefrostHeaterRelay,
};

static const ResolvedVoteRelayConnectorConfiguration_t relayConnectorConfiguration = {
   .resolvedRelayVoteErd = ResolvedVoteErd,
   .relayOutputErd = RelayOutputErd,
};

static const HeaterVotedState_t defaultData = {
   .state = HeaterState_Off,
   .care = Vote_DontCare,
};

static bool VotingErdCareDelegate(const void *votingErdData)
{
   const HeaterVotedState_t *data = votingErdData;
   return data->care;
}

static const ErdResolverConfiguration_t erdResolverConfiguration = {
   .votingErdCare = VotingErdCareDelegate,
   .defaultData = &defaultData,
   .winningVoterErd = WinningVoteErd,
   .resolvedStateErd = ResolvedVoteErd,
   .numberOfVotingErds = LastVoteErd - WinningVoteErd,
};

void ConvertibleCompartmentDefrostHeaterVotingFrameworkPlugin_Init(
   ConvertibleCompartmentDefrostHeaterVotingFrameworkPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   ErdResolver_Init(
      &instance->_private.erdResolver,
      DataModel_AsDataSource(dataModel),
      &erdResolverConfiguration);

   ResolvedVoteRelayConnector_Init(
      &instance->_private.relayConnector,
      dataModel,
      &relayConnectorConfiguration);
}
