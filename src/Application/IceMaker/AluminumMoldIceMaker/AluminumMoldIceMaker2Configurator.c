/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "AluminumMoldIceMakerConfigurator.h"
#include "PersonalityParametricData.h"

static const FreezerIceRateHandlerConfig_t freezerIceRateHandlerConfig = {
   .freezerIceRateTriggerSignalErd = Erd_FreezerIceRateTriggerSignal,
   .freezerSetpointFreezerIceRateVoteErd = Erd_FreezerSetpoint_FreezerIceRateVote,
   .freezerEvapFanSpeedFreezerIceRateVoteErd = Erd_FreezerEvapFanSpeed_FreezerIceRateVote,
   .freezerEvapFanSpeedResolvedVoteErd = Erd_FreezerEvapFanSpeed_ResolvedVote,
   .iceRateIsActiveErd = Erd_Freezer_IceRateIsActive
};

static const AluminumMoldIceMakerConfig_t aluminumMoldIceMakerConfig = {
   .aluminumMoldIceMakerHsmStateErd = Erd_IceMaker2_StateMachineState,
   .iceMakerWaterValveVoteErd = Erd_AluminumMoldIceMakerWaterValve_IceMakerVote,
   .moldHeaterVoteErd = Erd_IceMaker0_HeaterRelay_IceMakerVote,
   .rakeMotorVoteErd = Erd_AluminumMoldIceMakerRakeMotor_IceMakerVote,
   .harvestCountCalculationRequestErd = Erd_IceMaker2_HarvestCountCalculationRequest,
   .feelerArmMonitoringRequestErd = Erd_IceMaker0_FeelerArmMonitoringRequest,
   .feelerArmPositionErd = Erd_IceMaker0_FeelerArmPosition,
   .harvestCountIsReadyToHarvestErd = Erd_IceMaker2_HarvestCountIsReadyToHarvest,
   .moldFilteredTemperatureInDegFx100Erd = Erd_IceMaker2_MoldThermistor_FilteredTemperatureInDegFx100,
   .feelerArmIsReadyToEnterHarvestErd = Erd_FeelerArmIsReadyToEnterHarvest,
   .iceMakerEnabledErd = Erd_IceMakerEnabledResolved,
   .sabbathModeErd = Erd_SabbathModeEnable,
   .fillTubeHeaterVoteErd = Erd_FillTubeHeater_AluminumMoldIceMakerVote,
   .moldHeaterControlRequestErd = Erd_IceMaker0_MoldHeaterControlRequest,
   .rakeCompletedRevolutionErd = Erd_IceMaker0_RakeCompletedRevolution,
   .moldThermistorIsValidErd = Erd_IceMaker2_MoldThermistor_IsValidResolved,
   .skipFillRequestErd = Erd_IceMaker0_SkipFillRequest,
   .rakeControlRequestErd = Erd_IceMaker0_RakeControlRequest,
   .isolationWaterValveVoteErd = Erd_IsolationWaterValve_AluminumMoldIceMakerVote,
   .waterFillMonitoringRequestErd = Erd_IceMaker2_WaterFillMonitoringRequest,
   .stopIceMakerFillSignalErd = Erd_IceMaker2_StopFillSignal,
   .rakePositionErd = Erd_IceMaker0_RakePosition,
   .freezerIceRateTriggerSignalErd = Erd_FreezerIceRateTriggerSignal,
   .freezerIceRateIsActiveErd = Erd_Freezer_IceRateIsActive,
   .aluminumMoldIceMakerTestRequestErd = Erd_IceMaker2_TestRequest,
   .dispensingRequestStatusErd = Erd_DispensingRequestStatus,
   .coolingSystemOffStatusErd = Erd_CoolingOffStatus,
   .dispensingInhibitedReasonErd = Erd_DispensingInhibitedReason,
   .iceMakerFillInhibitedReasonErd = Erd_IceMakerFillInhibitedReason
};

static const FeelerArmMonitorConfig_t feelerArmMonitorConfig = {
   .feelerArmMonitoringRequestErd = Erd_IceMaker0_FeelerArmMonitoringRequest,
   .timerModuleErd = Erd_TimerModule,
   .feelerArmIsReadyToEnterHarvestErd = Erd_FeelerArmIsReadyToEnterHarvest,
   .feelerArmPositionErd = Erd_IceMaker0_FeelerArmPosition
};

