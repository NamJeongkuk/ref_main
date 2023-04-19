/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "TwistTrayIceMakerPlugin.h"
#include "SystemErds.h"
#include "PersonalityParametricData.h"
#include "DataModelErdPointerAccess.h"
#include "DataSource_Gpio.h"
#include "TwistTrayIceMakerMotorAction.h"

static const Output_TwistTrayIceMakerMotorStateConfig_t outputTwistIceMakerMotorStateConfig = {
   .motorDrivePositiveClockwiseChannel = Erd_BspGpio_MTR_DRV_00,
   .motorDriveNegativeCounterClockwiseChannel = Erd_BspGpio_MTR_DRV_01
};

static const TwistTrayIceMakerMotorRequestManagerConfig_t requestManagerConfig = {
   .resolvedVoteErd = Erd_TwistTrayIceMakerMotor_ResolvedVote,
   .motorRequestErd = Erd_TwistIceMakerMotorControlRequest,
   .motorEnableErd = Erd_TwistIceMakerMotorDriveEnable,
   .motorActionResultErd = Erd_TwistTrayIceMaker_MotorActionResult,
   .motorOperationStateErd = Erd_TwistTrayIceMaker_MotorOperationState
};

static const TwistTrayIceMakerMotorSwitchMonitorConfig_t switchMonitorConfig = {
   .switchInputErd = Erd_Gpio_GPIO_IN_02
};

static const SensorFilteringConfig_t sensorFilterConfig = {
   .sensorAdcCountErd = Erd_TwistTrayIceMakerThermistor_AdcCount,
   .sensorUnfilteredTemperatureInDegFx100Erd = Erd_TwistTrayIceMaker_UnfilteredTemperatureInDegFx100,
   .sensorFilteredTemperatureInDegFx100Erd = Erd_TwistTrayIceMaker_FilteredTemperatureInDegFx100,
   .sensorIsValidErd = Erd_TwistTrayIceMaker_ThermistorIsValid,
   .timerModuleErd = Erd_TimerModule
};

static const Erd_t twistTrayIceMakerThermistorValidOverrideArbiterRequestErdList[] = {
   Erd_TwistTrayIceMakerThermistor_IsValidOverrideRequest
};

static const Erd_t twistTrayIceMakerThermistorValidValueErdList[] = {
   Erd_TwistTrayIceMaker_ThermistorIsValid,
   Erd_TwistTrayIceMakerThermistor_IsValidOverrideValue
};

static const OverrideArbiterConfiguration_t twistTrayIceMakerThermistorValidArbiterConfiguration = {
   twistTrayIceMakerThermistorValidOverrideArbiterRequestErdList,
   twistTrayIceMakerThermistorValidValueErdList,
   Erd_TwistTrayIceMakerThermistor_IsValidResolved,
   NUM_ELEMENTS(twistTrayIceMakerThermistorValidOverrideArbiterRequestErdList)
};

static bool TwistMotorVotingErdCareDelegate(const void *votingErdData)
{
   const TwistTrayIceMakerMotorVotedAction_t *data = votingErdData;
   return (data->care);
}

static const TwistTrayIceMakerMotorVotedAction_t defaultMotorData = {
   .action = TwistTrayIceMakerMotorAction_Idle,
   .care = Vote_DontCare
};

static const ErdResolverConfiguration_t twistTrayMotorResolverConfiguration = {
   .votingErdCare = TwistMotorVotingErdCareDelegate,
   .defaultData = &defaultMotorData,
   .winningVoterErd = Erd_TwistTrayIceMakerMotor_WinningVoteErd,
   .resolvedStateErd = Erd_TwistTrayIceMakerMotor_ResolvedVote,
   .numberOfVotingErds = (Erd_TwistTrayIceMakerMotor_IceMakerVote - Erd_TwistTrayIceMakerMotor_WinningVoteErd)
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

static const ErdResolverConfiguration_t iceMakerWaterValveResolverConfiguration = {
   .votingErdCare = WaterValveVotingErdCareDelegate,
   .defaultData = &defaultWaterValveData,
   .winningVoterErd = Erd_TwistTrayIceMakerWaterValve_WinningVoteErd,
   .resolvedStateErd = Erd_TwistTrayIceMakerWaterValve_ResolvedVote,
   .numberOfVotingErds = (Erd_TwistTrayIceMakerWaterValve_IceMakerVote - Erd_TwistTrayIceMakerWaterValve_WinningVoteErd)
};

static const ResolvedVoteRelayConnectorConfiguration_t iceMakerWaterValveRelayConnectorConfiguration = {
   .resolvedRelayVoteErd = Erd_TwistTrayIceMakerWaterValve_ResolvedVote,
   .relayOutputErd = Erd_TwistTrayIceMakerWaterValveRelay
};

void TwistTrayIceMakerPlugin_Init(TwistTrayIceMakerPlugin_t *instance, I_DataModel_t *dataModel)
{
   const SensorData_t *sensorData = PersonalityParametricData_Get(dataModel)->sensorData;
   SensorFiltering_Init(
      &instance->_private.sensorFilter,
      dataModel,
      &sensorFilterConfig,
      sensorData->twistTrayIceMakerMoldThermistor,
      sensorData->periodicUpdateRateInMs);

   OverrideArbiter_Init(
      &instance->_private.twistTrayIceMakerThermistorIsValidOverrideArbiter,
      DataModel_AsDataSource(dataModel),
      &twistTrayIceMakerThermistorValidArbiterConfiguration);

   ErdResolver_Init(
      &instance->_private.twistTrayMotorVoteResolver,
      DataModel_AsDataSource(dataModel),
      &twistTrayMotorResolverConfiguration);

   ErdResolver_Init(
      &instance->_private.iceMakerWaterValveVoteResolver,
      DataModel_AsDataSource(dataModel),
      &iceMakerWaterValveResolverConfiguration);

   ResolvedVoteRelayConnector_Init(
      &instance->_private.iceMakerWaterValveRelayConnector,
      dataModel,
      &iceMakerWaterValveRelayConnectorConfiguration);

   TwistTrayIceMakerRunner_Init(
      &instance->_private.twistTrayIceMakerMotorControllerRunner,
      &instance->_private.twistTrayMotorController,
      DataModelErdPointerAccess_GetInterrupt(dataModel, Erd_SystemTickInterrupt));

   TwistTrayIceMakerMotorRequestManager_Init(
      &instance->_private.twistTrayMotorRequestManager,
      dataModel,
      &instance->_private.twistTrayMotorController,
      &requestManagerConfig);

   TwistTrayIceMakerMotorController_Init(
      &instance->_private.twistTrayMotorController,
      PersonalityParametricData_Get(dataModel)->iceMakerData->twistTrayIceMakerData,
      Output_TwistTrayIceMakerMotorState_Init(
         &instance->_private.motorStateOutput,
         DataModelErdPointerAccess_GetGpioGroup(dataModel, Erd_GpioGroupInterface),
         &outputTwistIceMakerMotorStateConfig));

   TwistTrayIceMakerMotorSwitchMonitor_Init(
      &instance->_private.twistTrayIceMakerSwitchMonitor,
      dataModel,
      &instance->_private.twistTrayMotorController,
      &switchMonitorConfig);

   TwistTrayIceMaker_Init(
      &instance->_private.twistTrayIceMaker,
      DataModelErdPointerAccess_GetTimerModule(dataModel, Erd_TimerModule),
      DataModel_AsDataSource(dataModel),
      PersonalityParametricData_Get(dataModel)->iceMakerData->twistTrayIceMakerData);
}
