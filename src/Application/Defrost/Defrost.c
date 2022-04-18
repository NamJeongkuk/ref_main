/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "Defrost.h"
#include "DefrostHsmState.h"
#include "PersonalityParametricData.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"
#include "CalculatedGridLines.h"
#include "DefrostTimerCounterRequest.h"
#include "DataModelErdPointerAccess.h"
#include "DefrostState.h"
#include "HeaterVotedState.h"
#include "DefrostTimerCounterFsmState.h"
#include "utils.h"

enum
{
   PowerUpFactor = 5
};

enum
{
   Signal_PowerUpDelayComplete = Hsm_UserSignalStart,
   Signal_MaxTimeBetweenDefrostsComplete,
   Signal_PeriodicTimeoutComplete,
   Signal_MaxPrechillHoldoffTimeComplete
};

static bool State_PowerUp(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_Idle(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_PrechillPrep(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_PostPrechill(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_HeaterOnEntry(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_Dwell(Hsm_t *hsm, HsmSignal_t signal, const void *data);

static const HsmStateHierarchyDescriptor_t stateList[] = {
   { State_PowerUp, HSM_NO_PARENT },
   { State_Idle, HSM_NO_PARENT },
   { State_Dwell, HSM_NO_PARENT },
   { State_PrechillPrep, HSM_NO_PARENT },
   { State_PostPrechill, HSM_NO_PARENT },
   { State_HeaterOnEntry, HSM_NO_PARENT }
};

static const HsmConfiguration_t hsmConfiguration = {
   stateList,
   NUM_ELEMENTS(stateList)
};

static Defrost_t *InstanceFromHsm(Hsm_t *hsm)
{
   return CONTAINER_OF(Defrost_t, _private.hsm, hsm);
};

static void SetHsmStateTo(Defrost_t *instance, DefrostHsmState_t state)
{
   DataModel_Write(instance->_private.dataModel, instance->_private.config->defrostHsmStateErd, &state);
};

static void SaveFzAbnormalDefrostData(Defrost_t *instance)
{
   bool fzDefrostWasAbnormal;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->fzDefrostWasAbnormalErd,
      &fzDefrostWasAbnormal);

   uint16_t fzAbnormalDefrostCycleCount;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->fzAbnormalDefrostCycleCountErd,
      &fzAbnormalDefrostCycleCount);

   uint16_t fzDefrostCycleCount;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->fzDefrostCycleCountErd,
      &fzDefrostCycleCount);

   if(fzAbnormalDefrostCycleCount != fzDefrostCycleCount)
   {
      uint16_t numberOfFzAbnormalDefrostCycles;
      DataModel_Read(
         instance->_private.dataModel,
         instance->_private.config->numberOfFzAbnormalDefrostCyclesErd,
         &numberOfFzAbnormalDefrostCycles);

      numberOfFzAbnormalDefrostCycles++;
      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->numberOfFzAbnormalDefrostCyclesErd,
         &numberOfFzAbnormalDefrostCycles);
   }

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->fzAbnormalDefrostCycleCountErd,
      &fzDefrostCycleCount);

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->fzDefrostWasAbnormalErd,
      set);
};

static bool LastDefrostWasAbnormalBecauseOfAbnormalFilteredFzCabinetTemperature(Defrost_t *instance)
{
   TemperatureDegFx100_t fzFilteredTemperature;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerFilteredTemperatureErd,
      &fzFilteredTemperature);

   CalculatedGridLines_t calcGridLines;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->calculatedGridLinesErd,
      &calcGridLines);

   TemperatureDegFx100_t gridFzExtremeHystTemperature = calcGridLines.gridLines[GridDelta_Fz].gridLinesDegFx100[GridLine_FzExtremeHigh];

   return (fzFilteredTemperature > gridFzExtremeHystTemperature ||
      fzFilteredTemperature >= instance->_private.defrostParametricData->fzDefrostTerminationTemperatureInDegFx100);
};

static void StartTimer(Defrost_t *instance, Timer_t *timer, TimerTicks_t ticks, TimerCallback_t callback)
{
   TimerModule_StartOneShot(
      DataModelErdPointerAccess_GetTimerModule(instance->_private.dataModel, instance->_private.config->timerModuleErd),
      timer,
      ticks,
      callback,
      instance);
};

static void VoteForFfDefrostHeater(Defrost_t *instance, bool state)
{
   HeaterVotedState_t vote;
   vote.state = state;
   vote.care = true;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->ffDefrostHeaterDefrostVoteErd,
      &vote);
}

static void VoteForFzDefrostHeater(Defrost_t *instance, bool state)
{
   HeaterVotedState_t vote;
   vote.state = state;
   vote.care = true;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->fzDefrostHeaterDefrostVoteErd,
      &vote);
}

static DefrostState_t LastDefrostState(Defrost_t *instance)
{
   DefrostState_t lastDefrostState;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->defrostStateErd,
      &lastDefrostState);

   return lastDefrostState;
}

