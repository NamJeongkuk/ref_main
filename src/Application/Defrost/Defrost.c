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
#include "DataModelErdPointerAccess.h"
#include "DefrostState.h"
#include "HeaterVotedState.h"
#include "utils.h"
#include "FanSpeed.h"
#include "Setpoint.h"
#include "CompressorState.h"
#include "Signal.h"

enum
{
   PowerUpFactor = 5
};

enum
{
   DontCare = false,
   Care = true
};

enum
{
   Signal_PowerUpDelayComplete = Hsm_UserSignalStart,
   Signal_PeriodicTimeoutComplete,
   Signal_FreezerEvaporatorThermistorIsInvalid,
   Signal_FreezerDefrostIsAbnormal,
   Signal_ValveHasBeenInPositionForPrechillTime,
   Signal_FreezerEvaporatorTemperatureIsBelowPrechillFreezerEvaporatorExitTemperature,
   Signal_NoFreezeLimitIsActive,
   Signal_FreezerCabinetTemperatureIsBelowAdjustedFreezerSetpoint,
   Signal_PrechillTimeIsMet,
   Signal_DoorHoldoffTimeIsMet,
   Signal_MaxHoldoffTimeIsMet,
   Signal_CompressorStateIsOff,
   Signal_CompressorStateIsOn,
   Signal_CompressorStateIsMinimumOnTime,
   Signal_CompressorStateIsMinimumOffTime,
   Signal_CompressorStateIsMinimumRunTime
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
   { State_HeaterOnEntry, HSM_NO_PARENT }
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

      switch(*state)
      {
         case CompressorState_Off:
            Hsm_SendSignal(&instance->_private.hsm, Signal_CompressorStateIsOff, NULL);
            break;

         case CompressorState_On:
            Hsm_SendSignal(&instance->_private.hsm, Signal_CompressorStateIsOn, NULL);
            break;

         case CompressorState_MinimumOnTime:
            Hsm_SendSignal(&instance->_private.hsm, Signal_CompressorStateIsMinimumOnTime, NULL);
            break;

         case CompressorState_MinimumOffTime:
            Hsm_SendSignal(&instance->_private.hsm, Signal_CompressorStateIsMinimumOffTime, NULL);
            break;

         case CompressorState_MinimumRunTime:
            Hsm_SendSignal(&instance->_private.hsm, Signal_CompressorStateIsMinimumRunTime, NULL);
            break;
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
   else if(erd == instance->_private.config->noFreezeLimitIsActiveErd)
   {
      REINTERPRET(state, onChangeData->data, const bool *);

      if(*state)
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_NoFreezeLimitIsActive, NULL);
      }
   }
   else if(erd == instance->_private.config->freezerFilteredTemperatureResolvedErd)
   {
      REINTERPRET(temperatureDegFx100, onChangeData->data, const TemperatureDegFx100_t *);

      TemperatureDegFx100_t adjustedFreezerSetpointDegFx100;
      DataModel_Read(
         instance->_private.dataModel,
         instance->_private.config->adjustedFreezerSetpointErd,
         &adjustedFreezerSetpointDegFx100);

      if(*temperatureDegFx100 < adjustedFreezerSetpointDegFx100)
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_FreezerCabinetTemperatureIsBelowAdjustedFreezerSetpoint, NULL);
      }
   }
   else if(erd == instance->_private.config->prechillTimeMetErd)
   {
      REINTERPRET(state, onChangeData->data, const bool *);

      if(*state)
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_PrechillTimeIsMet, NULL);
      }
   }
   else if(erd == instance->_private.config->doorHoldoffTimeIsSatisfiedErd)
   {
      REINTERPRET(state, onChangeData->data, const bool *);

      if(*state)
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_DoorHoldoffTimeIsMet, NULL);
      }
   }
   else if(erd == instance->_private.config->defrostMaxHoldoffMetErd)
   {
      REINTERPRET(state, onChangeData->data, const bool *);

      if(*state)
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_MaxHoldoffTimeIsMet, NULL);
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
      instance->_private.config->freezerFilteredTemperatureResolvedErd,
      &freezerFilteredTemperature);

   CalculatedGridLines_t calcGridLines;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->calculatedGridLinesErd,
      &calcGridLines);

   TemperatureDegFx100_t gridFreezerExtremeHystTemperature = calcGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerExtremeHigh];

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

static void VoteForFreshFoodDefrostHeater(Defrost_t *instance, bool state, bool care)
{
   HeaterVotedState_t vote;
   vote.state = state;
   vote.care = care;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->freshFoodDefrostHeaterDefrostVoteErd,
      &vote);
}

static void VoteForFreezerDefrostHeater(Defrost_t *instance, bool state, bool care)
{
   HeaterVotedState_t vote;
   vote.state = state;
   vote.care = care;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->freezerDefrostHeaterDefrostVoteErd,
      &vote);
}

static void VoteForIceCabinetFan(Defrost_t *instance, FanSpeed_t speed, bool care)
{
   FanVotedSpeed_t vote;
   vote.speed = speed;
   vote.care = care;

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

static void PeriodicTimeoutComplete(void *context)
{
   REINTERPRET(instance, context, Defrost_t *);

   Hsm_SendSignal(&instance->_private.hsm, Signal_PeriodicTimeoutComplete, NULL);
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

static bool CompressorStateIsNotMinimumOnTime(Defrost_t *instance)
{
   CompressorState_t state;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->compressorStateErd,
      &state);

   return (state != CompressorState_MinimumOnTime);
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

static bool DoorHoldoffOrMaxHoldoffAreMet(Defrost_t *instance)
{
   bool doorHoldoffMet;
   bool maxHoldoffMet;

   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->doorHoldoffTimeIsSatisfiedErd,
      &doorHoldoffMet);
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->defrostMaxHoldoffMetErd,
      &maxHoldoffMet);

   return doorHoldoffMet || maxHoldoffMet;
}

