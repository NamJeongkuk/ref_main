/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ThreeWaySealedSystemValvePlugin.h"
#include "Constants_Binary.h"
#include "utils.h"
#include "SystemErds.h"
#include "I_GpioGroup.h"
#include "DataSource_Gpio.h"
#include "DataModelErdPointerAccess.h"
#include "EventQueueInterruptSafePlugin.h"
#include "SealedSystemValveControlOnPowerUp.h"

static const StepperMotorPins_t valvePins = {
   .motorDriveA = Erd_BspGpio_REF_VAL_00,
   .motorDriveB = Erd_BspGpio_REF_VAL_01,
   .motorDriveABar = Erd_BspGpio_REF_VAL_02,
   .motorDriveBBar = Erd_BspGpio_REF_VAL_03
};

static const StepperMotorSubStep_t subSteps[] = {
   { ON, OFF, OFF, OFF },
   { ON, ON, OFF, OFF },
   { OFF, ON, OFF, OFF },
   { OFF, ON, ON, OFF },
   { OFF, OFF, ON, OFF },
   { OFF, OFF, ON, ON },
   { OFF, OFF, OFF, ON },
   { ON, OFF, OFF, ON }
};

static const StepperMotorSubStepConfiguration_t stepperMotorSubStepConfig = {
   .subSteps = subSteps,
   .numberOfSubSteps = NUM_ELEMENTS(subSteps)
};

static const SealedSystemValveRequestManagerConfiguration_t requestManagerConfig = {
   .positionRequestResolvedVoteErd = Erd_SealedSystemValvePosition_ResolvedVote,
   .stepperMotorPositionRequestErd = Erd_SealedSystemValveStepperMotorPositionRequest,
   .homingRequestErd = Erd_SealedSystemValveHomingRequest,
   .currentPositionErd = Erd_SealedSystemValveCurrentPosition,
   .previousPositionErd = Erd_SealedSystemValvePreviousPosition
};

static const SealedSystemValveControlOnPowerUpConfig_t valveControlOnPowerUpConfig = {
   .sealedSystemValvePositionPowerUpVoteErd = Erd_SealedSystemValvePosition_PowerUpVote
};

static const StepperMotorDriverConfiguration_t stepperMotorDriverConfig = {
   .stepperMotorPositionRequestErd = Erd_SealedSystemValveStepperMotorPositionRequest,
   .motorControlRequestErd = Erd_SealedSystemValveStepperMotorControlRequest,
   .motorEnableErd = Erd_SealedSystemValveStepperMotorControlRequest,
   .pins = &valvePins,
   .subStepConfig = &stepperMotorSubStepConfig
};

static const SealedSystemValveVotedPosition_t defaultData = {
   .position = SealedSystemValvePosition_B,
   .care = false
};

static bool VotingErdCareDelegate(const void *votingErdData)
{
   const SealedSystemValveVotedPosition_t *data = votingErdData;
   return (data->care);
}

static const ErdResolverConfiguration_t positionErdResolverConfiguration = {
   .votingErdCare = VotingErdCareDelegate,
   .defaultData = &defaultData,
   .winningVoterErd = Erd_SealedSystemValvePosition_WinningVoteErd,
   .resolvedStateErd = Erd_SealedSystemValvePosition_ResolvedVote,
   .numberOfVotingErds = (Erd_SealedSystemValvePosition_GridVote - Erd_SealedSystemValvePosition_WinningVoteErd)
};

void ThreeWaySealedSystemValvePlugin_Init(ThreeWaySealedSystemValvePlugin_t *instance, I_DataModel_t *dataModel)
{
   ErdResolver_Init(
      &instance->_private.positionErdResolver,
      DataModel_AsDataSource(dataModel),
      &positionErdResolverConfiguration);

   StepperMotorDriver_Init(
      &instance->_private.stepperMotorDriver,
      dataModel,
      &stepperMotorDriverConfig,
      DataModelErdPointerAccess_GetGpioGroup(dataModel, Erd_GpioGroupInterface),
      (DataModelErdPointerAccess_GetInterrupt(dataModel, Erd_SystemTickInterrupt))->OnInterrupt,
      EventQueueInterruptSafePlugin_GetEventQueueInterface(),
      &PersonalityParametricData_Get(dataModel)->sealedSystemValveData->delayBetweenStepEventsInMilliseconds,
      &PersonalityParametricData_Get(dataModel)->sealedSystemValveData->excitationDelayInMilliseconds);

   SealedSystemValveRequestManager_Init(
      &instance->_private.requestManager,
      dataModel,
      &requestManagerConfig,
      PersonalityParametricData_Get(dataModel)->sealedSystemValveData);

   SealedSystemValveControlOnPowerUp_Init(
      &instance->_private.valveControlOnPowerUp,
      dataModel,
      &valveControlOnPowerUpConfig,
      PersonalityParametricData_Get(dataModel)->sealedSystemValveData);
}
