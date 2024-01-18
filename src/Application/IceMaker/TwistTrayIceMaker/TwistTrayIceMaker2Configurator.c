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
   .resolvedVoteErd = Erd_TwistTrayIceMakerMotor_ResolvedVote,
   .motorRequestErd = Erd_TwistIceMakerMotorControlRequest,
   .motorEnableErd = Erd_TwistIceMakerMotorDriveEnable,
   .motorActionResultErd = Erd_IceMaker2_MotorActionResult,
   .motorDoActionErd = Erd_IceMaker2_MotorDoAction
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
   .sensorAdcCountErd = Erd_TwistTrayIceMakerThermistor_AdcCount,
   .sensorUnfilteredTemperatureInDegFx100Erd = Erd_TwistTrayIceMaker_UnfilteredTemperatureInDegFx100,
   .sensorFilteredTemperatureInDegFx100Erd = Erd_TwistTrayIceMaker_FilteredTemperatureInDegFx100,
   .sensorIsValidErd = Erd_TwistTrayIceMaker_ThermistorIsValid,
   .sensorIsInvalidFaultErd = Erd_IceMaker0ThermistorIsInvalidFault,
   .sensorDiscoveredErd = Erd_IceMaker0Present,
   .timerModuleErd = Erd_TimerModule
};

static const Erd_t thermistorValidOverrideArbiterRequestErdList[] = {
   Erd_TwistTrayIceMakerThermistor_IsValidOverrideRequest
};

static const Erd_t thermistorValidValueErdList[] = {
   Erd_TwistTrayIceMaker_ThermistorIsValid,
   Erd_TwistTrayIceMakerThermistor_IsValidOverrideValue
};

static const OverrideArbiterConfiguration_t thermistorValidArbiterConfig = {
   thermistorValidOverrideArbiterRequestErdList,
   thermistorValidValueErdList,
   Erd_TwistTrayIceMakerThermistor_IsValidResolved,
   NUM_ELEMENTS(thermistorValidOverrideArbiterRequestErdList)
};

static bool TwistMotorVotingErdCareDelegate(const void *votingErdData)
{
   const TwistTrayIceMakerMotorVotedAction_t *data = votingErdData;
   return (data->care);
}

static const TwistTrayIceMakerMotorVotedAction_t defaultMotorData = {
   .action = IceMakerMotorAction_Idle,
   .care = Vote_DontCare
};

static const ErdResolverConfiguration_t motorResolverConfiguration = {
   .votingErdCare = TwistMotorVotingErdCareDelegate,
   .defaultData = &defaultMotorData,
   .winningVoterErd = Erd_TwistTrayIceMakerMotor_WinningVoteErd,
   .resolvedStateErd = Erd_TwistTrayIceMakerMotor_ResolvedVote,
   .numberOfVotingErds = (Erd_TwistTrayIceMakerMotor_IceMakerVote - Erd_TwistTrayIceMakerMotor_WinningVoteErd)
};

static const IceMakerWaterFillMonitorConfig_t fillMonitorConfig = {
   .stopIceMakerFillSignalErd = Erd_TwistTrayIceMakerStopFillSignal,
   .flowMeterWaterDispensedOzx100Erd = Erd_TwistTrayIceMakerFlowMeterWaterDispensedOzX100,
   .waterFillMonitoringRequestErd = Erd_TwistTrayIceMakerWaterFillMonitoringRequest,
   .flowMeterMonitoringRequestErd = Erd_TwistTrayIceMakerFlowMeterMonitoringRequest,
   .timerModuleErd = Erd_TimerModule
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
   .winningVoterErd = Erd_TwistTrayIceMakerWaterValve_WinningVoteErd,
   .resolvedStateErd = Erd_TwistTrayIceMakerWaterValve_ResolvedVote,
   .numberOfVotingErds = (Erd_TwistTrayIceMakerWaterValve_IceMakerVote - Erd_TwistTrayIceMakerWaterValve_WinningVoteErd)
};

static const ResolvedVoteRelayConnectorConfiguration_t waterValveRelayConnectorConfig = {
   .resolvedRelayVoteErd = Erd_TwistTrayIceMakerWaterValve_ResolvedVote,
   .relayOutputErd = Erd_TwistTrayIceMakerWaterValveRelay
};

static const TwistTrayIceMakerMotorControllerConfig_t motorControllerConfig = {
   .motorDoActionErd = Erd_IceMaker2_MotorDoAction
};

static const HarvestCountCalculatorConfiguration_t harvestCountCalculatorConfig = {
   .harvestCountIsReadyToHarvestErd = Erd_TwistTrayIceMaker_HarvestCountIsReadyToHarvest,
   .harvestCountCalculationRequestErd = Erd_IceMaker2_HarvestCountCalculationRequest,
   .moldFilteredTemperatureInDegFx100Erd = Erd_TwistTrayIceMaker_FilteredTemperatureResolvedInDegFx100,
   .moldFreezeIntegrationCountErd = Erd_TwistTrayIceMaker_FreezeIntegrationCount,
   .moldIceMakerMinimumFreezeTimeCounterInMinutesErd = Erd_TwistTrayIceMaker_MinimumFreezeTimeCounterInMinutes
};

