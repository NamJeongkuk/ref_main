/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ReadyToDefrost.h"
#include "ReadyToDefrostHsmState.h"
#include "uassert.h"
#include "SystemErds.h"
#include "Constants_Time.h"
#include "DataModelErdPointerAccess.h"
#include "utils.h"

enum
{
   UpdatePeriodInSeconds = 1 * SECONDS_PER_MINUTE
};

enum
{
   Signal_MinimumTimeBetweenDefrostsTimeExpired = Fsm_UserSignalStart,
   Signal_CompressorIsOff,
   Signal_CompressorIsOn,
   Signal_UseAhamPrechillTimeBetweenDefrostsAndResetDefrostCounts,
   Signal_NotWaitingToDefrost,
   Signal_WaitingToDefrost,
   Signal_RemainingTimeBetweenDefrostsTimeExpired,
   Signal_ADoorOpened,
   Signal_ADoorClosed,
   Signal_UpdatePeriodExpired
};

static bool State_WaitingToGetReadyToDefrost(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_WaitingForMinimumTimeBetweenDefrosts(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_WaitingForRemainingTimeBetweenDefrosts(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_ReadyAndDefrosting(Hsm_t *hsm, HsmSignal_t signal, const void *data);

static const HsmStateHierarchyDescriptor_t stateList[] = {
   { State_WaitingToGetReadyToDefrost, HSM_NO_PARENT },
   { State_WaitingForMinimumTimeBetweenDefrosts, State_WaitingToGetReadyToDefrost },
   { State_WaitingForRemainingTimeBetweenDefrosts, State_WaitingToGetReadyToDefrost },
   { State_ReadyAndDefrosting, HSM_NO_PARENT }
};

static const HsmConfiguration_t hsmConfiguration = {
   stateList,
   NUM_ELEMENTS(stateList)
};

static ReadyToDefrost_t *InstanceFromHsm(Hsm_t *hsm)
{
   return CONTAINER_OF(ReadyToDefrost_t, _private.hsm, hsm);
}

static void SetHsmStateTo(ReadyToDefrost_t *instance, ReadyToDefrostHsmState_t state)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->readyToDefrostHsmStateErd,
      &state);
}

static bool WaitingToDefrost(ReadyToDefrost_t *instance)
{
   bool waitingToDefrost;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->waitingToDefrostErd,
      &waitingToDefrost);

   return waitingToDefrost;
}

static bool FreezerTooWarmAtPowerUp(ReadyToDefrost_t *instance)
{
   bool freezerTooWarmAtPowerUp;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerFilteredTemperatureWasTooWarmOnPowerUpErd,
      &freezerTooWarmAtPowerUp);

   return freezerTooWarmAtPowerUp;
}

static void ResetCompressorOnTimeInSecondsToZero(ReadyToDefrost_t *instance)
{
   uint32_t seconds = 0;
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->defrostCompressorOnTimeInSecondsErd,
      &seconds);
}

static void ResetWaitingForDefrostTimeInSecondsToZero(ReadyToDefrost_t *instance)
{
   uint32_t seconds = 0;
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->waitingForDefrostTimeInSecondsErd,
      &seconds);
}

static void ResetDoorAccelerationsInSecondsToZero(ReadyToDefrost_t *instance)
{
   uint32_t seconds = 0;
   for(uint8_t i = 0; i < instance->_private.config->numberOfDoors; i++)
   {
      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->doorsConfiguration[i].doorAccelerationErd,
         &seconds);
   }
}

static bool ConvertibleCompartmentIsAbnormal(ReadyToDefrost_t *instance)
{
   bool hasConvertibleCompartment;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->hasConvertibleCompartmentErd,
      &hasConvertibleCompartment);

   bool convertibleCompartmentDefrostWasAbnormal;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->convertibleCompartmentDefrostWasAbnormalErd,
      &convertibleCompartmentDefrostWasAbnormal);

   return (hasConvertibleCompartment && convertibleCompartmentDefrostWasAbnormal);
}

