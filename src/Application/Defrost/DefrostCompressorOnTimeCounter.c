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

enum
{
   PeriodicTimerTicksInMs = 1 * MSEC_PER_SEC
};

enum
{
   Signal_ActivelyWaitingForNextDefrost = Fsm_UserSignalStart,
   Signal_NotActivelyWaitingForNextDefrost,
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

static void ActivelyWaitingForNextDefrostChanged(void *context, const void *args)
{
   DefrostCompressorOnTimeCounter_t *instance = context;
   const bool *state = args;

   if(*state)
   {
      Fsm_SendSignal(&instance->_private.fsm, Signal_ActivelyWaitingForNextDefrost, NULL);
   }
   else
   {
      Fsm_SendSignal(&instance->_private.fsm, Signal_NotActivelyWaitingForNextDefrost, NULL);
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

      case Signal_NotActivelyWaitingForNextDefrost:
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

      case Signal_ActivelyWaitingForNextDefrost:
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

      case Signal_ActivelyWaitingForNextDefrost:
         Fsm_Transition(&instance->_private.fsm, State_Run);
         break;

      case Fsm_Exit:
         break;
   }
}

static FsmState_t InitStateBasedOnFreezerFilteredTemperature(DefrostCompressorOnTimeCounter_t *instance)
{
   TemperatureDegFx100_t freezerFilteredResolvedTemperatureInDegFx100;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerFilteredTemperatureResolvedErd,
      &freezerFilteredResolvedTemperatureInDegFx100);

   CalculatedGridLines_t calcGridLines;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->calculatedGridLinesErd,
      &calcGridLines);

   TemperatureDegFx100_t gridFreezerExtremeHystTemperature = calcGridLines.gridLines[GridDelta_Freezer].gridLinesDegFx100[GridLine_FreezerExtremeHigh];

   if(freezerFilteredResolvedTemperatureInDegFx100 > gridFreezerExtremeHystTemperature ||
      freezerFilteredResolvedTemperatureInDegFx100 >= instance->_private.defrostParametricData->freezerDefrostTerminationTemperatureInDegFx100)
   {
      return State_Stop;
   }
   else
   {
      return State_Pause;
   }
}

void DefrostCompressorOnTimeCounter_Init(
   DefrostCompressorOnTimeCounter_t *instance,
   I_DataModel_t *dataModel,
   const DefrostCompressorOnTimeCounterConfiguration_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;

   instance->_private.defrostParametricData = PersonalityParametricData_Get(dataModel)->defrostData;
   instance->_private.gridParametricData = PersonalityParametricData_Get(dataModel)->gridData;

   FsmState_t state = InitStateBasedOnFreezerFilteredTemperature(instance);
   Fsm_Init(&instance->_private.fsm, state);

   EventSubscription_Init(
      &instance->_private.subscription,
      instance,
      ActivelyWaitingForNextDefrostChanged);
   DataModel_Subscribe(
      instance->_private.dataModel,
      instance->_private.config->activelyWaitingForNextDefrostErd,
      &instance->_private.subscription);
}
