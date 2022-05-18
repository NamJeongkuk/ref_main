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
#include "FanSpeed.h"
#include "Setpoint.h"
#include "CompressorState.h"

enum
{
   PowerUpFactor = 5
};

enum
{
   Signal_PowerUpDelayComplete = Hsm_UserSignalStart,
   Signal_MaxTimeBetweenDefrostsComplete,
   Signal_PeriodicTimeoutComplete,
   Signal_MaxPrechillHoldoffTimeComplete,
   Signal_FreezerEvaporatorThermistorIsInvalid,
   Signal_FreezerDefrostIsAbnormal,
   Signal_CompressorStateTimeIsSatisfied,
   Signal_ValveHasBeenInPositionForPrechillTime,
   Signal_FreezerEvaporatorTemperatureIsBelowPrechillFreezerEvaporatorExitTemperature
};

static bool State_PowerUp(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_Idle(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_PrechillParent(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_PrechillPrep(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_Prechill(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_PostPrechill(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_HeaterOnEntry(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_Dwell(Hsm_t *hsm, HsmSignal_t signal, const void *data);

static const HsmStateHierarchyDescriptor_t stateList[] = {
   { State_PowerUp, HSM_NO_PARENT },
   { State_Idle, HSM_NO_PARENT },
   { State_Dwell, HSM_NO_PARENT },
   { State_PrechillParent, HSM_NO_PARENT },
   { State_PrechillPrep, State_PrechillParent },
   { State_Prechill, State_PrechillParent },
   { State_PostPrechill, State_PrechillParent },
   { State_HeaterOnEntry, State_PrechillParent }
};

static const HsmConfiguration_t hsmConfiguration = {
   stateList,
   NUM_ELEMENTS(stateList)
};

static Defrost_t *InstanceFromHsm(Hsm_t *hsm)
{
   return CONTAINER_OF(Defrost_t, _private.hsm, hsm);
}

static void SetHsmStateTo(Defrost_t *instance, DefrostHsmState_t state)
{
   DataModel_Write(instance->_private.dataModel, instance->_private.config->defrostHsmStateErd, &state);
}

static bool CompressorStateTimeIsSatisfiedFor(CompressorState_t state)
{
   return ((state != CompressorState_MinimumOffTime) &&
      (state != CompressorState_MinimumOnTime) &&
      (state != CompressorState_MinimumRunTime));
}

static uint8_t MaxPrechillTimeInMinutes(Defrost_t *instance)
{
   uint8_t maxPrechillTimeInMinutes;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->maxPrechillTimeInMinutesErd,
      &maxPrechillTimeInMinutes);

   return maxPrechillTimeInMinutes;
}

static void DataModelChanged(void *context, const void *args)
{
   REINTERPRET(instance, context, Defrost_t *);
   REINTERPRET(onChangeData, args, const DataModelOnDataChangeArgs_t *);
   REINTERPRET(erd, onChangeData->erd, Erd_t);

   if(erd == instance->_private.config->freezerEvaporatorThermistorIsValidErd)
   {
      REINTERPRET(valid, onChangeData->data, const bool *);

      if(!*valid)
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_FreezerEvaporatorThermistorIsInvalid, NULL);
      }
   }
   else if(erd == instance->_private.config->freezerDefrostWasAbnormalErd)
   {
      REINTERPRET(state, onChangeData->data, const bool *);

      if(*state)
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_FreezerDefrostIsAbnormal, NULL);
      }
   }
   else if(erd == instance->_private.config->compressorStateErd)
   {
      REINTERPRET(state, onChangeData->data, const CompressorState_t *);

      if(CompressorStateTimeIsSatisfiedFor(*state))
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_CompressorStateTimeIsSatisfied, NULL);
      }
   }
   else if(erd == instance->_private.config->timeInMinutesInValvePositionBErd)
   {
      if(instance->_private.defrostParametricData->threeWayValvePositionToCountAsPrechillTime == ValvePosition_B)
      {
         REINTERPRET(minutes, onChangeData->data, const uint16_t *);

         if(*minutes >= MaxPrechillTimeInMinutes(instance))
         {
            Hsm_SendSignal(&instance->_private.hsm, Signal_ValveHasBeenInPositionForPrechillTime, NULL);
         }
      }
   }
   else if(erd == instance->_private.config->freezerEvaporatorFilteredTemperatureErd)
   {
      REINTERPRET(temperatureDegFx100, onChangeData->data, const TemperatureDegFx100_t *);

      if(*temperatureDegFx100 < instance->_private.defrostParametricData->prechillFreezerEvapExitTemperatureInDegFx100)
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_FreezerEvaporatorTemperatureIsBelowPrechillFreezerEvaporatorExitTemperature, NULL);
      }
   }
}