static bool TimeBetweenDefrostsShouldBeMinimum(ReadyToDefrost_t *instance)
{
   bool freshFoodDefrostWasAbnormal;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freshFoodDefrostWasAbnormalErd,
      &freshFoodDefrostWasAbnormal);

   bool freezerDefrostWasAbnormal;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerDefrostWasAbnormalErd,
      &freezerDefrostWasAbnormal);

   bool invalidFreezerEvaporatorThermistorDuringDefrost;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->invalidFreezerEvaporatorThermistorDuringDefrostErd,
      &invalidFreezerEvaporatorThermistorDuringDefrost);

   bool freezerTooWarmOnPowerUp;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerFilteredTemperatureWasTooWarmOnPowerUpErd,
      &freezerTooWarmOnPowerUp);

   bool eepromWasCleared;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->eepromClearedErd,
      &eepromWasCleared);

   return (freshFoodDefrostWasAbnormal ||
      freezerDefrostWasAbnormal ||
      ConvertibleCompartmentIsAbnormal(instance) ||
      invalidFreezerEvaporatorThermistorDuringDefrost ||
      freezerTooWarmOnPowerUp ||
      eepromWasCleared);
}

static uint16_t TimeBetweenDefrostsInMinutesDependingOnCurrentSystemConditions(ReadyToDefrost_t *instance)
{
   uint16_t timeBetweenDefrostsInMinutes =
      instance->_private.defrostData->idleData.maxTimeBetweenDefrostsInMinutes;

   if(TimeBetweenDefrostsShouldBeMinimum(instance))
   {
      timeBetweenDefrostsInMinutes =
         instance->_private.defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes;
   }

   return timeBetweenDefrostsInMinutes;
}

static uint16_t TimeBetweenDefrostsInMinutes(ReadyToDefrost_t *instance)
{
   uint16_t timeBetweenDefrostsInMinutes;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->timeBetweenDefrostsInMinutesErd,
      &timeBetweenDefrostsInMinutes);

   return timeBetweenDefrostsInMinutes;
}

static void SetTimeBetweenDefrostsInMinutesTo(ReadyToDefrost_t *instance, uint16_t minutes)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->timeBetweenDefrostsInMinutesErd,
      &minutes);
}

static uint32_t DefrostCompressorOnTimeInSeconds(ReadyToDefrost_t *instance)
{
   uint32_t seconds;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->defrostCompressorOnTimeInSecondsErd,
      &seconds);

   return seconds;
}

static uint32_t DoorAccelerationsInSeconds(ReadyToDefrost_t *instance)
{
   uint32_t totalAccelerations = 0;
   for(uint8_t i = 0; i < instance->_private.config->numberOfDoors; i++)
   {
      uint32_t doorAccelerations;
      DataModel_Read(
         instance->_private.dataModel,
         instance->_private.config->doorsConfiguration[i].doorAccelerationErd,
         &doorAccelerations);

      totalAccelerations += doorAccelerations;
   }

   return totalAccelerations;
}

static bool CompressorIsOn(ReadyToDefrost_t *instance)
{
   bool compressorIsOn;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->compressorIsOnErd,
      &compressorIsOn);

   return compressorIsOn;
}

static void SetWaitingDefrostTimeInSecondsTo(ReadyToDefrost_t *instance, uint32_t seconds)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->waitingForDefrostTimeInSecondsErd,
      &seconds);
}

static void SetWaitingForDefrostTimeInSecondsToDefrostCompressorOnTimeInSeconds(ReadyToDefrost_t *instance)
{
   SetWaitingDefrostTimeInSecondsTo(instance, DefrostCompressorOnTimeInSeconds(instance));
}

static void SetWaitingForDefrostTimeInSecondsToDefrostCompressorOnTimeInSecondsPlusDoorAccelerationsInSeconds(ReadyToDefrost_t *instance)
{
   SetWaitingDefrostTimeInSecondsTo(instance, DefrostCompressorOnTimeInSeconds(instance) + DoorAccelerationsInSeconds(instance));
}

static bool UseAhamPrechillTimeBetweenDefrosts(ReadyToDefrost_t *instance)
{
   bool useAhamPrechillTime;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->useAhamPrechillReadyToDefrostTimeAndResetDefrostCountsErd,
      &useAhamPrechillTime);

   return useAhamPrechillTime;
}

static bool ReadyToDefrost(ReadyToDefrost_t *instance)
{
   return ((DefrostCompressorOnTimeInSeconds(instance) >= (instance->_private.defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE)) &&
      ((DefrostCompressorOnTimeInSeconds(instance) + DoorAccelerationsInSeconds(instance)) >= TimeBetweenDefrostsInMinutes(instance) * SECONDS_PER_MINUTE));
}

