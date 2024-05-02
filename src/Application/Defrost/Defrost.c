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
#include "CompressorVotedSpeed.h"
#include "Signal.h"
#include "uassert.h"
#include "Vote.h"
#include "DefrostType.h"
#include "CoolingMode.h"
#include "DefrostTestStateRequestMessage.h"
#include "BooleanVotedState.h"
#include "SystemErds.h"
#include "SetpointVotedTemperature.h"
#include "ConvertibleCompartmentStateType.h"
#include <string.h>

#define ForEachEvaporator(                                      \
   /* EvaporatorConfigs_t * */ evaporatorConfigs,               \
   /* EvaporatorConfiguration_t * */ evaporatorConfig,          \
   ...)                                                         \
   do                                                           \
   {                                                            \
      const EvaporatorConfiguration_t **evaporatorConfigArray = \
         (const EvaporatorConfiguration_t **)evaporatorConfigs; \
      for(uint8_t i = 0; i < 2; i++)                            \
      {                                                         \
         const EvaporatorConfiguration_t *evaporatorConfig =    \
            evaporatorConfigArray[i];                           \
         if(evaporatorConfig != NULL)                           \
         {                                                      \
            __VA_ARGS__                                         \
         }                                                      \
      }                                                         \
   } while(0)

enum
{
   Signal_ReadyToDefrost = Hsm_UserSignalStart,
   Signal_PrechillConditionsAreMet,
   Signal_DefrostTimerExpired,
   Signal_PrechillTemperatureExitConditionMet,
   Signal_FreezerThermistorIsInvalid,
   Signal_FreezerEvaporatorThermistorIsInvalid,
   Signal_FreezerAbnormalHeaterOnTimeReached,
   Signal_DisableDefrost,
   Signal_EnableDefrost,
   Signal_IdleTestRequest,
   Signal_PrechillTestRequest,
   Signal_DefrostTestRequest,
   Signal_ExitDefrostHeaterOnStateTestRequest,
   Signal_FreezerEvaporatorTemperatureChanged,
   Signal_SabbathIsReadyToDefrost,
   Signal_AtLeastOneSabbathModeIsEnabled,
   Signal_AtLeastOneSabbathModeIsDisabled,
   Signal_EnhancedSabbathIsRequestingToDefrost,
   Signal_FreezerTemperatureChanged,
   Signal_FreshFoodTemperatureChanged,
   Signal_ConvertibleCompartmentTemperatureChanged,
   Signal_FreshFoodEvaporatorThermistorIsInvalid,
   Signal_ConvertibleCompartmentEvaporatorThermistorIsInvalid,
   Signal_FreshFoodAbnormalHeaterOnTimeReached,
   Signal_ConvertibleCompartmentAbnormalHeaterOnTimeReached,
   Signal_FreshFoodEvaporatorTemperatureChanged,
   Signal_ConvertibleCompartmentEvaporatorTemperatureChanged,
   Signal_HeaterMaxOnTimeReached,
   Signal_AbnormalHeaterOnTimeReached,
   Signal_PrimaryEvaporatorTemperatureChanged,
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

static const DefrostPrechillData_t *PrechillData(Defrost_t *instance)
{
   return &instance->_private.defrostData->prechillData;
}

static ConvertibleCompartmentStateType_t ConvertibleCompartmentState(Defrost_t *instance)
{
   ConvertibleCompartmentStateType_t state;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->convertibleCompartmentStateErd,
      &state);
   return state;
}

static bool HasFreshFoodCompartment(Defrost_t *instance)
{
   return BITMAP_STATE(&instance->_private.platformData->compartmentBitmap, Compartment_FreshFood);
}

static bool HasFreezerCompartment(Defrost_t *instance)
{
   return BITMAP_STATE(&instance->_private.platformData->compartmentBitmap, Compartment_Freezer);
}

static bool HasConvertibleCompartment(Defrost_t *instance)
{
   return BITMAP_STATE(&instance->_private.platformData->compartmentBitmap, Compartment_Convertible);
}

static bool HasOneEvaporator(Defrost_t *instance)
{
   return instance->_private.platformData->numberOfEvaporators == 1;
}

static bool HasTwoEvaporators(Defrost_t *instance)
{
   return instance->_private.platformData->numberOfEvaporators == 2;
}

static bool HasThreeEvaporators(Defrost_t *instance)
{
   return instance->_private.platformData->numberOfEvaporators == 3;
}

static void SetPrimaryEvaporators(Defrost_t *instance)
{
   if(HasThreeEvaporators(instance) &&
      HasFreshFoodCompartment(instance) &&
      HasFreezerCompartment(instance) &&
      HasConvertibleCompartment(instance))
   {
      if(ConvertibleCompartmentState(instance) == ConvertibleCompartmentStateType_FreshFood)
      {
         instance->_private.primaryEvaporatorsConfigs.evaporator0 = &instance->_private.config->freezerEvaporator;
         instance->_private.primaryEvaporatorsConfigs.evaporator1 = NULL;
      }
      else if(ConvertibleCompartmentState(instance) == ConvertibleCompartmentStateType_Freezer)
      {
         instance->_private.primaryEvaporatorsConfigs.evaporator0 = &instance->_private.config->freezerEvaporator;
         instance->_private.primaryEvaporatorsConfigs.evaporator1 = &instance->_private.config->convertibleCompartmentEvaporator;
      }
   }
   else if((HasOneEvaporator(instance) && HasFreezerCompartment(instance)) ||
      HasTwoEvaporators(instance))
   {
      instance->_private.primaryEvaporatorsConfigs.evaporator0 = &instance->_private.config->freezerEvaporator;
      instance->_private.primaryEvaporatorsConfigs.evaporator1 = NULL;
   }
   else if(HasOneEvaporator(instance) &&
      HasFreshFoodCompartment(instance))
   {
      instance->_private.primaryEvaporatorsConfigs.evaporator0 = &instance->_private.config->freshFoodEvaporator;
      instance->_private.primaryEvaporatorsConfigs.evaporator1 = NULL;
   }
}

static bool FreezerIsAPrimaryEvap(Defrost_t *instance)
{
   return (
      instance->_private.primaryEvaporatorsConfigs.evaporator0 == &instance->_private.config->freezerEvaporator ||
      instance->_private.primaryEvaporatorsConfigs.evaporator1 == &instance->_private.config->freezerEvaporator);
}

static bool FreshFoodIsAPrimaryEvap(Defrost_t *instance)
{
   return (
      instance->_private.primaryEvaporatorsConfigs.evaporator0 == &instance->_private.config->freshFoodEvaporator ||
      instance->_private.primaryEvaporatorsConfigs.evaporator1 == &instance->_private.config->freshFoodEvaporator);
}

static bool ConvertibleCompartmentIsAPrimaryEvap(Defrost_t *instance)
{
   return (
      instance->_private.primaryEvaporatorsConfigs.evaporator0 == &instance->_private.config->convertibleCompartmentEvaporator ||
      instance->_private.primaryEvaporatorsConfigs.evaporator1 == &instance->_private.config->convertibleCompartmentEvaporator);
}

static void SetSecondaryEvaporators(Defrost_t *instance)
{
   if(HasThreeEvaporators(instance) &&
      HasFreshFoodCompartment(instance) &&
      HasFreezerCompartment(instance) &&
      HasConvertibleCompartment(instance))
   {
      if(ConvertibleCompartmentState(instance) == ConvertibleCompartmentStateType_FreshFood)
      {
         instance->_private.secondaryEvaporatorsConfigs.evaporator0 = &instance->_private.config->freshFoodEvaporator;
         instance->_private.secondaryEvaporatorsConfigs.evaporator1 = &instance->_private.config->convertibleCompartmentEvaporator;
      }
      else if(ConvertibleCompartmentState(instance) == ConvertibleCompartmentStateType_Freezer)
      {
         instance->_private.secondaryEvaporatorsConfigs.evaporator0 = &instance->_private.config->freshFoodEvaporator;
         instance->_private.secondaryEvaporatorsConfigs.evaporator1 = NULL;
      }
   }
   else if(HasOneEvaporator(instance))
   {
      instance->_private.secondaryEvaporatorsConfigs.evaporator0 = NULL;
      instance->_private.secondaryEvaporatorsConfigs.evaporator1 = NULL;
   }
   else if(HasTwoEvaporators(instance))
   {
      instance->_private.secondaryEvaporatorsConfigs.evaporator0 = &instance->_private.config->freshFoodEvaporator;
      instance->_private.secondaryEvaporatorsConfigs.evaporator1 = NULL;
   }
}

static DefrostType_t CurrentDefrostType(Defrost_t *instance)
{
   DefrostType_t currentDefrostType;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->currentDefrostTypeErd,
      &currentDefrostType);

   return currentDefrostType;
}

static void SetCurrentDefrostType(Defrost_t *instance, DefrostType_t defrostType)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->currentDefrostTypeErd,
      &defrostType);
}

static void SetCurrentDefrostTypeToNextDefrostTypeOverride(Defrost_t *instance)
{
   DefrostType_t nextDefrostTypeOverride;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->nextDefrostTypeOverrideErd,
      &nextDefrostTypeOverride);

   SetCurrentDefrostType(instance, nextDefrostTypeOverride);
}