static const TwistTrayIceMakerMotorControllerValueUpdaterConfig_t motorControllerValueUpdaterConfig = {
   .motorActionResultErd = Erd_IceMaker2_MotorActionResult,
   .motorOperationStateErd = Erd_IceMaker2_MotorOperationState,
   .motorErrorReasonErd = Erd_IceMaker2_MotorErrorReason
};

static const Erd_t filteredTemperatureOverrideRequestErdList[] = {
   Erd_TwistTrayIceMaker_FilteredTemperatureOverrideRequest
};

static const Erd_t filteredTemperatureValueErdList[] = {
   Erd_TwistTrayIceMaker_FilteredTemperatureInDegFx100,
   Erd_TwistTrayIceMaker_FilteredTemperatureOverrideValueInDegFx100
};

static const OverrideArbiterConfiguration_t filteredTemperatureArbiterConfig = {
   filteredTemperatureOverrideRequestErdList,
   filteredTemperatureValueErdList,
   Erd_TwistTrayIceMaker_FilteredTemperatureResolvedInDegFx100,
   NUM_ELEMENTS(filteredTemperatureOverrideRequestErdList)
};

static const TwistTrayIceMakerConfiguration_t twistTrayIceMakerConfig = {
   .highLevelStateErd = Erd_TwistTrayIceMaker_HighLevelState,
   .operationStateErd = Erd_TwistTrayIceMaker_OperationState,
   .thermistorIsValidResolvedErd = Erd_TwistTrayIceMakerThermistor_IsValidResolved,
   .filteredTemperatureResolvedInDegFx100Erd = Erd_TwistTrayIceMaker_FilteredTemperatureResolvedInDegFx100,
   .testRequestErd = Erd_IceMaker2_TestRequest,
   .stopFillSignalErd = Erd_TwistTrayIceMakerStopFillSignal,
   .harvestCountIsReadyToHarvestErd = Erd_TwistTrayIceMaker_HarvestCountIsReadyToHarvest,
   .harvestCountCalculationRequestErd = Erd_IceMaker2_HarvestCountCalculationRequest,
   .motorIceMakerVoteErd = Erd_TwistTrayIceMakerMotor_IceMakerVote,
   .waterValveIceMakerVoteErd = Erd_TwistTrayIceMakerWaterValve_IceMakerVote,
   .motorActionResultErd = Erd_IceMaker2_MotorActionResult,
   .motorFaultActiveErd = Erd_TwistTrayIceMaker_MotorFaultActive,
   .waterFillMonitoringRequestErd = Erd_TwistTrayIceMakerWaterFillMonitoringRequest,
   .isolationWaterValveVoteErd = Erd_IsolationWaterValve_TwistTrayIceMakerVote,
   .iceMakerEnabledResolvedErd = Erd_IceMakerEnabledResolved,
   .sabbathModeErd = Erd_SabbathModeEnable,
   .enhancedSabbathModeErd = Erd_EnhancedSabbathModeEnable,
   .freezerIceRateTriggerSignalErd = Erd_FreezerIceRateTriggerSignal,
   .fillTubeHeaterVoteErd = Erd_FillTubeHeater_TwistTrayIceMakerVote,
   .coolingOffStatusErd = Erd_CoolingOffStatus,
   .freezerIceRateIsActiveErd = Erd_Freezer_IceRateIsActive,
   .dispensingRequestStatusErd = Erd_DispensingRequestStatus,
   .leftSideFreezerDoorStatusResolvedErd = Erd_LeftSideFreezerDoorStatusResolved,
   .dispensingInhibitedReasonErd = Erd_DispensingInhibitedReason,
   .iceMakerFillInhibitedReasonErd = Erd_IceMakerFillInhibitedReason,
   .iceMakerFullStatusErd = Erd_IceMaker0FullStatus
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
   .waterValveResolverConfig = &waterValveResolverConfig,
   .waterValveRelayConnectorConfig = &waterValveRelayConnectorConfig,
   .motorControllerConfig = &motorControllerConfig,
   .harvestCountCalculatorConfig = &harvestCountCalculatorConfig,
   .motorControllerValueUpdaterConfig = &motorControllerValueUpdaterConfig,
   .filteredTemperatureArbiterConfig = &filteredTemperatureArbiterConfig,
   .twistTrayIceMakerConfig = &twistTrayIceMakerConfig
};

void TwistTrayIceMaker2Configurator_Init(TwistTrayIceMakerConfigurator_t *instance, I_DataModel_t *dataModel)
{
   const IceMakerSlotData_t *slotData = PersonalityParametricData_Get(dataModel)->iceMakerData->iceMakerSlots->slot2Data;
   const SensorData_t *sensorData = PersonalityParametricData_Get(dataModel)->sensorData;

   TwistTrayIceMakerPlugin_Init(
      &instance->_private.plugin,
      dataModel,
      slotData->location,
      slotData->twistTrayData,
      sensorData,
      slotData->freezerIceRateData,
      slotData->nonHarvestFillTubeHeaterData,
      &config);
}
