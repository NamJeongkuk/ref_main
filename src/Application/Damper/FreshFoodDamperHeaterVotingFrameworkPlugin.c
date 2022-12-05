/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SystemErds.h"
#include "FreshFoodDamperHeaterVotingFrameworkPlugin.h"

static const HeaterVotedState_t defaultData = {
   .state = HeaterState_Off,
   .care = Vote_DontCare
};

static bool VotingErdCareDelegate(const void *votingErdData)
{
   const HeaterVotedState_t *data = votingErdData;
   return (data->care);
}

static const ErdResolverConfiguration_t freshFoodDamperHeaterErdResolverConfiguration = {
   .votingErdCare = VotingErdCareDelegate,
   .defaultData = &defaultData,
   .winningVoterErd = Erd_FreshFoodDamperHeater_WinningVoteErd,
   .resolvedStateErd = Erd_FreshFoodDamperHeater_ResolvedVote,
   .numberOfVotingErds = (Erd_FreshFoodDamperHeater_DefrostHeaterSyncVote - Erd_FreshFoodDamperHeater_WinningVoteErd)
};

void FreshFoodDamperHeaterVotingFrameworkPlugin_Init(FreshFoodDamperHeaterVotingFrameworkPlugin_t *instance, I_DataModel_t *dataModel)
{
   ErdResolver_Init(
      &instance->_private.freshFoodDamperHeaterErdResolver,
      DataModel_AsDataSource(dataModel),
      &freshFoodDamperHeaterErdResolverConfiguration);
}