static void SetNextDefrostTypeOverrideTo(Defrost_t *instance, DefrostType_t defrostType)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->nextDefrostTypeOverrideErd,
      &defrostType);
}

static DefrostType_t NextDefrostTypeOverride(Defrost_t *instance)
{
   DefrostType_t defrostType;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->nextDefrostTypeOverrideErd,
      &defrostType);

   return defrostType;
}

static uint8_t NumberOfSecondaryOnlyDefrosts(Defrost_t *instance)
{
   uint8_t numberOfSecondaryOnlyDefrosts;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->numberOfSecondaryOnlyDefrostsErd,
      &numberOfSecondaryOnlyDefrosts);

   return numberOfSecondaryOnlyDefrosts;
}

static void SetNumberOfSecondaryOnlyDefrostsTo(Defrost_t *instance, uint8_t numberOfSecondaryOnlyDefrosts)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->numberOfSecondaryOnlyDefrostsErd,
      &numberOfSecondaryOnlyDefrosts);
}

static uint8_t NumberOfSecondaryOnlyDefrostsBeforeAFullDefrost(Defrost_t *instance)
{
   uint8_t numberOfSecondaryOnlyDefrostsBeforeAFullDefrost;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->numberOfSecondaryOnlyDefrostsBeforeAFullDefrostErd,
      &numberOfSecondaryOnlyDefrostsBeforeAFullDefrost);

   return numberOfSecondaryOnlyDefrostsBeforeAFullDefrost;
}

static void IncrementNumberOfSecondaryOnlyDefrosts(Defrost_t *instance)
{
   uint8_t numberOfSecondaryOnlyDefrosts;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->numberOfSecondaryOnlyDefrostsErd,
      &numberOfSecondaryOnlyDefrosts);

   numberOfSecondaryOnlyDefrosts++;

   SetNumberOfSecondaryOnlyDefrostsTo(instance, numberOfSecondaryOnlyDefrosts);
}

static bool EnhancedSabbathModeIsEnabled(Defrost_t *instance)
{
   bool enhancedSabbathMode;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->enhancedSabbathModeErd,
      &enhancedSabbathMode);

   return enhancedSabbathMode;
}

static bool AtLeastOneSabbathModeIsEnabled(Defrost_t *instance)
{
   bool sabbathMode;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->sabbathModeErd,
      &sabbathMode);

   return (sabbathMode || EnhancedSabbathModeIsEnabled(instance));
}

static bool AnyPrimaryEvaporatorDefrostWasAbnormal(Defrost_t *instance)
{
   bool anyPrimaryEvaporatorDefrostWasAbnormal = false;

   ForEachEvaporator(&instance->_private.primaryEvaporatorsConfigs, evapConfig, {
      bool defrostWasAbnormal;
      DataModel_Read(
         instance->_private.dataModel,
         evapConfig->defrostWasAbnormalErd,
         &defrostWasAbnormal);

      anyPrimaryEvaporatorDefrostWasAbnormal |= defrostWasAbnormal;
   });

   return anyPrimaryEvaporatorDefrostWasAbnormal;
}

static void DetermineNumberOfSecondaryOnlyDefrostsBeforeAFullDefrost(Defrost_t *instance)
{
   uint8_t numberOfSecondaryOnlyDefrostsBeforeAFullDefrost =
      instance->_private.defrostData->idleData.numberOfSecondaryOnlyDefrostsBeforeFullDefrost;

   if(AtLeastOneSabbathModeIsEnabled(instance))
   {
      numberOfSecondaryOnlyDefrostsBeforeAFullDefrost =
         instance->_private.enhancedSabbathData->numberOfSecondaryOnlyDefrostsBeforeFullDefrost;
   }
   else if(AnyPrimaryEvaporatorDefrostWasAbnormal(instance))
   {
      numberOfSecondaryOnlyDefrostsBeforeAFullDefrost =
         instance->_private.defrostData->idleData.numberOfSecondaryOnlyDefrostsBeforeFullDefrostWhenAbnormalIsSet;
   }

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->numberOfSecondaryOnlyDefrostsBeforeAFullDefrostErd,
      &numberOfSecondaryOnlyDefrostsBeforeAFullDefrost);
}

static void SetDefrostType(Defrost_t *instance)
{
   if(NextDefrostTypeOverride(instance) == DefrostType_DontCare)
   {
      if(NumberOfSecondaryOnlyDefrosts(instance) < NumberOfSecondaryOnlyDefrostsBeforeAFullDefrost(instance))
      {
         SetCurrentDefrostType(instance, DefrostType_SecondaryOnly);
      }
      else
      {
         SetCurrentDefrostType(instance, DefrostType_Full);
      }
   }
   else
   {
      SetCurrentDefrostTypeToNextDefrostTypeOverride(instance);
      SetNextDefrostTypeOverrideTo(instance, DefrostType_DontCare);
   }

   if(CurrentDefrostType(instance) == DefrostType_SecondaryOnly)
   {
      IncrementNumberOfSecondaryOnlyDefrosts(instance);
   }
   else
   {
      SetNumberOfSecondaryOnlyDefrostsTo(instance, 0);
   }
}

static uint8_t FreezerDefrostHeaterMaxOnTimeInMinutes(Defrost_t *instance)
{
   uint8_t maxOnTimeInMinutes;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerEvaporator.defrostHeaterMaxOnTimeInMinutesErd,
      &maxOnTimeInMinutes);

   return maxOnTimeInMinutes;
}

static uint8_t FreshFoodDefrostHeaterMaxOnTimeInMinutes(Defrost_t *instance)
{
   uint8_t maxOnTimeInMinutes;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freshFoodEvaporator.defrostHeaterMaxOnTimeInMinutesErd,
      &maxOnTimeInMinutes);

   return maxOnTimeInMinutes;
}

static uint8_t ConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutes(Defrost_t *instance)
{
   uint8_t maxOnTimeInMinutes;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->convertibleCompartmentEvaporator.defrostHeaterMaxOnTimeInMinutesErd,
      &maxOnTimeInMinutes);

   return maxOnTimeInMinutes;
}

static bool ConvertibleCompartmentIsActingAsAFreezer(Defrost_t *instance)
{
   ConvertibleCompartmentStateType_t stateType;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->convertibleCompartmentStateErd,
      &stateType);

   return (stateType == ConvertibleCompartmentStateType_Freezer);
}

static bool ConvertibleCompartmentIsActingAsAFreshFood(Defrost_t *instance)
{
   ConvertibleCompartmentStateType_t stateType;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->convertibleCompartmentStateErd,
      &stateType);

   return (stateType == ConvertibleCompartmentStateType_FreshFood);
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

   if(!compressorIsOn)
   {
      return false;
   }

   if((HasOneEvaporator(instance) ||
         HasTwoEvaporators(instance)) &&
      HasFreezerCompartment(instance) && HasFreshFoodCompartment(instance))
   {
      return coolingMode == CoolingMode_Freezer;
   }
   else if(HasThreeEvaporators(instance))
   {
      if(ConvertibleCompartmentIsActingAsAFreezer(instance))
      {
         return coolingMode == CoolingMode_ConvertibleCompartment;
      }
      else
      {
         return coolingMode == CoolingMode_Freezer;
      }
   }
   else
   {
      return true;
   }
}

static void StartTimer(Defrost_t *instance, Timer_t *timer, TimerTicks_t ticks, TimerCallback_t callback)
{
   TimerModule_StartOneShot(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         Erd_TimerModule),
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
         Erd_TimerModule),
      timer);
}

static bool FreshFoodDefrostWasAbnormal(Defrost_t *instance)
{
   bool defrostWasAbnormal;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freshFoodEvaporator.defrostWasAbnormalErd,
      &defrostWasAbnormal);

   return defrostWasAbnormal;
}

static bool FreezerDefrostWasAbnormal(Defrost_t *instance)
{
   bool defrostWasAbnormal;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerEvaporator.defrostWasAbnormalErd,
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
      instance->_private.config->convertibleCompartmentEvaporator.defrostWasAbnormalErd,
      &defrostWasAbnormal);

   return (hasConvertibleCompartment && defrostWasAbnormal);
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

static bool CabinetWasTooWarmOnPowerUp(Defrost_t *instance)
{
   bool state;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->cabinetFilteredTemperatureTooWarmOnPowerUpErd,
      &state);

   return state;
}

static void ResetCabinetWasTooWarmOnPowerUp(Defrost_t *instance)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->cabinetFilteredTemperatureTooWarmOnPowerUpErd,
      clear);
}

static bool EvaporatorThermistorIsValid(Defrost_t *instance, const EvaporatorConfiguration_t *evapConfig)
{
   bool state;
   DataModel_Read(
      instance->_private.dataModel,
      evapConfig->evaporatorThermistorIsValidResolvedErd,
      &state);
   return state;
}

static bool FreezerEvaporatorThermistorIsValid(Defrost_t *instance)
{
   bool state;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerEvaporator.evaporatorThermistorIsValidResolvedErd,
      &state);
   return state;
}

static bool FreshFoodEvaporatorThermistorIsValid(Defrost_t *instance)
{
   bool state;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freshFoodEvaporator.evaporatorThermistorIsValidResolvedErd,
      &state);
   return state;
}

