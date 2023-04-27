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
#include "Vote.h"
#include "DefrostType.h"
#include "CoolingMode.h"
#include "DefrostTestStateRequestMessage.h"

enum
{
   Signal_ReadyToDefrost = Hsm_UserSignalStart,
   Signal_PrechillConditionsAreMet,
   Signal_DefrostTimerExpired,
   Signal_PrechillTemperatureExitConditionMet,
   Signal_FreshFoodThermistorIsInvalid,
   Signal_FreezerThermistorIsInvalid,
   Signal_FreezerEvaporatorThermistorIsInvalid,
   Signal_FreezerHeaterMaxOnTimeReached,
   Signal_FreezerAbnormalHeaterOnTimeReached,
   Signal_DisableDefrost,
   Signal_EnableDefrost,
   Signal_IdleTestRequest,
   Signal_PrechillTestRequest,
   Signal_DefrostTestRequest,
   Signal_FreezerEvaporatorTemperatureChanged
};

static bool State_Idle(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_PrechillParent(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_PrechillPrep(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_Prechill(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_HeaterOnEntry(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_HeaterOn(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_Dwell(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_PostDwell(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_WaitingToDefrost(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_Defrosting(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_Disabled(Hsm_t *hsm, HsmSignal_t signal, const void *data);

static const HsmStateHierarchyDescriptor_t stateList[] = {
   { State_WaitingToDefrost, HSM_NO_PARENT },
   { State_Defrosting, HSM_NO_PARENT },
   { State_Idle, State_WaitingToDefrost },
   { State_PrechillParent, State_Defrosting },
   { State_PrechillPrep, State_PrechillParent },
   { State_Prechill, State_PrechillParent },
   { State_HeaterOnEntry, State_Defrosting },
   { State_HeaterOn, State_Defrosting },
   { State_Dwell, State_Defrosting },
   { State_PostDwell, State_WaitingToDefrost },
   { State_Disabled, HSM_NO_PARENT }
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

static bool PrechillConditionsAreMet(Defrost_t *instance)
{
   bool compressorIsOn;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->compressorIsOnErd,
      &compressorIsOn);

   CoolingMode_t coolingMode;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->coolingModeErd,
      &coolingMode);

   return compressorIsOn && (coolingMode == CoolingMode_Freezer);
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
   bool hasConvertibleCompartment;
   bool defrostWasAbnormal;

   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->hasConvertibleCompartmentErd,
      &hasConvertibleCompartment);
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->convertibleCompartmentDefrostWasAbnormalErd,
      &defrostWasAbnormal);

   return hasConvertibleCompartment && defrostWasAbnormal;
}

static bool AnyPreviousDefrostWasAbnormal(Defrost_t *instance)
{
   return (FreshFoodDefrostWasAbnormal(instance) ||
      FreezerDefrostWasAbnormal(instance) ||
      ConvertibleCompartmentDefrostWasAbnormal(instance));
}

static bool ClearedEepromStartup(Defrost_t *instance)
{
   bool eepromClearedStartup;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->eepromClearedErd,
      &eepromClearedStartup);

   return eepromClearedStartup;
}

static void SetClearedEepromStartupTo(Defrost_t *instance, bool state)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->eepromClearedErd,
      &state);
}

static bool FreezerCompartmentWasTooWarmOnPowerUp(Defrost_t *instance)
{
   bool state;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerFilteredTemperatureWasTooWarmAtPowerUpErd,
      &state);

   return state;
}

static void ResetFreezerCompartmentWasTooWarmOnPowerUp(Defrost_t *instance)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->freezerFilteredTemperatureWasTooWarmAtPowerUpErd,
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

static bool FreezerThermistorIsValid(Defrost_t *instance)
{
   bool state;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerThermistorIsValidErd,
      &state);
   return state;
}

static uint8_t MaxPrechillTime(Defrost_t *instance)
{
   uint8_t maxPrechillTime;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->maxPrechillTimeInMinutesErd,
      &maxPrechillTime);

   return maxPrechillTime;
}

static uint16_t TimeThatPrechillConditionsAreMet(Defrost_t *instance)
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

static void VoteForCompressorSpeed(Defrost_t *instance, CompressorSpeed_t speed, bool care)
{
   CompressorVotedSpeed_t compressorVote = {
      .speed = speed,
      .care = care
   };
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->compressorSpeedVoteErd,
      &compressorVote);
}