static void SaveFreezerAbnormalDefrostData(Defrost_t *instance)
{
   bool freezerDefrostWasAbnormal;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerDefrostWasAbnormalErd,
      &freezerDefrostWasAbnormal);

   uint16_t freezerAbnormalDefrostCycleCount;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerAbnormalDefrostCycleCountErd,
      &freezerAbnormalDefrostCycleCount);

   uint16_t freezerDefrostCycleCount;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerDefrostCycleCountErd,
      &freezerDefrostCycleCount);

   if(freezerAbnormalDefrostCycleCount != freezerDefrostCycleCount)
   {
      uint16_t numberOfFreezerAbnormalDefrostCycles;
      DataModel_Read(
         instance->_private.dataModel,
         instance->_private.config->numberOfFreezerAbnormalDefrostCyclesErd,
         &numberOfFreezerAbnormalDefrostCycles);

      numberOfFreezerAbnormalDefrostCycles++;
      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->numberOfFreezerAbnormalDefrostCyclesErd,
         &numberOfFreezerAbnormalDefrostCycles);
   }

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->freezerAbnormalDefrostCycleCountErd,
      &freezerDefrostCycleCount);

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->freezerDefrostWasAbnormalErd,
      set);
}

static bool LastDefrostWasAbnormalBecauseOfAbnormalFilteredFreezerCabinetTemperature(Defrost_t *instance)
{
   TemperatureDegFx100_t freezerFilteredTemperature;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerFilteredTemperatureErd,
      &freezerFilteredTemperature);

   CalculatedGridLines_t calcGridLines;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->calculatedGridLinesErd,
      &calcGridLines);

   TemperatureDegFx100_t gridFreezerExtremeHystTemperature = calcGridLines.gridLines[GridDelta_Freezer].gridLinesDegFx100[GridLine_FreezerExtremeHigh];

   return (freezerFilteredTemperature > gridFreezerExtremeHystTemperature ||
      freezerFilteredTemperature >= instance->_private.defrostParametricData->freezerDefrostTerminationTemperatureInDegFx100);
}

static void StartTimer(Defrost_t *instance, Timer_t *timer, TimerTicks_t ticks, TimerCallback_t callback)
{
   TimerModule_StartOneShot(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         instance->_private.config->timerModuleErd),
      timer,
      ticks,
      callback,
      instance);
}

static void VoteForFreshFoodDefrostHeater(Defrost_t *instance, bool state)
{
   HeaterVotedState_t vote;
   vote.state = state;
   vote.care = true;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->freshFoodDefrostHeaterDefrostVoteErd,
      &vote);
}

static void VoteForFreezerDefrostHeater(Defrost_t *instance, bool state)
{
   HeaterVotedState_t vote;
   vote.state = state;
   vote.care = true;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->freezerDefrostHeaterDefrostVoteErd,
      &vote);
}

static void VoteForIceCabinetFan(Defrost_t *instance, FanSpeed_t speed)
{
   FanVotedSpeed_t vote;
   vote.speed = speed;
   vote.care = true;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->iceCabinetFanDefrostVoteErd,
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

static bool FreezerDefrostWasAbnormal(Defrost_t *instance)
{
   bool defrostWasAbnormal;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerDefrostWasAbnormalErd,
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
}

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

static void PeriodicTimeoutComplete(void *context)
{
   REINTERPRET(instance, context, Defrost_t *);

   Hsm_SendSignal(&instance->_private.hsm, Signal_PeriodicTimeoutComplete, NULL);
}

static void StartPeriodicTimer(Defrost_t *instance)
{
   TimerModule_StartPeriodic(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         instance->_private.config->timerModuleErd),
      &instance->_private.periodicTimer,
      instance->_private.defrostParametricData->defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC,
      PeriodicTimeoutComplete,
      instance);
}

