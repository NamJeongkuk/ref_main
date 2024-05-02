/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "CompressorPlugin.h"
#include "SystemErds.h"

static const CompressorSpeedDriverConfig_t compressorSpeedDriverConfig = {
   .compressorRelayErd = Erd_CompressorRelay,
   .compressorFrequencyErd = Erd_CompressorInverterDriver,
   .coolingModeErd = Erd_CoolingMode,
   .freezerSetpointZoneErd = Erd_FreezerSetpointZone,
   .resolvedVoteCompressorSpeedErd = Erd_CompressorSpeed_ResolvedVoteWithSabbathDelay,
   .compressorIsOnErd = Erd_CompressorIsOn
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

static const CompressorStartupConfiguration_t compressorStartupConfig = {
   .compressorResolvedVotedSpeedErd = Erd_CompressorSpeed_ResolvedVote,
   .condenserFanSpeedStartupVoteErd = Erd_CondenserFanSpeed_CompressorStartUpVote,
   .freezerEvaporatorFanSpeedStartupVoteErd = Erd_FreezerEvapFanSpeed_CompressorStartUpVote,
   .freshFoodEvaporatorFanSpeedStartupVoteErd = Erd_FreshFoodEvapFanSpeed_CompressorStartUpVote,
   .compressorSpeedStartupVoteErd = Erd_CompressorSpeed_StartupVote,
   .coolingModeErd = Erd_CoolingMode
};

static const CompressorMinimumOnOffTimeConfiguration_t compressorMinimumOnOffTimeConfig = {
   .resolvedVoteErd = Erd_CompressorSpeed_ResolvedVote,
   .minimumOnOffTimeVoteErd = Erd_CompressorSpeed_MinimumOnOffTimeVote
};

static const Erd_t sealedSystemValveDelayFanVoteList[] = {
   Erd_FreezerEvapFanSpeed_CompressorSealedSystemValveDelayVote,
   Erd_FreshFoodEvapFanSpeed_CompressorSealedSystemValveDelayVote,
   Erd_CondenserFanSpeed_CompressorSealedSystemValveDelayVote
};

static const CompressorSealedSystemValveDelayConfiguration_t compressorSealedSystemValveDelayConfig = {
   .gridCompressorVoteErd = Erd_CompressorSpeed_GridVote,
   .winningVoteErd = Erd_CompressorSpeed_WinningVoteErd,
   .ambientTemperatureIsValidErd = Erd_AmbientTemperature_IsValidResolved,
   .ambientTemperatureErd = Erd_Ambient_FilteredTemperatureResolvedInDegFx100,
   .sealedSystemValveVotedPositionErd = Erd_SealedSystemValvePosition_ResolvedVote,
   .compressorDelayVoteErd = Erd_CompressorSpeed_SealedSystemValveDelayVote,
   .fanDelayVotesErdList = {
      .erds = sealedSystemValveDelayFanVoteList,
      .numberOfErds = NUM_ELEMENTS(sealedSystemValveDelayFanVoteList) }
};

static const CompressorFaultHandlerConfig_t compressorFaultHandlerConfig = {
   .compressorIsOnErd = Erd_CompressorIsOn,
   .coolingOffStatusErd = Erd_CoolingOffStatus,
   .excessiveCompressorRunTimeFaultErd = Erd_ExcessiveCompressorRunTimeFault,
   .compressorOffWhileCoolingSystemOnForMaxTimeFaultErd = Erd_CompressorOffWhileCoolingSystemOnForMaxTimeFault,
   .timerModuleErd = Erd_TimerModule
};

static const SabbathDelayHandlerConfiguration_t sabbathDelayConfig = {
   .compressorResolvedVote = Erd_CompressorSpeed_ResolvedVote,
   .loadResolvedVote = Erd_CompressorSpeed_ResolvedVote,
   .loadResolvedVoteWithSabbathDelay = Erd_CompressorSpeed_ResolvedVoteWithSabbathDelay
};

void CompressorPlugin_Init(CompressorPlugin_t *instance, I_DataModel_t *dataModel)
{
   ErdResolver_Init(
      &instance->_private.compressorSpeedErdResolver,
      DataModel_AsDataSource(dataModel),
      &compressorSpeedVoteResolverConfig);

   CompressorStartup_Init(
      &instance->_private.compressorStartup,
      dataModel,
      PersonalityParametricData_Get(dataModel)->compressorData,
      PersonalityParametricData_Get(dataModel)->platformData->numberOfEvaporators,
      &compressorStartupConfig);

   CompressorSealedSystemValveDelay_Init(
      &instance->_private.compressorSealedSystemValveDelay,
      dataModel,
      PersonalityParametricData_Get(dataModel)->compressorData,
      &compressorSealedSystemValveDelayConfig);

   CompressorMinimumOnOffTime_Init(
      &instance->_private.compressorMinimumOnOffTime,
      dataModel,
      &PersonalityParametricData_Get(dataModel)->compressorData->compressorTimes,
      &compressorMinimumOnOffTimeConfig);

   SabbathDelayHandler_Init(
      &instance->_private.sabbathDelayHandler,
      dataModel,
      &sabbathDelayConfig);

   CompressorSpeedDriver_Init(
      &instance->_private.compressorSpeedDriver,
      dataModel,
      &compressorSpeedDriverConfig);

   CompressorFaultHandler_Init(
      &instance->_private.compressorFaultHandler,
      dataModel,
      &compressorFaultHandlerConfig,
      &PersonalityParametricData_Get(dataModel)->compressorData->compressorTimes);
}
