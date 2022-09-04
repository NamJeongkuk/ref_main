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
#include "CompressorVotedSpeed.h"
#include "Signal.h"
#include "uassert.h"

enum
{
   DontCare = false,
   Care = true
};

enum
{
   Signal_DefrostReadyTimerIsSatisfied = Hsm_UserSignalStart,
   Signal_CompressorIsOn,
   Signal_DefrostTimerExpired,
   Signal_PrechillTemperatureExitConditionMet,
   Signal_FreshFoodThermistorIsInvalid,
   Signal_FreezerEvaporatorThermistorIsInvalid,
   Signal_FreezerEvaporatorTemperatureReachedHeaterOnTerminationTemperature,
   Signal_FreezerHeaterMaxOnTimeReached,
   Signal_FreezerAbnormalHeaterOnTimeReached
};

static bool State_Idle(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_PrechillParent(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_PrechillPrep(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_Prechill(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_HeaterOnEntry(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_Dwell(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_HeaterOn(Hsm_t *hsm, HsmSignal_t signal, const void *data);

static const HsmStateHierarchyDescriptor_t stateList[] = {
   { State_Idle, HSM_NO_PARENT },
   { State_Dwell, HSM_NO_PARENT },
   { State_HeaterOnEntry, HSM_NO_PARENT },
   { State_HeaterOn, HSM_NO_PARENT },
   { State_PrechillParent, HSM_NO_PARENT },
   { State_PrechillPrep, State_PrechillParent },
   { State_Prechill, State_PrechillParent },
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

static uint8_t FreezerDefrostHeaterMaxOnTimeInMinutes(Defrost_t *instance)
{
   uint8_t maxOnTimeInMinutes;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerDefrostHeaterMaxOnTimeInMinutesErd,
      &maxOnTimeInMinutes);

   return maxOnTimeInMinutes;
}

static void DataModelChanged(void *context, const void *args)
{
   Defrost_t *instance = context;
   const DataModelOnDataChangeArgs_t *onChangeData = args;
   Erd_t erd = onChangeData->erd;

   if(erd == instance->_private.config->defrostReadyTimerIsSatisfied)
   {
      const bool *state = onChangeData->data;

      if(*state)
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_DefrostReadyTimerIsSatisfied, NULL);
      }
   }
   else if(erd == instance->_private.config->compressorIsOnErd)
   {
      const bool *state = onChangeData->data;

      if(*state)
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_CompressorIsOn, NULL);
      }
   }
   else if(erd == instance->_private.config->freshFoodThermistorIsValidErd)
   {
      const bool *freshFoodThermistorIsValid = onChangeData->data;

      if(!(*freshFoodThermistorIsValid))
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_FreshFoodThermistorIsInvalid, NULL);
      }
   }
   else if(erd == instance->_private.config->freezerEvaporatorThermistorIsValidErd)
   {
      const bool *freezerEvaporatorThermistorIsValid = onChangeData->data;

      if(!(*freezerEvaporatorThermistorIsValid))
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_FreezerEvaporatorThermistorIsInvalid, NULL);
      }
   }
   else if(erd == instance->_private.config->freezerEvaporatorFilteredTemperatureResolvedErd)
   {
      const TemperatureDegFx100_t *freezerEvaporatorTemperature = onChangeData->data;

      if(*freezerEvaporatorTemperature <= instance->_private.defrostParametricData->prechillFreezerEvapExitTemperatureInDegFx100)
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_PrechillTemperatureExitConditionMet, NULL);
      }
      else if(*freezerEvaporatorTemperature >= instance->_private.defrostParametricData->freezerDefrostTerminationTemperatureInDegFx100)
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_FreezerEvaporatorTemperatureReachedHeaterOnTerminationTemperature, NULL);
      }
   }
   else if(erd == instance->_private.config->freezerFilteredTemperatureResolvedErd)
   {
      const TemperatureDegFx100_t *freezerTemperature = onChangeData->data;

      if(*freezerTemperature <= instance->_private.defrostParametricData->prechillFreezerMinTempInDegFx100)
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_PrechillTemperatureExitConditionMet, NULL);
      }
   }
   else if(erd == instance->_private.config->freshFoodFilteredTemperatureResolvedErd)
   {
      const TemperatureDegFx100_t *freshFoodTemperature = onChangeData->data;

      if(*freshFoodTemperature <= instance->_private.defrostParametricData->prechillFreshFoodMinTempInDegFx100 ||
         *freshFoodTemperature >= instance->_private.defrostParametricData->prechillFreshFoodMaxTempInDegFx100)
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_PrechillTemperatureExitConditionMet, NULL);
      }
   }
   else if(erd == instance->_private.config->freezerDefrostHeaterOnTimeInMinutesErd)
   {
      const uint8_t *freezerDefrostHeaterOnTimeInMinutes = onChangeData->data;

      if(*freezerDefrostHeaterOnTimeInMinutes >= FreezerDefrostHeaterMaxOnTimeInMinutes(instance))
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_FreezerHeaterMaxOnTimeReached, NULL);
      }
      else if(*freezerDefrostHeaterOnTimeInMinutes ==
         instance->_private.defrostParametricData->freezerHeaterOnTimeToSetAbnormalDefrostInMinutes)
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_FreezerAbnormalHeaterOnTimeReached, NULL);
      }
   }
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

