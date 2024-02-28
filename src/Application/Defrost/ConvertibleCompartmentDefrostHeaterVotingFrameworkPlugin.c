/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ConvertibleCompartmentDefrostHeaterVotingFrameworkPlugin.h"
#include "SystemErds.h"

static const ResolvedVoteRelayConnectorConfiguration_t relayConnectorConfiguration = {
   .resolvedRelayVoteErd = Erd_ConvertibleCompartmentDefrostHeater_ResolvedVote,
   .relayOutputErd = Erd_ConvertibleCompartmentDefrostHeaterRelay
};

static const HeaterVotedState_t defaultData = {
   .state = HeaterState_Off,
   .care = Vote_DontCare
};

static bool VotingErdCareDelegate(const void *votingErdData)
{
   const HeaterVotedState_t *data = votingErdData;
   return data->care;
}

static const ErdResolverConfiguration_t erdResolverConfiguration = {
   .votingErdCare = VotingErdCareDelegate,
   .defaultData = &defaultData,
   .winningVoterErd = Erd_ConvertibleCompartmentDefrostHeater_WinningVoteErd,
   .resolvedStateErd = Erd_ConvertibleCompartmentDefrostHeater_ResolvedVote,
   .numberOfVotingErds = Erd_ConvertibleCompartmentDefrostHeater_DefrostVote - Erd_ConvertibleCompartmentDefrostHeater_WinningVoteErd
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