static void StartOneMinutePeriodicTimer(Defrost_t *instance)
{
   TimerModule_StartPeriodic(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         instance->_private.config->timerModuleErd),
      &instance->_private.periodicTimer,
      1 * MSEC_PER_MIN,
      PeriodicTimeoutComplete,
      instance);
}

static void StopOneMinutePeriodicTimer(Defrost_t *instance)
{
   TimerModule_Stop(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         instance->_private.config->timerModuleErd),
      &instance->_private.periodicTimer);
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

static void StartMaxPrechillHoldoffTimer(Defrost_t *instance)
{
   StartTimer(
      instance,
      &instance->_private.maxPrechillHoldoffTimer,
      instance->_private.defrostParametricData->maxPrechillHoldoffTimeAfterDefrostTimerSatisfiedInSeconds * MSEC_PER_SEC,
      MaxPrechillHoldoffTimeComplete);
}

static bool MaxPrechillHoldoffTimerIsNotRunning(Defrost_t *instance)
{
   return !TimerModule_IsRunning(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         instance->_private.config->timerModuleErd),
      &instance->_private.maxPrechillHoldoffTimer);
}

static void StopMaxPrechillHoldoffTimer(Defrost_t *instance)
{
   TimerModule_Stop(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         instance->_private.config->timerModuleErd),
      &instance->_private.maxPrechillHoldoffTimer);
}

static ValvePosition_t PositionToExitIdle(Defrost_t *instance)
{
   return instance->_private.defrostParametricData->threeWayValvePositionToExitIdle;
}

static bool FreezerEvaporatorThermistorIsInvalid(Defrost_t *instance)
{
   bool state;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerEvaporatorThermistorIsValidErd,
      &state);

   return !state;
}

static void IncrementNumberOfFreshFoodDefrostsBeforeAFreezerDefrost(Defrost_t *instance)
{
   uint8_t number;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->numberOfFreshFoodDefrostsBeforeAFreezerDefrostErd,
      &number);

   number++;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->numberOfFreshFoodDefrostsBeforeAFreezerDefrostErd,
      &number);
}

static uint8_t NumberOfEvaporators(Defrost_t *instance)
{
   return instance->_private.evaporatorParametricData->numberOfEvaporators;
}

static void VoteForFreezerSetpoint(Defrost_t *instance)
{
   SetpointVotedTemperature_t resolvedSetPoint;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerResolvedSetpointErd,
      &resolvedSetPoint);

   SetpointVotedTemperature_t vote;
   vote.care = true;
   vote.temperature = instance->_private.defrostParametricData->prechillFreezerSetpointInDegFx100;

   if(NumberOfEvaporators(instance) == 1)
   {
      if(resolvedSetPoint.temperature < instance->_private.defrostParametricData->prechillFreezerSetpointInDegFx100)
      {
         vote.temperature = resolvedSetPoint.temperature;
      }
   }

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->freezerSetpointDefrostVoteErd,
      &vote);
}

static bool MaxPrechillHoldoffTimeGreaterThanZero(Defrost_t *instance)
{
   return instance->_private.defrostParametricData->maxPrechillHoldoffTimeAfterDefrostTimerSatisfiedInSeconds > 0;
}

static void VoteForMaxPrechillHoldoffValvePosition(Defrost_t *instance)
{
   ValveVotedPosition_t vote;
   vote.position = instance->_private.defrostParametricData->threeWayValvePositionForMaxPrechillHoldoff;
   vote.care = true;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->sealedSystemValvePositionDefrostVoteErd,
      &vote);
}

static bool CurrentDefrostIsNotFreshFoodOnly(Defrost_t *instance)
{
   bool freshFoodOnly;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->defrostIsFreshFoodOnlyErd,
      &freshFoodOnly);

   return !freshFoodOnly;
}

static void VoteForFreshFoodSetpoint(Defrost_t *instance)
{
   SetpointVotedTemperature_t vote;
   vote.care = true;
   vote.temperature = instance->_private.defrostParametricData->prechillFreshFoodSetpointInDegFx100;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->freshFoodSetpointDefrostVoteErd,
      &vote);
}

static bool ValveIsInPositionToExtendDefrost(Defrost_t *instance)
{
   ValvePosition_t position;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->sealedSystemValvePositionErd,
      &position);

   return position == instance->_private.defrostParametricData->threeWayValvePositionToExtendDefrostWithFreshFoodCycleDefrost;
}