static void SetReadyToDefrostTo(ReadyToDefrost_t *instance, bool state)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->readyToDefrostErd,
      &state);
}

static bool DoorIsOpen(ReadyToDefrost_t *instance, uint8_t doorIndex)
{
   bool doorIsOpen;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->doorsConfiguration[doorIndex].doorIsOpenErd,
      &doorIsOpen);

   return doorIsOpen;
}

static bool AtLeastOneDoorIsOpen(ReadyToDefrost_t *instance)
{
   for(uint8_t i = 0; i < instance->_private.config->numberOfDoors; i++)
   {
      if(DoorIsOpen(instance, i))
      {
         return true;
      }
   }
   return false;
}

static uint16_t DoorAccelerationFactor(ReadyToDefrost_t *instance, uint8_t doorIndex)
{
   uint16_t factor;
   memcpy(
      &factor,
      ((const uint8_t *)instance->_private.defrostData) + instance->_private.config->doorsConfiguration[doorIndex].offsetInParametricForDoorFactor,
      sizeof(factor));

   return factor;
}

static uint32_t SumOfOpenDoorFactors(ReadyToDefrost_t *instance)
{
   uint32_t sumOfOpenDoorFactorsInSecondsPerSecond = 0;

   for(uint8_t i = 0; i < instance->_private.config->numberOfDoors; i++)
   {
      if(DoorIsOpen(instance, i))
      {
         sumOfOpenDoorFactorsInSecondsPerSecond += DoorAccelerationFactor(instance, i);
      }
   }

   return sumOfOpenDoorFactorsInSecondsPerSecond;
}

static void MinimumTimeBetweenDefrostsTimeExpired(void *context)
{
   ReadyToDefrost_t *instance = context;
   Hsm_SendSignal(&instance->_private.hsm, Signal_MinimumTimeBetweenDefrostsTimeExpired, NULL);
}

static TimerTicks_t RemainingTimeUntilMinimumTimeBetweenDefrostsIsReachedInMilliseconds(ReadyToDefrost_t *instance)
{
   uint16_t timeBetweenDefrostsInMinutes;
   if(UseAhamPrechillTimeBetweenDefrosts(instance))
   {
      timeBetweenDefrostsInMinutes =
         instance->_private.defrostData->idleData.ahamPrechillTimeBetweenDefrostsInMinutes;
   }
   else
   {
      timeBetweenDefrostsInMinutes =
         instance->_private.defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes;
   }

   return TRUNCATE_UNSIGNED_SUBTRACTION(
      (timeBetweenDefrostsInMinutes * MSEC_PER_MIN),
      (DefrostCompressorOnTimeInSeconds(instance) * MSEC_PER_SEC));
}

static void StartMinimumTimeBetweenDefrostsTimer(ReadyToDefrost_t *instance)
{
   TimerModule_StartOneShot(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         Erd_TimerModule),
      &instance->_private.timer,
      RemainingTimeUntilMinimumTimeBetweenDefrostsIsReachedInMilliseconds(instance),
      MinimumTimeBetweenDefrostsTimeExpired,
      instance);
}

static void UpdatePeriodExpired(void *context)
{
   ReadyToDefrost_t *instance = context;
   Hsm_SendSignal(&instance->_private.hsm, Signal_UpdatePeriodExpired, NULL);
}

static void StartPeriodicUpdateTimer(ReadyToDefrost_t *instance)
{
   TimerModule_StartOneShot(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         Erd_TimerModule),
      &instance->_private.timer,
      UpdatePeriodInSeconds * MSEC_PER_SEC,
      UpdatePeriodExpired,
      instance);
}

static void RemainingTimeBetweenDefrostsTimeExpired(void *context)
{
   ReadyToDefrost_t *instance = context;
   Hsm_SendSignal(&instance->_private.hsm, Signal_RemainingTimeBetweenDefrostsTimeExpired, NULL);
}

static void StartRemainingTimeBetweenDefrostsTimerFor(ReadyToDefrost_t *instance, TimerTicks_t ticks)
{
   TimerModule_StartOneShot(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         Erd_TimerModule),
      &instance->_private.timer,
      ticks,
      RemainingTimeBetweenDefrostsTimeExpired,
      instance);
}

