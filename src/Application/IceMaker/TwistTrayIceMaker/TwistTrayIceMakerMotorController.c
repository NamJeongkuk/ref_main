/*!
 * @file
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "TwistTrayIceMakerMotorController.h"
#include "utils.h"
#include "Constants_Time.h"
#include "Constants_Binary.h"
#include "TwistTrayIceMakerMotorState.h"
#include "IceMakerMotorOperationState.h"
#include "IceMakerMotorRequestedState.h"

enum
{
   Signal_SwitchHigh = Fsm_UserSignalStart,
   Signal_SwitchLow,
   Signal_MotorTimeoutPeriodElapsed,
   Signal_InitialHomingTwistPeriodElapsed,
   Signal_HomingJumpOutPeriodElapsed,
   Signal_HomePositionLandingDelayElapsed,
   Signal_FullBucketTriggerWaitPeriodElapsed,
   Signal_StoppingPeriodElapsed,
   Signal_NewMotorRequestedState,
};

#define FSM_STATE_EXPAND_AS_FUNCTION_DEFINITION(Name) \
   static void Name(Fsm_t *fsm, FsmSignal_t signal, const void *data);

#define FSM_STATE_TABLE(ENTRY)                      \
   ENTRY(State_Homing_MakingSureTheTrayIsHome)      \
   ENTRY(State_Homing_JumpingOutOfHome)             \
   ENTRY(State_Homing_ReadyToLandInHomePosition)    \
                                                    \
   ENTRY(State_Harvest_CheckingIfBucketIsFull)      \
   ENTRY(State_Harvest_BucketWasFoundToBeFull)      \
   ENTRY(State_Harvest_StoppingAtFullTwistPosition) \
   ENTRY(State_Harvest_Untwisting)                  \
   ENTRY(State_Harvest_ReadyToLandInHomePosition)   \
                                                    \
   ENTRY(State_Idle)

FSM_STATE_TABLE(FSM_STATE_EXPAND_AS_FUNCTION_DEFINITION)

#define LongMotorErrorTimeout (instance->_private.data->harvestData.longMotorErrorTimeoutPeriodSec)
#define ShortMotorErrorTimeout (instance->_private.data->harvestData.shortMotorErrorTimeoutPeriodSec)

#define HomePositionLandingDelayPeriod (instance->_private.data->harvestData.homeLandingDelayPeriodSecX10 * 100)
#define FullBucketDetectionPeriod (instance->_private.data->harvestData.fullBucketDetectionPeriodSecX10 * 100)
#define InitialHomingTwistPeriod (instance->_private.data->harvestData.initialHomingTwistPeriodSecX10 * 100)
#define StoppingPeriod (1 * MSEC_PER_SEC)

static TwistTrayIceMakerMotorController_t *InterfaceFrom(Fsm_t *fsm)
{
   return CONTAINER_OF(TwistTrayIceMakerMotorController_t, _private.fsm, fsm);
}

static void UpdateMotorOperationState(
   TwistTrayIceMakerMotorController_t *instance,
   IceMakerMotorOperationState_t newState)
{
   instance->_private.motorOperationState = newState;
}

static void UpdateMotorActionResult(
   TwistTrayIceMakerMotorController_t *instance,
   IceMakerMotorActionResult_t actionResult)
{
   instance->_private.motorActionResult = actionResult;
}

static void UpdateMotorErrorReason(
   TwistTrayIceMakerMotorController_t *instance,
   IceMakerMotorErrorReason_t errorReason)
{
   instance->_private.motorErrorReason = errorReason;
}

static void InterruptContextTimerModule_StartOneShot(
   TwistTrayIceMakerMotorController_t *instance,
   uint8_t timerIndex,
   uint32_t timerDuration,
   FsmSignal_t signal)
{
   instance->_private.timeCount[timerIndex] = 0;
   instance->_private.timerDuration[timerIndex] = timerDuration;
   instance->_private.timerSignal[timerIndex] = signal;
}

static void InterruptContextTimerModule_Stop(
   TwistTrayIceMakerMotorController_t *instance,
   uint8_t timerIndex)
{
   instance->_private.timeCount[timerIndex] = 0;
   instance->_private.timerDuration[timerIndex] = 0;
   instance->_private.timerSignal[timerIndex] = 0;
}

static void UpdateMotorState(TwistTrayIceMakerMotorController_t *instance, uint8_t newState, uint16_t motorErrorTimeoutInSeconds)
{
   Output_Write(instance->_private.motorStateOutput, &newState);

   // I am using comments instead of helper functions to reduce stack usage
   // Because this module runs in the interrupt context

   // Start motor error detection timer if:
   // (You are going to make the motor actually move)
   if((newState == TwistTrayIceMakerMotorState_Untwisting) || (newState == TwistTrayIceMakerMotorState_Twisting))
   {
      // AND (the motor is changing from Twisting to Untwisting -- or Untwisting to Twisting)
      if(instance->_private.lastMotorState != newState)
      {
         InterruptContextTimerModule_StartOneShot(
            instance,
            Timer_MotorError,
            motorErrorTimeoutInSeconds * MSEC_PER_SEC,
            Signal_MotorTimeoutPeriodElapsed);
      }
   }
   else
   {
      InterruptContextTimerModule_Stop(instance, Timer_MotorError);
   }

   instance->_private.lastMotorState = newState;
}

static void ThrowMotorError(TwistTrayIceMakerMotorController_t *instance, IceMakerMotorErrorReason_t reason)
{
   UpdateMotorState(instance, TwistTrayIceMakerMotorState_Coasting, 0);
   UpdateMotorActionResult(instance, IceMakerMotorActionResult_MotorError);
   instance->_private.harvestActionResult = IceMakerMotorActionResult_MotorError;
   UpdateMotorErrorReason(instance, reason);
}

static void State_Homing_MakingSureTheTrayIsHome(Fsm_t *fsm, FsmSignal_t signal, const void *data)
{
   TwistTrayIceMakerMotorController_t *instance = InterfaceFrom(fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         UpdateMotorOperationState(instance, IceMakerMotorOperationState_HomingMakingSureTheTrayIsHome);
         UpdateMotorState(instance, TwistTrayIceMakerMotorState_Untwisting, LongMotorErrorTimeout);

         if(instance->_private.lastSwitchState)
         {
            InterruptContextTimerModule_StartOneShot(
               instance,
               Timer_GeneralPurpose,
               InitialHomingTwistPeriod,
               Signal_InitialHomingTwistPeriodElapsed);
         }
         break;

      case Signal_SwitchLow:
         InterruptContextTimerModule_Stop(instance, Timer_GeneralPurpose);
         break;

      case Signal_SwitchHigh:
         InterruptContextTimerModule_StartOneShot(
            instance,
            Timer_GeneralPurpose,
            InitialHomingTwistPeriod,
            Signal_InitialHomingTwistPeriodElapsed);
         break;

      case Signal_InitialHomingTwistPeriodElapsed:
         Fsm_Transition(fsm, State_Homing_JumpingOutOfHome);
         break;

      case Signal_MotorTimeoutPeriodElapsed:
         ThrowMotorError(instance, IceMakerMotorErrorReason_MotorStallsInStateHomingCheckingTrayIsHome);
         Fsm_Transition(fsm, State_Idle);
         break;

      case Fsm_Exit:
         UpdateMotorState(instance, TwistTrayIceMakerMotorState_Coasting, 0);
         InterruptContextTimerModule_Stop(instance, Timer_GeneralPurpose);
         break;
   }
}

static void State_Homing_JumpingOutOfHome(Fsm_t *fsm, FsmSignal_t signal, const void *data)
{
   TwistTrayIceMakerMotorController_t *instance = InterfaceFrom(fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         UpdateMotorOperationState(instance, IceMakerMotorOperationState_HomingJumpingOutOfHome);
         UpdateMotorState(instance, TwistTrayIceMakerMotorState_Twisting, ShortMotorErrorTimeout);
         break;

      case Signal_SwitchLow:
         InterruptContextTimerModule_StartOneShot(
            instance,
            Timer_GeneralPurpose,
            HomePositionLandingDelayPeriod,
            Signal_HomingJumpOutPeriodElapsed);
         break;

      case Signal_HomingJumpOutPeriodElapsed:
         Fsm_Transition(fsm, State_Homing_ReadyToLandInHomePosition);
         break;

      case Signal_MotorTimeoutPeriodElapsed:
         ThrowMotorError(instance, IceMakerMotorErrorReason_MotorStallsInStateHomingJumpingOutOfHome);
         Fsm_Transition(fsm, State_Idle);
         break;

      case Fsm_Exit:
         UpdateMotorState(instance, TwistTrayIceMakerMotorState_Coasting, 0);
         InterruptContextTimerModule_Stop(instance, Timer_GeneralPurpose);
         break;
   }
}

static void State_Homing_ReadyToLandInHomePosition(Fsm_t *fsm, FsmSignal_t signal, const void *data)
{
   TwistTrayIceMakerMotorController_t *instance = InterfaceFrom(fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         UpdateMotorOperationState(instance, IceMakerMotorOperationState_HomingReadyToLandInHomePosition);
         UpdateMotorState(instance, TwistTrayIceMakerMotorState_Untwisting, ShortMotorErrorTimeout);
         break;

      case Signal_SwitchHigh:
         InterruptContextTimerModule_StartOneShot(
            instance,
            Timer_GeneralPurpose,
            HomePositionLandingDelayPeriod,
            Signal_HomePositionLandingDelayElapsed);
         break;

      case Signal_HomePositionLandingDelayElapsed:
         UpdateMotorState(instance, TwistTrayIceMakerMotorState_Braking, 0);
         InterruptContextTimerModule_StartOneShot(
            instance,
            Timer_GeneralPurpose,
            StoppingPeriod,
            Signal_StoppingPeriodElapsed);
         break;

      case Signal_StoppingPeriodElapsed:
         UpdateMotorActionResult(instance, IceMakerMotorActionResult_Homed);
         Fsm_Transition(fsm, State_Idle);
         break;

      case Signal_MotorTimeoutPeriodElapsed:
         ThrowMotorError(instance, IceMakerMotorErrorReason_MotorStallsInStateHomingReadyToLandInHomePosition);
         Fsm_Transition(fsm, State_Idle);
         break;

      case Fsm_Exit:
         UpdateMotorState(instance, TwistTrayIceMakerMotorState_Coasting, 0);
         InterruptContextTimerModule_Stop(instance, Timer_GeneralPurpose);
         break;
   }
}

static void State_Harvest_CheckingIfBucketIsFull(Fsm_t *fsm, FsmSignal_t signal, const void *data)
{
   TwistTrayIceMakerMotorController_t *instance = InterfaceFrom(fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         UpdateMotorOperationState(instance, IceMakerMotorOperationState_HarvestCheckingIfBucketIsFull);
         UpdateMotorState(instance, TwistTrayIceMakerMotorState_Twisting, LongMotorErrorTimeout);

         InterruptContextTimerModule_StartOneShot(
            instance,
            Timer_GeneralPurpose,
            FullBucketDetectionPeriod,
            Signal_FullBucketTriggerWaitPeriodElapsed);
         break;

      case Signal_FullBucketTriggerWaitPeriodElapsed:
         instance->_private.harvestActionResult = IceMakerMotorActionResult_Harvested;
         // The motor does not stop moving when transitioning here
         Fsm_Transition(fsm, State_Harvest_StoppingAtFullTwistPosition);
         break;

      case Signal_SwitchHigh:
         instance->_private.harvestActionResult = IceMakerMotorActionResult_BucketWasFull;
         UpdateMotorState(instance, TwistTrayIceMakerMotorState_Coasting, 0);
         Fsm_Transition(fsm, State_Harvest_BucketWasFoundToBeFull);
         break;

      case Fsm_Exit:
         InterruptContextTimerModule_Stop(instance, Timer_GeneralPurpose);
         break;
   }
}

static void State_Harvest_BucketWasFoundToBeFull(Fsm_t *fsm, FsmSignal_t signal, const void *data)
{
   TwistTrayIceMakerMotorController_t *instance = InterfaceFrom(fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         UpdateMotorOperationState(instance, IceMakerMotorOperationState_HarvestBucketWasFoundToBeFull);
         UpdateMotorState(instance, TwistTrayIceMakerMotorState_Coasting, 0);

         InterruptContextTimerModule_StartOneShot(
            instance,
            Timer_GeneralPurpose,
            1000,
            Signal_StoppingPeriodElapsed);
         break;

      case Signal_StoppingPeriodElapsed:
         Fsm_Transition(fsm, State_Harvest_ReadyToLandInHomePosition);
         break;

      case Fsm_Exit:
         InterruptContextTimerModule_Stop(instance, Timer_GeneralPurpose);
         break;
   }
}

static void State_Harvest_StoppingAtFullTwistPosition(Fsm_t *fsm, FsmSignal_t signal, const void *data)
{
   TwistTrayIceMakerMotorController_t *instance = InterfaceFrom(fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         UpdateMotorOperationState(instance, IceMakerMotorOperationState_HarvestStoppingAtFullTwistPosition);
         break;

      case Signal_SwitchHigh:
         UpdateMotorState(instance, TwistTrayIceMakerMotorState_Coasting, 0);
         InterruptContextTimerModule_StartOneShot(
            instance,
            Timer_GeneralPurpose,
            1000,
            Signal_StoppingPeriodElapsed);
         break;

      case Signal_StoppingPeriodElapsed:
         Fsm_Transition(fsm, State_Harvest_Untwisting);
         break;

      case Signal_MotorTimeoutPeriodElapsed:
         ThrowMotorError(instance, IceMakerMotorErrorReason_MotorStallsInStateHarvestStoppingAtFullTwistPosition);
         Fsm_Transition(fsm, State_Idle);
         break;

      case Fsm_Exit:
         InterruptContextTimerModule_Stop(instance, Timer_GeneralPurpose);

         // TODO: Increment Harvest Count
         break;
   }
}

static void State_Harvest_Untwisting(Fsm_t *fsm, FsmSignal_t signal, const void *data)
{
   TwistTrayIceMakerMotorController_t *instance = InterfaceFrom(fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         UpdateMotorOperationState(instance, IceMakerMotorOperationState_HarvestUntwisting);
         UpdateMotorState(instance, TwistTrayIceMakerMotorState_Untwisting, LongMotorErrorTimeout);
         break;

      case Signal_SwitchHigh:
         // When the switch goes high at this point it is part of the "Switch Check"
         // No action needs to be taken until the switch goes high again

         // So the motor does not stop moving when transitioning here
         Fsm_Transition(fsm, State_Harvest_ReadyToLandInHomePosition);
         break;

      case Signal_MotorTimeoutPeriodElapsed:
         ThrowMotorError(instance, IceMakerMotorErrorReason_MotorStallsInStateHarvestUntwisting);
         Fsm_Transition(fsm, State_Idle);
         break;
   }
}

static void State_Harvest_ReadyToLandInHomePosition(Fsm_t *fsm, FsmSignal_t signal, const void *data)
{
   TwistTrayIceMakerMotorController_t *instance = InterfaceFrom(fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         UpdateMotorOperationState(instance, IceMakerMotorOperationState_HarvestReadyToLandInHomePosition);
         UpdateMotorState(instance, TwistTrayIceMakerMotorState_Untwisting, LongMotorErrorTimeout);
         break;

      case Signal_SwitchHigh:
         InterruptContextTimerModule_StartOneShot(
            instance,
            Timer_GeneralPurpose,
            HomePositionLandingDelayPeriod,
            Signal_HomePositionLandingDelayElapsed);
         break;

      case Signal_HomePositionLandingDelayElapsed:
         UpdateMotorState(instance, TwistTrayIceMakerMotorState_Braking, 0);
         InterruptContextTimerModule_StartOneShot(
            instance,
            Timer_GeneralPurpose,
            StoppingPeriod,
            Signal_StoppingPeriodElapsed);
         break;

      case Signal_StoppingPeriodElapsed:
         Fsm_Transition(fsm, State_Idle);
         break;

      case Signal_MotorTimeoutPeriodElapsed:
         ThrowMotorError(instance, IceMakerMotorErrorReason_MotorStallsInStateHarvestReadyToLandInHomePosition);
         Fsm_Transition(fsm, State_Idle);
         break;

      case Fsm_Exit:
         UpdateMotorState(instance, TwistTrayIceMakerMotorState_Coasting, 0);
         UpdateMotorActionResult(instance, instance->_private.harvestActionResult);
         InterruptContextTimerModule_Stop(instance, Timer_GeneralPurpose);
         break;
   }
}

static void State_Idle(Fsm_t *fsm, FsmSignal_t signal, const void *data)
{
   TwistTrayIceMakerMotorController_t *instance = InterfaceFrom(fsm);
   const IceMakerMotorState_t *newMotorState = data;

   switch(signal)
   {
      case Fsm_Entry:
         UpdateMotorOperationState(instance, IceMakerMotorOperationState_Idle);
         InterruptContextTimerModule_Stop(instance, Timer_GeneralPurpose);
         InterruptContextTimerModule_Stop(instance, Timer_MotorError);
         break;

      case Signal_NewMotorRequestedState:
         if(*newMotorState == IceMakerMotorState_Homing)
         {
            UpdateMotorActionResult(instance, IceMakerMotorActionResult_Homing);
            Fsm_Transition(fsm, State_Homing_MakingSureTheTrayIsHome);
         }
         else if(*newMotorState == IceMakerMotorState_Run)
         {
            UpdateMotorActionResult(instance, IceMakerMotorActionResult_Harvesting);
            Fsm_Transition(fsm, State_Harvest_CheckingIfBucketIsFull);
         }
         else
         {
            UpdateMotorActionResult(instance, IceMakerMotorActionResult_NoAction);
         }
         break;
   }
}

static void MotorRequestedStateUpdated(void *context, const void *args)
{
   TwistTrayIceMakerMotorController_t *instance = context;
   const IceMakerMotorRequestedState_t *motorRequestedState = args;

   Fsm_SendSignal(&instance->_private.fsm, Signal_NewMotorRequestedState, &motorRequestedState->state);
}

void TwistTrayIceMakerMotorController_Init(
   TwistTrayIceMakerMotorController_t *instance,
   const TwistTrayIceMakerData_t *data,
   I_Output_t *motorStateOutput,
   I_DataModel_t *dataModel,
   const TwistTrayIceMakerMotorControllerConfig_t *config)
{
   instance->_private.motorStateOutput = motorStateOutput;
   instance->_private.lastSwitchState = OFF;
   instance->_private.data = data;
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;

   instance->_private.lastMotorState = TwistTrayIceMakerMotorState_Coasting;

   EventSubscription_Init(
      &instance->_private.motorRequestedStateSubscription,
      instance,
      MotorRequestedStateUpdated);
   DataModel_Subscribe(
      dataModel,
      instance->_private.config->motorRequestedStateErd,
      &instance->_private.motorRequestedStateSubscription);

   Fsm_Init(&instance->_private.fsm, State_Idle);

   instance->_private.initialized = true;
}

void TwistTrayIceMakerMotorController_Run(TwistTrayIceMakerMotorController_t *instance)
{
   for(uint8_t i = 0; i < Timer_Max; i++)
   {
      if(instance->_private.timerDuration[i] > 0)
      {
         instance->_private.timeCount[i] = TRUNCATE_UNSIGNED_ADDITION(instance->_private.timeCount[i], 1, UINT16_MAX);

         if(instance->_private.timeCount[i] >= instance->_private.timerDuration[i])
         {
            FsmSignal_t signal = instance->_private.timerSignal[i];
            InterruptContextTimerModule_Stop(instance, i);
            Fsm_SendSignal(&instance->_private.fsm, signal, NULL);
         }
      }
   }
}

void TwistTrayIceMakerMotorController_UpdateSwitchState(
   TwistTrayIceMakerMotorController_t *instance,
   bool newSwitchState)
{
   if(instance->_private.initialized && (instance->_private.lastSwitchState != newSwitchState))
   {
      instance->_private.lastSwitchState = newSwitchState;
      if(newSwitchState)
      {
         Fsm_SendSignal(&instance->_private.fsm, Signal_SwitchHigh, NULL);
      }
      else
      {
         Fsm_SendSignal(&instance->_private.fsm, Signal_SwitchLow, NULL);
      }
   }
}

IceMakerMotorActionResult_t TwistTrayIceMakerMotorController_MotorActionResult(
   TwistTrayIceMakerMotorController_t *instance)
{
   return instance->_private.motorActionResult;
}

IceMakerMotorOperationState_t TwistTrayIceMakerMotorController_MotorOperationState(
   TwistTrayIceMakerMotorController_t *instance)
{
   return instance->_private.motorOperationState;
}

IceMakerMotorErrorReason_t TwistTrayIceMakerMotorController_MotorErrorReason(
   TwistTrayIceMakerMotorController_t *instance)
{
   return instance->_private.motorErrorReason;
}
