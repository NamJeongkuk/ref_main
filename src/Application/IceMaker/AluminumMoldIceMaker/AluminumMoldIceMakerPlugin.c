/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SystemErds.h"
#include "AluminumMoldIceMakerPlugin.h"
#include "PersonalityParametricData.h"
#include "IceMakerEnableResolver.h"
#include "Vote.h"

FreezerIceRateHandlerConfig_t iceRateHandlerConfig = {
   .freezerIceRateTriggerSignal = Erd_FreezerIceRateTriggerSignal,
   .freezerSetpointUserVote = Erd_FreezerSetpoint_UserVote,
   .freezerSetpointFreezerIceRateVote = Erd_FreezerSetpoint_FreezerIceRateVote,
   .freezerEvapFanSpeedFreezerIceRateVote = Erd_FreezerEvapFanSpeed_FreezerIceRateVote,
   .freezerEvapFanSpeedResolvedVote = Erd_FreezerEvapFanSpeed_ResolvedVote,
};

static const SoftPwmConfiguration_t softPwmConfig = {
   .pwmDutyCycleErd = Erd_FillTubeHeaterPwmDutyCycle,
   .gpioErd = Erd_FillTubeHeater,
   .timerModuleErd = Erd_TimerModule,
   .frequencyX100 = 10
};

static const AluminumMoldIceMakerConfig_t aluminumMoldIceMakerConfig = {
   .aluminumMoldIceMakerHsmStateErd = Erd_AluminumMoldIceMakerHsmState,
   .waterValveVoteErd = Erd_AluminumMoldIceMakerWaterValve_IceMakerVote,
   .iceMakerHeaterVoteErd = Erd_AluminumMoldIceMakerHeater_IceMakerVote,
   .iceMakerMotorVoteErd = Erd_AluminumMoldIceMakerMotor_IceMakerVote,
   .harvestCountCalculationRequestErd = Erd_HarvestCountCalculationRequest,
   .feelerArmMonitoringRequestErd = Erd_FeelerArmMonitoringRequest,
   .harvestCountIsReadyToHarvestErd = Erd_HarvestCountIsReadyToHarvest,
   .iceMakerFilteredTemperatureInDegFx100Erd = Erd_AluminumMoldIceMaker_FilteredTemperatureInDegFx100,
   .feelerArmIsReadyToEnterHarvestErd = Erd_FeelerArmIsReadyToEnterHarvest,
   .iceMakerEnabledErd = Erd_IceMakerEnabledResolved,
   .sabbathModeErd = Erd_SabbathMode
};

static const FeelerArmMonitorConfig_t feelerArmMonitorConfig = {
   .feelerArmMonitoringRequestErd = Erd_FeelerArmMonitoringRequest,
   .timerModuleErd = Erd_TimerModule,
   .feelerArmIsReadyToEnterHarvestErd = Erd_FeelerArmIsReadyToEnterHarvest,
   .feelerArmPositionErd = Erd_FeelerArmPosition
};

static const Erd_t enableErdsList[] = {
   Erd_IceMakerEnabledByUser, Erd_IceMakerEnabledByGrid, Erd_IceMakerEnabledByDemandResponse
};

static const ErdLogicServiceConfigurationEntry_t configurationEntries[] = {
   { ErdLogicServiceOperator_Or, { enableErdsList, NUM_ELEMENTS(enableErdsList) }, Erd_IceMakerEnabledResolved },
};

static const ErdLogicServiceConfiguration_t iceMakerEnableResolverConfiguration = {
   configurationEntries,
   NUM_ELEMENTS(configurationEntries)
};

static bool HeaterVotingErdCareDelegate(const void *votingErdData)
{
   const HeaterVotedState_t *data = votingErdData;
   return (data->care);
}

static const HeaterVotedState_t defaultHeaterData = {
   .state = HeaterState_Off,
   .care = Vote_DontCare
};

static bool MotorVotingErdCareDelegate(const void *votingErdData)
{
   const AluminumMoldIceMakerMotorState_t *data = votingErdData;
   return (data->care);
}

