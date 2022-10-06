/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SystemErds.h"
#include "FillTubeHeaterVotingFrameworkPlugin.h"
#include "FillTubeHeaterVotedDutyCycle.h"

static const FillTubeHeaterVotedDutyCycle_t defaultData = {
   .dutyCycle = 0,
   .care = false
};

static bool VotingErdCareDelegate(const void *votingErdData)
{
   const FillTubeHeaterVotedDutyCycle_t *data = votingErdData;
   return (data->care);
}

static const ErdResolverConfiguration_t fillTubeHeaterErdResolverConfiguration = {
   .votingErdCare = VotingErdCareDelegate,
   .defaultData = &defaultData,
   .winningVoterErd = Erd_FillTubeHeater_WinningVoteErd,
   .resolvedStateErd = Erd_FillTubeHeater_ResolvedVote,
   .numberOfVotingErds = (Erd_FillTubeHeater_ContinuousVote - Erd_FillTubeHeater_WinningVoteErd)
};

void FillTubeHeaterVotingFrameworkPlugin_Init(FillTubeHeaterVotingFrameworkPlugin_t *instance, I_DataModel_t *dataModel)
{
   ErdResolver_Init(
      &instance->_private.fillTubeHeaterErdResolver,
      DataModel_AsDataSource(dataModel),
      &fillTubeHeaterErdResolverConfiguration);
}