static TimerTicks_t RemainingTimeBetweenDefrostsInMilliseconds(ReadyToDefrost_t *instance)
{
   return (TRUNCATE_UNSIGNED_SUBTRACTION(
             (TimeBetweenDefrostsInMinutes(instance) * SECONDS_PER_MINUTE),
             (DefrostCompressorOnTimeInSeconds(instance) +
                DoorAccelerationsInSeconds(instance)))) *
      MSEC_PER_SEC;
}

static TimerTicks_t RemainingTimeUntilReadyToDefrostInMilliseconds(ReadyToDefrost_t *instance)
{
   if(CompressorIsOn(instance))
   {
      if(AtLeastOneDoorIsOpen(instance))
      {
         return RemainingTimeBetweenDefrostsInMilliseconds(instance) / (1 + SumOfOpenDoorFactors(instance));
      }
      else
      {
         return RemainingTimeBetweenDefrostsInMilliseconds(instance);
      }
   }
   else if(AtLeastOneDoorIsOpen(instance))
   {
      return RemainingTimeBetweenDefrostsInMilliseconds(instance) / (SumOfOpenDoorFactors(instance));
   }

   uassert(!"This should never happen!");
   return 0;
}

static TimerTicks_t ElapsedTicks(ReadyToDefrost_t *instance)
{
   return TimerModule_TicksSinceLastStarted(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         Erd_TimerModule),
      &instance->_private.timer);
}

static void AddElapsedTicksToCompressorOnTime(ReadyToDefrost_t *instance)
{
   uint32_t updatedTimeInSeconds = DefrostCompressorOnTimeInSeconds(instance) + (ElapsedTicks(instance) / MSEC_PER_SEC);

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->defrostCompressorOnTimeInSecondsErd,
      &updatedTimeInSeconds);
}

static void StopTimer(ReadyToDefrost_t *instance)
{
   TimerModule_Stop(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         Erd_TimerModule),
      &instance->_private.timer);
}

static void StartTimerForRemainingTimeBetweenDefrostsIfRemainingTimeIsLessThanPeriodicUpdateTimeOtherwiseStartTimerForPeriodicUpdateTime(ReadyToDefrost_t *instance)
{
   if((CompressorIsOn(instance) || AtLeastOneDoorIsOpen(instance)) && (RemainingTimeUntilReadyToDefrostInMilliseconds(instance) < (UpdatePeriodInSeconds * MSEC_PER_SEC)))
   {
      StartRemainingTimeBetweenDefrostsTimerFor(instance, RemainingTimeUntilReadyToDefrostInMilliseconds(instance));
   }
   else if(CompressorIsOn(instance) || AtLeastOneDoorIsOpen(instance))
   {
      StartPeriodicUpdateTimer(instance);
   }
   else
   {
      StopTimer(instance);
   }
}

static void IncrementDoorAccelerationByElapsedTicks(ReadyToDefrost_t *instance, TimerTicks_t elapsedTicks, uint8_t doorIndex)
{
   uint32_t accelerations;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->doorsConfiguration[doorIndex].doorAccelerationErd,
      &accelerations);

   accelerations += ((elapsedTicks * DoorAccelerationFactor(instance, doorIndex)) / MSEC_PER_SEC);
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->doorsConfiguration[doorIndex].doorAccelerationErd,
      &accelerations);
}

static void AddElapsedTicksToDoorAccelerationForDoorThatClosed(ReadyToDefrost_t *instance, uint8_t doorIndex)
{
   IncrementDoorAccelerationByElapsedTicks(instance, ElapsedTicks(instance), doorIndex);
}

static void AddElapsedTicksToDoorAccelerationForOpenDoors(ReadyToDefrost_t *instance)
{
   uint32_t elapsedTicks = ElapsedTicks(instance);

   for(uint8_t i = 0; i < instance->_private.config->numberOfDoors; i++)
   {
      if(DoorIsOpen(instance, i))
      {
         IncrementDoorAccelerationByElapsedTicks(instance, elapsedTicks, i);
      }
   }
}

static void AddElapsedTicksToDoorAccelerationForOpenDoorsExceptForDoorThatJustOpened(ReadyToDefrost_t *instance, uint8_t doorIndex)
{
   uint32_t elapsedTicks = ElapsedTicks(instance);

   for(uint8_t i = 0; i < instance->_private.config->numberOfDoors; i++)
   {
      if(i != doorIndex && DoorIsOpen(instance, i))
      {
         IncrementDoorAccelerationByElapsedTicks(instance, elapsedTicks, i);
      }
   }
}