static bool ConvertibleCompartmentEvaporatorThermistorIsValid(Defrost_t *instance)
{
   bool state;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->convertibleCompartmentEvaporator.evaporatorThermistorIsValidResolvedErd,
      &state);
   return state;
}

static bool FreshFoodThermistorIsValid(Defrost_t *instance)
{
   bool state;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freshFoodThermistorIsValidResolvedErd,
      &state);
   return state;
}

static bool FreezerThermistorIsValid(Defrost_t *instance)
{
   bool state;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerThermistorIsValidResolvedErd,
      &state);
   return state;
}

static bool FreezerThermistorHasBeenDiscovered(Defrost_t *instance)
{
   bool state;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerThermistorHasBeenDiscoveredErd,
      &state);

   return state;
}

static bool ConvertibleCompartmentThermistorIsValid(Defrost_t *instance)
{
   bool state;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->convertibleCompartmentThermistorIsValidResolvedErd,
      &state);

   return state;
}

static bool FreezerEvaporatorThermistorHasBeenDiscovered(Defrost_t *instance)
{
   bool state;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerEvaporatorThermistorHasBeenDiscoveredErd,
      &state);

   return state;
}

static bool FreshFoodEvaporatorThermistorHasBeenDiscovered(Defrost_t *instance)
{
   bool state;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freshFoodEvaporatorThermistorHasBeenDiscoveredErd,
      &state);

   return state;
}

static bool ConvertibleCompartmentEvaporatorThermistorHasBeenDiscovered(Defrost_t *instance)
{
   bool state;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->convertibleCompartmentEvaporatorThermistorHasBeenDiscoveredErd,
      &state);

   return state;
}

static uint8_t MaxPrechillTimeInMinutes(Defrost_t *instance)
{
   if(CurrentDefrostType(instance) == DefrostType_SecondaryOnly)
   {
      return instance->_private.defrostData->prechillData.maxPrechillTimeForSecondaryOnlyDefrostInMinutes;
   }
   else
   {
      return instance->_private.defrostData->prechillData.maxPrechillTimeInMinutes;
   }
}

static uint8_t PostDwellExitTimeInMinutes(Defrost_t *instance)
{
   if(CurrentDefrostType(instance) == DefrostType_SecondaryOnly)
   {
      return instance->_private.defrostData->postDwellData.secondaryOnlyPostDwellExitTimeInMinutes;
   }
   else
   {
      return instance->_private.defrostData->postDwellData.postDwellExitTimeInMinutes;
   }
}

static uint16_t TimeThatPrechillConditionsAreMetInMinutes(Defrost_t *instance)
{
   uint16_t timeThatPrechillConditionsAreMetInMinutes;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->timeThatPrechillConditionsAreMetInMinutesErd,
      &timeThatPrechillConditionsAreMetInMinutes);

   return timeThatPrechillConditionsAreMetInMinutes;
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
   FanVotedSpeed_t fanVote = {
      .speed = speed,
      .care = care
   };
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->freezerEvaporator.evaporatorFanVoteErd,
      &fanVote);
}

static void VoteForFreshFoodEvapFanSpeed(Defrost_t *instance, FanSpeed_t speed, bool care)
{
   FanVotedSpeed_t fanVote = {
      .speed = speed,
      .care = care
   };
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->freshFoodEvaporator.evaporatorFanVoteErd,
      &fanVote);
}

static void VoteForConvertibleCompartmentEvapFanSpeed(Defrost_t *instance, FanSpeed_t speed, bool care)
{
   FanVotedSpeed_t fanVote = {
      .speed = speed,
      .care = care
   };
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->convertibleCompartmentEvaporator.evaporatorFanVoteErd,
      &fanVote);
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

static void VoteForDeliFanSpeed(Defrost_t *instance, FanSpeed_t speed, bool care)
{
   FanVotedSpeed_t fanVote = {
      .speed = speed,
      .care = care
   };
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->deliFanSpeedVoteErd,
      &fanVote);
}

static void VoteForFreshFoodDamperPosition(Defrost_t *instance, DamperPosition_t position, bool care)
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

static void VoteForDeliDamperPosition(Defrost_t *instance, DamperPosition_t position, bool care)
{
   DamperVotedPosition_t damperVote = {
      .position = position,
      .care = care
   };
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->deliDamperPositionVoteErd,
      &damperVote);
}

static void VoteForSealedSystemValvePosition(Defrost_t *instance, SealedSystemValvePosition_t position, bool care)
{
   SealedSystemValveVotedPosition_t valvePositionVote = {
      .position = position,
      .care = care
   };
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->sealedSystemValvePositionVoteErd,
      &valvePositionVote);
}

static void VoteForConvertibleCompartmentDamperPosition(Defrost_t *instance, DamperPosition_t position, bool care)
{
   DamperVotedPosition_t vote = {
      .position = position,
      .care = care
   };
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->convertibleCompartmentDamperPositionVoteErd,
      &vote);
}

static void VoteForPrechillLoads(Defrost_t *instance, bool care)
{
   VoteForSealedSystemValvePosition(instance, instance->_private.defrostData->prechillData.prechillSealedSystemValvePosition, care);
   VoteForFreshFoodDamperPosition(instance, instance->_private.defrostData->prechillData.prechillFreshFoodDamperPosition, care);
   VoteForCompressorSpeed(instance, instance->_private.defrostData->prechillData.prechillCompressorSpeed, care);
   VoteForCondenserFanSpeed(instance, instance->_private.defrostData->prechillData.prechillCondenserFanSpeed, care);
   VoteForFreezerEvapFanSpeed(instance, instance->_private.defrostData->prechillData.prechillFreezerEvaporatorFanSpeed, care);
   VoteForFreshFoodEvapFanSpeed(instance, instance->_private.defrostData->prechillData.prechillFreshFoodEvaporatorFanSpeed, care);
   VoteForIceCabinetFanSpeed(instance, instance->_private.defrostData->prechillData.prechillIceCabinetFanSpeed, care);
   VoteForDeliFanSpeed(instance, instance->_private.defrostData->prechillData.prechillDeliFanSpeed, care);
   VoteForDeliDamperPosition(instance, instance->_private.defrostData->prechillData.prechillDeliDamperPosition, care);
   VoteForConvertibleCompartmentDamperPosition(instance, instance->_private.defrostData->prechillData.prechillConvertibleCompartmentDamperPosition, care);
   VoteForConvertibleCompartmentEvapFanSpeed(instance, instance->_private.defrostData->prechillData.prechillConvertibleCompartmentEvapFanSpeed, care);
}

static void VoteForHeaterOnEntryLoads(Defrost_t *instance, bool care)
{
   VoteForCompressorSpeed(instance, CompressorSpeed_Off, care);
   VoteForCondenserFanSpeed(instance, FanSpeed_Off, care);
   VoteForFreezerEvapFanSpeed(instance, FanSpeed_Off, care);
   VoteForFreshFoodEvapFanSpeed(instance, FanSpeed_Off, care);
   VoteForConvertibleCompartmentEvapFanSpeed(instance, FanSpeed_Off, care);
   VoteForIceCabinetFanSpeed(instance, FanSpeed_Off, care);
   VoteForDeliFanSpeed(instance, FanSpeed_Off, care);
   VoteForFreshFoodDamperPosition(instance, instance->_private.defrostData->heaterOnEntryData.heaterOnEntryFreshFoodDamperPosition, care);
   VoteForConvertibleCompartmentDamperPosition(instance, DamperPosition_Closed, care);
   VoteForDeliDamperPosition(instance, DamperPosition_Closed, care);
   VoteForSealedSystemValvePosition(instance, instance->_private.defrostData->heaterOnEntryData.heaterOnEntrySealedSystemValvePosition, care);
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
      instance->_private.config->freezerEvaporator.heaterVoteErd,
      &heaterVote);
}

static void VoteForDefrostHeater(
   Defrost_t *instance,
   const EvaporatorConfiguration_t *evapConfig,
   HeaterState_t state,
   bool care)
{
   HeaterVotedState_t heaterVote = {
      .state = state,
      .care = care
   };
   DataModel_Write(
      instance->_private.dataModel,
      evapConfig->heaterVoteErd,
      &heaterVote);
}

static void VoteForFreshFoodDefrostHeater(
   Defrost_t *instance,
   HeaterState_t state,
   Vote_t care)
{
   HeaterVotedState_t heaterVote = {
      .state = state,
      .care = care
   };
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->freshFoodEvaporator.heaterVoteErd,
      &heaterVote);
}

static void VoteForConvertibleCompartmentDefrostHeater(
   Defrost_t *instance,
   HeaterState_t state,
   Vote_t care)
{
   HeaterVotedState_t heaterVote = {
      .state = state,
      .care = care
   };
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->convertibleCompartmentEvaporator.heaterVoteErd,
      &heaterVote);
}

static void VoteForPrimaryEvaporatorHeaters(
   Defrost_t *instance,
   HeaterState_t state,
   Vote_t care)
{
   ForEachEvaporator(&instance->_private.primaryEvaporatorsConfigs, evapConfig, {
      HeaterVotedState_t heaterVotedState = { .state = state, .care = care };
      DataModel_Write(
         instance->_private.dataModel,
         evapConfig->heaterVoteErd,
         &heaterVotedState);
   });
}

