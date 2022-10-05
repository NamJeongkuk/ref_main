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

static const Erd_t damperPins[] = {
   Erd_BspGpio_MotorPinB2,
   Erd_BspGpio_MotorPinB6,
   Erd_BspGpio_MotorPinB0,
   Erd_BspGpio_MotorPinB4,
   Erd_BspGpio_MotorPinC0
};

static const StepperMotorPinArray_t pinArray = {
   damperPins,
   NUM_ELEMENTS(damperPins)
};

static const StepperMotorDriverConfiguration_t driverConfig = {
   .stepperMotorPositionRequestErd = Erd_FreshFoodDamperStepperMotorPositionRequest,
   .pinArray = &pinArray
};

static const FreshFoodDamperRequestManagerConfiguration_t requestManagerConfig = {
   .damperPositionRequestResolvedVoteErd = Erd_FreshFoodDamperPosition_ResolvedVote,
   .damperStepperMotorPositionRequestErd = Erd_FreshFoodDamperStepperMotorPositionRequest,
   .damperHomingRequestErd = Erd_FreshFoodDamperHomingRequest,
   .damperCurrentPositionErd = Erd_FreshFoodDamperCurrentPosition
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

static const ErdResolverConfiguration_t damperErdResolverConfiguration = {
   .votingErdCare = VotingErdCareDelegate,
   .defaultData = &defaultData,
   .winningVoterErd = Erd_FreshFoodDamperPosition_WinningVoteErd,
   .resolvedStateErd = Erd_FreshFoodDamperPosition_ResolvedVote,
   .numberOfVotingErds = (Erd_FreshFoodDamperPosition_GridVote - Erd_FreshFoodDamperPosition_WinningVoteErd)
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
      &instance->_private.damperErdResolver,
      DataModel_AsDataSource(dataModel),
      &damperErdResolverConfiguration);

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

   TimerModule_StartPeriodic(
      DataModelErdPointerAccess_GetTimerModule(dataModel, Erd_TimerModule),
      &instance->_private.stepEventTimer,
      1,
      TriggerStepEvent,
      instance);
}
