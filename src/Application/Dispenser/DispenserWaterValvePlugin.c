/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "DispenserWaterValvePlugin.h"
#include "SystemErds.h"

static bool DispenserWaterValveVotingErdCareDelegate(const void *votingErdData)
{
   const WaterValveVotedState_t *data = votingErdData;
   return (data->care);
}

static const WaterValveVotedState_t defaultDispenserWaterValveData = {
   .state = WaterValveState_Off,
   .care = Vote_DontCare
};

static const ErdResolverConfiguration_t dispenserWaterValveResolverConfiguration = {
   .votingErdCare = DispenserWaterValveVotingErdCareDelegate,
   .defaultData = &defaultDispenserWaterValveData,
   .winningVoterErd = Erd_DispenserWaterValve_WinningVoteErd,
   .resolvedStateErd = Erd_DispenserWaterValve_ResolvedVote,
   .numberOfVotingErds = (Erd_DispenserWaterValve_DispensingVote - Erd_DispenserWaterValve_WinningVoteErd)
};

static const ResolvedVoteRelayConnectorConfiguration_t dispenserWaterValveRelayConnectorConfiguration = {
   .resolvedRelayVoteErd = Erd_DispenserWaterValve_ResolvedVote,
   .relayOutputErd = Erd_DispenserValveRelay
};

void DispenserWaterValvePlugin_Init(
   DispenserWaterValvePlugin_t *instance,
   I_DataModel_t *dataModel)
{
   ErdResolver_Init(
      &instance->_private.dispenserWaterValveErdResolver,
      DataModel_AsDataSource(dataModel),
      &dispenserWaterValveResolverConfiguration);

   ResolvedVoteRelayConnector_Init(
      &instance->_private.dispenserWaterValveRelayConnector,
      dataModel,
      &dispenserWaterValveRelayConnectorConfiguration);
}