static void VoteForFreezerEvapFanSpeed(Defrost_t *instance, FanSpeed_t speed, bool care)
{
   FanVotedSpeed_t freezerEvapFanVote = {
      .speed = speed,
      .care = care
   };
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->freezerEvapFanSpeedVoteErd,
      &freezerEvapFanVote);
}

static void VoteForCondenserFanSpeed(Defrost_t *instance, FanSpeed_t speed, bool care)
{
   FanVotedSpeed_t fanVote = {
      .speed = speed,
      .care = care
   };
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->condenserFanSpeedVoteErd,
      &fanVote);
}

static void VoteForIceCabinetFanSpeed(Defrost_t *instance, FanSpeed_t speed, bool care)
{
   FanVotedSpeed_t fanVote = {
      .speed = speed,
      .care = care
   };
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->iceCabinetFanSpeedVoteErd,
      &fanVote);
}

static void VoteForDamperPosition(Defrost_t *instance, DamperPosition_t position, bool care)
{
   DamperVotedPosition_t damperVote = {
      .position = position,
      .care = care
   };
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->freshFoodDamperPositionVoteErd,
      &damperVote);
}

static void VoteForPrechillLoads(Defrost_t *instance, bool care)
{
   VoteForCompressorSpeed(instance, instance->_private.defrostParametricData->prechillData.prechillCompressorSpeed, care);
   VoteForFreezerEvapFanSpeed(instance, instance->_private.defrostParametricData->prechillData.prechillFreezerEvapFanSpeed, care);
   VoteForDamperPosition(instance, instance->_private.defrostParametricData->prechillData.prechillFreshFoodDamperPosition, care);
}

static void VoteForHeaterOnEntryLoads(Defrost_t *instance, bool care)
{
   VoteForCompressorSpeed(instance, CompressorSpeed_Off, care);
   VoteForCondenserFanSpeed(instance, FanSpeed_Off, care);
   VoteForFreezerEvapFanSpeed(instance, FanSpeed_Off, care);
   VoteForIceCabinetFanSpeed(instance, FanSpeed_Off, care);
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

static void EnableMinimumCompressorTimes(Defrost_t *instance)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->disableMinimumTimeRequestErd,
      clear);
}

static void DisableMinimumCompressorTimes(Defrost_t *instance)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->disableMinimumTimeRequestErd,
      set);
}

static void VoteForDwellLoads(Defrost_t *instance, bool care)
{
   VoteForFreezerDefrostHeater(instance, HeaterState_Off, care);
   VoteForCompressorSpeed(instance, CompressorSpeed_Off, care);
   VoteForCondenserFanSpeed(instance, FanSpeed_Off, care);
   VoteForFreezerEvapFanSpeed(instance, FanSpeed_Off, care);
   VoteForIceCabinetFanSpeed(instance, FanSpeed_Off, care);
   VoteForDamperPosition(instance, instance->_private.defrostParametricData->dwellData.dwellFreshFoodDamperPosition, care);
}

static void VoteForPostDwellLoads(Defrost_t *instance, bool care)
{
   VoteForCompressorSpeed(instance, instance->_private.defrostParametricData->postDwellData.postDwellCompressorSpeed, care);
   VoteForCondenserFanSpeed(instance, instance->_private.defrostParametricData->postDwellData.postDwellCondenserFanSpeed, care);
   VoteForDamperPosition(instance, instance->_private.defrostParametricData->postDwellData.postDwellFreshFoodDamperPosition, care);
}

static void VoteDontCareForPostDwellLoads(Defrost_t *instance)
{
   VoteForFreezerDefrostHeater(instance, HeaterState_Off, Vote_DontCare);
   VoteForCompressorSpeed(instance, CompressorSpeed_Off, Vote_DontCare);
   VoteForIceCabinetFanSpeed(instance, FanSpeed_Off, Vote_DontCare);
   VoteForCondenserFanSpeed(instance, FanSpeed_Off, Vote_DontCare);
   VoteForFreezerEvapFanSpeed(instance, FanSpeed_Off, Vote_DontCare);
   VoteForDamperPosition(instance, DamperPosition_Closed, Vote_DontCare);
}

