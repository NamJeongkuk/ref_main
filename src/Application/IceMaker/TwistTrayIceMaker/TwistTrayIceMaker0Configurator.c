/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "TwistTrayIceMakerConfigurator.h"
#include "SystemErds.h"
#include "DataSource_Gpio.h"

static const TwistTrayIceMakerMotorRequestManagerConfig_t motorRequestManagerConfig = {
   .resolvedVoteErd = Erd_IceMaker0_TwistMotor_ResolvedVote,
   .motorRequestErd = Erd_IceMaker0_TwistMotorControlRequest,
   .motorEnableErd = Erd_TwistIceMakerMotorDriveEnable,
   .motorActionResultErd = Erd_IceMaker0_MotorActionResult,
   .motorRequestedStateErd = Erd_IceMaker0_MotorRequestedState
};

static const TwistTrayIceMakerMotorSwitchMonitorConfig_t motorSwitchMonitorConfig = {
   .switchInputErd = Erd_IceMaker0_TwistMotorSwitchState
};

static const FreezerIceRateHandlerConfig_t freezerIceRateHandlerConfig = {
   .freezerIceRateTriggerSignalErd = Erd_FreezerIceRateTriggerSignal,
   .freezerSetpointFreezerIceRateVoteErd = Erd_FreezerSetpoint_FreezerIceRateVote,
   .freezerEvapFanSpeedFreezerIceRateVoteErd = Erd_FreezerEvapFanSpeed_FreezerIceRateVote,
   .freezerEvapFanSpeedResolvedVoteErd = Erd_FreezerEvapFanSpeed_ResolvedVote,
   .freezerIceRateIsActiveErd = Erd_Freezer_IceRateIsActive
};

static const SensorFilteringConfig_t sensorFilterConfig = {
   .sensorAdcCountErd = Erd_IceMaker0_MoldThermistor_AdcCount,
   .sensorUnfilteredTemperatureInDegFx100Erd = Erd_IceMaker0_MoldThermistor_UnfilteredTemperatureInDegFx100,
   .sensorFilteredTemperatureInDegFx100Erd = Erd_IceMaker0_MoldThermistor_FilteredTemperatureInDegFx100,
   .sensorIsValidErd = Erd_IceMaker0_MoldThermistorIsValid,
   .sensorIsInvalidFaultErd = Erd_IceMaker0ThermistorIsInvalidFault,
   .sensorDiscoveredErd = Erd_IceMaker0_HasBeenDiscovered,
   .timerModuleErd = Erd_TimerModule
};

static const Erd_t thermistorValidOverrideArbiterRequestErdList[] = {
   Erd_IceMaker0_MoldThermistor_IsValidOverrideRequest
};

static const Erd_t thermistorValidValueErdList[] = {
   Erd_IceMaker0_MoldThermistorIsValid,
   Erd_IceMaker0_MoldThermistor_IsValidOverrideValue
};

static const OverrideArbiterConfiguration_t thermistorValidArbiterConfig = {
   thermistorValidOverrideArbiterRequestErdList,
   thermistorValidValueErdList,
   Erd_IceMaker0_MoldThermistor_IsValidResolved,
   NUM_ELEMENTS(thermistorValidOverrideArbiterRequestErdList)
};

static bool TwistMotorVotingErdCareDelegate(const void *votingErdData)
{
   const IceMakerMotorVotedState_t *data = votingErdData;
   return (data->care);
}

static const IceMakerMotorVotedState_t defaultMotorData = {
   .state = IceMakerMotorState_Off,
   .care = Vote_DontCare
};

static const ErdResolverConfiguration_t motorResolverConfiguration = {
   .votingErdCare = TwistMotorVotingErdCareDelegate,
   .defaultData = &defaultMotorData,
   .winningVoterErd = Erd_IceMaker0_TwistMotor_WinningVoteErd,
   .resolvedStateErd = Erd_IceMaker0_TwistMotor_ResolvedVote,
   .numberOfVotingErds = (Erd_IceMaker0_TwistMotor_IceMakerVote - Erd_IceMaker0_TwistMotor_WinningVoteErd)
};

static const IceMakerWaterFillMonitorConfig_t fillMonitorConfig = {
   .stopIceMakerFillSignalErd = Erd_IceMaker0_StopFillSignal,
   .flowMeterWaterDispensedOzx100Erd = Erd_IceMaker0_FlowMeterWaterDispensedOzX100,
   .waterFillMonitoringRequestErd = Erd_IceMaker0_WaterFillMonitoringRequest,
   .flowMeterMonitoringRequestErd = Erd_IceMaker0_FlowMeterMonitoringRequest,
   .timerModuleErd = Erd_TimerModule
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
   .winningVoterErd = Erd_IceMaker0_FillTubeHeater_WinningVoteErd,
   .resolvedStateErd = Erd_IceMaker0_FillTubeHeater_ResolvedVote,
   .numberOfVotingErds = (Erd_IceMaker0_FillTubeHeater_NonHarvestVote - Erd_IceMaker0_FillTubeHeater_WinningVoteErd)
};

