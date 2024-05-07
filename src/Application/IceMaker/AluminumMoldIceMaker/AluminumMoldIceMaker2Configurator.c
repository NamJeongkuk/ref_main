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
   .freezerIceRateIsActiveErd = Erd_Freezer_IceRateIsActive
};

static const AluminumMoldIceMakerConfig_t aluminumMoldIceMakerConfig = {
   .aluminumMoldIceMakerHsmStateErd = Erd_IceMaker2_StateMachineState,
   .iceMakerWaterValveVoteErd = Erd_IceMaker2_WaterValve_IceMakerVote,
   .moldHeaterVoteErd = Erd_IceMaker2_HeaterRelay_IceMakerVote,
   .rakeMotorVoteErd = Erd_IceMaker2_RakeMotor_IceMakerVote,
   .harvestCountCalculationRequestErd = Erd_IceMaker2_HarvestCountCalculationRequest,
   .feelerArmMonitoringRequestErd = Erd_IceMaker2_FeelerArmMonitoringRequest,
   .feelerArmPositionErd = Erd_IceMaker2_FeelerArmPosition,
   .harvestCountIsReadyToHarvestErd = Erd_IceMaker2_HarvestCountIsReadyToHarvest,
   .moldFilteredTemperatureInDegFx100Erd = Erd_IceMaker2_MoldThermistor_FilteredTemperatureInDegFx100,
   .feelerArmIsReadyToEnterHarvestErd = Erd_IceMaker2_FeelerArmIsReadyToEnterHarvest,
   .iceMakerEnabledErd = Erd_IceMaker2_EnabledResolved,
   .sabbathModeErd = Erd_SabbathModeEnable,
   .enhancedSabbathModeErd = Erd_EnhancedSabbathModeEnable,
   .fillTubeHeaterVoteErd = Erd_IceMaker2_FillTubeHeater_IceMakerVote,
   .moldHeaterControlRequestErd = Erd_IceMaker2_MoldHeaterControlRequest,
   .rakeCompletedRevolutionErd = Erd_IceMaker2_RakeCompletedRevolution,
   .moldThermistorIsValidErd = Erd_IceMaker2_MoldThermistor_IsValidResolved,
   .skipFillRequestErd = Erd_IceMaker2_SkipFillRequest,
   .rakeControlRequestErd = Erd_IceMaker2_RakeControlRequest,
   .isolationWaterValveVoteErd = Erd_IsolationWaterValve_IceMaker2Vote,
   .waterFillMonitoringRequestErd = Erd_IceMaker2_WaterFillMonitoringRequest,
   .stopIceMakerFillSignalErd = Erd_IceMaker2_StopFillSignal,
   .rakePositionErd = Erd_IceMaker2_RakePosition,
   .freezerIceRateTriggerSignalErd = Erd_FreezerIceRateTriggerSignal,
   .freezerIceRateIsActiveErd = Erd_Freezer_IceRateIsActive,
   .aluminumMoldIceMakerTestRequestErd = Erd_IceMaker2_TestRequest,
   .dispensingRequestStatusErd = Erd_DispensingRequestStatus,
   .coolingSystemOffStatusErd = Erd_CoolingOffStatus,
   .dispensingInhibitedReasonErd = Erd_DispensingInhibitedReason,
   .iceMakerFillInhibitedReasonErd = Erd_IceMakerFillInhibitedReason
};

static const FeelerArmMonitorConfig_t feelerArmMonitorConfig = {
   .feelerArmMonitoringRequestErd = Erd_IceMaker2_FeelerArmMonitoringRequest,
   .timerModuleErd = Erd_TimerModule,
   .feelerArmIsReadyToEnterHarvestErd = Erd_IceMaker2_FeelerArmIsReadyToEnterHarvest,
   .feelerArmPositionErd = Erd_IceMaker2_FeelerArmPosition
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
   { ErdLogicServiceOperator_And, { enableErdsList, NUM_ELEMENTS(enableErdsList) }, Erd_IceMaker2_EnabledResolved },
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
   const IceMakerMotorVotedState_t *data = votingErdData;
   return (data->care);
}

static const IceMakerMotorVotedState_t defaultRakeMotorData = {
   .state = IceMakerMotorState_Off,
   .care = Vote_DontCare
};

static const ErdResolverConfiguration_t heaterResolverConfig = {
   .votingErdCare = HeaterVotingErdCareDelegate,
   .defaultData = &defaultHeaterData,
   .winningVoterErd = Erd_IceMaker2_HeaterRelay_WinningVoteErd,
   .resolvedStateErd = Erd_IceMaker2_HeaterRelay_ResolvedVote,
   .numberOfVotingErds = (Erd_IceMaker2_HeaterRelay_IceMakerVote - Erd_IceMaker2_HeaterRelay_WinningVoteErd)
};

static const ErdResolverConfiguration_t waterValveResolverConfig = {
   .votingErdCare = WaterValveVotingErdCareDelegate,
   .defaultData = &defaultWaterValveData,
   .winningVoterErd = Erd_IceMaker2_WaterValve_WinningVoteErd,
   .resolvedStateErd = Erd_IceMaker2_WaterValve_ResolvedVote,
   .numberOfVotingErds = (Erd_IceMaker2_WaterValve_IceMakerVote - Erd_IceMaker2_WaterValve_WinningVoteErd)
};

static const ErdResolverConfiguration_t rakeMotorResolverConfig = {
   .votingErdCare = RakeMotorVotingErdCareDelegate,
   .defaultData = &defaultRakeMotorData,
   .winningVoterErd = Erd_IceMaker2_RakeMotor_WinningVoteErd,
   .resolvedStateErd = Erd_IceMaker2_RakeMotor_ResolvedVote,
   .numberOfVotingErds = (Erd_IceMaker2_RakeMotor_IceMakerVote - Erd_IceMaker2_RakeMotor_WinningVoteErd)
};

