/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "IceCabinetFanPlugin.h"
#include "FanSpeed.h"
#include "SystemErds.h"

static bool VotingErdCareDelegate(const void *votingErdData)
{
   const FanVotedSpeed_t *data = votingErdData;
   return (data->care);
}

static const FanVotedSpeed_t defaultData = {
   .speed = FanSpeed_Off,
   .care = Vote_DontCare
};

static const ErdResolverConfiguration_t iceCabinetFanSpeedResolverConfiguration = {
   .votingErdCare = VotingErdCareDelegate,
   .defaultData = &defaultData,
   .winningVoterErd = Erd_IceCabinetFanSpeed_WinningVoteErd,
   .resolvedStateErd = Erd_IceCabinetFanSpeed_ResolvedVote,
   .numberOfVotingErds = (Erd_IceCabinetFanSpeed_IceCabinetGridVote - Erd_IceCabinetFanSpeed_WinningVoteErd)
};

void IceCabinetFanPlugin_Init(
   IceCabinetFanPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   ErdResolver_Init(
      &instance->_private.fanSpeedVoteResolver,
      DataModel_AsDataSource(dataModel),
      &iceCabinetFanSpeedResolverConfiguration);
}
