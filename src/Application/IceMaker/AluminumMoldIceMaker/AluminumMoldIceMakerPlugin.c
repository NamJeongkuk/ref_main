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

static const AluminumMoldIceMakerConfig_t aluminumMoldIceMakerConfig = {
   .aluminumMoldIceMakerHsmStateErd = Erd_AluminumMoldIceMakerHsmState,
   .waterValveVoteErd = Erd_AluminumMoldIceMakerWaterValve_IceMakerVote,
   .iceMakerHeaterVoteErd = Erd_AluminumMoldIceMakerHeaterRelay_IceMakerVote,
   .iceMakerMotorVoteErd = Erd_AluminumMoldIceMakerMotor_IceMakerVote,
   .harvestCountCalculationRequestErd = Erd_HarvestCountCalculationRequest,
   .feelerArmMonitoringRequestErd = Erd_FeelerArmMonitoringRequest,
   .harvestCountIsReadyToHarvestErd = Erd_HarvestCountIsReadyToHarvest,
   .iceMakerFilteredTemperatureInDegFx100Erd = Erd_AluminumMoldIceMaker_FilteredTemperatureResolvedInDegFx100,
   .feelerArmIsReadyToEnterHarvestErd = Erd_FeelerArmIsReadyToEnterHarvest,
   .iceMakerEnabledErd = Erd_IceMakerEnabledOverrideResolved,
   .sabbathModeErd = Erd_SabbathMode,
   .fillTubeHeaterVoteErd = Erd_AluminumMoldIceMakerFillTubeHeater_IceMakerVote,
   .moldHeaterControlRequestErd = Erd_AluminumMoldIceMakerMoldHeaterControlRequest,
   .rakeCompletedRevolutionErd = Erd_AluminumMoldIceMakerRakeCompletedRevolution,
   .moldThermistorIsValidErd = Erd_AluminumMoldIceMakerThermistorIsValidResolved,
   .skipFillRequestErd = Erd_AluminumMoldIceMakerSkipFillRequest,
   .rakeControlRequestErd = Erd_AluminumMoldIceMakerRakeControlRequest
};

static const FeelerArmMonitorConfig_t feelerArmMonitorConfig = {
   .feelerArmMonitoringRequestErd = Erd_FeelerArmMonitoringRequest,
   .timerModuleErd = Erd_TimerModule,
   .feelerArmIsReadyToEnterHarvestErd = Erd_FeelerArmIsReadyToEnterHarvest,
   .feelerArmPositionErd = Erd_AluminumMoldIceMakerFeelerArmPosition
};

static const HarvestCountCalculatorConfiguration_t harvestCountCalculatorConfig = {
   .harvestCountIsReadyToHarvestErd = Erd_HarvestCountIsReadyToHarvest,
   .harvestCountCalculationRequestErd = Erd_HarvestCountCalculationRequest,
   .iceMakerFilteredTemperatureInDegFx100Erd = Erd_AluminumMoldIceMaker_FilteredTemperatureResolvedInDegFx100
};

static const Erd_t enableErdsList[] = {
   Erd_IceMakerEnabledByUser, Erd_IceMakerEnabledByGrid, Erd_IceMakerEnabledByDemandResponse
};

static const ErdLogicServiceConfigurationEntry_t configurationEntries[] = {
   { ErdLogicServiceOperator_And, { enableErdsList, NUM_ELEMENTS(enableErdsList) }, Erd_IceMakerEnabledResolved },
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
   const AluminumMoldIceMakerMotorVotedState_t *data = votingErdData;
   return (data->care);
}

