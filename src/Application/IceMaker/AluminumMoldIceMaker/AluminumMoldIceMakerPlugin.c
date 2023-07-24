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

static const FreezerIceRateHandlerConfig_t iceRateHandlerConfig = {
   .freezerIceRateTriggerSignalErd = Erd_FreezerIceRateTriggerSignal,
   .freezerSetpointUserVoteErd = Erd_FreezerSetpoint_UserVote,
   .freezerSetpointFreezerIceRateVoteErd = Erd_FreezerSetpoint_FreezerIceRateVote,
   .freezerEvapFanSpeedFreezerIceRateVoteErd = Erd_FreezerEvapFanSpeed_FreezerIceRateVote,
   .freezerEvapFanSpeedResolvedVoteErd = Erd_FreezerEvapFanSpeed_ResolvedVote,
   .iceRateIsActiveErd = Erd_Freezer_IceRateIsActive
};

static const AluminumMoldIceMakerConfig_t aluminumMoldIceMakerConfig = {
   .aluminumMoldIceMakerHsmStateErd = Erd_AluminumMoldIceMakerHsmState,
   .iceMakerWaterValveVoteErd = Erd_AluminumMoldIceMakerWaterValve_IceMakerVote,
   .moldHeaterVoteErd = Erd_AluminumMoldIceMakerHeaterRelay_IceMakerVote,
   .rakeMotorVoteErd = Erd_AluminumMoldIceMakerRakeMotor_IceMakerVote,
   .harvestCountCalculationRequestErd = Erd_AluminumMoldIceMaker_HarvestCountCalculationRequest,
   .feelerArmMonitoringRequestErd = Erd_FeelerArmMonitoringRequest,
   .feelerArmPositionErd = Erd_AluminumMoldIceMakerFeelerArmPosition,
   .harvestCountIsReadyToHarvestErd = Erd_AluminumMoldIceMaker_HarvestCountIsReadyToHarvest,
   .moldFilteredTemperatureInDegFx100Erd = Erd_AluminumMoldIceMaker_FilteredTemperatureResolvedInDegFx100,
   .feelerArmIsReadyToEnterHarvestErd = Erd_FeelerArmIsReadyToEnterHarvest,
   .iceMakerEnabledErd = Erd_IceMakerEnabledResolved,
   .sabbathModeErd = Erd_SabbathMode,
   .fillTubeHeaterVoteErd = Erd_FillTubeHeater_AluminumMoldIceMakerVote,
   .moldHeaterControlRequestErd = Erd_AluminumMoldIceMakerMoldHeaterControlRequest,
   .rakeCompletedRevolutionErd = Erd_AluminumMoldIceMakerRakeCompletedRevolution,
   .moldThermistorIsValidErd = Erd_AluminumMoldIceMakerMoldThermistor_IsValidResolved,
   .skipFillRequestErd = Erd_AluminumMoldIceMakerSkipFillRequest,
   .rakeControlRequestErd = Erd_AluminumMoldIceMakerRakeControlRequest,
   .isolationWaterValveVoteErd = Erd_IsolationWaterValve_AluminumMoldIceMakerVote,
   .waterFillMonitoringRequestErd = Erd_AluminumMoldIceMakerWaterFillMonitoringRequest,
   .stopIceMakerFillSignalErd = Erd_AluminumMoldIceMakerStopFillSignal,
   .rakePositionErd = Erd_AluminumMoldIceMakerRakePosition,
   .freezerIceRateTriggerSignalErd = Erd_FreezerIceRateTriggerSignal,
   .freezerIceRateIsActiveErd = Erd_Freezer_IceRateIsActive,
   .aluminumMoldIceMakerTestRequestErd = Erd_AluminumMoldIceMakerTestRequest,
   .dispensingRequestStatusErd = Erd_DispensingRequestStatus,
   .coolingSystemOffStatus = Erd_CoolingOffStatus
};