static const HarvestCountCalculatorConfiguration_t harvestCountCalculatorConfig = {
   .harvestCountIsReadyToHarvestErd = Erd_IceMaker2_HarvestCountIsReadyToHarvest,
   .harvestCountCalculationRequestErd = Erd_IceMaker2_HarvestCountCalculationRequest,
   .moldFilteredTemperatureInDegFx100Erd = Erd_IceMaker2_MoldThermistor_FilteredTemperatureResolvedInDegFx100,
   .moldFreezeIntegrationCountErd = Erd_IceMaker2_FreezeIntegrationCount,
   .moldIceMakerMinimumFreezeTimeCounterInMinutesErd = Erd_IceMaker2_MinimumFreezeTimeCounterInMinutes
};

static const Erd_t enableErdsList[] = {
   Erd_IceMaker2_EnableStatus, Erd_IceMakerEnabledByGrid
};

static const ErdLogicServiceConfigurationEntry_t configurationEntries[] = {
   { ErdLogicServiceOperator_And, { enableErdsList, NUM_ELEMENTS(enableErdsList) }, Erd_IceMakerEnabledResolved },
};

static const ErdLogicServiceConfiguration_t iceMakerEnableResolverConfig = {
   configurationEntries,
   NUM_ELEMENTS(configurationEntries)
};

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

static bool HeaterVotingErdCareDelegate(const void *votingErdData)
{
   const HeaterVotedState_t *data = votingErdData;
   return (data->care);
}

static bool RakeMotorVotingErdCareDelegate(const void *votingErdData)
{
   const AluminumMoldIceMakerMotorVotedState_t *data = votingErdData;
   return (data->care);
}

static const AluminumMoldIceMakerMotorVotedState_t defaultRakeMotorData = {
   .state = MotorState_Off,
   .care = Vote_DontCare
};

static const ErdResolverConfiguration_t heaterResolverConfig = {
   .votingErdCare = HeaterVotingErdCareDelegate,
   .defaultData = &defaultHeaterData,
   .winningVoterErd = Erd_IceMaker0_HeaterRelay_WinningVoteErd,
   .resolvedStateErd = Erd_IceMaker0_HeaterRelay_ResolvedVote,
   .numberOfVotingErds = (Erd_IceMaker0_HeaterRelay_IceMakerVote - Erd_IceMaker0_HeaterRelay_WinningVoteErd)
};

static const ErdResolverConfiguration_t waterValveResolverConfig = {
   .votingErdCare = WaterValveVotingErdCareDelegate,
   .defaultData = &defaultWaterValveData,
   .winningVoterErd = Erd_AluminumMoldIceMakerWaterValve_WinningVoteErd,
   .resolvedStateErd = Erd_AluminumMoldIceMakerWaterValve_ResolvedVote,
   .numberOfVotingErds = (Erd_AluminumMoldIceMakerWaterValve_IceMakerVote - Erd_AluminumMoldIceMakerWaterValve_WinningVoteErd)
};

static const ErdResolverConfiguration_t rakeMotorResolverConfig = {
   .votingErdCare = RakeMotorVotingErdCareDelegate,
   .defaultData = &defaultRakeMotorData,
   .winningVoterErd = Erd_AluminumMoldIceMakerRakeMotor_WinningVoteErd,
   .resolvedStateErd = Erd_AluminumMoldIceMakerRakeMotor_ResolvedVote,
   .numberOfVotingErds = (Erd_AluminumMoldIceMakerRakeMotor_IceMakerVote - Erd_AluminumMoldIceMakerRakeMotor_WinningVoteErd)
};

static const RakeControllerConfig_t rakeControllerConfig = {
   .rakeControlRequestErd = Erd_IceMaker0_RakeControlRequest,
   .rakeMotorVoteErd = Erd_AluminumMoldIceMakerRakeMotor_IceMakerVote,
   .rakeCompletedRevolutionErd = Erd_IceMaker0_RakeCompletedRevolution,
   .timerModuleErd = Erd_TimerModule,
   .rakePositionErd = Erd_IceMaker0_RakePosition,
   .feelerArmPositionErd = Erd_IceMaker0_FeelerArmPosition,
   .rakePositionHasNotBeenHomeErd = Erd_IceMaker0_RakeHasNotBeenHome,
   .feelerArmPositionHasBeenBucketFullErd = Erd_IceMaker0_FeelerArmHasBeenBucketFull
};