static void StopTimer(Defrost_t *instance, Timer_t *timer)
{
   TimerModule_Stop(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         instance->_private.config->timerModuleErd),
      timer);
}

static bool FreshFoodDefrostWasAbnormal(Defrost_t *instance)
{
   bool defrostWasAbnormal;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freshFoodDefrostWasAbnormalErd,
      &defrostWasAbnormal);

   return defrostWasAbnormal;
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

static bool ConvertibleCompartmentDefrostWasAbnormal(Defrost_t *instance)
{
   bool defrostWasAbnormal;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->convertibleCompartmentDefrostWasAbnormalErd,
      &defrostWasAbnormal);

   return defrostWasAbnormal;
}

static bool AnyPreviousDefrostWasAbnormal(Defrost_t *instance)
{
   return (FreshFoodDefrostWasAbnormal(instance) ||
      FreezerDefrostWasAbnormal(instance) ||
      ConvertibleCompartmentDefrostWasAbnormal(instance));
}

static bool CompressorIsOn(Defrost_t *instance)
{
   bool compressorIsOn;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->compressorIsOnErd,
      &compressorIsOn);
   return compressorIsOn;
}

static bool FreezerCompartmentWasTooWarmOnPowerUp(Defrost_t *instance)
{
   bool state;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerFilteredTemperatureWasTooWarmOnPowerUpErd,
      &state);

   return state;
}

static void ResetFreezerCompartmentWasTooWarmOnPowerUp(Defrost_t *instance)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->freezerFilteredTemperatureWasTooWarmOnPowerUpErd,
      clear);
}

static bool FreezerEvaporatorThermistorIsValid(Defrost_t *instance)
{
   bool state;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerEvaporatorThermistorIsValidErd,
      &state);
   return state;
}

static bool FreshFoodThermistorIsValid(Defrost_t *instance)
{
   bool state;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freshFoodThermistorIsValidErd,
      &state);
   return state;
}

static uint8_t GetMaxPrechillTime(Defrost_t *instance)
{
   uint8_t maxPrechillTime;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->maxPrechillTimeInMinutesErd,
      &maxPrechillTime);

   return maxPrechillTime;
}

static uint16_t GetTimeThatPrechillConditionsAreMet(Defrost_t *instance)
{
   uint16_t timeThatPrechillConditionsAreMet;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->timeThatPrechillConditionsAreMetInMinutesErd,
      &timeThatPrechillConditionsAreMet);

   return timeThatPrechillConditionsAreMet;
}