static bool PrechillTimeIsMet(Defrost_t *instance)
{
   bool state;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->prechillTimeMetErd,
      &state);

   return state;
}

static bool NoFreezeLimitIsActive(Defrost_t *instance)
{
   bool state;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->noFreezeLimitIsActiveErd,
      &state);

   return state;
}

static bool CompressorIsOff(Defrost_t *instance)
{
   bool state;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->compressorStateErd,
      &state);

   return (state == CompressorState_Off);
}

static void VoteForFreezerEvapFan(Defrost_t *instance, FanSpeed_t speed, bool care)
{
   FanVotedSpeed_t vote;
   vote.speed = speed;
   vote.care = care;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->freezerEvapFanDefrostVoteErd,
      &vote);
}

static void VoteForFreshFoodEvapFan(Defrost_t *instance, FanSpeed_t speed, bool care)
{
   FanVotedSpeed_t vote;
   vote.speed = speed;
   vote.care = care;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->freshFoodEvapFanDefrostVoteErd,
      &vote);
}

static void VoteDontCareForValvePosition(Defrost_t *instance)
{
   ValveVotedPosition_t vote;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->sealedSystemValvePositionDefrostVoteErd,
      &vote);
   vote.care = false;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->sealedSystemValvePositionDefrostVoteErd,
      &vote);
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
   IGNORE(instance);

   switch(signal)
   {
      case Hsm_Entry:
         SetHsmStateTo(instance, DefrostHsmState_Idle);
         break;

      case Hsm_Exit:
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
         VoteForFreshFoodDefrostHeater(instance, OFF, Care);
         VoteForFreezerDefrostHeater(instance, OFF, Care);
         VoteForIceCabinetFan(instance, FanSpeed_High, Care);
         break;

      case Signal_FreezerEvaporatorThermistorIsInvalid:
         Hsm_Transition(hsm, State_HeaterOnEntry);
         break;

      case Signal_FreezerDefrostIsAbnormal:
         Hsm_Transition(hsm, State_PostPrechill);
         break;

      case Hsm_Exit:
         VoteForFreezerDefrostHeater(instance, OFF, DontCare);
         VoteForFreshFoodDefrostHeater(instance, OFF, DontCare);
         VoteForIceCabinetFan(instance, FanSpeed_Off, DontCare);
         VoteForFreezerEvapFan(instance, FanSpeed_Off, DontCare);
         VoteForFreshFoodEvapFan(instance, FanSpeed_Off, DontCare);
         VoteDontCareForValvePosition(instance);
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
            else if(CompressorStateTimeIsSatisfied(instance))
            {
               Hsm_Transition(hsm, State_Prechill);
            }
         }
         break;

      case Signal_CompressorStateIsOff:
      case Signal_CompressorStateIsOn:
      case Signal_CompressorStateIsMinimumOffTime:
      case Signal_CompressorStateIsMinimumRunTime:
         if(CompressorStateTimeIsSatisfied(instance))
         {
            Hsm_Transition(hsm, State_Prechill);
         }
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

         if(NumberOfEvaporators(instance) == 1)
         {
            if(CompressorIsOff(instance))
            {
               Hsm_Transition(hsm, State_PostPrechill);
            }
            else if(NoFreezeLimitIsActive(instance))
            {
               Hsm_Transition(hsm, State_HeaterOnEntry);
            }
         }
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

      case Signal_NoFreezeLimitIsActive:
         if(NumberOfEvaporators(instance) == 1)
         {
            Hsm_Transition(hsm, State_HeaterOnEntry);
         }
         break;

      case Signal_PrechillTimeIsMet:
         if(NumberOfEvaporators(instance) == 1)
         {
            Hsm_Transition(hsm, State_PostPrechill);
         }
         else if(NumberOfEvaporators(instance) == 2)
         {
            if(DoorHoldoffOrMaxHoldoffAreMet(instance))
            {
               Hsm_Transition(hsm, State_PostPrechill);
            }
         }
         break;

      case Signal_DoorHoldoffTimeIsMet:
      case Signal_MaxHoldoffTimeIsMet:
         if(NumberOfEvaporators(instance) == 2)
         {
            if(PrechillTimeIsMet(instance))
            {
               Hsm_Transition(hsm, State_PostPrechill);
            }
         }
         break;

      case Signal_FreezerCabinetTemperatureIsBelowAdjustedFreezerSetpoint:
         if(NumberOfEvaporators(instance) == 2)
         {
            Hsm_Transition(hsm, State_PostPrechill);
         }
         break;

      case Signal_CompressorStateIsOff:
         if(NumberOfEvaporators(instance) == 1)
         {
            Hsm_Transition(hsm, State_HeaterOnEntry);
         }
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

         if(MaxPrechillHoldoffTimeGreaterThanZero(instance))
         {
            VoteForMaxPrechillHoldoffValvePosition(instance);
         }

         if(NumberOfEvaporators(instance) == 1)
         {
            VoteForFreezerEvapFan(instance, FanSpeed_Low, Care);
            VoteForFreshFoodEvapFan(instance, FanSpeed_Off, Care);
         }

         if(CompressorStateIsNotMinimumOnTime(instance))
         {
            Hsm_Transition(hsm, State_HeaterOnEntry);
         }
         break;

      case Signal_CompressorStateIsOff:
      case Signal_CompressorStateIsOn:
      case Signal_CompressorStateIsMinimumOffTime:
      case Signal_CompressorStateIsMinimumRunTime:
         Hsm_Transition(hsm, State_HeaterOnEntry);
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