static const AluminumMoldIceMakerFullStatusUpdaterConfig_t iceMakerFullStatusUpdaterConfig = {
   .feelerArmPositionErd = Erd_IceMaker0_FeelerArmPosition,
   .aluminumMoldIceMakerHsmStateErd = Erd_IceMaker2_StateMachineState,
   .iceMakerFullStatusErd = Erd_IceMaker2_FullStatus
};

static const ResolvedVoteRelayConnectorConfiguration_t rakeMotorDriverConfig = {
   .resolvedRelayVoteErd = Erd_AluminumMoldIceMakerRakeMotor_ResolvedVote,
   .relayOutputErd = Erd_IceMaker0_RakeMotorRelay
};

static const IceMakerMoldHeaterControllerConfig_t heaterControllerConfig = {
   .moldHeaterControlRequestErd = Erd_IceMaker0_MoldHeaterControlRequest,
   .moldHeaterVoteErd = Erd_IceMaker0_HeaterRelay_IceMakerVote,
   .moldFilteredTemperatureInDegFx100Erd = Erd_IceMaker2_MoldThermistor_FilteredTemperatureResolvedInDegFx100,
   .timerModuleErd = Erd_TimerModule,
};

static const SensorFilteringConfig_t moldThermistorConfig = {
   .sensorAdcCountErd = Erd_IceMaker2_MoldThermistor_AdcCount,
   .sensorUnfilteredTemperatureInDegFx100Erd = Erd_IceMaker2_MoldThermistor_UnfilteredTemperatureInDegFx100,
   .sensorFilteredTemperatureInDegFx100Erd = Erd_IceMaker2_MoldThermistor_FilteredTemperatureInDegFx100,
   .sensorIsValidErd = Erd_IceMaker2_MoldThermistorIsValid,
   .sensorIsInvalidFaultErd = Erd_IceMaker2ThermistorIsInvalidFault,
   .sensorDiscoveredErd = Erd_IceMaker2_HasBeenDiscovered,
   .timerModuleErd = Erd_TimerModule
};

static const IceMakerWaterFillMonitorConfig_t fillMonitorConfig = {
   .stopIceMakerFillSignalErd = Erd_IceMaker2_StopFillSignal,
   .flowMeterWaterDispensedOzx100Erd = Erd_IceMaker2_FlowMeterWaterDispensedOzX100,
   .waterFillMonitoringRequestErd = Erd_IceMaker2_WaterFillMonitoringRequest,
   .flowMeterMonitoringRequestErd = Erd_IceMaker2_FlowMeterMonitoringRequest,
   .timerModuleErd = Erd_TimerModule
};

static const ResolvedVoteRelayConnectorConfiguration_t waterValveRelayConnectorConfig = {
   .resolvedRelayVoteErd = Erd_AluminumMoldIceMakerWaterValve_ResolvedVote,
   .relayOutputErd = Erd_AluminumMoldIceMakerWaterValveRelay
};

static const Erd_t filteredTemperatureOverrideRequestErdList[] = {
   Erd_IceMaker2_MoldThermistor_FilteredTemperatureOverrideRequest
};

static const Erd_t filteredTemperatureValueErdList[] = {
   Erd_IceMaker2_MoldThermistor_FilteredTemperatureInDegFx100,
   Erd_IceMaker2_MoldThermistor_FilteredTemperatureOverrideValueInDegFx100
};

static const OverrideArbiterConfiguration_t filteredTemperatureArbiterConfig = {
   filteredTemperatureOverrideRequestErdList,
   filteredTemperatureValueErdList,
   Erd_IceMaker2_MoldThermistor_FilteredTemperatureResolvedInDegFx100,
   NUM_ELEMENTS(filteredTemperatureOverrideRequestErdList)
};