static void VoteForSecondaryEvaporatorHeaters(
   Defrost_t *instance,
   HeaterState_t state,
   Vote_t care)
{
   ForEachEvaporator(&instance->_private.secondaryEvaporatorsConfigs, evapConfig, {
      HeaterVotedState_t heaterVotedState = { .state = state, .care = care };
      DataModel_Write(
         instance->_private.dataModel,
         evapConfig->heaterVoteErd,
         &heaterVotedState);
   });
}

static void VoteForHeaterOnLoads(Defrost_t *instance, bool care)
{
   VoteForHeaterOnEntryLoads(instance, care);
}

static void VoteForDwellLoads(Defrost_t *instance, bool care)
{
   VoteForCompressorSpeed(instance, CompressorSpeed_Off, care);
   VoteForCondenserFanSpeed(instance, FanSpeed_Off, care);
   VoteForFreezerDefrostHeater(instance, HeaterState_Off, care);
   VoteForFreshFoodDefrostHeater(instance, HeaterState_Off, care);
   VoteForConvertibleCompartmentDefrostHeater(instance, HeaterState_Off, care);
   VoteForFreezerEvapFanSpeed(instance, FanSpeed_Off, care);
   VoteForFreshFoodEvapFanSpeed(instance, FanSpeed_Off, care);
   VoteForConvertibleCompartmentEvapFanSpeed(instance, FanSpeed_Off, care);
   VoteForIceCabinetFanSpeed(instance, FanSpeed_Off, care);
   VoteForDeliFanSpeed(instance, FanSpeed_Off, care);
   VoteForFreshFoodDamperPosition(
      instance,
      instance->_private.defrostData->dwellData.dwellFreshFoodDamperPosition,
      care);
   VoteForConvertibleCompartmentDamperPosition(instance, DamperPosition_Closed, care);
   VoteForDeliDamperPosition(instance, DamperPosition_Closed, care);
   VoteForSealedSystemValvePosition(
      instance,
      instance->_private.defrostData->dwellData.dwellSealedSystemValvePosition,
      care);
}

static void VoteForPostDwellLoads(Defrost_t *instance, bool care)
{
   VoteForCompressorSpeed(
      instance,
      instance->_private.defrostData->postDwellData.postDwellCompressorSpeed,
      care);
   VoteForCondenserFanSpeed(
      instance,
      instance->_private.defrostData->postDwellData.postDwellCondenserFanSpeed,
      care);
   VoteForFreezerDefrostHeater(instance, HeaterState_Off, care);
   VoteForFreshFoodDefrostHeater(instance, HeaterState_Off, care);
   VoteForConvertibleCompartmentDefrostHeater(instance, HeaterState_Off, care);
   VoteForFreezerEvapFanSpeed(instance, FanSpeed_Off, care);
   VoteForFreshFoodEvapFanSpeed(instance, FanSpeed_Off, care);
   VoteForConvertibleCompartmentEvapFanSpeed(instance, FanSpeed_Off, care);
   VoteForIceCabinetFanSpeed(instance, FanSpeed_Off, care);
   VoteForDeliFanSpeed(instance, FanSpeed_Off, care);
   VoteForFreshFoodDamperPosition(
      instance,
      instance->_private.defrostData->postDwellData.postDwellFreshFoodDamperPosition,
      care);
   VoteForConvertibleCompartmentDamperPosition(
      instance,
      instance->_private.defrostData->postDwellData.postDwellConvertibleCompartmentDamperPosition,
      care);
   VoteForDeliDamperPosition(
      instance,
      instance->_private.defrostData->postDwellData.postDwellDeliDamperPosition,
      care);
   VoteForSealedSystemValvePosition(
      instance,
      instance->_private.defrostData->postDwellData.postDwellSealedSystemValvePosition,
      care);
}

static void IncrementNumberOfDefrostsForSecondaryOnly(Defrost_t *instance)
{
   ForEachEvaporator(&instance->_private.secondaryEvaporatorsConfigs, evapConfig, {
      uint16_t numberOfDefrosts;
      DataModel_Read(
         instance->_private.dataModel,
         evapConfig->numberOfDefrostsErd,
         &numberOfDefrosts);

      numberOfDefrosts++;
      DataModel_Write(
         instance->_private.dataModel,
         evapConfig->numberOfDefrostsErd,
         &numberOfDefrosts);
   });
}

static void IncrementNumberOfDefrostsForPrimaryAndSecondary(Defrost_t *instance)
{
   ForEachEvaporator(&instance->_private.primaryEvaporatorsConfigs, evapConfig, {
      uint16_t numberOfDefrosts;
      DataModel_Read(
         instance->_private.dataModel,
         evapConfig->numberOfDefrostsErd,
         &numberOfDefrosts);

      numberOfDefrosts++;
      DataModel_Write(
         instance->_private.dataModel,
         evapConfig->numberOfDefrostsErd,
         &numberOfDefrosts);
   });

   IncrementNumberOfDefrostsForSecondaryOnly(instance);
}

static void SetDefrostWasAbnormal(Defrost_t *instance, const EvaporatorConfiguration_t *evapConfig)
{
   DataModel_Write(
      instance->_private.dataModel,
      evapConfig->defrostWasAbnormalErd,
      set);
}

static void ClearFreezerDefrostWasAbnormal(Defrost_t *instance)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->freezerEvaporator.defrostWasAbnormalErd,
      clear);
}

static void ClearFreshFoodDefrostWasAbnormal(Defrost_t *instance)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->freshFoodEvaporator.defrostWasAbnormalErd,
      clear);
}

static void ClearConvertibleCompartmentDefrostWasAbnormal(Defrost_t *instance)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->convertibleCompartmentEvaporator.defrostWasAbnormalErd,
      clear);
}

static void IncrementNumberOfAbnormalDefrosts(Defrost_t *instance, const EvaporatorConfiguration_t *evapConfig)
{
   uint16_t numberOfAbnormalDefrosts;
   DataModel_Read(
      instance->_private.dataModel,
      evapConfig->numberOfAbnormalDefrostsErd,
      &numberOfAbnormalDefrosts);

   numberOfAbnormalDefrosts++;
   DataModel_Write(
      instance->_private.dataModel,
      evapConfig->numberOfAbnormalDefrostsErd,
      &numberOfAbnormalDefrosts);
}

static bool FreezerDefrostHeaterOnTimeLessThanAbnormalDefrostTime(Defrost_t *instance)
{
   uint8_t freezerHeaterOnTimeInMinutes;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerEvaporator.defrostHeaterOnTimeInMinutesErd,
      &freezerHeaterOnTimeInMinutes);

   return freezerHeaterOnTimeInMinutes <
      instance->_private.defrostData->heaterOnData.freezerHeater.heaterOnTimeToSetAbnormalDefrostInMinutes;
}

static bool FreshFoodDefrostHeaterOnTimeLessThanAbnormalDefrostTime(Defrost_t *instance)
{
   uint8_t freshFoodHeaterOnTimeInMinutes;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freshFoodEvaporator.defrostHeaterOnTimeInMinutesErd,
      &freshFoodHeaterOnTimeInMinutes);

   return freshFoodHeaterOnTimeInMinutes <
      instance->_private.defrostData->heaterOnData.freshFoodHeater.heaterOnTimeToSetAbnormalDefrostInMinutes;
}

static bool ConvertibleCompartmentDefrostHeaterOnTimeLessThanAbnormalDefrostTime(Defrost_t *instance)
{
   uint8_t convertibleCompartmentHeaterOnTimeInMinutes;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->convertibleCompartmentEvaporator.defrostHeaterOnTimeInMinutesErd,
      &convertibleCompartmentHeaterOnTimeInMinutes);

   if(ConvertibleCompartmentIsActingAsAFreezer(instance))
   {
      return convertibleCompartmentHeaterOnTimeInMinutes <
         instance->_private.defrostData->heaterOnData.convertibleCompartmentHeaterAsFreezer.heaterOnTimeToSetAbnormalDefrostInMinutes;
   }
   else
   {
      return convertibleCompartmentHeaterOnTimeInMinutes <
         instance->_private.defrostData->heaterOnData.convertibleCompartmentHeaterAsFreshFood.heaterOnTimeToSetAbnormalDefrostInMinutes;
   }
}

static HsmState_t InitialState(Defrost_t *instance)
{
   DefrostState_t defrostState;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->defrostStateErd,
      &defrostState);

   HsmState_t initialState = State_Idle;

   if(ClearedEepromStartup(instance))
   {
      initialState = State_Idle;
   }
   else if(CabinetWasTooWarmOnPowerUp(instance))
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
      else if(defrostState == DefrostState_PrechillPrep ||
         defrostState == DefrostState_Prechill ||
         defrostState == DefrostState_HeaterOn)
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

static void SetInvalidEvaporatorThermistorDuringDefrostTo(Defrost_t *instance, bool state)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->invalidEvaporatorThermistorDuringDefrostErd,
      &state);
}

static bool InvalidEvaporatorThermistorDuringDefrostIsSet(Defrost_t *instance)
{
   bool state;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->invalidEvaporatorThermistorDuringDefrostErd,
      &state);

   return state;
}