static void SendExtendDefrostSignal(Defrost_t *instance)
{
   Signal_SendViaErd(
      DataModel_AsDataSource(instance->_private.dataModel),
      instance->_private.config->extendDefrostSignalErd);
}

static void SendDoorHoldoffEnableRequest(Defrost_t *instance)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->defrostDoorHoldoffRequestErd,
      enabled);
}

static void SendDoorHoldoffDisableRequest(Defrost_t *instance)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->defrostDoorHoldoffRequestErd,
      disabled);
}

static bool CompressorStateTimeIsSatisfied(Defrost_t *instance)
{
   CompressorState_t state;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->compressorStateErd,
      &state);

   return CompressorStateTimeIsSatisfiedFor(state);
}

static void ResetPrechillRunCounterInMinutesToZero(Defrost_t *instance)
{
   uint16_t prechillRunCounterInMinutes = 0;
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->defrostPrechillRunCounterInMinutesErd,
      &prechillRunCounterInMinutes);
}

static void SetDefrostMaxHoldoffMetTo(Defrost_t *instance, bool state)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->defrostMaxHoldoffMetErd,
      &state);
}

static void SetPrechillTimeMetTo(Defrost_t *instance, bool state)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->prechillTimeMetErd,
      &state);
}

static uint16_t IncrementAndReturnPrechillRunCounterInMinutes(Defrost_t *instance)
{
   uint16_t prechillRunCounterInMinutes;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->defrostPrechillRunCounterInMinutesErd,
      &prechillRunCounterInMinutes);

   prechillRunCounterInMinutes++;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->defrostPrechillRunCounterInMinutesErd,
      &prechillRunCounterInMinutes);

   return prechillRunCounterInMinutes;
}

static bool TimeReachedDefrostMaxHoldoffTimeInMinutes(Defrost_t *instance, uint16_t prechillRunCounterInMinutes)
{
   return prechillRunCounterInMinutes >=
      instance->_private.defrostParametricData->defrostMaxHoldoffTimeInMinutes;
}

