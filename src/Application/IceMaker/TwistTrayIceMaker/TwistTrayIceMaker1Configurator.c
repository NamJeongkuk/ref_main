/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "TwistTrayIceMakerConfigurator.h"
#include "SystemErds.h"
#include "DataSource_Gpio.h"

static const Output_TwistTrayIceMakerMotorStateConfig_t outputMotorStateConfig = {
   .motorDrivePositiveClockwiseChannel = Erd_BspGpio_MTR_DRV_00,
   .motorDriveNegativeCounterClockwiseChannel = Erd_BspGpio_MTR_DRV_01
};

static const TwistTrayIceMakerMotorRequestManagerConfig_t motorRequestManagerConfig = {
   .resolvedVoteErd = Erd_IceMaker1_TwistMotor_ResolvedVote,
   .motorRequestErd = Erd_TwistIceMakerMotorControlRequest,
   .motorEnableErd = Erd_TwistIceMakerMotorDriveEnable,
   .motorActionResultErd = Erd_IceMaker1_MotorActionResult,
   .motorDoActionErd = Erd_IceMaker1_MotorDoAction
};

static const TwistTrayIceMakerMotorSwitchMonitorConfig_t motorSwitchMonitorConfig = {
   .switchInputErd = Erd_Gpio_GPIO_IN_02
};

static const FreezerIceRateHandlerConfig_t freezerIceRateHandlerConfig = {
   .freezerIceRateTriggerSignalErd = Erd_FreezerIceRateTriggerSignal,
   .freezerSetpointFreezerIceRateVoteErd = Erd_FreezerSetpoint_FreezerIceRateVote,
   .freezerEvapFanSpeedFreezerIceRateVoteErd = Erd_FreezerEvapFanSpeed_FreezerIceRateVote,
   .freezerEvapFanSpeedResolvedVoteErd = Erd_FreezerEvapFanSpeed_ResolvedVote,
   .iceRateIsActiveErd = Erd_Freezer_IceRateIsActive
};

static const SensorFilteringConfig_t sensorFilterConfig = {
   .sensorAdcCountErd = Erd_IceMaker1_MoldThermistor_AdcCount,
   .sensorUnfilteredTemperatureInDegFx100Erd = Erd_IceMaker1_MoldThermistor_UnfilteredTemperatureInDegFx100,
   .sensorFilteredTemperatureInDegFx100Erd = Erd_IceMaker1_MoldThermistor_FilteredTemperatureInDegFx100,
   .sensorIsValidErd = Erd_IceMaker1_MoldThermistorIsValid,
   .sensorIsInvalidFaultErd = Erd_IceMaker1ThermistorIsInvalidFault,
   .sensorDiscoveredErd = Erd_IceMaker1_HasBeenDiscovered,
   .timerModuleErd = Erd_TimerModule
};

static const Erd_t thermistorValidOverrideArbiterRequestErdList[] = {
   Erd_IceMaker1_MoldThermistor_IsValidOverrideRequest
};

static const Erd_t thermistorValidValueErdList[] = {
   Erd_IceMaker1_MoldThermistorIsValid,
   Erd_IceMaker1_MoldThermistor_IsValidOverrideValue
};

static const OverrideArbiterConfiguration_t thermistorValidArbiterConfig = {
   thermistorValidOverrideArbiterRequestErdList,
   thermistorValidValueErdList,
   Erd_IceMaker1_MoldThermistor_IsValidResolved,
   NUM_ELEMENTS(thermistorValidOverrideArbiterRequestErdList)
};

static bool TwistMotorVotingErdCareDelegate(const void *votingErdData)
{
   const IceMakerTwistMotorVotedAction_t *data = votingErdData;
   return (data->care);
}

static const IceMakerTwistMotorVotedAction_t defaultMotorData = {
   .action = IceMakerMotorAction_Idle,
   .care = Vote_DontCare
};

