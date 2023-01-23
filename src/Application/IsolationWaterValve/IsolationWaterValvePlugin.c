/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "IsolationWaterValvePlugin.h"
#include "SystemErds.h"

static bool IsolationWaterValveVotingErdCareDelegate(const void *votingErdData)
{
   const WaterValveVotedState_t *data = votingErdData;
   return (data->care);
}

static const WaterValveVotedState_t defaultIsolationWaterValveData = {
   .state = WaterValveState_Off,
   .care = Vote_DontCare
};

static const ErdResolverConfiguration_t isolationWaterValveResolverConfiguration = {
   .votingErdCare = IsolationWaterValveVotingErdCareDelegate,
   .defaultData = &defaultIsolationWaterValveData,
   .winningVoterErd = Erd_IsolationWaterValve_WinningVoteErd,
   .resolvedStateErd = Erd_IsolationWaterValve_ResolvedVote,
   .numberOfVotingErds = (Erd_IsolationWaterValve_AluminumMoldIceMakerVote - Erd_IsolationWaterValve_WinningVoteErd)
};

void IsolationWaterValvePlugin_Init(
   IsolationWaterValvePlugin_t *instance,
   I_DataModel_t *dataModel)
{
   ErdResolver_Init(
      &instance->_private.isolationWaterValveErdResolver,
      DataModel_AsDataSource(dataModel),
      &isolationWaterValveResolverConfiguration);
}
