/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "DataModelErdPointerAccess.h"
#include "DataSource_Gpio.h"
#include "FreshFoodDamperMotorPlugin.h"
#include "SystemErds.h"

static const StepperMotorPins_t damperPins = {
   .motorDriveA = Erd_BspGpio_MTR_DRV_00,
   .motorDriveB = Erd_BspGpio_MTR_DRV_02,
   .motorDriveABar = Erd_BspGpio_MTR_DRV_01,
   .motorDriveBBar = Erd_BspGpio_MTR_DRV_03
};

static const StepperMotorDriverConfiguration_t driverConfig = {
   .stepperMotorPositionRequestErd = Erd_FreshFoodDamperStepperMotorPositionRequest,
   .motorControlRequestErd = Erd_FreshFoodDamperStepperMotorControlRequest,
   .motorEnableErd = Erd_FreshFoodDamperStepperMotorDriveEnable,
   .pins = &damperPins
};

static const FreshFoodDamperRequestManagerConfiguration_t requestManagerConfig = {
   .damperPositionRequestResolvedVoteErd = Erd_FreshFoodDamperPosition_ResolvedVote,
   .damperStepperMotorPositionRequestErd = Erd_FreshFoodDamperStepperMotorPositionRequest,
   .damperHomingRequestErd = Erd_FreshFoodDamperHomingRequest,
   .damperCurrentPositionErd = Erd_FreshFoodDamperCurrentPosition
};

static const DamperMaxOpenTimeConfiguration_t maxOpenDamperConfig = {
   .damperPositionMaxOpenTimeVoteErd = Erd_FreshFoodDamperPosition_MaxOpenTimeVote,
   .damperCurrentPositionErd = Erd_FreshFoodDamperCurrentPosition
};

static const DamperFreezePreventionConfiguration_t damperFreezePreventionConfig = {
   .damperHeaterVoteErd = Erd_FreshFoodDamperHeater_DamperFreezePreventionVote,
   .damperPositionVoteErd = Erd_FreshFoodDamperPosition_DamperFreezePreventionVote,
   .sourceThermistorIsValidResolvedErd = Erd_FreezerThermistor_IsValidResolved,
   .targetThermistorIsValidResolvedErd = Erd_FreshFoodThermistor_IsValidResolved,
   .sourceFilteredTemperatureErd = Erd_Freezer_FilteredTemperatureResolvedInDegFx100,
   .targetFilteredTemperatureErd = Erd_FreshFood_FilteredTemperatureResolvedInDegFx100,
   .damperCurrentPositionErd = Erd_FreshFoodDamperCurrentPosition,
   .timerModuleErd = Erd_TimerModule,
   .damperFreezePreventionFsmStateErd = Erd_DamperFreezePreventionFsmState
};

static const DamperHeaterDefrostControlConfig_t freshFoodDamperHeaterDefrostControlConfig = {
   .defrostHeaterStateErd = Erd_FreezerDefrostHeaterRelay,
   .damperHeaterDefrostHeaterSyncVoteErd = Erd_FreshFoodDamperHeater_DefrostHeaterSyncVote,
   .defrostHeaterVoteErd = Erd_FreezerDefrostHeater_DefrostVote
};

static const DamperVotedPosition_t defaultData = {
   .position = DamperPosition_Closed,
   .care = false
};

static bool VotingErdCareDelegate(const void *votingErdData)
{
   const DamperVotedPosition_t *data = votingErdData;
   return (data->care);
}

static const ErdResolverConfiguration_t damperPositionErdResolverConfiguration = {
   .votingErdCare = VotingErdCareDelegate,
   .defaultData = &defaultData,
   .winningVoterErd = Erd_FreshFoodDamperPosition_WinningVoteErd,
   .resolvedStateErd = Erd_FreshFoodDamperPosition_ResolvedVote,
   .numberOfVotingErds = (Erd_FreshFoodDamperPosition_GridVote - Erd_FreshFoodDamperPosition_WinningVoteErd)
};

static bool VotingDamperHeaterErdCareDelegate(const void *votingErdData)
{
   const HeaterVotedState_t *data = votingErdData;
   return (data->care);
}

static const ErdResolverConfiguration_t damperHeaterErdResolverConfiguration = {
   .votingErdCare = VotingDamperHeaterErdCareDelegate,
   .defaultData = &defaultData,
   .winningVoterErd = Erd_FreshFoodDamperHeater_WinningVoteErd,
   .resolvedStateErd = Erd_FreshFoodDamperHeater_ResolvedVote,
   .numberOfVotingErds = (Erd_FreshFoodDamperHeater_DefrostHeaterSyncVote - Erd_FreshFoodDamperHeater_WinningVoteErd)
};

static void TriggerStepEvent(void *context)
{
   FreshFoodDamperMotorPlugin_t *instance = context;
   Event_Synchronous_Publish(&instance->_private.damperStepEvent, NULL);
}

void FreshFoodDamperMotorPlugin_Init(FreshFoodDamperMotorPlugin_t *instance, I_DataModel_t *dataModel)
{
   Event_Synchronous_Init(&instance->_private.damperStepEvent);

   ErdResolver_Init(
      &instance->_private.damperPositionErdResolver,
      DataModel_AsDataSource(dataModel),
      &damperPositionErdResolverConfiguration);

   ErdResolver_Init(
      &instance->_private.damperHeaterErdResolver,
      DataModel_AsDataSource(dataModel),
      &damperHeaterErdResolverConfiguration);

   StepperMotorDriver_Init(
      &instance->_private.stepperMotorDriver,
      dataModel,
      &driverConfig,
      DataModelErdPointerAccess_GetGpioGroup(dataModel, Erd_GpioGroupInterface),
      &instance->_private.damperStepEvent.interface);

   FreshFoodDamperRequestManager_Init(
      &instance->_private.damperRequestManager,
      dataModel,
      &requestManagerConfig);

   DamperFreezePrevention_Init(
      &instance->_private.damperFreezePrevention,
      dataModel,
      &damperFreezePreventionConfig,
      PersonalityParametricData_Get(dataModel)->freshFoodDamperData);

   DamperMaxOpenTimeMonitor_Init(
      &instance->_private.damperMaxOpenTime,
      dataModel,
      &maxOpenDamperConfig,
      PersonalityParametricData_Get(dataModel)->freshFoodDamperData);

   DamperHeaterDefrostControl_Init(
      &instance->_private.freshFoodDamperHeaterDefrostControl,
      dataModel,
      &freshFoodDamperHeaterDefrostControlConfig,
      DataModelErdPointerAccess_GetTimerModule(dataModel, Erd_TimerModule),
      PersonalityParametricData_Get(dataModel)->damperHeaterData);

   TimerModule_StartPeriodic(
      DataModelErdPointerAccess_GetTimerModule(dataModel, Erd_TimerModule),
      &instance->_private.stepEventTimer,
      1,
      TriggerStepEvent,
      instance);
}
