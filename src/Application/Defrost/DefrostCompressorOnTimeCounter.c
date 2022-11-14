/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "DefrostCompressorOnTimeCounter.h"
#include "PersonalityParametricData.h"
#include "DataModelErdPointerAccess.h"
#include "DefrostCompressorOnTimeCounterFsmState.h"
#include "CalculatedGridLines.h"
#include "Constants_Time.h"
#include "CompressorVotedSpeed.h"
#include "utils.h"
#include "uassert.h"

enum
{
   PeriodicTimerTicksInMs = 1 * MSEC_PER_SEC
};

enum
{
   Signal_WaitingToDefrost = Fsm_UserSignalStart,
   Signal_NotWaitingToDefrost,
   Signal_PeriodicTimeout
};

static void State_Stop(Fsm_t *fsm, const FsmSignal_t signal, const void *data);
static void State_Pause(Fsm_t *fsm, const FsmSignal_t signal, const void *data);
static void State_Run(Fsm_t *fsm, const FsmSignal_t signal, const void *data);

static DefrostCompressorOnTimeCounter_t *InstanceFromFsm(Fsm_t *fsm)
{
   return CONTAINER_OF(DefrostCompressorOnTimeCounter_t, _private.fsm, fsm);
}

static void SetFsmStateTo(DefrostCompressorOnTimeCounter_t *instance, DefrostCompressorOnTimeCounterFsmState_t state)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->defrostCompressorOnTimeCounterFsmStateErd,
      &state);
}

static void WaitingToDefrostChanged(void *context, const void *args)
{
   DefrostCompressorOnTimeCounter_t *instance = context;
   const bool *state = args;

   if(*state)
   {
      Fsm_SendSignal(&instance->_private.fsm, Signal_WaitingToDefrost, NULL);
   }
   else
   {
      Fsm_SendSignal(&instance->_private.fsm, Signal_NotWaitingToDefrost, NULL);
   }
}

static void ResetCompressorOnTimeInSecondsToZero(DefrostCompressorOnTimeCounter_t *instance)
{
   uint32_t seconds = 0;
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->defrostCompressorOnTimeInSecondsErd,
      &seconds);
}

static void PeriodicTimeout(void *context)
{
   DefrostCompressorOnTimeCounter_t *instance = context;

   Fsm_SendSignal(&instance->_private.fsm, Signal_PeriodicTimeout, NULL);
}

static void StartPeriodicTimer(DefrostCompressorOnTimeCounter_t *instance)
{
   TimerModule_StartPeriodic(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         instance->_private.config->timerModuleErd),
      &instance->_private.periodicTimer,
      PeriodicTimerTicksInMs,
      PeriodicTimeout,
      instance);
}

static bool CompressorIsOn(DefrostCompressorOnTimeCounter_t *instance)
{
   bool state;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->compressorIsOnErd,
      &state);

   return state;
}

static void IncrementCompressorOnTimeInSeconds(DefrostCompressorOnTimeCounter_t *instance)
{
   uint32_t seconds;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->defrostCompressorOnTimeInSecondsErd,
      &seconds);

   seconds++;
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->defrostCompressorOnTimeInSecondsErd,
      &seconds);
}

static void State_Run(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   DefrostCompressorOnTimeCounter_t *instance = InstanceFromFsm(fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         SetFsmStateTo(instance, DefrostCompressorOnTimeCounterFsmState_Run);
         StartPeriodicTimer(instance);
         break;

      case Signal_NotWaitingToDefrost:
         Fsm_Transition(&instance->_private.fsm, State_Stop);
         break;

      case Signal_PeriodicTimeout:
         if(CompressorIsOn(instance))
         {
            IncrementCompressorOnTimeInSeconds(instance);
         }
         break;

      case Fsm_Exit:
         break;
   }
}

static void State_Pause(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   DefrostCompressorOnTimeCounter_t *instance = InstanceFromFsm(fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         SetFsmStateTo(instance, DefrostCompressorOnTimeCounterFsmState_Pause);
         break;

      case Signal_WaitingToDefrost:
         Fsm_Transition(&instance->_private.fsm, State_Run);
         break;

      case Fsm_Exit:
         break;
   }
}

static void State_Stop(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   DefrostCompressorOnTimeCounter_t *instance = InstanceFromFsm(fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         SetFsmStateTo(instance, DefrostCompressorOnTimeCounterFsmState_Stop);
         ResetCompressorOnTimeInSecondsToZero(instance);
         break;

      case Signal_WaitingToDefrost:
         Fsm_Transition(&instance->_private.fsm, State_Run);
         break;

      case Fsm_Exit:
         break;
   }
}

static FsmState_t InitStateBasedOnFreezerFilteredTemperature(DefrostCompressorOnTimeCounter_t *instance)
{
   bool freezerWasTooWarmOnPowerUp;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerFilteredTemperatureWasTooWarmOnPowerUpErd,
      &freezerWasTooWarmOnPowerUp);

   bool waitingToDefrost;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->waitingToDefrostErd,
      &waitingToDefrost);

   if(freezerWasTooWarmOnPowerUp)
   {
      ResetCompressorOnTimeInSecondsToZero(instance);
   }

   if(waitingToDefrost)
   {
      return State_Run;
   }

   return State_Pause;
}

void DefrostCompressorOnTimeCounter_Init(
   DefrostCompressorOnTimeCounter_t *instance,
   I_DataModel_t *dataModel,
   const DefrostCompressorOnTimeCounterConfiguration_t *config)
{
   bool freezerFilteredTemperatureTooWarmOnPowerUpReady;
   DataModel_Read(
      dataModel,
      config->freezerFilteredTemperatureTooWarmOnPowerUpReadyErd,
      &freezerFilteredTemperatureTooWarmOnPowerUpReady);

   uassert(freezerFilteredTemperatureTooWarmOnPowerUpReady);

   instance->_private.dataModel = dataModel;
   instance->_private.config = config;

   FsmState_t state = InitStateBasedOnFreezerFilteredTemperature(instance);
   Fsm_Init(&instance->_private.fsm, state);

   EventSubscription_Init(
      &instance->_private.subscription,
      instance,
      WaitingToDefrostChanged);
   DataModel_Subscribe(
      instance->_private.dataModel,
      instance->_private.config->waitingToDefrostErd,
      &instance->_private.subscription);

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->defrostCompressorOnTimeCounterReadyErd,
      set);
}
