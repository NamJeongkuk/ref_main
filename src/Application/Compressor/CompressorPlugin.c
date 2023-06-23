/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "CompressorPlugin.h"
#include "CompressorIsOn.h"
#include "SystemErds.h"

enum
{
   CompressorSabbathDelayTimeInSeconds = 20
};

static Erd_t sabbathFanVoteErdList[] = {
   Erd_CondenserFanSpeed_SabbathVote,
   Erd_FreezerEvapFanSpeed_SabbathVote,
   Erd_FreshFoodEvapFanSpeed_SabbathVote
};

static const SabbathFanDelayHandlerConfiguration_t sabbathFanDelayHandlerConfig = {
   .compressorStateErd = Erd_CompressorState,
   .sabbathFanVoteErdList = {
      .erds = sabbathFanVoteErdList,
      .numberOfErds = NUM_ELEMENTS(sabbathFanVoteErdList) }
};

static const CompressorSpeedDriverConfig_t compressorSpeedDriverConfig = {
   .compressorRelayErd = Erd_CompressorRelay,
   .compressorFrequencyErd = Erd_CompressorInverterDriver,
   .coolingModeErd = Erd_CoolingMode,
   .freezerSetpointZoneErd = Erd_FreezerSetpointZone,
   .compressorControllerSpeedErd = Erd_CompressorControllerSpeedRequest
};

static const CompressorSpeedControllerConfiguration_t compressorSpeedControllerConfig = {
   .compressorStateErd = Erd_CompressorState,
   .compressorSpeedRequestErd = Erd_CompressorControllerSpeedRequest,
   .compressorSpeedResolvedVoteErd = Erd_CompressorSpeed_ResolvedVote,
   .valvePositionResolvedVoteErd = Erd_ValvePosition_ResolvedVote,
   .filteredAmbientTemperatureInDegFx100Erd = Erd_Ambient_FilteredTemperatureResolvedInDegFx100,
   .disableMinimumCompressorTimeErd = Erd_DisableMinimumCompressorTimes,
   .sabbathDelayTimeInSeconds = CompressorSabbathDelayTimeInSeconds,
};

static bool VotingErdCareDelegate(const void *votingErdData)
{
   const CompressorVotedSpeed_t *data = votingErdData;
   return (data->care);
}

static const CompressorVotedSpeed_t defaultCompressorSpeedData = {
   .speed = CompressorSpeed_Off,
   .care = Vote_DontCare
};

static const ErdResolverConfiguration_t compressorSpeedVoteResolverConfig = {
   .votingErdCare = VotingErdCareDelegate,
   .defaultData = &defaultCompressorSpeedData,
   .winningVoterErd = Erd_CompressorSpeed_WinningVoteErd,
   .resolvedStateErd = Erd_CompressorSpeed_ResolvedVote,
   .numberOfVotingErds = (Erd_CompressorSpeed_GridVote - Erd_CompressorSpeed_WinningVoteErd)
};

static CompressorStartupFanVotesConfiguration_t compressorStartupFanVotesConfig = {
   .compressorStateErd = Erd_CompressorState,
   .condenserFanSpeedVoteErd = Erd_CondenserFanSpeed_CompressorStartUpVote,
   .freezerEvaporatorFanSpeedVoteErd = Erd_FreezerEvapFanSpeed_CompressorStartUpVote,
   .freshFoodEvaporatorFanSpeedVoteErd = Erd_FreshFoodEvapFanSpeed_CompressorStartUpVote,
   .coolingModeErd = Erd_CoolingMode
};

void CompressorPlugin_Init(CompressorPlugin_t *instance, I_DataModel_t *dataModel)
{
   ErdResolver_Init(
      &instance->_private.compressorSpeedErdResolver,
      DataModel_AsDataSource(dataModel),
      &compressorSpeedVoteResolverConfig);

   CompressorStartupFanVotes_Init(
      &instance->_private.compressorStartupFanVotes,
      dataModel,
      &compressorStartupFanVotesConfig);

   CompressorIsOn_Init(dataModel);

   CompressorSpeedDriver_Init(
      &instance->_private.compressorSpeedDriver,
      dataModel,
      &compressorSpeedDriverConfig);

   CompressorSpeedController_Init(
      &instance->_private.compressorSpeedController,
      dataModel,
      &compressorSpeedControllerConfig);

   SabbathFanDelayHandler_Init(
      &instance->_private.sabbathFanDelayHandler,
      dataModel,
      &sabbathFanDelayHandlerConfig);
}