static void StartMinimumTimeBetweenDefrostsTimerIfCompressorIsOnAndMinimumTimeBetweenDefrostsIsLessThanPeriodicUpdateOtherwiseStartTimerForPeriodicUpdateTimeIfCompressorIsOnOrADoorIsOpen(ReadyToDefrost_t *instance)
{
   if(CompressorIsOn(instance) && (RemainingTimeUntilMinimumTimeBetweenDefrostsIsReachedInMilliseconds(instance) < (UpdatePeriodInSeconds * MSEC_PER_SEC)))
   {
      StartMinimumTimeBetweenDefrostsTimer(instance);
   }
   else if(CompressorIsOn(instance) || AtLeastOneDoorIsOpen(instance))
   {
      StartPeriodicUpdateTimer(instance);
   }
}

static void AddElapsedTicksToCompressorOnTimeIfCompressorIsOn(ReadyToDefrost_t *instance)
{
   if(CompressorIsOn(instance))
   {
      AddElapsedTicksToCompressorOnTime(instance);
   }
}

static void SetWaitingForDefrostTimeInSecondsToDefrostCompressorOnTimeInSecondsIfCompressorIsOn(ReadyToDefrost_t *instance)
{
   if(CompressorIsOn(instance))
   {
      SetWaitingDefrostTimeInSecondsTo(instance, DefrostCompressorOnTimeInSeconds(instance));
   }
}

