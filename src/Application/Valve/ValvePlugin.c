/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ValvePlugin.h"
#include "SystemErds.h"
#include "ValvePosition.h"

static const ValveVotedPosition_t defaultValveVotedData = {
   .position = ValvePosition_A,
   .care = false
};

static bool ValveVotedErdCareDelegate(const void *valveErdData)
{
   const ValveVotedPosition_t *data = valveErdData;
   return (data->care);
}

static const ErdResolverConfiguration_t valvePositionVoteResolverConfig = {
   .votingErdCare = ValveVotedErdCareDelegate,
   .defaultData = &defaultValveVotedData,
   .winningVoterErd = Erd_ValvePosition_WinningVoteErd,
   .resolvedStateErd = Erd_ValvePosition_ResolvedVote,
   .numberOfVotingErds = (Erd_ValvePosition_GridVote - Erd_ValvePosition_WinningVoteErd)
};

void ValvePlugin_Init(ValvePlugin_t *instance, I_DataModel_t *dataModel)
{
   ErdResolver_Init(
      &instance->_private.valvePositionErdResolver,
      DataModel_AsDataSource(dataModel),
      &valvePositionVoteResolverConfig);
}