static const ErdResolverConfiguration_t motorResolverConfiguration = {
   .votingErdCare = TwistMotorVotingErdCareDelegate,
   .defaultData = &defaultMotorData,
   .winningVoterErd = Erd_IceMaker1_TwistMotor_WinningVoteErd,
   .resolvedStateErd = Erd_IceMaker1_TwistMotor_ResolvedVote,
   .numberOfVotingErds = (Erd_IceMaker1_TwistMotor_IceMakerVote - Erd_IceMaker1_TwistMotor_WinningVoteErd)
};

static const IceMakerWaterFillMonitorConfig_t fillMonitorConfig = {
   .stopIceMakerFillSignalErd = Erd_IceMaker1_StopFillSignal,
   .flowMeterWaterDispensedOzx100Erd = Erd_IceMaker1_FlowMeterWaterDispensedOzX100,
   .waterFillMonitoringRequestErd = Erd_IceMaker1_WaterFillMonitoringRequest,
   .flowMeterMonitoringRequestErd = Erd_IceMaker1_FlowMeterMonitoringRequest,
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
   .winningVoterErd = Erd_IceMaker1_FillTubeHeater_WinningVoteErd,
   .resolvedStateErd = Erd_IceMaker1_FillTubeHeater_ResolvedVote,
   .numberOfVotingErds = (Erd_IceMaker1_FillTubeHeater_NonHarvestVote - Erd_IceMaker1_FillTubeHeater_WinningVoteErd)
};

static const PercentageDutyCycleVoteToPwmDutyCycleConverterConfig_t fillTubeHeaterDutyCycleToPercentageCalculatorConfig = {
   .inputPercentageDutyCycleVoteErd = Erd_IceMaker1_FillTubeHeater_ResolvedVote,
   .outputPwmDutyCycleErd = Erd_FillTubeHeater_Pwm
};

static const NonHarvestFillTubeHeaterControlConfig_t nonHarvestFillTubeHeaterControlConfig = {
   .iceMakingActiveErd = Erd_Freezer_IceRateIsActive,
   .cabinetTemperatureErd = Erd_Freezer_FilteredTemperatureResolvedInDegFx100,
   .compressorIsOnErd = Erd_CompressorIsOn,
   .fanResolvedVoteErd = Erd_FreezerEvapFanSpeed_ResolvedVote,
   .nonHarvestFillTubeHeaterVoteErd = Erd_IceMaker1_FillTubeHeater_NonHarvestVote
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
   .winningVoterErd = Erd_IceMaker1_WaterValve_WinningVoteErd,
   .resolvedStateErd = Erd_IceMaker1_WaterValve_ResolvedVote,
   .numberOfVotingErds = (Erd_IceMaker1_WaterValve_IceMakerVote - Erd_IceMaker1_WaterValve_WinningVoteErd)
};

static const ResolvedVoteRelayConnectorConfiguration_t waterValveRelayConnectorConfig = {
   .resolvedRelayVoteErd = Erd_IceMaker1_WaterValve_ResolvedVote,
   .relayOutputErd = Erd_TwistTrayIceMakerWaterValveRelay
};

static const TwistTrayIceMakerMotorControllerConfig_t motorControllerConfig = {
   .motorDoActionErd = Erd_IceMaker1_MotorDoAction
};

static const HarvestCountCalculatorConfiguration_t harvestCountCalculatorConfig = {
   .harvestCountIsReadyToHarvestErd = Erd_IceMaker1_HarvestCountIsReadyToHarvest,
   .harvestCountCalculationRequestErd = Erd_IceMaker1_HarvestCountCalculationRequest,
   .moldFilteredTemperatureInDegFx100Erd = Erd_IceMaker1_MoldThermistor_FilteredTemperatureResolvedInDegFx100,
   .moldFreezeIntegrationCountErd = Erd_IceMaker1_FreezeIntegrationCount,
   .moldIceMakerMinimumFreezeTimeCounterInMinutesErd = Erd_IceMaker1_MinimumFreezeTimeCounterInMinutes
};

static const TwistTrayIceMakerMotorControllerValueUpdaterConfig_t motorControllerValueUpdaterConfig = {
   .motorActionResultErd = Erd_IceMaker1_MotorActionResult,
   .motorOperationStateErd = Erd_IceMaker1_MotorOperationState,
   .motorErrorReasonErd = Erd_IceMaker1_MotorErrorReason
};