static const FeelerArmMonitorConfig_t feelerArmMonitorConfig = {
   .feelerArmMonitoringRequestErd = Erd_FeelerArmMonitoringRequest,
   .timerModuleErd = Erd_TimerModule,
   .feelerArmIsReadyToEnterHarvestErd = Erd_FeelerArmIsReadyToEnterHarvest,
   .feelerArmPositionErd = Erd_AluminumMoldIceMakerFeelerArmPosition
};

static const HarvestCountCalculatorConfiguration_t harvestCountCalculatorConfig = {
   .harvestCountIsReadyToHarvestErd = Erd_AluminumMoldIceMaker_HarvestCountIsReadyToHarvest,
   .harvestCountCalculationRequestErd = Erd_AluminumMoldIceMaker_HarvestCountCalculationRequest,
   .moldFilteredTemperatureInDegFx100Erd = Erd_AluminumMoldIceMaker_FilteredTemperatureResolvedInDegFx100,
   .moldFreezeIntegrationCountErd = Erd_AluminumMoldFreezeIntegrationCount,
   .moldIceMakerMinimumFreezeTimeCounterInMinutesErd = Erd_AluminumMoldIceMakerMinimumFreezeTimeCounterInMinutes
};

static const Erd_t enableErdsList[] = {
   Erd_IceMaker0EnableStatus, Erd_IceMakerEnabledByGrid
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

static const ResolvedVoteRelayConnectorConfiguration_t rakeMotorDriverConfig = {
   .resolvedRelayVoteErd = Erd_AluminumMoldIceMakerRakeMotor_ResolvedVote,
   .relayOutputErd = Erd_IceMakerRakeMotorRelay
};

static const IceMakerMoldHeaterControllerConfig_t iceMakerMoldHeaterControllerConfig = {
   .moldHeaterControlRequestErd = Erd_AluminumMoldIceMakerMoldHeaterControlRequest,
   .moldHeaterVoteErd = Erd_AluminumMoldIceMakerHeaterRelay_IceMakerVote,
   .moldFilteredTemperatureInDegFx100Erd = Erd_AluminumMoldIceMaker_FilteredTemperatureResolvedInDegFx100,
   .timerModuleErd = Erd_TimerModule,
};

static const SensorFilteringConfig_t moldThermistorConfig = {
   .sensorAdcCountErd = Erd_AluminumMoldIceMakerMoldThermistor_AdcCount,
   .sensorUnfilteredTemperatureInDegFx100Erd = Erd_AluminumMoldIceMakerMold_UnfilteredTemperatureInDegFx100,
   .sensorFilteredTemperatureInDegFx100Erd = Erd_AluminumMoldIceMakerMold_FilteredTemperatureInDegFx100,
   .sensorIsValidErd = Erd_AluminumMoldIceMakerMoldThermistorIsValid,
   .timerModuleErd = Erd_TimerModule
};

static const FlowMeterMonitorConfig_t flowMeterMonitorConfig = {
   .flowMeterMonitoringRequest = Erd_FlowMeterMonitoringRequest,
   .flowMeterInputCaptureCountsErd = Erd_FlowMeter_InputCaptureCount,
   .flowMeterWaterDispensedOzX100Erd = Erd_FlowMeterWaterDispensedOzX100
};

static const IceMakerWaterFillMonitorConfig_t iceMakerFillMonitorConfig = {
   .stopIceMakerFillSignalErd = Erd_AluminumMoldIceMakerStopFillSignal,
   .flowMeterWaterDispensedOzx100Erd = Erd_FlowMeterWaterDispensedOzX100,
   .waterFillMonitoringRequestErd = Erd_AluminumMoldIceMakerWaterFillMonitoringRequest,
   .flowMeterMonitoringRequestErd = Erd_FlowMeterMonitoringRequest,
   .timerModuleErd = Erd_TimerModule
};

static const ResolvedVoteRelayConnectorConfiguration_t iceMakerWaterValveRelayConnectorConfiguration = {
   .resolvedRelayVoteErd = Erd_AluminumMoldIceMakerWaterValve_ResolvedVote,
   .relayOutputErd = Erd_AluminumMoldIceMakerWaterValveRelay
};

static const Erd_t aluminumMoldIceMakerFilteredTemperatureOverrideRequestErdList[] = {
   Erd_AluminumMoldIceMaker_FilteredTemperatureOverrideRequest
};

static const Erd_t aluminumMoldIceMakerFilteredTemperatureValueErdList[] = {
   Erd_AluminumMoldIceMakerMold_FilteredTemperatureInDegFx100,
   Erd_AluminumMoldIceMaker_FilteredTemperatureOverrideValueInDegFx100
};

static const OverrideArbiterConfiguration_t aluminumMoldIceMakerFilteredTemperatureArbiterConfiguration = {
   aluminumMoldIceMakerFilteredTemperatureOverrideRequestErdList,
   aluminumMoldIceMakerFilteredTemperatureValueErdList,
   Erd_AluminumMoldIceMaker_FilteredTemperatureResolvedInDegFx100,
   NUM_ELEMENTS(aluminumMoldIceMakerFilteredTemperatureOverrideRequestErdList)
};

static const ResolvedVoteRelayConnectorConfiguration_t heaterRelayConnectorConfiguration = {
   .resolvedRelayVoteErd = Erd_AluminumMoldIceMakerHeaterRelay_ResolvedVote,
   .relayOutputErd = Erd_AluminumMoldIceMakerHeaterRelay
};

static const Erd_t aluminumMoldIceMakerThermistorValidOverrideArbiterRequestErdList[] = {
   Erd_AluminumMoldIceMakerMoldThermistor_IsValidOverrideRequest
};

static const Erd_t aluminumMoldIceMakerThermistorValidValueErdList[] = {
   Erd_AluminumMoldIceMakerMoldThermistorIsValid,
   Erd_AluminumMoldIceMakerMoldThermistor_IsValidOverrideValue
};

static const OverrideArbiterConfiguration_t aluminumMoldIceMakerThermistorValidArbiterConfiguration = {
   aluminumMoldIceMakerThermistorValidOverrideArbiterRequestErdList,
   aluminumMoldIceMakerThermistorValidValueErdList,
   Erd_AluminumMoldIceMakerMoldThermistor_IsValidResolved,
   NUM_ELEMENTS(aluminumMoldIceMakerThermistorValidOverrideArbiterRequestErdList)
};

static const Erd_t iceMakerEnabledOverrideRequestErdList[] = {
   Erd_IceMakerEnabledEnhancedSabbathOverrideRequest,
   Erd_IceMakerEnabledOverrideRequest,
};

static const Erd_t iceMakerEnabledOverrideValueErdList[] = {
   Erd_IceMaker0EnableStatus,
   Erd_IceMakerEnabledEnhancedSabbathOverrideValue,
   Erd_IceMakerEnabledOverrideValue
};

static const OverrideArbiterConfiguration_t iceMakerEnabledOverrideConfiguration = {
   iceMakerEnabledOverrideRequestErdList,
   iceMakerEnabledOverrideValueErdList,
   Erd_IceMakerEnabledResolved,
   NUM_ELEMENTS(iceMakerEnabledOverrideRequestErdList)
};

static void InitializeIceMakerOverrideArbiters(AluminumMoldIceMakerPlugin_t *instance, I_DataModel_t *dataModel)
{
   OverrideArbiter_Init(
      &instance->_private.aluminumMoldIceMakerFilteredTemperatureArbiter,
      DataModel_AsDataSource(dataModel),
      &aluminumMoldIceMakerFilteredTemperatureArbiterConfiguration);

   OverrideArbiter_Init(
      &instance->_private.aluminumMoldIceMakerThermistorValidArbiter,
      DataModel_AsDataSource(dataModel),
      &aluminumMoldIceMakerThermistorValidArbiterConfiguration);

   OverrideArbiter_Init(
      &instance->_private.iceMakerEnabledArbiter,
      DataModel_AsDataSource(dataModel),
      &iceMakerEnabledOverrideConfiguration);
}

static void InitializeErdResolvers(AluminumMoldIceMakerPlugin_t *instance, I_DataModel_t *dataModel)
{
   ErdResolver_Init(
      &instance->_private.iceMakerHeaterVoteResolver,
      DataModel_AsDataSource(dataModel),
      &iceMakerHeaterResolverConfiguration);

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
   const SensorData_t *sensorData = PersonalityParametricData_Get(dataModel)->sensorData;
   const AluminumMoldIceMakerData_t *aluminumMoldIceMakerData =
      PersonalityParametricData_Get(dataModel)->iceMakerData->aluminumMoldIceMakerData;

   DataModel_Write(
      dataModel,
      Erd_IceMaker0TypeInformation,
      &PersonalityParametricData_Get(dataModel)->iceMakerData->aluminumMoldIceMakerData->typeInformation);

   InitializeIceMakerOverrideArbiters(instance, dataModel);

   SensorFiltering_Init(
      &instance->_private.moldThermistor,
      dataModel,
      &moldThermistorConfig,
      sensorData->aluminumIceMakerMoldThermistor,
      sensorData->periodicUpdateRateInMs);

   InitializeErdResolvers(
      instance,
      dataModel);

   ResolvedVoteRelayConnector_Init(
      &instance->_private.iceMakerWaterValveRelayConnector,
      dataModel,
      &iceMakerWaterValveRelayConnectorConfiguration);

   FillTubeHeaterVotingFrameworkPlugin_Init(
      &instance->_private.fillTubeHeaterVotingFrameworkPlugin,
      dataModel);

   FreezerIceRateHandler_Init(
      &instance->_private.iceRateHandler,
      dataModel,
      &iceRateHandlerConfig,
      PersonalityParametricData_Get(dataModel)->iceMakerData->freezerIceRateData);

   FeelerArmMonitor_Init(
      &instance->_private.feelerArmMonitor,
      dataModel,
      &feelerArmMonitorConfig);

   RakeController_Init(
      &instance->_private.rakeController,
      dataModel,
      &rakeControllerConfig,
      &PersonalityParametricData_Get(dataModel)->iceMakerData->aluminumMoldIceMakerData->harvestData);

   ResolvedVoteRelayConnector_Init(
      &instance->_private.rakeMotorRelayConnector,
      dataModel,
      &rakeMotorDriverConfig);

   HarvestCountCalculator_Init(
      &instance->_private.harvestCountCalculator,
      dataModel,
      &harvestCountCalculatorConfig,
      aluminumMoldIceMakerData->freezeData.harvestCountCalculatorData);

   IceMakerEnableResolver_Init(
      &instance->_private.iceMakerEnableResolver,
      dataModel,
      &iceMakerEnableResolverConfiguration);

   IceMakerMoldHeaterController_Init(
      &instance->_private.iceMakerMoldHeaterController,
      dataModel,
      &iceMakerMoldHeaterControllerConfig,
      &PersonalityParametricData_Get(dataModel)->iceMakerData->aluminumMoldIceMakerData->harvestData);

   ResolvedVoteRelayConnector_Init(
      &instance->_private.heaterRelayConnector,
      dataModel,
      &heaterRelayConnectorConfiguration);

   FlowMeterMonitor_Init(
      &instance->_private.flowMeterMonitor,
      dataModel,
      &flowMeterMonitorConfig,
      PersonalityParametricData_Get(dataModel)->flowMeterData);

   IceMakerWaterFillMonitor_Init(
      &instance->_private.iceMakerFillMonitor,
      dataModel,
      &iceMakerFillMonitorConfig,
      PersonalityParametricData_Get(dataModel)->iceMakerData->aluminumMoldIceMakerData->fillData.iceMakerFillMonitorData);

   AluminumMoldIceMaker_Init(
      &instance->_private.aluminumMoldIceMaker,
      dataModel,
      &aluminumMoldIceMakerConfig,
      aluminumMoldIceMakerData);
}