static void SetDefrostHeaterOnForMaxTimeFaultTo(Defrost_t *instance, const EvaporatorConfiguration_t *evapConfig, bool state)
{
   DataModel_Write(
      instance->_private.dataModel,
      evapConfig->defrostHeaterOnForMaxTimeFaultErd,
      &state);
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

static void ClearUseAhamPrechillReadyToDefrostTimeFlag(Defrost_t *instance)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->useAhamPrechillReadyToDefrostTimeAndResetDefrostCountsErd,
      clear);
}

static bool AllSabbathModesAreDisabled(Defrost_t *instance)
{
   return !AtLeastOneSabbathModeIsEnabled(instance);
}

static bool SabbathIsReadyToDefrost(Defrost_t *instance)
{
   bool state;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->sabbathIsReadyToDefrostErd,
      &state);

   return state;
}

static bool ReadyToDefrost(Defrost_t *instance)
{
   bool state;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->readyToDefrostErd,
      &state);

   return state;
}

static bool EnhancedSabbathIsRequestingDefrost(Defrost_t *instance)
{
   bool enhancedSabbathIsRequestingDefrost;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->enhancedSabbathIsRequestingDefrostErd,
      &enhancedSabbathIsRequestingDefrost);

   return enhancedSabbathIsRequestingDefrost;
}

static void RequestValveHoming(Defrost_t *instance)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->sealedSystemValveHomingRequestErd,
      set);
}

static TimerTicks_t PrechillTimeLeft(Defrost_t *instance)
{
   uint16_t maxPrechillTime = MaxPrechillTimeInMinutes(instance);
   uint8_t minutes = TRUNCATE_UNSIGNED_SUBTRACTION(maxPrechillTime, TimeThatPrechillConditionsAreMetInMinutes(instance));
   return minutes * MSEC_PER_MIN;
}

static bool EvapThermistorIsValid(Defrost_t *instance, const EvaporatorConfiguration_t *evap)
{
   bool evapThermistorIsValid;
   DataModel_Read(
      instance->_private.dataModel,
      evap->evaporatorThermistorIsValidResolvedErd,
      &evapThermistorIsValid);

   return evapThermistorIsValid;
}

static bool PrechillExitConditionMetForAnyPrimaryEvaporators(Defrost_t *instance)
{
   ForEachEvaporator(&instance->_private.primaryEvaporatorsConfigs, evapConfig, {
      TemperatureDegFx100_t temperature;
      DataModel_Read(
         instance->_private.dataModel,
         evapConfig->evaporatorFilteredTemperatureResolvedInDegFx100Erd,
         &temperature);

      if(EvapThermistorIsValid(instance, evapConfig) && temperature <= PrechillData(instance)->prechillPrimaryEvaporatorExitTemperatureInDegFx100)
      {
         return true;
      }
   });

   return false;
}

static bool PrechillExitConditionMetForFreezer(Defrost_t *instance)
{
   TemperatureDegFx100_t temperature;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerFilteredTemperatureResolvedInDegFx100Erd,
      &temperature);

   return temperature <= PrechillData(instance)->prechillFreezerMinTempInDegFx100;
}

static bool PrechillExitConditionMetForFreshFood(Defrost_t *instance)
{
   TemperatureDegFx100_t temperature;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freshFoodFilteredTemperatureResolvedInDegFx100Erd,
      &temperature);

   TemperatureDegFx100_t min = PrechillData(instance)->prechillFreshFoodMinTempInDegFx100;
   TemperatureDegFx100_t max = PrechillData(instance)->prechillFreshFoodMaxTempInDegFx100;
   return !BETWEEN(min, temperature, max);
}

static bool PrechillExitConditionMetForConvertibleCompartment(Defrost_t *instance)
{
   TemperatureDegFx100_t temperature;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->convertibleCompartmentFilteredTemperatureResolvedInDegFx100Erd,
      &temperature);

   switch(ConvertibleCompartmentState(instance))
   {
      case ConvertibleCompartmentStateType_Freezer:
         return temperature <= PrechillData(instance)->prechillConvertibleCompartmentAsFreezerMinTempInDegFx100;

      case ConvertibleCompartmentStateType_FreshFood:
         return !BETWEEN(
            PrechillData(instance)->prechillConvertibleCompartmentAsFreshFoodMinTempInDegFx100,
            temperature,
            PrechillData(instance)->prechillConvertibleCompartmentAsFreshFoodMaxTempInDegFx100);
   }

   return false;
}

static bool PrimaryAndSecondaryEvaporatorsAreAllValid(Defrost_t *instance)
{
   ForEachEvaporator(&instance->_private.primaryEvaporatorsConfigs, evapConfig, {
      bool evaporatorThermistorIsValid;
      DataModel_Read(
         instance->_private.dataModel,
         evapConfig->evaporatorThermistorIsValidResolvedErd,
         &evaporatorThermistorIsValid);

      if(!evaporatorThermistorIsValid)
      {
         return false;
      }
   });

   ForEachEvaporator(&instance->_private.secondaryEvaporatorsConfigs, evapConfig, {
      bool evaporatorThermistorIsValid;
      DataModel_Read(
         instance->_private.dataModel,
         evapConfig->evaporatorThermistorIsValidResolvedErd,
         &evaporatorThermistorIsValid);

      if(!evaporatorThermistorIsValid)
      {
         return false;
      }
   });

   return true;
}

static bool AllHeatersAreOff(Defrost_t *instance)
{
   ForEachEvaporator(&instance->_private.primaryEvaporatorsConfigs, evapConfig, {
      HeaterVotedState_t heaterVote;
      DataModel_Read(
         instance->_private.dataModel,
         evapConfig->heaterVoteErd,
         &heaterVote);

      if(heaterVote.state != HeaterState_Off)
      {
         return false;
      }
   });

   ForEachEvaporator(&instance->_private.secondaryEvaporatorsConfigs, evapConfig, {
      HeaterVotedState_t heaterVote;
      DataModel_Read(
         instance->_private.dataModel,
         evapConfig->heaterVoteErd,
         &heaterVote);

      if(heaterVote.state != HeaterState_Off)
      {
         return false;
      }
   });

   return true;
}