static bool VotingErdCareDelegate(const void *votingErdData)
{
   const PercentageDutyCycleVote_t *data = votingErdData;
   return (data->care);
}

static const PercentageDutyCycleVote_t defaultFillTubeHeaterData = {
   .percentageDutyCycle = 0,
   .care = false
};

static const ErdResolverConfiguration_t fillTubeHeaterResolverConfig = {
   .votingErdCare = VotingErdCareDelegate,
   .defaultData = &defaultFillTubeHeaterData,
   .winningVoterErd = Erd_IceMaker2_FillTubeHeater_WinningVoteErd,
   .resolvedStateErd = Erd_IceMaker2_FillTubeHeater_ResolvedVote,
   .numberOfVotingErds = (Erd_IceMaker2_FillTubeHeater_NonHarvestVote - Erd_IceMaker2_FillTubeHeater_WinningVoteErd)
};

static const PercentageDutyCycleVoteToPwmDutyCycleConverterConfig_t fillTubeHeaterDutyCycleToPercentageCalculatorConfig = {
   .inputPercentageDutyCycleVoteErd = Erd_IceMaker2_FillTubeHeater_ResolvedVote,
   .outputPwmDutyCycleErd = Erd_IceMaker2_FillTubeHeater_Pwm
};

static const NonHarvestFillTubeHeaterControlConfig_t nonHarvestFillTubeHeaterControlConfig = {
   .iceMakingActiveErd = Erd_Freezer_IceRateIsActive,
   .cabinetTemperatureErd = Erd_Freezer_FilteredTemperatureResolvedInDegFx100,
   .compressorIsOnErd = Erd_CompressorIsOn,
   .fanResolvedVoteErd = Erd_FreezerEvapFanSpeed_ResolvedVote,
   .nonHarvestFillTubeHeaterVoteErd = Erd_IceMaker2_FillTubeHeater_NonHarvestVote
};

static const RakeControllerConfig_t rakeControllerConfig = {
   .rakeControlRequestErd = Erd_IceMaker2_RakeControlRequest,
   .rakeMotorVoteErd = Erd_IceMaker2_RakeMotor_IceMakerVote,
   .rakeCompletedRevolutionErd = Erd_IceMaker2_RakeCompletedRevolution,
   .timerModuleErd = Erd_TimerModule,
   .rakePositionErd = Erd_IceMaker2_RakePosition,
   .feelerArmPositionErd = Erd_IceMaker2_FeelerArmPosition,
   .rakePositionHasNotBeenHomeErd = Erd_IceMaker2_RakeHasNotBeenHome,
   .feelerArmPositionHasBeenBucketFullErd = Erd_IceMaker2_FeelerArmHasBeenBucketFull
};

static const AluminumMoldIceMakerFullStatusUpdaterConfig_t iceMakerFullStatusUpdaterConfig = {
   .feelerArmPositionErd = Erd_IceMaker2_FeelerArmPosition,
   .aluminumMoldIceMakerHsmStateErd = Erd_IceMaker2_StateMachineState,
   .iceMakerFullStatusErd = Erd_IceMaker2_FullStatus
};

static const ResolvedVoteRelayConnectorConfiguration_t rakeMotorDriverConfig = {
   .resolvedRelayVoteErd = Erd_IceMaker2_RakeMotor_ResolvedVote,
   .relayOutputErd = Erd_IceMaker2_RakeMotorRelay
};

static const IceMakerMoldHeaterControllerConfig_t heaterControllerConfig = {
   .moldHeaterControlRequestErd = Erd_IceMaker2_MoldHeaterControlRequest,
   .moldHeaterVoteErd = Erd_IceMaker2_HeaterRelay_IceMakerVote,
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
   .resolvedRelayVoteErd = Erd_IceMaker2_WaterValve_ResolvedVote,
   .relayOutputErd = Erd_IceMaker2_WaterValveRelay
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
   .resolvedRelayVoteErd = Erd_IceMaker2_HeaterRelay_ResolvedVote,
   .relayOutputErd = Erd_IceMaker2_HeaterRelay
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

static const IceCabinetFanHarvestFixVotingConfig_t iceCabinetFanHarvestFixVotingConfig = {
   .iceMakerStateErd = Erd_IceMaker2_StateMachineState,
   .iceCabinetFanHarvestFixVoteErd = Erd_IceCabinetFanSpeed_HarvestFixVote
};

static const AluminumMoldIceMakerPluginConfig_t config = {
   .freezerIceRateHandlerConfig = &freezerIceRateHandlerConfig,
   .aluminumMoldIceMakerConfig = &aluminumMoldIceMakerConfig,
   .feelerArmMonitorConfig = &feelerArmMonitorConfig,
   .harvestCountCalculatorConfig = &harvestCountCalculatorConfig,
   .iceMakerEnableResolverConfig = &iceMakerEnableResolverConfig,
   .heaterResolverConfig = &heaterResolverConfig,
   .waterValveResolverConfig = &waterValveResolverConfig,
   .rakeMotorResolverConfig = &rakeMotorResolverConfig,
   .fillTubeHeaterResolverConfig = &fillTubeHeaterResolverConfig,
   .fillTubeHeaterDutyCycleToPercentageCalculatorConfig = &fillTubeHeaterDutyCycleToPercentageCalculatorConfig,
   .nonHarvestFillTubeHeaterControlConfig = &nonHarvestFillTubeHeaterControlConfig,
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
   .iceCabinetFanHarvestFixVotingConfig = &iceCabinetFanHarvestFixVotingConfig
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