static void DefrostTimerExpired(void *context)
{
   Defrost_t *instance = context;

   Hsm_SendSignal(&instance->_private.hsm, Signal_DefrostTimerExpired, NULL);
}

static void StartDefrostTimer(Defrost_t *instance, TimerTicks_t ticks)
{
   StartTimer(
      instance,
      &instance->_private.defrostTimer,
      ticks,
      DefrostTimerExpired);
}

static void VoteForPrechillLoads(Defrost_t *instance, bool care)
{
   CompressorVotedSpeed_t compressorVote = {
      .speed = instance->_private.defrostParametricData->prechillCompressorSpeed,
      .care = care
   };
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->compressorSpeedVoteErd,
      &compressorVote);

   FanVotedSpeed_t freezerFanVote = {
      .speed = instance->_private.defrostParametricData->prechillFreezerFanSpeed,
      .care = care
   };
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->freezerFanSpeedVoteErd,
      &freezerFanVote);

   DamperVotedPosition_t damperVote = {
      .position = instance->_private.defrostParametricData->prechillFreshFoodDamperPosition,
      .care = care
   };
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->freshFoodDamperPositionVoteErd,
      &damperVote);
}

static void VoteForHeaterOnEntryLoads(Defrost_t *instance, bool care)
{
   CompressorVotedSpeed_t compressorVote = {
      .speed = CompressorSpeed_Off,
      .care = care,
   };
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->compressorSpeedVoteErd,
      &compressorVote);

   FanVotedSpeed_t fanVote = {
      .speed = FanSpeed_Off,
      .care = care,
   };
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->condenserFanSpeedVoteErd,
      &fanVote);

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->freezerFanSpeedVoteErd,
      &fanVote);

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->iceCabinetFanSpeedVoteErd,
      &fanVote);
}

static void VoteForFreezerDefrostHeater(
   Defrost_t *instance,
   HeaterState_t state,
   bool care)
{
   HeaterVotedState_t heaterVote = {
      .state = state,
      .care = care
   };
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->freezerDefrostHeaterVoteErd,
      &heaterVote);
}

static void CheckIfPrechillTemperatureExitConditionMet(Defrost_t *instance)
{
   TemperatureDegFx100_t freezerEvaporatorTemperature;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerEvaporatorFilteredTemperatureResolvedErd,
      &freezerEvaporatorTemperature);

   TemperatureDegFx100_t freezerTemperature;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerFilteredTemperatureResolvedErd,
      &freezerTemperature);

   TemperatureDegFx100_t freshFoodTemperature;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freshFoodFilteredTemperatureResolvedErd,
      &freshFoodTemperature);

   if(freezerEvaporatorTemperature <= instance->_private.defrostParametricData->prechillFreezerEvapExitTemperatureInDegFx100 ||
      freezerTemperature <= instance->_private.defrostParametricData->prechillFreezerMinTempInDegFx100 ||
      freshFoodTemperature <= instance->_private.defrostParametricData->prechillFreshFoodMinTempInDegFx100 ||
      freshFoodTemperature >= instance->_private.defrostParametricData->prechillFreshFoodMaxTempInDegFx100)
   {
      Hsm_SendSignal(&instance->_private.hsm, Signal_PrechillTemperatureExitConditionMet, NULL);
   }
}

static void IncrementFreezerDefrostCycleCount(Defrost_t *instance)
{
   uint16_t freezerDefrostCycleCount;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerDefrostCycleCountErd,
      &freezerDefrostCycleCount);

   freezerDefrostCycleCount++;
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->freezerDefrostCycleCountErd,
      &freezerDefrostCycleCount);
}

static void SetFreezerDefrostWasAbnormal(Defrost_t *instance)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->freezerDefrostWasAbnormalErd,
      set);
}

static void ClearFreezerDefrostWasAbnormal(Defrost_t *instance)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->freezerDefrostWasAbnormalErd,
      clear);
}