static bool State_WaitingToGetReadyToDefrost(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   ReadyToDefrost_t *instance = InstanceFromHsm(hsm);
   IGNORE(data);

   switch(signal)
   {
      case Hsm_Entry:
         break;

      case Signal_NotWaitingToDefrost:
         Hsm_Transition(&instance->_private.hsm, State_ReadyAndDefrosting);
         break;

      case Hsm_Exit:
         ResetWaitingForDefrostTimeInSecondsToZero(instance);
         ResetCompressorOnTimeInSecondsToZero(instance);
         ResetDoorAccelerationsInSecondsToZero(instance);
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

static bool State_WaitingForMinimumTimeBetweenDefrosts(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   ReadyToDefrost_t *instance = InstanceFromHsm(hsm);
   const uint8_t *doorIndex = data;

   switch(signal)
   {
      case Hsm_Entry:
         SetHsmStateTo(instance, ReadyToDefrostHsmState_WaitingForMinimumTimeBetweenDefrosts);
         SetReadyToDefrostTo(instance, false);
         SetWaitingForDefrostTimeInSecondsToDefrostCompressorOnTimeInSeconds(instance);

         if(UseAhamPrechillTimeBetweenDefrosts(instance))
         {
            SetTimeBetweenDefrostsInMinutesTo(
               instance,
               instance->_private.defrostData->idleData.ahamPrechillTimeBetweenDefrostsInMinutes);
         }
         else
         {
            SetTimeBetweenDefrostsInMinutesTo(
               instance,
               TimeBetweenDefrostsInMinutesDependingOnCurrentSystemConditions(instance));
         }

         StartMinimumTimeBetweenDefrostsTimerIfCompressorIsOnAndMinimumTimeBetweenDefrostsIsLessThanPeriodicUpdateOtherwiseStartTimerForPeriodicUpdateTimeIfCompressorIsOnOrADoorIsOpen(instance);
         break;

      case Signal_UpdatePeriodExpired:
         AddElapsedTicksToCompressorOnTimeIfCompressorIsOn(instance);
         SetWaitingForDefrostTimeInSecondsToDefrostCompressorOnTimeInSecondsIfCompressorIsOn(instance);
         AddElapsedTicksToDoorAccelerationForOpenDoors(instance);
         StartMinimumTimeBetweenDefrostsTimerIfCompressorIsOnAndMinimumTimeBetweenDefrostsIsLessThanPeriodicUpdateOtherwiseStartTimerForPeriodicUpdateTimeIfCompressorIsOnOrADoorIsOpen(instance);
         break;

      case Signal_CompressorIsOff:
         AddElapsedTicksToCompressorOnTime(instance);
         SetWaitingForDefrostTimeInSecondsToDefrostCompressorOnTimeInSeconds(instance);
         AddElapsedTicksToDoorAccelerationForOpenDoors(instance);
         StartMinimumTimeBetweenDefrostsTimerIfCompressorIsOnAndMinimumTimeBetweenDefrostsIsLessThanPeriodicUpdateOtherwiseStartTimerForPeriodicUpdateTimeIfCompressorIsOnOrADoorIsOpen(instance);
         break;

      case Signal_CompressorIsOn:
         AddElapsedTicksToDoorAccelerationForOpenDoors(instance);
         StartMinimumTimeBetweenDefrostsTimerIfCompressorIsOnAndMinimumTimeBetweenDefrostsIsLessThanPeriodicUpdateOtherwiseStartTimerForPeriodicUpdateTimeIfCompressorIsOnOrADoorIsOpen(instance);
         break;

      case Signal_MinimumTimeBetweenDefrostsTimeExpired:
         AddElapsedTicksToDoorAccelerationForOpenDoors(instance);
         AddElapsedTicksToCompressorOnTimeIfCompressorIsOn(instance);
         SetWaitingForDefrostTimeInSecondsToDefrostCompressorOnTimeInSecondsIfCompressorIsOn(instance);

         if(TimeBetweenDefrostsInMinutes(instance) ==
               instance->_private.defrostData->idleData.ahamPrechillTimeBetweenDefrostsInMinutes ||
            TimeBetweenDefrostsInMinutes(instance) ==
               instance->_private.defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes)
         {
            SetReadyToDefrostTo(instance, true);
         }
         else
         {
            Hsm_Transition(&instance->_private.hsm, State_WaitingForRemainingTimeBetweenDefrosts);
         }
         break;

      case Signal_ADoorOpened:
         AddElapsedTicksToDoorAccelerationForOpenDoorsExceptForDoorThatJustOpened(instance, *doorIndex);
         AddElapsedTicksToCompressorOnTimeIfCompressorIsOn(instance);
         SetWaitingForDefrostTimeInSecondsToDefrostCompressorOnTimeInSecondsIfCompressorIsOn(instance);
         StartMinimumTimeBetweenDefrostsTimerIfCompressorIsOnAndMinimumTimeBetweenDefrostsIsLessThanPeriodicUpdateOtherwiseStartTimerForPeriodicUpdateTimeIfCompressorIsOnOrADoorIsOpen(instance);
         break;

      case Signal_ADoorClosed:
         AddElapsedTicksToDoorAccelerationForDoorThatClosed(instance, *doorIndex);
         AddElapsedTicksToDoorAccelerationForOpenDoors(instance);
         AddElapsedTicksToCompressorOnTimeIfCompressorIsOn(instance);
         SetWaitingForDefrostTimeInSecondsToDefrostCompressorOnTimeInSecondsIfCompressorIsOn(instance);
         StartMinimumTimeBetweenDefrostsTimerIfCompressorIsOnAndMinimumTimeBetweenDefrostsIsLessThanPeriodicUpdateOtherwiseStartTimerForPeriodicUpdateTimeIfCompressorIsOnOrADoorIsOpen(instance);
         break;

      case Signal_UseAhamPrechillTimeBetweenDefrostsAndResetDefrostCounts:
         ResetWaitingForDefrostTimeInSecondsToZero(instance);
         ResetCompressorOnTimeInSecondsToZero(instance);
         ResetDoorAccelerationsInSecondsToZero(instance);
         SetTimeBetweenDefrostsInMinutesTo(
            instance,
            instance->_private.defrostData->idleData.ahamPrechillTimeBetweenDefrostsInMinutes);
         if(CompressorIsOn(instance) || AtLeastOneDoorIsOpen(instance))
         {
            StartPeriodicUpdateTimer(instance);
         }
         break;

      case Hsm_Exit:
         StopTimer(instance);
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

static bool State_WaitingForRemainingTimeBetweenDefrosts(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   ReadyToDefrost_t *instance = InstanceFromHsm(hsm);
   const uint8_t *doorIndex = data;

   switch(signal)
   {
      case Hsm_Entry:
         SetHsmStateTo(instance, ReadyToDefrostHsmState_WaitingForRemainingTimeBetweenDefrosts);
         SetTimeBetweenDefrostsInMinutesTo(instance,
            TimeBetweenDefrostsInMinutesDependingOnCurrentSystemConditions(instance));
         SetWaitingForDefrostTimeInSecondsToDefrostCompressorOnTimeInSecondsPlusDoorAccelerationsInSeconds(instance);
         if(ReadyToDefrost(instance))
         {
            StartRemainingTimeBetweenDefrostsTimerFor(instance, 0);
         }
         else
         {
            StartTimerForRemainingTimeBetweenDefrostsIfRemainingTimeIsLessThanPeriodicUpdateTimeOtherwiseStartTimerForPeriodicUpdateTime(instance);
         }
         break;

      case Signal_UpdatePeriodExpired:
         AddElapsedTicksToCompressorOnTimeIfCompressorIsOn(instance);
         AddElapsedTicksToDoorAccelerationForOpenDoors(instance);
         SetWaitingForDefrostTimeInSecondsToDefrostCompressorOnTimeInSecondsPlusDoorAccelerationsInSeconds(instance);
         StartTimerForRemainingTimeBetweenDefrostsIfRemainingTimeIsLessThanPeriodicUpdateTimeOtherwiseStartTimerForPeriodicUpdateTime(instance);
         break;

      case Signal_CompressorIsOn:
         AddElapsedTicksToDoorAccelerationForOpenDoors(instance);
         SetWaitingForDefrostTimeInSecondsToDefrostCompressorOnTimeInSecondsPlusDoorAccelerationsInSeconds(instance);
         StartTimerForRemainingTimeBetweenDefrostsIfRemainingTimeIsLessThanPeriodicUpdateTimeOtherwiseStartTimerForPeriodicUpdateTime(instance);
         break;

      case Signal_CompressorIsOff:
         AddElapsedTicksToCompressorOnTime(instance);
         AddElapsedTicksToDoorAccelerationForOpenDoors(instance);
         SetWaitingForDefrostTimeInSecondsToDefrostCompressorOnTimeInSecondsPlusDoorAccelerationsInSeconds(instance);
         StartTimerForRemainingTimeBetweenDefrostsIfRemainingTimeIsLessThanPeriodicUpdateTimeOtherwiseStartTimerForPeriodicUpdateTime(instance);
         break;

      case Signal_ADoorOpened:
         AddElapsedTicksToDoorAccelerationForOpenDoorsExceptForDoorThatJustOpened(instance, *doorIndex);
         AddElapsedTicksToCompressorOnTimeIfCompressorIsOn(instance);
         SetWaitingForDefrostTimeInSecondsToDefrostCompressorOnTimeInSecondsPlusDoorAccelerationsInSeconds(instance);
         StartTimerForRemainingTimeBetweenDefrostsIfRemainingTimeIsLessThanPeriodicUpdateTimeOtherwiseStartTimerForPeriodicUpdateTime(instance);
         break;

      case Signal_ADoorClosed:
         AddElapsedTicksToDoorAccelerationForDoorThatClosed(instance, *doorIndex);
         AddElapsedTicksToDoorAccelerationForOpenDoors(instance);
         AddElapsedTicksToCompressorOnTimeIfCompressorIsOn(instance);
         SetWaitingForDefrostTimeInSecondsToDefrostCompressorOnTimeInSecondsPlusDoorAccelerationsInSeconds(instance);
         StartTimerForRemainingTimeBetweenDefrostsIfRemainingTimeIsLessThanPeriodicUpdateTimeOtherwiseStartTimerForPeriodicUpdateTime(instance);
         break;

      case Signal_RemainingTimeBetweenDefrostsTimeExpired:
         AddElapsedTicksToDoorAccelerationForOpenDoors(instance);
         AddElapsedTicksToCompressorOnTimeIfCompressorIsOn(instance);
         SetWaitingForDefrostTimeInSecondsToDefrostCompressorOnTimeInSecondsPlusDoorAccelerationsInSeconds(instance);
         SetReadyToDefrostTo(instance, true);
         break;

      case Signal_UseAhamPrechillTimeBetweenDefrostsAndResetDefrostCounts:
         ResetWaitingForDefrostTimeInSecondsToZero(instance);
         ResetCompressorOnTimeInSecondsToZero(instance);
         ResetDoorAccelerationsInSecondsToZero(instance);
         Hsm_Transition(&instance->_private.hsm, State_WaitingForMinimumTimeBetweenDefrosts);
         break;

      case Hsm_Exit:
         StopTimer(instance);
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

static bool State_ReadyAndDefrosting(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   ReadyToDefrost_t *instance = InstanceFromHsm(hsm);
   IGNORE(data);

   switch(signal)
   {
      case Hsm_Entry:
         SetHsmStateTo(instance, ReadyToDefrostHsmState_ReadyAndDefrosting);
         break;

      case Signal_WaitingToDefrost:
         Hsm_Transition(&instance->_private.hsm, State_WaitingForMinimumTimeBetweenDefrosts);
         break;

      case Signal_UseAhamPrechillTimeBetweenDefrostsAndResetDefrostCounts:
         ResetCompressorOnTimeInSecondsToZero(instance);
         ResetDoorAccelerationsInSecondsToZero(instance);
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
   ReadyToDefrost_t *instance = context;
   const DataModelOnDataChangeArgs_t *onChangeData = args;
   Erd_t erd = onChangeData->erd;

   if(erd == instance->_private.config->compressorIsOnErd)
   {
      const bool *compressorIsOn = onChangeData->data;

      if(*compressorIsOn)
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_CompressorIsOn, NULL);
      }
      else
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_CompressorIsOff, NULL);
      }
   }
   else if(erd == instance->_private.config->useAhamPrechillReadyToDefrostTimeAndResetDefrostCountsErd)
   {
      const bool *useAhamPrechillReadyToDefrostTimeAndResetCounts = onChangeData->data;

      if(*useAhamPrechillReadyToDefrostTimeAndResetCounts)
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_UseAhamPrechillTimeBetweenDefrostsAndResetDefrostCounts, NULL);
      }
   }
   else if(erd == instance->_private.config->waitingToDefrostErd)
   {
      const bool *waitingToDefrost = onChangeData->data;

      if(*waitingToDefrost)
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_WaitingToDefrost, NULL);
      }
      else
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_NotWaitingToDefrost, NULL);
      }
   }
   else
   {
      for(uint8_t i = 0; i < instance->_private.config->numberOfDoors; i++)
      {
         uint8_t doorIndex = i;
         if(erd == instance->_private.config->doorsConfiguration[i].doorIsOpenErd)
         {
            const bool *doorIsOpen = onChangeData->data;
            if(*doorIsOpen)
            {
               Hsm_SendSignal(&instance->_private.hsm, Signal_ADoorOpened, &doorIndex);
            }
            else
            {
               Hsm_SendSignal(&instance->_private.hsm, Signal_ADoorClosed, &doorIndex);
            }
            return;
         }
      }
   }
}