static const ResolvedVoteRelayConnectorConfiguration_t heaterRelayConnectorConfig = {
   .resolvedRelayVoteErd = Erd_IceMaker0_HeaterRelay_ResolvedVote,
   .relayOutputErd = Erd_IceMaker0_HeaterRelay
};

static const Erd_t thermistorValidOverrideArbiterRequestErdList[] = {
   Erd_IceMaker2_MoldThermistor_IsValidOverrideRequest
};

static const Erd_t thermistorValidValueErdList[] = {
   Erd_IceMaker2_MoldThermistorIsValid,
   Erd_IceMaker2_MoldThermistor_IsValidOverrideValue
};

static const OverrideArbiterConfiguration_t thermistorValidArbiterConfig = {
   thermistorValidOverrideArbiterRequestErdList,
   thermistorValidValueErdList,
   Erd_IceMaker2_MoldThermistor_IsValidResolved,
   NUM_ELEMENTS(thermistorValidOverrideArbiterRequestErdList)
};

static const Erd_t iceMakerEnabledOverrideRequestErdList[] = {
   Erd_IceMakerEnabledEnhancedSabbathOverrideRequest,
   Erd_IceMakerEnabledOverrideRequest,
};

static const Erd_t iceMakerEnabledOverrideValueErdList[] = {
   Erd_IceMaker2_EnableStatus,
   Erd_IceMakerEnabledEnhancedSabbathOverrideValue,
   Erd_IceMakerEnabledOverrideValue
};

static const OverrideArbiterConfiguration_t iceMakerEnabledOverrideConfig = {
   iceMakerEnabledOverrideRequestErdList,
   iceMakerEnabledOverrideValueErdList,
   Erd_IceMakerEnabledResolved,
   NUM_ELEMENTS(iceMakerEnabledOverrideRequestErdList)
};

static const AluminumMoldIceMakerPluginConfig_t config = {
   .freezerIceRateHandlerConfig = &freezerIceRateHandlerConfig,
   .aluminumMoldIceMakerConfig = &aluminumMoldIceMakerConfig,
   .feelerArmMonitorConfig = &feelerArmMonitorConfig,
   .harvestCountCalculatorConfig = &harvestCountCalculatorConfig,
   .iceMakerEnableResolverConfig = &iceMakerEnableResolverConfig,
   .heaterResolverConfig = &heaterResolverConfig,
   .rakeMotorResolverConfig = &rakeMotorResolverConfig,
   .waterValveResolverConfig = &waterValveResolverConfig,
   .rakeControllerConfig = &rakeControllerConfig,
   .iceMakerFullStatusUpdaterConfig = &iceMakerFullStatusUpdaterConfig,
   .rakeMotorDriverConfig = &rakeMotorDriverConfig,
   .heaterControllerConfig = &heaterControllerConfig,
   .moldThermistorConfig = &moldThermistorConfig,
   .fillMonitorConfig = &fillMonitorConfig,
   .waterValveRelayConnectorConfig = &waterValveRelayConnectorConfig,
   .filteredTemperatureArbiterConfig = &filteredTemperatureArbiterConfig,
   .heaterRelayConnectorConfig = &heaterRelayConnectorConfig,
   .thermistorValidArbiterConfig = &thermistorValidArbiterConfig,
   .iceMakerEnabledOverrideConfig = &iceMakerEnabledOverrideConfig
};

void AluminumMoldIceMaker2Configurator_Init(
   AluminumMoldIceMakerConfigurator_t *instance,
   I_DataModel_t *dataModel)
{
   const IceMakerSlotData_t *slotData = PersonalityParametricData_Get(dataModel)->iceMakerData->iceMakerSlots->slot2Data;
   const SensorDataSensorType_t *sensorData = PersonalityParametricData_Get(dataModel)->sensorData->iceMaker2MoldThermistor;

   AluminumMoldIceMakerPlugin_Init(
      &instance->_private.plugin,
      dataModel,
      slotData->location,
      slotData->aluminumMoldData,
      sensorData,
      &PersonalityParametricData_Get(dataModel)->sensorData->periodicUpdateRateInMs,
      slotData->freezerIceRateData,
      slotData->nonHarvestFillTubeHeaterData,
      &config);
}