static void CheckIfPrechillTemperatureExitConditionMet(Defrost_t *instance)
{
   TemperatureDegFx100_t freezerEvaporatorTemperature;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerEvaporatorFilteredTemperatureResolvedInDegFx100Erd,
      &freezerEvaporatorTemperature);

   TemperatureDegFx100_t freezerTemperature;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerFilteredTemperatureResolvedInDegFx100Erd,
      &freezerTemperature);

   TemperatureDegFx100_t freshFoodTemperature;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freshFoodFilteredTemperatureResolvedInDegFx100Erd,
      &freshFoodTemperature);

   bool freshFoodThermistorIsValid;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freshFoodThermistorIsValidErd,
      &freshFoodThermistorIsValid);

   if(freezerEvaporatorTemperature <= instance->_private.defrostParametricData->prechillData.prechillFreezerEvapExitTemperatureInDegFx100 ||
      freezerTemperature <= instance->_private.defrostParametricData->prechillData.prechillFreezerMinTempInDegFx100 ||
      ((freshFoodTemperature <= instance->_private.defrostParametricData->prechillData.prechillFreshFoodMinTempInDegFx100 ||
          freshFoodTemperature >= instance->_private.defrostParametricData->prechillData.prechillFreshFoodMaxTempInDegFx100) &&
         freshFoodThermistorIsValid))
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

static bool FreezerDefrostHeaterOnTimeLessThanAbnormalDefrostTime(Defrost_t *instance)
{
   uint8_t freezerHeaterOnTimeInMinutes;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerDefrostHeaterOnTimeInMinutesErd,
      &freezerHeaterOnTimeInMinutes);

   return freezerHeaterOnTimeInMinutes <
      instance->_private.defrostParametricData->heaterOnData.freezerHeaterOnTimeToSetAbnormalDefrostInMinutes;
}

static void SetCurrentDefrostTypeToNextDefrostType(Defrost_t *instance)
{
   DefrostType_t nextDefrostType;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->nextDefrostTypeErd,
      &nextDefrostType);
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->currentDefrostTypeErd,
      &nextDefrostType);
}

static HsmState_t InitialState(Defrost_t *instance)
{
   bool freezerFilteredTemperatureTooWarmAtPowerUp;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerFilteredTemperatureWasTooWarmAtPowerUpErd,
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

static void SetWaitingToDefrostTo(Defrost_t *instance, bool state)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->waitingForDefrostErd,
      &state);
}

static void SetDefrostingTo(Defrost_t *instance, bool state)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->defrostingErd,
      &state);
}

static void SetInvalidFreezerEvaporatorThermistorDuringDefrostTo(Defrost_t *instance, bool state)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->invalidFreezerEvaporatorThermistorDuringDefrostErd,
      &state);
}

static bool InvalidFreezerEvaporatorThermistorDuringDefrostIsSet(Defrost_t *instance)
{
   bool state;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->invalidFreezerEvaporatorThermistorDuringDefrostErd,
      &state);

   return state;
}

static void ClearDefrostTestStateRequest(Defrost_t *instance)
{
   DefrostTestStateRequestMessage_t requestMessage;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->defrostTestStateRequestErd,
      &requestMessage);

   requestMessage.request = DefrostTestStateRequest_None;
   requestMessage.requestId++;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->defrostTestStateRequestErd,
      &requestMessage);
}

static DefrostTestStateRequest_t DefrostTestStateRequest(Defrost_t *instance)
{
   DefrostTestStateRequestMessage_t requestMessage;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->defrostTestStateRequestErd,
      &requestMessage);

   return requestMessage.request;
}

static bool DontSkipPrechill(Defrost_t *instance)
{
   bool state;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->dontSkipDefrostPrechillErd,
      &state);

   return state;
}

static void ClearDontSkipPrechillFlag(Defrost_t *instance)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->dontSkipDefrostPrechillErd,
      clear);
}

static void ClearUseMinimumReadyToDefrostTimeFlag(Defrost_t *instance)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->useMinimumReadyToDefrostTimeAndResetDefrostCountsErd,
      clear);
}