static void IncrementNumberOfFreezerAbnormalDefrostCycleCount(Defrost_t *instance)
{
   uint16_t numberOfFreezerAbnormalDefrostCycleCount;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->numberOfFreezerAbnormalDefrostCycleCountErd,
      &numberOfFreezerAbnormalDefrostCycleCount);

   numberOfFreezerAbnormalDefrostCycleCount++;
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->numberOfFreezerAbnormalDefrostCycleCountErd,
      &numberOfFreezerAbnormalDefrostCycleCount);
}

static void SaveTheFreezerDefrostCountAsTheLastFreezerAbnormalDefrostCount(Defrost_t *instance)
{
   uint16_t freezerDefrostCycleCount;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerDefrostCycleCountErd,
      &freezerDefrostCycleCount);

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->freezerAbnormalDefrostCycleCountErd,
      &freezerDefrostCycleCount);
}

static bool FreezerDefrostHeaterOnTimeLessThanAbnormalDefrostTime(Defrost_t *instance)
{
   uint8_t freezerHeaterOnTimeInMinutes;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerDefrostHeaterOnTimeInMinutesErd,
      &freezerHeaterOnTimeInMinutes);

   return freezerHeaterOnTimeInMinutes <
      instance->_private.defrostParametricData->freezerHeaterOnTimeToSetAbnormalDefrostInMinutes;
}

static HsmState_t InitialState(Defrost_t *instance)
{
   bool freezerFilteredTemperatureTooWarmAtPowerUp;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerFilteredTemperatureWasTooWarmOnPowerUpErd,
      &freezerFilteredTemperatureTooWarmAtPowerUp);

   DefrostState_t defrostState;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->defrostStateErd,
      &defrostState);

   HsmState_t initialState = State_Idle;

   if(freezerFilteredTemperatureTooWarmAtPowerUp)
   {
      if(defrostState == DefrostState_HeaterOn)
      {
         initialState = State_Dwell;
      }
   }
   else
   {
      if(defrostState == DefrostState_Dwell)
      {
         initialState = State_Dwell;
      }
      else if(defrostState == DefrostState_Prechill || defrostState == DefrostState_HeaterOn)
      {
         initialState = State_HeaterOnEntry;
      }
   }

   return initialState;
}