static bool ExitPostDwellConditionIsMetWithPrimaryEvapTempOf(Defrost_t *instance, TemperatureDegFx100_t primaryEvapTemp)
{
   const DefrostType_t currentDefrostType = CurrentDefrostType(instance);
   return (
      (currentDefrostType == DefrostType_Full &&
         primaryEvapTemp <= instance->_private.defrostData->postDwellData.postDwellPrimaryEvaporatorExitTemperatureInDegFx100) ||

      (currentDefrostType == DefrostType_SecondaryOnly &&
         primaryEvapTemp <= instance->_private.defrostData->postDwellData.secondaryOnlyPostDwellPrimaryEvaporatorExitTemperatureInDegFx100));
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
      case Signal_AtLeastOneSabbathModeIsDisabled:
         if(AllSabbathModesAreDisabled(instance) && ReadyToDefrost(instance))
         {
            if(DontSkipPrechill(instance))
            {
               Hsm_Transition(hsm, State_PrechillPrep);
            }
            else
            {
               if(AnyPreviousDefrostWasAbnormal(instance) ||
                  CabinetWasTooWarmOnPowerUp(instance) ||
                  InvalidEvaporatorThermistorDuringDefrostIsSet(instance) ||
                  ClearedEepromStartup(instance))
               {
                  Hsm_Transition(hsm, State_HeaterOnEntry);
               }
               else
               {
                  Hsm_Transition(hsm, State_PrechillPrep);
               }
            }
         }
         break;

      case Signal_EnhancedSabbathIsRequestingToDefrost:
      case Signal_SabbathIsReadyToDefrost:
      case Signal_AtLeastOneSabbathModeIsEnabled:
         if(AtLeastOneSabbathModeIsEnabled(instance) && SabbathIsReadyToDefrost(instance))
         {
            if(EnhancedSabbathIsRequestingDefrost(instance) && EnhancedSabbathModeIsEnabled(instance))
            {
               Hsm_Transition(hsm, State_HeaterOnEntry);
            }
            else if(!EnhancedSabbathModeIsEnabled(instance))
            {
               Hsm_Transition(hsm, State_PrechillPrep);
            }
         }
         break;

      case Hsm_Exit:
         ResetCabinetWasTooWarmOnPowerUp(instance);
         SetClearedEepromStartupTo(instance, false);
         SetPrimaryEvaporators(instance);
         SetSecondaryEvaporators(instance);
         if(!instance->_private.skipDefrostTypeDeterminationDueToTestRequest &&
            !instance->_private.skipDefrostTypeDeterminationDueToDefrostDisable)
         {
            DetermineNumberOfSecondaryOnlyDefrostsBeforeAFullDefrost(instance);
            SetDefrostType(instance);
         }
         instance->_private.skipDefrostTypeDeterminationDueToTestRequest = false;
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

      case Signal_ExitDefrostHeaterOnStateTestRequest:
         ClearDefrostTestStateRequest(instance);
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
   IGNORE(data);
   IGNORE(instance);

   switch(signal)
   {
      case Hsm_Entry:
         break;

      case Signal_FreezerThermistorIsInvalid:
      case Signal_FreezerEvaporatorThermistorIsInvalid:
      case Signal_FreshFoodEvaporatorThermistorIsInvalid:
      case Signal_ConvertibleCompartmentEvaporatorThermistorIsInvalid:
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
         if((!FreezerThermistorIsValid(instance) && FreezerThermistorHasBeenDiscovered(instance)) ||
            (!FreezerEvaporatorThermistorIsValid(instance) && FreezerEvaporatorThermistorHasBeenDiscovered(instance)) ||
            (!FreshFoodEvaporatorThermistorIsValid(instance) && FreshFoodEvaporatorThermistorHasBeenDiscovered(instance)) ||
            (!ConvertibleCompartmentEvaporatorThermistorIsValid(instance) && ConvertibleCompartmentEvaporatorThermistorHasBeenDiscovered(instance)))
         {
            Hsm_Transition(hsm, State_HeaterOnEntry);
            break;
         }

         SetHsmStateTo(instance, DefrostHsmState_PrechillPrep);
         ClearDontSkipPrechillFlag(instance);
         ClearDefrostTestStateRequest(instance);
         ClearUseAhamPrechillReadyToDefrostTimeFlag(instance);

         if(PrechillConditionsAreMet(instance))
         {
            Hsm_Transition(hsm, State_Prechill);
         }
         else
         {
            StartDefrostTimer(
               instance,
               instance->_private.defrostData->prechillPrepData.maxPrechillPrepTimeInMinutes * MSEC_PER_MIN);
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
   (void)data;

   switch(signal)
   {
      case Hsm_Entry:
         SetHsmStateTo(instance, DefrostHsmState_Prechill);
         VoteForPrechillLoads(instance, Vote_Care);

         if(PrechillTimeLeft(instance) == 0 ||
            (FreezerThermistorIsValid(instance) && PrechillExitConditionMetForFreezer(instance)) ||
            (PrechillExitConditionMetForAnyPrimaryEvaporators(instance)) ||
            (FreshFoodThermistorIsValid(instance) && PrechillExitConditionMetForFreshFood(instance)) ||
            (ConvertibleCompartmentThermistorIsValid(instance) && PrechillExitConditionMetForConvertibleCompartment(instance)))
         {
            Hsm_SendSignal(hsm, Signal_PrechillTemperatureExitConditionMet, NULL);
            break;
         }

         StartDefrostTimer(instance, PrechillTimeLeft(instance));
         break;

      case Signal_FreezerTemperatureChanged:
         if(PrechillExitConditionMetForFreezer(instance))
         {
            Hsm_SendSignal(hsm, Signal_PrechillTemperatureExitConditionMet, NULL);
         }
         break;

      case Signal_PrimaryEvaporatorTemperatureChanged: {
         const TemperatureDegFx100_t *temperature = data;
         if(*temperature <= PrechillData(instance)->prechillPrimaryEvaporatorExitTemperatureInDegFx100)
         {
            Hsm_SendSignal(hsm, Signal_PrechillTemperatureExitConditionMet, NULL);
         }
         break;
      }

      case Signal_FreshFoodTemperatureChanged:
         if(PrechillExitConditionMetForFreshFood(instance))
         {
            Hsm_SendSignal(hsm, Signal_PrechillTemperatureExitConditionMet, NULL);
         }
         break;

      case Signal_ConvertibleCompartmentTemperatureChanged:
         if(PrechillExitConditionMetForConvertibleCompartment(instance))
         {
            Hsm_SendSignal(hsm, Signal_PrechillTemperatureExitConditionMet, NULL);
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
         VoteForHeaterOnEntryLoads(instance, Vote_Care);
         StartDefrostTimer(
            instance,
            instance->_private.defrostData->heaterOnEntryData.defrostHeaterOnDelayAfterCompressorOffInSeconds * MSEC_PER_SEC);
         break;

      case Signal_DefrostTimerExpired:
         Hsm_Transition(hsm, State_HeaterOn);
         break;

      case Signal_ExitDefrostHeaterOnStateTestRequest:
         Hsm_Transition(hsm, State_Dwell);
         break;

      case Hsm_Exit:
         StopTimer(instance, &instance->_private.defrostTimer);
         VoteForHeaterOnEntryLoads(instance, Vote_DontCare);
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

static bool State_HeaterOn(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   Defrost_t *instance = InstanceFromHsm(hsm);
   const EvaporatorConfiguration_t *evapConfig = data;

   switch(signal)
   {
      case Hsm_Entry:
         SetHsmStateTo(instance, DefrostHsmState_HeaterOn);
         VoteForHeaterOnLoads(instance, Vote_Care);

         switch(CurrentDefrostType(instance))
         {
            case DefrostType_Full:
               VoteForPrimaryEvaporatorHeaters(instance, HeaterState_On, Vote_Care);
               VoteForSecondaryEvaporatorHeaters(instance, HeaterState_On, Vote_Care);
               break;

            case DefrostType_SecondaryOnly:
               VoteForSecondaryEvaporatorHeaters(instance, HeaterState_On, Vote_Care);
               break;
         }
         break;

      case Signal_HeaterMaxOnTimeReached:
         VoteForDefrostHeater(instance, evapConfig, HeaterState_Off, Vote_Care);

         if(EvaporatorThermistorIsValid(instance, evapConfig))
         {
            SetDefrostHeaterOnForMaxTimeFaultTo(instance, evapConfig, true);
         }
         else
         {
            SetInvalidEvaporatorThermistorDuringDefrostTo(instance, true);
         }

         if(AllHeatersAreOff(instance))
         {
            Hsm_Transition(hsm, State_Dwell);
         }
         break;

      case Signal_AbnormalHeaterOnTimeReached:
         IncrementNumberOfAbnormalDefrosts(instance, evapConfig);
         SetDefrostWasAbnormal(instance, evapConfig);
         break;

      case Signal_FreezerEvaporatorTemperatureChanged: {
         const TemperatureDegFx100_t *freezerEvaporatorTemperature = data;
         if(*freezerEvaporatorTemperature >= instance->_private.defrostData->heaterOnData.freezerHeater.defrostTerminationTemperatureInDegFx100)
         {
            VoteForFreezerDefrostHeater(instance, HeaterState_Off, Vote_Care);

            if(FreezerDefrostHeaterOnTimeLessThanAbnormalDefrostTime(instance))
            {
               ClearFreezerDefrostWasAbnormal(instance);
            }

            SetDefrostHeaterOnForMaxTimeFaultTo(instance, &instance->_private.config->freezerEvaporator, false);

            if(AllHeatersAreOff(instance))
            {
               Hsm_Transition(hsm, State_Dwell);
            }
         }
      }
      break;

      case Signal_FreshFoodEvaporatorTemperatureChanged: {
         const TemperatureDegFx100_t *freshFoodEvaporatorTemperature = data;
         if(*freshFoodEvaporatorTemperature >= instance->_private.defrostData->heaterOnData.freshFoodHeater.defrostTerminationTemperatureInDegFx100)
         {
            VoteForFreshFoodDefrostHeater(instance, HeaterState_Off, Vote_Care);

            if(FreshFoodDefrostHeaterOnTimeLessThanAbnormalDefrostTime(instance))
            {
               ClearFreshFoodDefrostWasAbnormal(instance);
            }

            SetDefrostHeaterOnForMaxTimeFaultTo(instance, &instance->_private.config->freshFoodEvaporator, false);

            if(AllHeatersAreOff(instance))
            {
               Hsm_Transition(hsm, State_Dwell);
            }
         }
      }
      break;

      case Signal_ConvertibleCompartmentEvaporatorTemperatureChanged: {
         const TemperatureDegFx100_t *convertibleCompartmentEvaporatorTemperature = data;
         if(*convertibleCompartmentEvaporatorTemperature >= instance->_private.defrostData->heaterOnData.convertibleCompartmentHeater.defrostTerminationTemperatureInDegFx100)
         {
            VoteForConvertibleCompartmentDefrostHeater(instance, HeaterState_Off, Vote_Care);

            if(ConvertibleCompartmentDefrostHeaterOnTimeLessThanAbnormalDefrostTime(instance))
            {
               ClearConvertibleCompartmentDefrostWasAbnormal(instance);
            }

            SetDefrostHeaterOnForMaxTimeFaultTo(instance, &instance->_private.config->convertibleCompartmentEvaporator, false);

            if(AllHeatersAreOff(instance))
            {
               Hsm_Transition(hsm, State_Dwell);
            }
         }
      }
      break;

      case Signal_ExitDefrostHeaterOnStateTestRequest:
         Hsm_Transition(hsm, State_Dwell);
         break;

      case Hsm_Exit:
         if(PrimaryAndSecondaryEvaporatorsAreAllValid(instance))
         {
            SetInvalidEvaporatorThermistorDuringDefrostTo(instance, false);
         }

         VoteForHeaterOnLoads(instance, Vote_DontCare);

         switch(CurrentDefrostType(instance))
         {
            case DefrostType_Full:
               VoteForPrimaryEvaporatorHeaters(instance, HeaterState_Off, Vote_DontCare);
               VoteForSecondaryEvaporatorHeaters(instance, HeaterState_Off, Vote_DontCare);
               IncrementNumberOfDefrostsForPrimaryAndSecondary(instance);
               break;

            case DefrostType_SecondaryOnly:
               VoteForSecondaryEvaporatorHeaters(instance, HeaterState_Off, Vote_DontCare);
               IncrementNumberOfDefrostsForSecondaryOnly(instance);
               break;
         }
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

static bool PostDwellPrimaryEvapExitConditionIsMet(Defrost_t *instance)
{
   ForEachEvaporator(&instance->_private.primaryEvaporatorsConfigs, evapConfig, {
      TemperatureDegFx100_t temp;
      DataModel_Read(
         instance->_private.dataModel,
         evapConfig->evaporatorFilteredTemperatureResolvedInDegFx100Erd,
         &temp);
      if(ExitPostDwellConditionIsMetWithPrimaryEvapTempOf(instance, temp))
      {
         return true;
      }
   });
   return false;
}

static bool State_Dwell(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   Defrost_t *instance = InstanceFromHsm(hsm);
   IGNORE(data);

   switch(signal)
   {
      case Hsm_Entry:
         SetHsmStateTo(instance, DefrostHsmState_Dwell);
         RequestValveHoming(instance);
         VoteForDwellLoads(instance, Vote_Care);
         StartDefrostTimer(
            instance,
            instance->_private.defrostData->dwellData.dwellTimeInMinutes * MSEC_PER_MIN);
         ClearDefrostTestStateRequest(instance);
         break;

      case Signal_DefrostTimerExpired:
         if(PostDwellPrimaryEvapExitConditionIsMet(instance))
         {
            Hsm_Transition(hsm, State_Idle);
         }
         else
         {
            Hsm_Transition(hsm, State_PostDwell);
         }
         break;

      case Hsm_Exit:
         StopTimer(instance, &instance->_private.defrostTimer);
         VoteForDwellLoads(instance, Vote_DontCare);
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
         instance->_private.skipDefrostTypeDeterminationDueToDefrostDisable = true;
         Hsm_Transition(hsm, State_Disabled);
         break;

      case Signal_IdleTestRequest:
      case Signal_PrechillTestRequest:
      case Signal_DefrostTestRequest:
         instance->_private.skipDefrostTypeDeterminationDueToTestRequest = true;
         Hsm_Transition(hsm, State_Idle);
         break;

      case Signal_ExitDefrostHeaterOnStateTestRequest:
         ClearDefrostTestStateRequest(instance);
         break;

      case Hsm_Exit:
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
         StartDefrostTimer(
            instance,
            PostDwellExitTimeInMinutes(instance) * MSEC_PER_MIN);
         break;

      case Signal_DefrostTimerExpired:
         Hsm_Transition(hsm, State_Idle);
         break;

      case Signal_PrimaryEvaporatorTemperatureChanged: {
         const TemperatureDegFx100_t *primaryEvapTemp = data;
         if(ExitPostDwellConditionIsMetWithPrimaryEvapTempOf(instance, *primaryEvapTemp))
         {
            Hsm_Transition(hsm, State_Idle);
         }
         break;
      }

      case Hsm_Exit:
         VoteForPostDwellLoads(instance, Vote_DontCare);
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
         VoteForFreshFoodDefrostHeater(instance, HeaterState_Off, Vote_Care);
         VoteForConvertibleCompartmentDefrostHeater(instance, HeaterState_Off, Vote_Care);
         SetWaitingToDefrostTo(instance, false);
         SetDefrostingTo(instance, false);
         break;

      case Signal_EnableDefrost:
         Hsm_Transition(hsm, State_Idle);
         break;

      case Signal_IdleTestRequest:
      case Signal_DefrostTestRequest:
      case Signal_PrechillTestRequest:
      case Signal_ExitDefrostHeaterOnStateTestRequest:
         ClearDefrostTestStateRequest(instance);
         break;

      case Hsm_Exit:
         VoteForFreezerDefrostHeater(instance, HeaterState_Off, Vote_DontCare);
         VoteForFreshFoodDefrostHeater(instance, HeaterState_Off, Vote_DontCare);
         VoteForConvertibleCompartmentDefrostHeater(instance, HeaterState_Off, Vote_DontCare);
         instance->_private.skipDefrostTypeDeterminationDueToDefrostDisable = false;
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
   else if(erd == instance->_private.config->freezerThermistorIsValidResolvedErd)
   {
      const bool *freezerThermistorIsValid = onChangeData->data;

      if(!(*freezerThermistorIsValid) && FreezerThermistorHasBeenDiscovered(instance))
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_FreezerThermistorIsInvalid, NULL);
      }
   }
   else if(erd == instance->_private.config->freezerEvaporator.evaporatorThermistorIsValidResolvedErd)
   {
      const bool *freezerEvaporatorThermistorIsValid = onChangeData->data;

      if(!(*freezerEvaporatorThermistorIsValid) && FreezerEvaporatorThermistorHasBeenDiscovered(instance))
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_FreezerEvaporatorThermistorIsInvalid, NULL);
      }
   }
   else if(erd == instance->_private.config->freshFoodEvaporator.evaporatorThermistorIsValidResolvedErd)
   {
      const bool *freshFoodEvaporatorThermistorIsValid = onChangeData->data;

      if(!(*freshFoodEvaporatorThermistorIsValid) && FreshFoodEvaporatorThermistorHasBeenDiscovered(instance))
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_FreshFoodEvaporatorThermistorIsInvalid, NULL);
      }
   }
   else if(erd == instance->_private.config->convertibleCompartmentEvaporator.evaporatorThermistorIsValidResolvedErd)
   {
      const bool *convertibleCompartmentEvaporatorThermistorIsValid = onChangeData->data;

      if(!(*convertibleCompartmentEvaporatorThermistorIsValid) && ConvertibleCompartmentEvaporatorThermistorHasBeenDiscovered(instance))
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_ConvertibleCompartmentEvaporatorThermistorIsInvalid, NULL);
      }
   }
   else if(erd == instance->_private.config->freezerEvaporator.evaporatorFilteredTemperatureResolvedInDegFx100Erd)
   {
      const TemperatureDegFx100_t *freezerEvaporatorTemperature = onChangeData->data;

      if(FreezerEvaporatorThermistorIsValid(instance))
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_FreezerEvaporatorTemperatureChanged, freezerEvaporatorTemperature);
         if(FreezerIsAPrimaryEvap(instance))
         {
            Hsm_SendSignal(&instance->_private.hsm, Signal_PrimaryEvaporatorTemperatureChanged, freezerEvaporatorTemperature);
         }
      }
   }
   else if(erd == instance->_private.config->freshFoodEvaporator.evaporatorFilteredTemperatureResolvedInDegFx100Erd)
   {
      const TemperatureDegFx100_t *freshFoodEvaporatorTemperature = onChangeData->data;

      if(FreshFoodEvaporatorThermistorIsValid(instance))
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_FreshFoodEvaporatorTemperatureChanged, freshFoodEvaporatorTemperature);
         if(FreshFoodIsAPrimaryEvap(instance))
         {
            Hsm_SendSignal(&instance->_private.hsm, Signal_PrimaryEvaporatorTemperatureChanged, freshFoodEvaporatorTemperature);
         }
      }
   }
   else if(erd == instance->_private.config->convertibleCompartmentEvaporator.evaporatorFilteredTemperatureResolvedInDegFx100Erd)
   {
      const TemperatureDegFx100_t *convertibleCompartmentEvaporatorTemperature = onChangeData->data;

      if(ConvertibleCompartmentEvaporatorThermistorIsValid(instance))
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_ConvertibleCompartmentEvaporatorTemperatureChanged, convertibleCompartmentEvaporatorTemperature);
         if(ConvertibleCompartmentIsAPrimaryEvap(instance))
         {
            Hsm_SendSignal(&instance->_private.hsm, Signal_PrimaryEvaporatorTemperatureChanged, convertibleCompartmentEvaporatorTemperature);
         }
      }
   }
   else if(erd == instance->_private.config->freezerFilteredTemperatureResolvedInDegFx100Erd)
   {
      if(FreezerThermistorIsValid(instance))
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_FreezerTemperatureChanged, NULL);
      }
   }
   else if(erd == instance->_private.config->freshFoodFilteredTemperatureResolvedInDegFx100Erd)
   {
      if(FreshFoodThermistorIsValid(instance))
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_FreshFoodTemperatureChanged, NULL);
      }
   }
   else if(erd == instance->_private.config->convertibleCompartmentFilteredTemperatureResolvedInDegFx100Erd)
   {
      if(ConvertibleCompartmentThermistorIsValid(instance))
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_ConvertibleCompartmentTemperatureChanged, NULL);
      }
   }
   else if(erd == instance->_private.config->freezerEvaporator.defrostHeaterOnTimeInMinutesErd)
   {
      const uint8_t *freezerDefrostHeaterOnTimeInMinutes = onChangeData->data;

      if(*freezerDefrostHeaterOnTimeInMinutes >= FreezerDefrostHeaterMaxOnTimeInMinutes(instance))
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_HeaterMaxOnTimeReached, &instance->_private.config->freezerEvaporator);
      }
      else if(*freezerDefrostHeaterOnTimeInMinutes ==
         instance->_private.defrostData->heaterOnData.freezerHeater.heaterOnTimeToSetAbnormalDefrostInMinutes)
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_AbnormalHeaterOnTimeReached, &instance->_private.config->freezerEvaporator);
      }
   }
   else if(erd == instance->_private.config->freshFoodEvaporator.defrostHeaterOnTimeInMinutesErd)
   {
      const uint8_t *freshFoodDefrostHeaterOnTimeInMinutes = onChangeData->data;

      if(*freshFoodDefrostHeaterOnTimeInMinutes >= FreshFoodDefrostHeaterMaxOnTimeInMinutes(instance))
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_HeaterMaxOnTimeReached, &instance->_private.config->freshFoodEvaporator);
      }
      else if(*freshFoodDefrostHeaterOnTimeInMinutes ==
         instance->_private.defrostData->heaterOnData.freshFoodHeater.heaterOnTimeToSetAbnormalDefrostInMinutes)
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_AbnormalHeaterOnTimeReached, &instance->_private.config->freshFoodEvaporator);
      }
   }
   else if(erd == instance->_private.config->convertibleCompartmentEvaporator.defrostHeaterOnTimeInMinutesErd)
   {
      const uint8_t *convertibleCompartmentDefrostHeaterOnTimeInMinutes = onChangeData->data;

      if(*convertibleCompartmentDefrostHeaterOnTimeInMinutes >= ConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutes(instance))
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_HeaterMaxOnTimeReached, &instance->_private.config->convertibleCompartmentEvaporator);
      }
      else if((ConvertibleCompartmentIsActingAsAFreezer(instance) && (*convertibleCompartmentDefrostHeaterOnTimeInMinutes == instance->_private.defrostData->heaterOnData.convertibleCompartmentHeaterAsFreezer.heaterOnTimeToSetAbnormalDefrostInMinutes)) ||
         (ConvertibleCompartmentIsActingAsAFreshFood(instance) && (*convertibleCompartmentDefrostHeaterOnTimeInMinutes == instance->_private.defrostData->heaterOnData.convertibleCompartmentHeaterAsFreshFood.heaterOnTimeToSetAbnormalDefrostInMinutes)))
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_AbnormalHeaterOnTimeReached, &instance->_private.config->convertibleCompartmentEvaporator);
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

         case DefrostTestStateRequest_ExitDefrostHeaterOnState:
            Hsm_SendSignal(&instance->_private.hsm, Signal_ExitDefrostHeaterOnStateTestRequest, NULL);
            break;

         default:
            break;
      }
   }
   else if(erd == instance->_private.config->sabbathIsReadyToDefrostErd)
   {
      const bool *state = onChangeData->data;
      if(*state)
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_SabbathIsReadyToDefrost, NULL);
      }
   }
   else if((erd == instance->_private.config->sabbathModeErd) || (erd == instance->_private.config->enhancedSabbathModeErd))
   {
      const bool *state = onChangeData->data;
      if(*state)
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_AtLeastOneSabbathModeIsEnabled, NULL);
      }
      else
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_AtLeastOneSabbathModeIsDisabled, NULL);
      }
   }
   else if(erd == instance->_private.config->enhancedSabbathIsRequestingDefrostErd)
   {
      const bool *enhancedSabbathIsRequestingDefrost = onChangeData->data;
      if(*enhancedSabbathIsRequestingDefrost)
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_EnhancedSabbathIsRequestingToDefrost, NULL);
      }
   }
   else if(erd == instance->_private.config->convertibleCompartmentStateErd)
   {
      SetPrimaryEvaporators(instance);
      SetSecondaryEvaporators(instance);

      if(ConvertibleCompartmentIsAPrimaryEvap(instance))
      {
         TemperatureDegFx100_t temperature;
         DataModel_Read(
            instance->_private.dataModel,
            Erd_ConvertibleCompartmentEvap_FilteredTemperatureResolvedInDegFx100,
            &temperature);
         Hsm_SendSignal(&instance->_private.hsm, Signal_PrimaryEvaporatorTemperatureChanged, &temperature);
      }
   }
}