static const AluminumMoldIceMakerMotorVotedState_t defaultMotorData = {
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

static bool RakeMotorVotingErdCareDelegate(const void *votingErdData)
{
   const AluminumMoldIceMakerMotorVotedState_t *data = votingErdData;
   return (data->care);
}

static const AluminumMoldIceMakerMotorVotedState_t defaultRakeMotorData = {
   .state = MotorState_Off,
   .care = Vote_DontCare
};

static const ErdResolverConfiguration_t iceMakerHeaterResolverConfiguration = {
   .votingErdCare = HeaterVotingErdCareDelegate,
   .defaultData = &defaultHeaterData,
   .winningVoterErd = Erd_AluminumMoldIceMakerHeaterRelay_WinningVoteErd,
   .resolvedStateErd = Erd_AluminumMoldIceMakerHeaterRelay_ResolvedVote,
   .numberOfVotingErds = (Erd_AluminumMoldIceMakerHeaterRelay_IceMakerVote - Erd_AluminumMoldIceMakerHeaterRelay_WinningVoteErd)
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

static const ErdResolverConfiguration_t iceMakerRakeMotorResolverConfiguration = {
   .votingErdCare = RakeMotorVotingErdCareDelegate,
   .defaultData = &defaultRakeMotorData,
   .winningVoterErd = Erd_AluminumMoldIceMakerRakeMotor_WinningVoteErd,
   .resolvedStateErd = Erd_AluminumMoldIceMakerRakeMotor_ResolvedVote,
   .numberOfVotingErds = (Erd_AluminumMoldIceMakerRakeMotor_IceMakerVote - Erd_AluminumMoldIceMakerRakeMotor_WinningVoteErd)
};

static const RakeControllerConfig_t rakeControllerConfig = {
   .rakeControlRequestErd = Erd_AluminumMoldIceMakerRakeControlRequest,
   .rakeMotorVoteErd = Erd_AluminumMoldIceMakerRakeMotor_IceMakerVote,
   .rakeCompletedRevolutionErd = Erd_AluminumMoldIceMakerRakeCompletedRevolution,
   .timerModuleErd = Erd_TimerModule,
   .rakePositionErd = Erd_AluminumMoldIceMakerRakePosition,
   .feelerArmPositionErd = Erd_AluminumMoldIceMakerFeelerArmPosition,
   .rakePositionHasNotBeenHomeErd = Erd_AluminumMoldIceMakerRakeHasNotBeenHome,
   .feelerArmPositionHasBeenBucketFullErd = Erd_AluminumMoldIceMakerFeelerArmHasBeenBucketFull
};

static const IceMakerMoldHeaterControllerConfig_t iceMakerMoldHeaterControllerConfig = {
   .iceMakerMoldHeaterControlRequestErd = Erd_AluminumMoldIceMakerMoldHeaterControlRequest,
   .iceMakerMoldHeaterHarvestVoteErd = Erd_AluminumMoldIceMakerHeaterRelay_IceMakerVote,
   .iceMakerMoldFilteredTemperatureErd = Erd_AluminumMoldIceMaker_FilteredTemperatureResolvedInDegFx100,
   .timerModuleErd = Erd_TimerModule,
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

   ErdResolver_Init(
      &instance->_private.iceMakerRakeMotorVoteResolver,
      DataModel_AsDataSource(dataModel),
      &iceMakerRakeMotorResolverConfiguration);
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
      &instance->_private.iceRateHandler,
      dataModel,
      &iceRateHandlerConfig,
      PersonalityParametricData_Get(dataModel)->freezerIceRateData);

   FeelerArmMonitor_Init(
      &instance->_private.feelerArmMonitor,
      dataModel,
      &feelerArmMonitorConfig);

   RakeController_Init(
      &instance->_private.rakeController,
      dataModel,
      &rakeControllerConfig,
      &PersonalityParametricData_Get(dataModel)->aluminumMoldIceMakerData->harvestData);

   HarvestCountCalculator_Init(
      &instance->_private.harvestCountCalculator,
      dataModel,
      &harvestCountCalculatorConfig,
      PersonalityParametricData_Get(dataModel)->aluminumMoldIceMakerData);

   IceMakerEnableResolver_Init(
      &instance->_private.iceMakerEnableResolver,
      dataModel,
      &iceMakerEnableResolverConfiguration);

   IceMakerMoldHeaterController_Init(
      &instance->_private.iceMakerMoldHeaterController,
      dataModel,
      &iceMakerMoldHeaterControllerConfig,
      &PersonalityParametricData_Get(dataModel)->aluminumMoldIceMakerData->harvestData);

   AluminumMoldIceMaker_Init(
      &instance->_private.aluminumMoldIceMaker,
      dataModel,
      &aluminumMoldIceMakerConfig);
}