static bool State_Idle(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   Defrost_t *instance = InstanceFromHsm(hsm);
   IGNORE(data);

   switch(signal)
   {
      case Hsm_Entry:
         SetHsmStateTo(instance, DefrostHsmState_Idle);
         switch(DefrostTestStateRequest(instance))
         {
            case DefrostTestStateRequest_Prechill:
               Hsm_Transition(hsm, State_PrechillPrep);
               break;

            case DefrostTestStateRequest_Defrost:
               Hsm_Transition(hsm, State_HeaterOnEntry);
               break;

            case DefrostTestStateRequest_Idle:
               ClearDefrostTestStateRequest(instance);
               break;

            default:
               break;
         }
         break;

      case Signal_ReadyToDefrost:
         if(DontSkipPrechill(instance))
         {
            Hsm_Transition(hsm, State_PrechillPrep);
         }
         else
         {
            if(AnyPreviousDefrostWasAbnormal(instance) ||
               FreezerCompartmentWasTooWarmOnPowerUp(instance) ||
               InvalidFreezerEvaporatorThermistorDuringDefrostIsSet(instance) ||
               ClearedEepromStartup(instance))
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
         }
         break;

      case Hsm_Exit:
         SetClearedEepromStartupTo(instance, false);
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

static bool State_Defrosting(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   Defrost_t *instance = InstanceFromHsm(hsm);
   IGNORE(data);
   IGNORE(instance);

   switch(signal)
   {
      case Hsm_Entry:
         SetWaitingToDefrostTo(instance, false);
         SetDefrostingTo(instance, true);
         break;

      case Signal_DisableDefrost:
         Hsm_Transition(hsm, State_Disabled);
         break;

      case Signal_IdleTestRequest:
      case Signal_PrechillTestRequest:
      case Signal_DefrostTestRequest:
         Hsm_Transition(hsm, State_Idle);
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

      case Signal_FreezerThermistorIsInvalid:
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
         if(!FreezerThermistorIsValid(instance) || !FreezerEvaporatorThermistorIsValid(instance))
         {
            Hsm_Transition(hsm, State_HeaterOnEntry);
            break;
         }

         SetHsmStateTo(instance, DefrostHsmState_PrechillPrep);
         ClearDontSkipPrechillFlag(instance);
         ClearDefrostTestStateRequest(instance);
         ClearUseMinimumReadyToDefrostTimeFlag(instance);

         if(PrechillConditionsAreMet(instance))
         {
            Hsm_Transition(hsm, State_Prechill);
         }
         else
         {
            StartDefrostTimer(instance, instance->_private.defrostParametricData->prechillPrepData.maxPrechillPrepTimeInMinutes * MSEC_PER_MIN);
         }
         break;

      case Signal_PrechillConditionsAreMet:
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

   switch(signal)
   {
      case Hsm_Entry:
         SetHsmStateTo(instance, DefrostHsmState_Prechill);
         VoteForPrechillLoads(instance, Vote_Care);

         if(TimeThatPrechillConditionsAreMet(instance) >= MaxPrechillTime(instance))
         {
            Hsm_Transition(hsm, State_HeaterOnEntry);
         }
         else
         {
            StartDefrostTimer(
               instance,
               TRUNCATE_UNSIGNED_SUBTRACTION((uint16_t)MaxPrechillTime(instance),
                  TimeThatPrechillConditionsAreMet(instance)) *
                  MSEC_PER_MIN);
         }

         CheckIfPrechillTemperatureExitConditionMet(instance);
         break;

      case Signal_FreezerEvaporatorTemperatureChanged: {
         const TemperatureDegFx100_t *freezerEvaporatorTemperature = data;

         if(*freezerEvaporatorTemperature <= instance->_private.defrostParametricData->prechillData.prechillFreezerEvapExitTemperatureInDegFx100)
         {
            Hsm_Transition(hsm, State_HeaterOnEntry);
         }
      }
      break;

      case Signal_PrechillTemperatureExitConditionMet:
      case Signal_DefrostTimerExpired:
         Hsm_Transition(hsm, State_HeaterOnEntry);
         break;

      case Hsm_Exit:
         VoteForPrechillLoads(instance, Vote_DontCare);
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
         ClearDefrostTestStateRequest(instance);
         DisableMinimumCompressorTimes(instance);
         VoteForHeaterOnEntryLoads(instance, Vote_Care);
         StartDefrostTimer(
            instance,
            instance->_private.defrostParametricData->heaterOnEntryData.defrostHeaterOnDelayAfterCompressorOffInSeconds * MSEC_PER_SEC);
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

   switch(signal)
   {
      case Hsm_Entry:
         SetHsmStateTo(instance, DefrostHsmState_HeaterOn);
         VoteForFreezerDefrostHeater(instance, HeaterState_On, Vote_Care);
         break;

      case Signal_FreezerHeaterMaxOnTimeReached:
         if(!FreezerEvaporatorThermistorIsValid(instance))
         {
            SetInvalidFreezerEvaporatorThermistorDuringDefrostTo(instance, true);
         }
         Hsm_Transition(hsm, State_Dwell);
         break;

      case Signal_FreezerAbnormalHeaterOnTimeReached:
         IncrementNumberOfFreezerAbnormalDefrostCycleCount(instance);
         SetFreezerDefrostWasAbnormal(instance);
         break;

      case Signal_FreezerEvaporatorTemperatureChanged: {
         const TemperatureDegFx100_t *freezerEvaporatorTemperature = data;
         if((*freezerEvaporatorTemperature >= instance->_private.defrostParametricData->heaterOnData.freezerDefrostTerminationTemperatureInDegFx100) &&
            FreezerEvaporatorThermistorIsValid(instance))
         {
            if(FreezerDefrostHeaterOnTimeLessThanAbnormalDefrostTime(instance))
            {
               ClearFreezerDefrostWasAbnormal(instance);
            }
            Hsm_Transition(hsm, State_Dwell);
         }
      }
      break;

      case Hsm_Exit:
         if(FreezerEvaporatorThermistorIsValid(instance))
         {
            SetInvalidFreezerEvaporatorThermistorDuringDefrostTo(instance, false);
         }
         VoteForFreezerDefrostHeater(instance, HeaterState_Off, Vote_Care);
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
         DisableMinimumCompressorTimes(instance);
         VoteForDwellLoads(instance, Vote_Care);
         StartDefrostTimer(
            instance,
            instance->_private.defrostParametricData->dwellData.dwellTimeInMinutes * MSEC_PER_MIN);
         break;

      case Signal_DefrostTimerExpired:
         Hsm_Transition(hsm, State_PostDwell);
         break;

      case Hsm_Exit:
         StopTimer(instance, &instance->_private.defrostTimer);
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

static bool State_WaitingToDefrost(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   Defrost_t *instance = InstanceFromHsm(hsm);
   IGNORE(data);

   switch(signal)
   {
      case Hsm_Entry:
         SetWaitingToDefrostTo(instance, true);
         SetDefrostingTo(instance, false);
         break;

      case Signal_DisableDefrost:
         Hsm_Transition(hsm, State_Disabled);
         break;

      case Signal_IdleTestRequest:
      case Signal_PrechillTestRequest:
      case Signal_DefrostTestRequest:
         Hsm_Transition(hsm, State_Idle);
         break;

      case Hsm_Exit:
         SetCurrentDefrostTypeToNextDefrostType(instance);
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

static bool State_PostDwell(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   Defrost_t *instance = InstanceFromHsm(hsm);

   switch(signal)
   {
      case Hsm_Entry:
         SetHsmStateTo(instance, DefrostHsmState_PostDwell);
         VoteForPostDwellLoads(instance, Vote_Care);
         DisableMinimumCompressorTimes(instance);
         StartDefrostTimer(
            instance,
            instance->_private.defrostParametricData->postDwellData.postDwellExitTimeInMinutes * MSEC_PER_MIN);
         break;

      case Signal_DefrostTimerExpired:
         Hsm_Transition(hsm, State_Idle);
         break;

      case Signal_FreezerEvaporatorTemperatureChanged: {
         const TemperatureDegFx100_t *freezerEvaporatorTemperature = data;

         if(*freezerEvaporatorTemperature <= instance->_private.defrostParametricData->postDwellData.postDwellFreezerEvapExitTemperatureInDegFx100 &&
            FreezerEvaporatorThermistorIsValid(instance))
         {
            Hsm_Transition(hsm, State_Idle);
         }
         break;
      }

      case Hsm_Exit:
         EnableMinimumCompressorTimes(instance);
         VoteDontCareForPostDwellLoads(instance);
         StopTimer(instance, &instance->_private.defrostTimer);
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

static bool State_Disabled(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   Defrost_t *instance = InstanceFromHsm(hsm);
   IGNORE(data);

   switch(signal)
   {
      case Hsm_Entry:
         SetHsmStateTo(instance, DefrostHsmState_Disabled);
         VoteForFreezerDefrostHeater(instance, HeaterState_Off, Vote_Care);
         SetWaitingToDefrostTo(instance, false);
         SetDefrostingTo(instance, false);
         break;

      case Signal_EnableDefrost:
         Hsm_Transition(hsm, State_Idle);
         break;

      case Hsm_Exit:
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

static void DataModelChanged(void *context, const void *args)
{
   Defrost_t *instance = context;
   const DataModelOnDataChangeArgs_t *onChangeData = args;
   Erd_t erd = onChangeData->erd;

   if(erd == instance->_private.config->readyToDefrostErd)
   {
      const bool *state = onChangeData->data;

      if(*state)
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_ReadyToDefrost, NULL);
      }
   }
   else if(erd == instance->_private.config->compressorIsOnErd ||
      erd == instance->_private.config->coolingModeErd)
   {
      if(PrechillConditionsAreMet(instance))
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_PrechillConditionsAreMet, NULL);
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
   else if(erd == instance->_private.config->freezerThermistorIsValidErd)
   {
      const bool *freezerThermistorIsValid = onChangeData->data;

      if(!(*freezerThermistorIsValid))
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_FreezerThermistorIsInvalid, NULL);
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
   else if(erd == instance->_private.config->freezerEvaporatorFilteredTemperatureResolvedInDegFx100Erd)
   {
      const TemperatureDegFx100_t *freezerEvaporatorTemperature = onChangeData->data;
      Hsm_SendSignal(&instance->_private.hsm, Signal_FreezerEvaporatorTemperatureChanged, freezerEvaporatorTemperature);
   }
   else if(erd == instance->_private.config->freezerFilteredTemperatureResolvedInDegFx100Erd)
   {
      const TemperatureDegFx100_t *freezerTemperature = onChangeData->data;

      if(*freezerTemperature <= instance->_private.defrostParametricData->prechillData.prechillFreezerMinTempInDegFx100)
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_PrechillTemperatureExitConditionMet, NULL);
      }
   }
   else if(erd == instance->_private.config->freshFoodFilteredTemperatureResolvedInDegFx100Erd)
   {
      const TemperatureDegFx100_t *freshFoodTemperature = onChangeData->data;

      if((*freshFoodTemperature <= instance->_private.defrostParametricData->prechillData.prechillFreshFoodMinTempInDegFx100 ||
            *freshFoodTemperature >= instance->_private.defrostParametricData->prechillData.prechillFreshFoodMaxTempInDegFx100) &&
         FreshFoodThermistorIsValid(instance))
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
         instance->_private.defrostParametricData->heaterOnData.freezerHeaterOnTimeToSetAbnormalDefrostInMinutes)
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_FreezerAbnormalHeaterOnTimeReached, NULL);
      }
   }
   else if(erd == instance->_private.config->disableDefrostErd)
   {
      const bool *disableDefrost = onChangeData->data;

      if(*disableDefrost)
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_DisableDefrost, NULL);
      }
      else
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_EnableDefrost, NULL);
      }
   }
   else if(erd == instance->_private.config->defrostTestStateRequestErd)
   {
      const DefrostTestStateRequestMessage_t *requestMessage = onChangeData->data;

      switch(requestMessage->request)
      {
         case DefrostTestStateRequest_Idle:
            Hsm_SendSignal(&instance->_private.hsm, Signal_IdleTestRequest, NULL);
            break;

         case DefrostTestStateRequest_Prechill:
            Hsm_SendSignal(&instance->_private.hsm, Signal_PrechillTestRequest, NULL);
            break;

         case DefrostTestStateRequest_Defrost:
            Hsm_SendSignal(&instance->_private.hsm, Signal_DefrostTestRequest, NULL);
            break;

         default:
            break;
      }
   }
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

   DataModel_SubscribeAll(
      dataModel,
      &instance->_private.dataModelSubscription);
}