static const Erd_t filteredTemperatureOverrideRequestErdList[] = {
   Erd_IceMaker1_MoldThermistor_FilteredTemperatureOverrideRequest
};

static const Erd_t filteredTemperatureValueErdList[] = {
   Erd_IceMaker1_MoldThermistor_FilteredTemperatureInDegFx100,
   Erd_IceMaker1_MoldThermistor_FilteredTemperatureOverrideValueInDegFx100
};

static const OverrideArbiterConfiguration_t filteredTemperatureArbiterConfig = {
   filteredTemperatureOverrideRequestErdList,
   filteredTemperatureValueErdList,
   Erd_IceMaker1_MoldThermistor_FilteredTemperatureResolvedInDegFx100,
   NUM_ELEMENTS(filteredTemperatureOverrideRequestErdList)
};

static const TwistTrayIceMakerConfiguration_t twistTrayIceMakerConfig = {
   .highLevelStateErd = Erd_TwistTrayIceMaker_HighLevelState,
   .fsmStateErd = Erd_IceMaker1_StateMachineState,
   .thermistorIsValidResolvedErd = Erd_IceMaker1_MoldThermistor_IsValidResolved,
   .filteredTemperatureResolvedInDegFx100Erd = Erd_IceMaker1_MoldThermistor_FilteredTemperatureResolvedInDegFx100,
   .testRequestErd = Erd_IceMaker1_TestRequest,
   .stopFillSignalErd = Erd_IceMaker1_StopFillSignal,
   .harvestCountIsReadyToHarvestErd = Erd_IceMaker1_HarvestCountIsReadyToHarvest,
   .harvestCountCalculationRequestErd = Erd_IceMaker1_HarvestCountCalculationRequest,
   .motorIceMakerVoteErd = Erd_IceMaker1_TwistMotor_IceMakerVote,
   .waterValveIceMakerVoteErd = Erd_IceMaker1_WaterValve_IceMakerVote,
   .motorActionResultErd = Erd_IceMaker1_MotorActionResult,
   .motorFaultActiveErd = Erd_TwistTrayIceMaker_MotorFaultActive,
   .waterFillMonitoringRequestErd = Erd_IceMaker1_WaterFillMonitoringRequest,
   .isolationWaterValveVoteErd = Erd_IsolationWaterValve_IceMaker1Vote,
   .iceMakerEnabledResolvedErd = Erd_IceMakerEnabledResolved,
   .sabbathModeErd = Erd_SabbathModeEnable,
   .enhancedSabbathModeErd = Erd_EnhancedSabbathModeEnable,
   .freezerIceRateTriggerSignalErd = Erd_FreezerIceRateTriggerSignal,
   .fillTubeHeaterVoteErd = Erd_IceMaker1_FillTubeHeater_IceMakerVote,
   .coolingOffStatusErd = Erd_CoolingOffStatus,
   .freezerIceRateIsActiveErd = Erd_Freezer_IceRateIsActive,
   .dispensingRequestStatusErd = Erd_DispensingRequestStatus,
   .leftSideFreezerDoorStatusResolvedErd = Erd_LeftSideFreezerDoorStatusResolved,
   .dispensingInhibitedReasonErd = Erd_DispensingInhibitedReason,
   .iceMakerFillInhibitedReasonErd = Erd_IceMakerFillInhibitedReason,
   .iceMakerFullStatusErd = Erd_IceMaker1_FullStatus
};

static const TwistTrayIceMakerPlugConfig_t config = {
   .outputMotorStateConfig = &outputMotorStateConfig,
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
   .twistTrayIceMakerConfig = &twistTrayIceMakerConfig
};

void TwistTrayIceMaker1Configurator_Init(TwistTrayIceMakerConfigurator_t *instance, I_DataModel_t *dataModel)
{
   const IceMakerSlotData_t *slotData = PersonalityParametricData_Get(dataModel)->iceMakerData->iceMakerSlots->slot1Data;
   const SensorDataSensorType_t *sensorData = PersonalityParametricData_Get(dataModel)->sensorData->iceMaker1MoldThermistor;

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