static bool State_Idle(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   Defrost_t *instance = InstanceFromHsm(hsm);
   IGNORE(data);

   switch(signal)
   {
      case Hsm_Entry:
         SetHsmStateTo(instance, DefrostHsmState_Idle);
         break;

      case Signal_DefrostReadyTimerIsSatisfied:
         if(AnyPreviousDefrostWasAbnormal(instance) ||
            FreezerCompartmentWasTooWarmOnPowerUp(instance) ||
            !FreezerEvaporatorThermistorIsValid(instance))
         {
            if(FreezerCompartmentWasTooWarmOnPowerUp(instance))
            {
               ResetFreezerCompartmentWasTooWarmOnPowerUp(instance);
            }
            Hsm_Transition(hsm, State_HeaterOnEntry);
         }
         else
         {
            Hsm_Transition(hsm, State_PrechillPrep);
         }
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
   IGNORE(data);

   switch(signal)
   {
      case Hsm_Entry:
         break;

      case Signal_FreshFoodThermistorIsInvalid:
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

         if(!FreshFoodThermistorIsValid(instance))
         {
            Hsm_Transition(hsm, State_HeaterOnEntry);
         }
         else if(CompressorIsOn(instance))
         {
            Hsm_Transition(hsm, State_Prechill);
         }
         else
         {
            StartDefrostTimer(instance, instance->_private.defrostParametricData->maxPrechillPrepTimeInMinutes * MSEC_PER_MIN);
         }
         break;

      case Signal_CompressorIsOn:
      case Signal_DefrostTimerExpired:
         Hsm_Transition(hsm, State_Prechill);
         break;

      case Hsm_Exit:
         StopTimer(instance, &instance->_private.defrostTimer);
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

   switch(signal)
   {
      case Hsm_Entry:
         SetHsmStateTo(instance, DefrostHsmState_Prechill);
         VoteForPrechillLoads(instance, Care);

         if(GetTimeThatPrechillConditionsAreMet(instance) >= GetMaxPrechillTime(instance))
         {
            Hsm_Transition(hsm, State_HeaterOnEntry);
         }
         else
         {
            StartDefrostTimer(instance, TRUNCATE_UNSIGNED_SUBTRACTION((uint16_t)GetMaxPrechillTime(instance), GetTimeThatPrechillConditionsAreMet(instance)) * MSEC_PER_MIN);
         }

         CheckIfPrechillTemperatureExitConditionMet(instance);
         break;

      case Signal_PrechillTemperatureExitConditionMet:
      case Signal_DefrostTimerExpired:
         Hsm_Transition(hsm, State_HeaterOnEntry);
         break;

      case Hsm_Exit:
         VoteForPrechillLoads(instance, DontCare);
         StopTimer(instance, &instance->_private.defrostTimer);
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
         VoteForHeaterOnEntryLoads(instance, Care);
         StartDefrostTimer(instance, instance->_private.defrostParametricData->defrostHeaterOnDelayAfterCompressorOffInSeconds * MSEC_PER_SEC);
         break;

      case Signal_DefrostTimerExpired:
         Hsm_Transition(hsm, State_HeaterOn);
         break;

      case Hsm_Exit:
         StopTimer(instance, &instance->_private.defrostTimer);
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

static bool State_HeaterOn(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   Defrost_t *instance = InstanceFromHsm(hsm);
   IGNORE(data);

   switch(signal)
   {
      case Hsm_Entry:
         SetHsmStateTo(instance, DefrostHsmState_HeaterOn);
         VoteForFreezerDefrostHeater(instance, HeaterState_On, Care);
         break;

      case Signal_FreezerHeaterMaxOnTimeReached:
         Hsm_Transition(hsm, State_Dwell);
         break;

      case Signal_FreezerAbnormalHeaterOnTimeReached:
         IncrementNumberOfFreezerAbnormalDefrostCycleCount(instance);
         SetFreezerDefrostWasAbnormal(instance);
         SaveTheFreezerDefrostCountAsTheLastFreezerAbnormalDefrostCount(instance);
         break;

      case Signal_FreezerEvaporatorTemperatureReachedHeaterOnTerminationTemperature:
         if(FreezerDefrostHeaterOnTimeLessThanAbnormalDefrostTime(instance))
         {
            ClearFreezerDefrostWasAbnormal(instance);
         }

         Hsm_Transition(hsm, State_Dwell);
         break;

      case Hsm_Exit:
         VoteForFreezerDefrostHeater(instance, HeaterState_Off, Care);
         IncrementFreezerDefrostCycleCount(instance);
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
   bool freezerFilteredTemperatureTooWarmOnPowerUpReady;
   DataModel_Read(
      dataModel,
      defrostConfig->freezerFilteredTemperatureTooWarmOnPowerUpReadyErd,
      &freezerFilteredTemperatureTooWarmOnPowerUpReady);

   uassert(freezerFilteredTemperatureTooWarmOnPowerUpReady);

   instance->_private.dataModel = dataModel;
   instance->_private.config = defrostConfig;
   instance->_private.defrostParametricData = PersonalityParametricData_Get(dataModel)->defrostData;
   instance->_private.sabbathParametricData = PersonalityParametricData_Get(dataModel)->sabbathData;
   instance->_private.gridParametricData = PersonalityParametricData_Get(dataModel)->gridData;
   instance->_private.evaporatorParametricData = PersonalityParametricData_Get(dataModel)->evaporatorData;

   Hsm_Init(&instance->_private.hsm, &hsmConfiguration, InitialState(instance));

   EventSubscription_Init(
      &instance->_private.dataModelSubscription,
      instance,
      DataModelChanged);

   Event_Subscribe(
      dataModel->OnDataChange,
      &instance->_private.dataModelSubscription);
}