static const AluminumMoldIceMakerMotorState_t defaultMotorData = {
   .state = MotorState_Off,
   .care = Vote_DontCare
};

static bool WaterValveVotingErdCareDelegate(const void *votingErdData)
{
   const WaterValveVotedState_t *data = votingErdData;
   return (data->care);
}

static const WaterValveVotedState_t defaultWaterValveData = {
   .state = WaterValveState_Off,
   .care = Vote_DontCare
};

static const ErdResolverConfiguration_t iceMakerHeaterResolverConfiguration = {
   .votingErdCare = HeaterVotingErdCareDelegate,
   .defaultData = &defaultHeaterData,
   .winningVoterErd = Erd_AluminumMoldIceMakerHeater_WinningVoteErd,
   .resolvedStateErd = Erd_AluminumMoldIceMakerHeater_ResolvedVote,
   .numberOfVotingErds = (Erd_AluminumMoldIceMakerHeater_IceMakerVote - Erd_AluminumMoldIceMakerHeater_WinningVoteErd)
};

static const ErdResolverConfiguration_t iceMakerMotorResolverConfiguration = {
   .votingErdCare = MotorVotingErdCareDelegate,
   .defaultData = &defaultMotorData,
   .winningVoterErd = Erd_AluminumMoldIceMakerMotor_WinningVoteErd,
   .resolvedStateErd = Erd_AluminumMoldIceMakerMotor_ResolvedVote,
   .numberOfVotingErds = (Erd_AluminumMoldIceMakerMotor_IceMakerVote - Erd_AluminumMoldIceMakerMotor_WinningVoteErd)
};

static const ErdResolverConfiguration_t iceMakerWaterValveResolverConfiguration = {
   .votingErdCare = WaterValveVotingErdCareDelegate,
   .defaultData = &defaultWaterValveData,
   .winningVoterErd = Erd_AluminumMoldIceMakerWaterValve_WinningVoteErd,
   .resolvedStateErd = Erd_AluminumMoldIceMakerWaterValve_ResolvedVote,
   .numberOfVotingErds = (Erd_AluminumMoldIceMakerWaterValve_IceMakerVote - Erd_AluminumMoldIceMakerWaterValve_WinningVoteErd)
};

static void InitializeErdResolvers(AluminumMoldIceMakerPlugin_t *instance, I_DataModel_t *dataModel)
{
   ErdResolver_Init(
      &instance->_private.iceMakerHeaterVoteResolver,
      DataModel_AsDataSource(dataModel),
      &iceMakerHeaterResolverConfiguration);

   ErdResolver_Init(
      &instance->_private.iceMakerMotorVoteResolver,
      DataModel_AsDataSource(dataModel),
      &iceMakerMotorResolverConfiguration);

   ErdResolver_Init(
      &instance->_private.iceMakerWaterValveVoteResolver,
      DataModel_AsDataSource(dataModel),
      &iceMakerWaterValveResolverConfiguration);
}

void AluminumMoldIceMakerPlugin_Init(AluminumMoldIceMakerPlugin_t *instance, I_DataModel_t *dataModel)
{
   InitializeErdResolvers(
      instance,
      dataModel);

   FillTubeHeaterVotingFrameworkPlugin_Init(
      &instance->_private.fillTubeHeaterVotingFrameworkPlugin,
      dataModel);

   FreezerIceRateHandler_Init(
      &instance->_private.iceRatehandler,
      dataModel,
      &iceRateHandlerConfig,
      PersonalityParametricData_Get(dataModel)->freezerIceRateData);

   SoftPwm_Init(
      &instance->_private.fillTubeHeaterSoftPwm,
      dataModel,
      &softPwmConfig);

   FeelerArmMonitor_Init(
      &instance->_private.feelerArmMonitor,
      dataModel,
      &feelerArmMonitorConfig);

   IceMakerEnableResolver_Init(
      &instance->_private.iceMakerEnableResolver,
      dataModel,
      &iceMakerEnableResolverConfiguration);

   AluminumMoldIceMaker_Init(
      &instance->_private.aluminumMoldIceMaker,
      dataModel,
      &aluminumMoldIceMakerConfig);
}