static void SetErdIfCabinetFilteredTemperatureIsTooWarm(Defrost_t *instance)
{
   bool cabinetTemperatureIsTooWarm = false;

   if(FreezerThermistorIsValid(instance))
   {
      TemperatureDegFx100_t freezerFilteredResolvedTemperatureInDegFx100;
      DataModel_Read(
         instance->_private.dataModel,
         instance->_private.config->freezerFilteredTemperatureResolvedInDegFx100Erd,
         &freezerFilteredResolvedTemperatureInDegFx100);

      TwoDimensionalCalculatedGridLines_t calcGridLines;
      DataModel_Read(
         instance->_private.dataModel,
         Erd_FreshFoodAndFreezerGrid_CalculatedGridLines,
         &calcGridLines);

      TemperatureDegFx100_t gridFreezerExtremeHystTemperature = calcGridLines.secondDimensionGridLines.gridLinesDegFx100[GridLine_FreezerExtremeHigh];

      cabinetTemperatureIsTooWarm = ((freezerFilteredResolvedTemperatureInDegFx100 > gridFreezerExtremeHystTemperature) ||
         (freezerFilteredResolvedTemperatureInDegFx100 >= instance->_private.defrostData->heaterOnData.freezerHeater.defrostTerminationTemperatureInDegFx100));
   }

   ForEachEvaporator(&instance->_private.primaryEvaporatorsConfigs, evapConfig, {
      TemperatureDegFx100_t evaporatorHeaterTerminationTemperatureInDegFX100;
      memcpy(
         &evaporatorHeaterTerminationTemperatureInDegFX100,
         ((const uint8_t *)instance->_private.defrostData) + evapConfig->offsetInParametricForEvaporatorHeaterTerminationTemperature,
         sizeof(evaporatorHeaterTerminationTemperatureInDegFX100));

      bool evaporatorThermistorIsValid;
      DataModel_Read(
         instance->_private.dataModel,
         evapConfig->evaporatorThermistorIsValidResolvedErd,
         &evaporatorThermistorIsValid);

      TemperatureDegFx100_t evaporatorTemperatureInDegFX100;
      DataModel_Read(
         instance->_private.dataModel,
         evapConfig->evaporatorFilteredTemperatureResolvedInDegFx100Erd,
         &evaporatorTemperatureInDegFX100);

      if(evaporatorThermistorIsValid &&
         (evaporatorTemperatureInDegFX100 >= evaporatorHeaterTerminationTemperatureInDegFX100))
      {
         cabinetTemperatureIsTooWarm = true;
         break;
      }
   });

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->cabinetFilteredTemperatureTooWarmOnPowerUpErd,
      &cabinetTemperatureIsTooWarm);
}