static bool TimeReachedMaxPrechillTimeInMinutes(Defrost_t *instance, uint16_t prechillRunCounterInMinutes)
{
   return prechillRunCounterInMinutes >= MaxPrechillTimeInMinutes(instance);
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

         if(LastDefrostWasAbnormalBecauseOfAbnormalFilteredFreezerCabinetTemperature(instance))
         {
            SetFlagToResetRequiredWhenEnablingDefrostTimerCounterTo(instance, true);
            SaveFreezerAbnormalDefrostData(instance);

            (lastDefrostState == DefrostState_HeaterOn) ? Hsm_Transition(hsm, State_Dwell) : Hsm_Transition(hsm, State_Idle);
         }
         else
         {
            if(lastDefrostState == DefrostState_Prechill)
            {
               (FreezerDefrostWasAbnormal(instance)) ? Hsm_Transition(hsm, State_PostPrechill) : Hsm_Transition(hsm, State_PrechillPrep);
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

   switch(signal)
   {
      case Hsm_Entry:
         SetHsmStateTo(instance, DefrostHsmState_Idle);
         SendDoorHoldoffEnableRequest(instance);
         VoteForFreshFoodDefrostHeater(instance, OFF);
         VoteForFreezerDefrostHeater(instance, OFF);

         if(DefrostTimerCounterIsDisabled(instance))
         {
            RequestDefrostTimerCounterTo(instance, DefrostTimer_Enable);

            if(DefrostTimerCounterResetRequiredFromPowerUp(instance))
            {
               RequestDefrostTimerCounterTo(instance, DefrostTimer_Reset);
               SetFlagToResetRequiredWhenEnablingDefrostTimerCounterTo(instance, false);
            }
         }

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
               if(MaxPrechillHoldoffTimerIsNotRunning(instance))
               {
                  StartMaxPrechillHoldoffTimer(instance);
               }
            }
         }
         break;

      case Signal_MaxPrechillHoldoffTimeComplete:
         Hsm_Transition(hsm, State_PrechillPrep);
         break;

      case Hsm_Exit:
         StopMaxPrechillHoldoffTimer(instance);
         RequestDefrostTimerCounterTo(instance, DefrostTimer_Disable);
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

static bool State_PrechillParent(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   Defrost_t *instance = InstanceFromHsm(hsm);
   IGNORE(instance);
   IGNORE(data);

   switch(signal)
   {
      case Hsm_Entry:
         break;

      case Signal_FreezerEvaporatorThermistorIsInvalid:
         Hsm_Transition(hsm, State_HeaterOnEntry);
         break;

      case Hsm_Exit:
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

   switch(signal)
   {
      case Hsm_Entry:
         SetHsmStateTo(instance, DefrostHsmState_PrechillPrep);
         if(FreezerEvaporatorThermistorIsInvalid(instance))
         {
            Hsm_Transition(hsm, State_HeaterOnEntry);
         }
         else
         {
            IncrementNumberOfFreshFoodDefrostsBeforeAFreezerDefrost(instance);
            VoteForFreshFoodDefrostHeater(instance, OFF);
            VoteForFreezerDefrostHeater(instance, OFF);
            VoteForIceCabinetFan(instance, FanSpeed_High);
            VoteForFreezerSetpoint(instance);
            SendDoorHoldoffEnableRequest(instance);

            if(MaxPrechillHoldoffTimeGreaterThanZero(instance))
            {
               VoteForMaxPrechillHoldoffValvePosition(instance);
            }

            if(CurrentDefrostIsNotFreshFoodOnly(instance))
            {
               VoteForFreshFoodSetpoint(instance);
            }

            if(ValveIsInPositionToExtendDefrost(instance))
            {
               SendExtendDefrostSignal(instance);
            }

            if(FreezerDefrostWasAbnormal(instance))
            {
               Hsm_Transition(hsm, State_PostPrechill);
            }
            else
            {
               if(CompressorStateTimeIsSatisfied(instance))
               {
                  Hsm_Transition(hsm, State_Prechill);
               }
            }
         }
         break;

      case Signal_FreezerDefrostIsAbnormal:
         Hsm_Transition(hsm, State_PostPrechill);
         break;

      case Signal_CompressorStateTimeIsSatisfied:
         Hsm_Transition(hsm, State_Prechill);
         break;

      case Hsm_Exit:
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

static bool State_Prechill(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   Defrost_t *instance = InstanceFromHsm(hsm);
   IGNORE(data);
   uint16_t prechillRunCounterInMinutes;

   switch(signal)
   {
      case Hsm_Entry:
         SetHsmStateTo(instance, DefrostHsmState_Prechill);
         ResetPrechillRunCounterInMinutesToZero(instance);
         SetDefrostMaxHoldoffMetTo(instance, CLEAR);
         StartOneMinutePeriodicTimer(instance);
         break;

      case Signal_PeriodicTimeoutComplete:
         prechillRunCounterInMinutes = IncrementAndReturnPrechillRunCounterInMinutes(instance);

         if(TimeReachedDefrostMaxHoldoffTimeInMinutes(instance, prechillRunCounterInMinutes))
         {
            SetDefrostMaxHoldoffMetTo(instance, SET);
         }

         if(TimeReachedMaxPrechillTimeInMinutes(instance, prechillRunCounterInMinutes))
         {
            SetPrechillTimeMetTo(instance, SET);
         }
         break;

      case Signal_ValveHasBeenInPositionForPrechillTime:
         if(instance->_private.defrostParametricData->threeWayValveTimePriorToPrechillCountsAsPrechillTime)
         {
            SetPrechillTimeMetTo(instance, SET);
         }
         break;

      case Signal_FreezerEvaporatorTemperatureIsBelowPrechillFreezerEvaporatorExitTemperature:
         Hsm_Transition(hsm, State_PostPrechill);
         break;

      case Hsm_Exit:
         StopOneMinutePeriodicTimer(instance);
         SendDoorHoldoffDisableRequest(instance);
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
   instance->_private.evaporatorParametricData = PersonalityParametricData_Get(dataModel)->evaporatorData;

   Hsm_Init(&instance->_private.hsm, &hsmConfiguration, State_PowerUp);

   EventSubscription_Init(
      &instance->_private.dataModelSubscription,
      instance,
      DataModelChanged);

   Event_Subscribe(
      dataModel->OnDataChange,
      &instance->_private.dataModelSubscription);
}