static const PercentageDutyCycleVoteToPwmDutyCycleConverterConfig_t fillTubeHeaterDutyCycleToPercentageCalculatorConfig = {
   .inputPercentageDutyCycleVoteErd = Erd_IceMaker0_FillTubeHeater_ResolvedVote,
   .outputPwmDutyCycleErd = Erd_IceMaker0_FillTubeHeater_Pwm
};

static const NonHarvestFillTubeHeaterControlConfig_t nonHarvestFillTubeHeaterControlConfig = {
   .iceMakingActiveErd = Erd_Freezer_IceRateIsActive,
   .cabinetTemperatureErd = Erd_Freezer_FilteredTemperatureResolvedInDegFx100,
   .compressorIsOnErd = Erd_CompressorIsOn,
   .fanResolvedVoteErd = Erd_FreezerEvapFanSpeed_ResolvedVote,
   .nonHarvestFillTubeHeaterVoteErd = Erd_IceMaker0_FillTubeHeater_NonHarvestVote
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

static const ErdResolverConfiguration_t waterValveResolverConfig = {
   .votingErdCare = WaterValveVotingErdCareDelegate,
   .defaultData = &defaultWaterValveData,
   .winningVoterErd = Erd_IceMaker0_WaterValve_WinningVoteErd,
   .resolvedStateErd = Erd_IceMaker0_WaterValve_ResolvedVote,
   .numberOfVotingErds = (Erd_IceMaker0_WaterValve_IceMakerVote - Erd_IceMaker0_WaterValve_WinningVoteErd)
};

static const ResolvedVoteRelayConnectorConfiguration_t waterValveRelayConnectorConfig = {
   .resolvedRelayVoteErd = Erd_IceMaker0_WaterValve_ResolvedVote,
   .relayOutputErd = Erd_IceMaker0_WaterValveRelay
};

static const TwistTrayIceMakerMotorControllerConfig_t motorControllerConfig = {
   .motorRequestedStateErd = Erd_IceMaker0_MotorRequestedState
};

static const HarvestCountCalculatorConfiguration_t harvestCountCalculatorConfig = {
   .harvestCountIsReadyToHarvestErd = Erd_IceMaker0_HarvestCountIsReadyToHarvest,
   .harvestCountCalculationRequestErd = Erd_IceMaker0_HarvestCountCalculationRequest,
   .moldFilteredTemperatureInDegFx100Erd = Erd_IceMaker0_MoldThermistor_FilteredTemperatureResolvedInDegFx100,
   .moldFreezeIntegrationCountErd = Erd_IceMaker0_FreezeIntegrationCount,
   .moldIceMakerMinimumFreezeTimeCounterInMinutesErd = Erd_IceMaker0_MinimumFreezeTimeCounterInMinutes
};

static const Erd_t enableErdsList[] = {
   Erd_IceMaker0_EnableStatus,
   Erd_IceMakerEnabledByGrid
};

static const ErdLogicServiceConfigurationEntry_t configurationEntries[] = {
   { ErdLogicServiceOperator_And, { enableErdsList, NUM_ELEMENTS(enableErdsList) }, Erd_IceMaker0_EnabledResolved },
};

static const ErdLogicServiceConfiguration_t iceMakerEnableResolverConfig = {
   configurationEntries,
   NUM_ELEMENTS(configurationEntries)
};

static const TwistTrayIceMakerMotorControllerValueUpdaterConfig_t motorControllerValueUpdaterConfig = {
   .motorActionResultErd = Erd_IceMaker0_MotorActionResult,
   .motorOperationStateErd = Erd_IceMaker0_MotorOperationState,
   .motorErrorReasonErd = Erd_IceMaker0_MotorErrorReason
};

static const Erd_t filteredTemperatureOverrideRequestErdList[] = {
   Erd_IceMaker0_MoldThermistor_FilteredTemperatureOverrideRequest
};

static const Erd_t filteredTemperatureValueErdList[] = {
   Erd_IceMaker0_MoldThermistor_FilteredTemperatureInDegFx100,
   Erd_IceMaker0_MoldThermistor_FilteredTemperatureOverrideValueInDegFx100
};

static const OverrideArbiterConfiguration_t filteredTemperatureArbiterConfig = {
   filteredTemperatureOverrideRequestErdList,
   filteredTemperatureValueErdList,
   Erd_IceMaker0_MoldThermistor_FilteredTemperatureResolvedInDegFx100,
   NUM_ELEMENTS(filteredTemperatureOverrideRequestErdList)
};

static const IceCabinetFanHarvestFixVotingConfig_t iceCabinetFanHarvestFixVotingConfig = {
   .iceMakerStateErd = Erd_IceMaker0_StateMachineState,
   .iceCabinetFanHarvestFixVoteErd = Erd_IceCabinetFanSpeed_HarvestFixVote
};

static const TwistTrayIceMakerConfiguration_t twistTrayIceMakerConfig = {
   .fsmStateErd = Erd_IceMaker0_StateMachineState,
   .thermistorIsValidResolvedErd = Erd_IceMaker0_MoldThermistor_IsValidResolved,
   .filteredTemperatureResolvedInDegFx100Erd = Erd_IceMaker0_MoldThermistor_FilteredTemperatureResolvedInDegFx100,
   .testRequestErd = Erd_IceMaker0_TestRequest,
   .stopFillSignalErd = Erd_IceMaker0_StopFillSignal,
   .harvestCountIsReadyToHarvestErd = Erd_IceMaker0_HarvestCountIsReadyToHarvest,
   .harvestCountCalculationRequestErd = Erd_IceMaker0_HarvestCountCalculationRequest,
   .motorIceMakerVoteErd = Erd_IceMaker0_TwistMotor_IceMakerVote,
   .waterValveIceMakerVoteErd = Erd_IceMaker0_WaterValve_IceMakerVote,
   .motorActionResultErd = Erd_IceMaker0_MotorActionResult,
   .motorFaultActiveErd = Erd_IceMaker0_TwistTrayMotorFault,
   .waterFillMonitoringRequestErd = Erd_IceMaker0_WaterFillMonitoringRequest,
   .isolationWaterValveVoteErd = Erd_IsolationWaterValve_IceMaker0Vote,
   .iceMakerEnabledResolvedErd = Erd_IceMaker0_EnabledResolved,
   .sabbathModeErd = Erd_SabbathModeEnable,
   .enhancedSabbathModeErd = Erd_EnhancedSabbathModeEnable,
   .freezerIceRateTriggerSignalErd = Erd_FreezerIceRateTriggerSignal,
   .fillTubeHeaterVoteErd = Erd_IceMaker0_FillTubeHeater_IceMakerVote,
   .coolingOffStatusErd = Erd_CoolingOffStatus,
   .freezerIceRateIsActiveErd = Erd_Freezer_IceRateIsActive,
   .dispensingRequestStatusErd = Erd_DispensingRequestStatus,
   .leftSideFreezerDoorStatusResolvedErd = Erd_LeftSideFreezerDoorStatusResolved,
   .dispensingInhibitedReasonErd = Erd_DispensingInhibitedReason,
   .iceMakerFillInhibitedReasonErd = Erd_IceMakerFillInhibitedReason,
   .iceMakerFullStatusErd = Erd_IceMaker0_FullStatus
};

static const TwistTrayIceMakerPlugConfig_t config = {
   .iceMakerEnableResolverConfig = &iceMakerEnableResolverConfig,
   .motorRequestManagerConfig = &motorRequestManagerConfig,
   .motorSwitchMonitorConfig = &motorSwitchMonitorConfig,
   .freezerIceRateHandlerConfig = &freezerIceRateHandlerConfig,
   .sensorFilterConfig = &sensorFilterConfig,
   .thermistorValidArbiterConfig = &thermistorValidArbiterConfig,
   .motorResolverConfiguration = &motorResolverConfiguration,
   .fillMonitorConfig = &fillMonitorConfig,
   .fillTubeHeaterResolverConfig = &fillTubeHeaterResolverConfig,
   .fillTubeHeaterDutyCycleToPercentageCalculatorConfig = &fillTubeHeaterDutyCycleToPercentageCalculatorConfig,
   .nonHarvestFillTubeHeaterControlConfig = &nonHarvestFillTubeHeaterControlConfig,
   .waterValveResolverConfig = &waterValveResolverConfig,
   .waterValveRelayConnectorConfig = &waterValveRelayConnectorConfig,
   .motorControllerConfig = &motorControllerConfig,
   .harvestCountCalculatorConfig = &harvestCountCalculatorConfig,
   .motorControllerValueUpdaterConfig = &motorControllerValueUpdaterConfig,
   .filteredTemperatureArbiterConfig = &filteredTemperatureArbiterConfig,
   .iceCabinetFanHarvestFixVotingConfig = &iceCabinetFanHarvestFixVotingConfig,
   .twistTrayIceMakerConfig = &twistTrayIceMakerConfig,
   .twistTrayMotorOutputErd = Erd_IceMaker0_TwistTrayMotorOutput
};

void TwistTrayIceMaker0Configurator_Init(TwistTrayIceMakerConfigurator_t *instance, I_DataModel_t *dataModel)
{
   const IceMakerSlotData_t *slotData = PersonalityParametricData_Get(dataModel)->iceMakerData->iceMakerSlots->slot0Data;
   const SensorDataSensorType_t *sensorData = PersonalityParametricData_Get(dataModel)->sensorData->iceMaker0MoldThermistor;

   TwistTrayIceMakerPlugin_Init(
      &instance->_private.plugin,
      dataModel,
      slotData->location,
      slotData->twistTrayData,
      sensorData,
      &PersonalityParametricData_Get(dataModel)->sensorData->periodicUpdateRateInMs,
      slotData->freezerIceRateData,
      slotData->nonHarvestFillTubeHeaterData,
      &config);
}