static void OverrideNextDefrostTypeIfEepromClearedOnPowerUpOrPreviouslyDisabledOrCabinetIsTooWarm(Defrost_t *instance)
{
   DefrostState_t defrostState;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->defrostStateErd,
      &defrostState);

   if(ClearedEepromStartup(instance) ||
      CabinetWasTooWarmOnPowerUp(instance) ||
      defrostState == DefrostState_Disabled)
   {
      DefrostType_t type = DefrostType_Full;
      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->nextDefrostTypeOverrideErd,
         &type);
   }
}

static void UpdateDefrostStateOnlyWhenDefrostHsmStateIsIdle(Defrost_t *instance)
{
   DefrostHsmState_t initialHsmState;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->defrostHsmStateErd,
      &initialHsmState);

   // DefrostStateOnCompareMatch doesn't cover the case.
   // Because on change event doesn't occur when the initial HSM state is idle.
   if(initialHsmState == DefrostHsmState_Idle)
   {
      DefrostState_t initialDefrostState;

      initialDefrostState = DefrostState_Idle;
      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->defrostStateErd,
         &initialDefrostState);
   }
}

void Defrost_Init(
   Defrost_t *instance,
   I_DataModel_t *dataModel,
   const DefrostConfiguration_t *defrostConfig,
   const DefrostData_t *defrostData,
   const PlatformData_t *platformData,
   const EnhancedSabbathData_t *enhancedSabbathData)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = defrostConfig;
   instance->_private.defrostData = defrostData;
   instance->_private.platformData = platformData;
   instance->_private.enhancedSabbathData = enhancedSabbathData;
   instance->_private.skipDefrostTypeDeterminationDueToTestRequest = false;
   instance->_private.skipDefrostTypeDeterminationDueToDefrostDisable = false;

   SetPrimaryEvaporators(instance);
   SetSecondaryEvaporators(instance);

   if(HasOneEvaporator(instance))
   {
      SetCurrentDefrostType(instance, DefrostType_Full);
   }

   DetermineNumberOfSecondaryOnlyDefrostsBeforeAFullDefrost(instance);

   SetErdIfCabinetFilteredTemperatureIsTooWarm(instance);

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->defrostPowerUpReadyErd,
      set);

   OverrideNextDefrostTypeIfEepromClearedOnPowerUpOrPreviouslyDisabledOrCabinetIsTooWarm(instance);

   Hsm_Init(&instance->_private.hsm, &hsmConfiguration, InitialState(instance));

   EventSubscription_Init(
      &instance->_private.dataModelSubscription,
      instance,
      DataModelChanged);

   DataModel_SubscribeAll(
      dataModel,
      &instance->_private.dataModelSubscription);

   UpdateDefrostStateOnlyWhenDefrostHsmStateIsIdle(instance);
}