static bool FzDefrostWasAbnormal(Defrost_t *instance)
{
   bool defrostWasAbnormal;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->fzDefrostWasAbnormalErd,
      &defrostWasAbnormal);

   return defrostWasAbnormal;
}

static bool DefrostTimerCounterIsDisabled(Defrost_t *instance)
{
   DefrostTimerCounterFsmState_t state;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->defrostTimerCounterFsmStateErd,
      &state);

   return (state == DefrostTimerCounterFsmState_Disabled);
}

static void RequestDefrostTimerCounterTo(Defrost_t *instance, DefrostTimer_t request)
{
   DefrostTimerCounterRequest_t defrostTimerCounterRequest;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->defrostTimerCounterRequestErd,
      &defrostTimerCounterRequest);

   defrostTimerCounterRequest.request = request;
   defrostTimerCounterRequest.requestId++;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->defrostTimerCounterRequestErd,
      &defrostTimerCounterRequest);
}

static void PowerUpDelayComplete(void *context)
{
   REINTERPRET(instance, context, Defrost_t *);

   Hsm_SendSignal(&instance->_private.hsm, Signal_PowerUpDelayComplete, NULL);
};

static void StartPowerUpDelayTimer(Defrost_t *instance)
{
   StartTimer(
      instance,
      &instance->_private.powerUpDelayTimer,
      instance->_private.gridParametricData->gridPeriodicRunRateInMSec * PowerUpFactor,
      PowerUpDelayComplete);
}

static void SetFlagToResetRequiredWhenEnablingDefrostTimerCounterTo(Defrost_t *instance, bool state)
{
   instance->_private.resetRequiredWhenEnablingDefrostTimerCounter = state;
}

static bool DefrostTimerCounterResetRequiredFromPowerUp(Defrost_t *instance)
{
   return instance->_private.resetRequiredWhenEnablingDefrostTimerCounter;
}

static void MaxTimeBetweenDefrostsComplete(void *context)
{
   REINTERPRET(instance, context, Defrost_t *);

   Hsm_SendSignal(&instance->_private.hsm, Signal_MaxTimeBetweenDefrostsComplete, NULL);
}

static void StartMaxTimeBetweenDefrostsTimer(Defrost_t *instance)
{
   StartTimer(
      instance,
      &instance->_private.timeBetweenDefrostsTimer,
      instance->_private.defrostParametricData->maxTimeBetweenDefrostsInMinutes * MSEC_PER_MIN,
      MaxTimeBetweenDefrostsComplete);
}

static void PeriodicTimeoutComplete(void *context)
{
   REINTERPRET(instance, context, Defrost_t *);

   Hsm_SendSignal(&instance->_private.hsm, Signal_PeriodicTimeoutComplete, NULL);
}

static void StartPeriodicTimer(Defrost_t *instance)
{
   TimerModule_StartPeriodic(
      DataModelErdPointerAccess_GetTimerModule(instance->_private.dataModel, instance->_private.config->timerModuleErd),
      &instance->_private.periodicTimer,
      instance->_private.defrostParametricData->defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC,
      PeriodicTimeoutComplete,
      instance);
}

static bool DefrostTimerIsSatisfied(Defrost_t *instance)
{
   bool state;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->defrostTimerIsSatisfiedErd,
      &state);

   return state;
}

static bool SealedSystemIsInValvePosition(Defrost_t *instance, ValvePosition_t expectedPosition)
{
   ValvePosition_t actualPosition;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->sealedSystemValvePositionErd,
      &actualPosition);

   return actualPosition == expectedPosition;
}

static void MaxPrechillHoldoffTimeComplete(void *context)
{
   REINTERPRET(instance, context, Defrost_t *);

   Hsm_SendSignal(&instance->_private.hsm, Signal_MaxPrechillHoldoffTimeComplete, NULL);
}

static void StartPrechillHoldoffTimer(Defrost_t *instance)
{
   StartTimer(
      instance,
      &instance->_private.prechillHoldoffTimer,
      instance->_private.defrostParametricData->maxPrechillHoldoffTimeAfterDefrostTimerSatisfiedInSeconds * MSEC_PER_SEC,
      MaxPrechillHoldoffTimeComplete);
}

static bool PrechillHoldoffTimerIsNotRunning(Defrost_t *instance)
{
   return !TimerModule_IsRunning(
      DataModelErdPointerAccess_GetTimerModule(instance->_private.dataModel, instance->_private.config->timerModuleErd),
      &instance->_private.prechillHoldoffTimer);
}

static void StopPrechillHoldoffTimer(Defrost_t *instance)
{
   TimerModule_Stop(
      DataModelErdPointerAccess_GetTimerModule(instance->_private.dataModel, instance->_private.config->timerModuleErd),
      &instance->_private.prechillHoldoffTimer);
}

static ValvePosition_t PositionToExitIdle(Defrost_t *instance)
{
   return instance->_private.defrostParametricData->threeWayValvePositionToExitIdle;
}