static HsmState_t InitialState(ReadyToDefrost_t *instance)
{
   if(WaitingToDefrost(instance))
   {
      if(DefrostCompressorOnTimeInSeconds(instance) <
         instance->_private.defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE)
      {
         return State_WaitingForMinimumTimeBetweenDefrosts;
      }
      else
      {
         return State_WaitingForRemainingTimeBetweenDefrosts;
      }
   }
   return State_ReadyAndDefrosting;
}

void ReadyToDefrost_Init(
   ReadyToDefrost_t *instance,
   I_DataModel_t *dataModel,
   const ReadyToDefrostConfiguration_t *config,
   const DefrostData_t *defrostData)
{
   bool freezerTooWarmOnPowerUpReady;
   DataModel_Read(
      dataModel,
      config->freezerFilteredTemperatureWasTooWarmOnPowerUpReadyErd,
      &freezerTooWarmOnPowerUpReady);

   uassert(freezerTooWarmOnPowerUpReady);

   instance->_private.dataModel = dataModel;
   instance->_private.config = config;
   instance->_private.defrostData = defrostData;

   if(FreezerTooWarmAtPowerUp(instance))
   {
      ResetCompressorOnTimeInSecondsToZero(instance);
      ResetDoorAccelerationsInSecondsToZero(instance);
   }

   Hsm_Init(&instance->_private.hsm, &hsmConfiguration, InitialState(instance));

   EventSubscription_Init(
      &instance->_private.dataModelSubscription,
      instance,
      DataModelChanged);

   DataModel_SubscribeAll(
      instance->_private.dataModel,
      &instance->_private.dataModelSubscription);
}