static bool State_PowerUp(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   Defrost_t *instance = InstanceFromHsm(hsm);
   DefrostState_t lastDefrostState;
   IGNORE(data);

   switch(signal)
   {
      case Hsm_Entry:
         SetHsmStateTo(instance, DefrostHsmState_PowerUp);
         StartPowerUpDelayTimer(instance);
         break;

      case Signal_PowerUpDelayComplete:
         lastDefrostState = LastDefrostState(instance);

         if(LastDefrostWasAbnormalBecauseOfAbnormalFilteredFzCabinetTemperature(instance))
         {
            SetFlagToResetRequiredWhenEnablingDefrostTimerCounterTo(instance, true);
            SaveFzAbnormalDefrostData(instance);

            (lastDefrostState == DefrostState_HeaterOn) ? Hsm_Transition(hsm, State_Dwell) : Hsm_Transition(hsm, State_Idle);
         }
         else
         {
            if(lastDefrostState == DefrostState_Prechill)
            {
               (FzDefrostWasAbnormal(instance)) ? Hsm_Transition(hsm, State_PostPrechill) : Hsm_Transition(hsm, State_PrechillPrep);
            }
            else if(lastDefrostState == DefrostState_HeaterOn)
            {
               Hsm_Transition(hsm, State_HeaterOnEntry);
            }
            else
            {
               Hsm_Transition(hsm, State_Idle);
            }
         }
         break;

      case Hsm_Exit:
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

static bool State_Idle(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   Defrost_t *instance = InstanceFromHsm(hsm);
   IGNORE(data);
   IGNORE(instance);

   switch(signal)
   {
      case Hsm_Entry:
         SetHsmStateTo(instance, DefrostHsmState_Idle);

         VoteForFfDefrostHeater(instance, OFF);
         VoteForFzDefrostHeater(instance, OFF);

         if(DefrostTimerCounterIsDisabled(instance))
         {
            RequestDefrostTimerCounterTo(instance, DefrostTimer_Enable);

            if(DefrostTimerCounterResetRequiredFromPowerUp(instance))
            {
               RequestDefrostTimerCounterTo(instance, DefrostTimer_Reset);
               SetFlagToResetRequiredWhenEnablingDefrostTimerCounterTo(instance, false);
            }
         }

         StartMaxTimeBetweenDefrostsTimer(instance);
         StartPeriodicTimer(instance);
         break;

      case Signal_PeriodicTimeoutComplete:
         if(DefrostTimerIsSatisfied(instance))
         {
            if(SealedSystemIsInValvePosition(instance, PositionToExitIdle(instance)))
            {
               Hsm_Transition(hsm, State_PrechillPrep);
            }
            else
            {
               if(PrechillHoldoffTimerIsNotRunning(instance))
               {
                  StartPrechillHoldoffTimer(instance);
               }
            }
         }
         break;

      case Signal_MaxTimeBetweenDefrostsComplete:
         Hsm_Transition(hsm, State_PrechillPrep);
         break;

      case Signal_MaxPrechillHoldoffTimeComplete:
         Hsm_Transition(hsm, State_PrechillPrep);
         break;

      case Hsm_Exit:
         StopPrechillHoldoffTimer(instance);
         RequestDefrostTimerCounterTo(instance, DefrostTimer_Disable);
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

static bool State_PrechillPrep(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   Defrost_t *instance = InstanceFromHsm(hsm);
   IGNORE(data);
   IGNORE(instance);

   switch(signal)
   {
      case Hsm_Entry:
         SetHsmStateTo(instance, DefrostHsmState_PrechillPrep);
         break;

      case Hsm_Exit:
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

static bool State_PostPrechill(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   Defrost_t *instance = InstanceFromHsm(hsm);
   IGNORE(data);
   IGNORE(instance);

   switch(signal)
   {
      case Hsm_Entry:
         SetHsmStateTo(instance, DefrostHsmState_PostPrechill);
         break;

      case Hsm_Exit:
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

static bool State_HeaterOnEntry(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   Defrost_t *instance = InstanceFromHsm(hsm);
   IGNORE(data);
   IGNORE(instance);

   switch(signal)
   {
      case Hsm_Entry:
         SetHsmStateTo(instance, DefrostHsmState_HeaterOnEntry);
         break;

      case Hsm_Exit:
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

static bool State_Dwell(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   Defrost_t *instance = InstanceFromHsm(hsm);
   IGNORE(data);
   IGNORE(instance);

   switch(signal)
   {
      case Hsm_Entry:
         SetHsmStateTo(instance, DefrostHsmState_Dwell);
         break;

      case Hsm_Exit:
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

void Defrost_Init(
   Defrost_t *instance,
   I_DataModel_t *dataModel,
   const DefrostConfiguration_t *defrostConfig)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = defrostConfig;
   instance->_private.defrostParametricData = PersonalityParametricData_Get(dataModel)->defrostData;
   instance->_private.sabbathParametricData = PersonalityParametricData_Get(dataModel)->sabbathData;
   instance->_private.gridParametricData = PersonalityParametricData_Get(dataModel)->gridData;

   Hsm_Init(&instance->_private.hsm, &hsmConfiguration, State_PowerUp);
}