/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ReadyToDefrostImproved.h"
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
   Signal_UseMinimumTimeBetweenDefrostsAndResetDefrostCounts,
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

static ReadyToDefrostImproved_t *InstanceFromHsm(Hsm_t *hsm)
{
   return CONTAINER_OF(ReadyToDefrostImproved_t, _private.hsm, hsm);
}

static void SetHsmStateTo(ReadyToDefrostImproved_t *instance, ReadyToDefrostHsmState_t state)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->readyToDefrostHsmStateErd,
      &state);
}

static bool WaitingToDefrost(ReadyToDefrostImproved_t *instance)
{
   bool waitingToDefrost;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->waitingToDefrostErd,
      &waitingToDefrost);

   return waitingToDefrost;
}

static bool FreezerTooWarmAtPowerUp(ReadyToDefrostImproved_t *instance)
{
   bool freezerTooWarmAtPowerUp;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerFilteredTemperatureWasTooWarmAtPowerUpErd,
      &freezerTooWarmAtPowerUp);

   return freezerTooWarmAtPowerUp;
}

static void ResetCompressorOnTimeInSecondsToZero(ReadyToDefrostImproved_t *instance)
{
   uint32_t seconds = 0;
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->defrostCompressorOnTimeInSecondsErd,
      &seconds);
}

static void ResetWaitingForDefrostTimeInSecondsToZero(ReadyToDefrostImproved_t *instance)
{
   uint32_t seconds = 0;
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->waitingForDefrostTimeInSecondsErd,
      &seconds);
}

static void ResetDoorAccelerationsInSecondsToZero(ReadyToDefrostImproved_t *instance)
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

static bool ConvertibleCompartmentIsAbnormal(ReadyToDefrostImproved_t *instance)
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

   return hasConvertibleCompartment && convertibleCompartmentDefrostWasAbnormal;
}

static bool TimeBetweenDefrostsShouldBeMinimum(ReadyToDefrostImproved_t *instance)
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
      instance->_private.config->freezerFilteredTemperatureWasTooWarmAtPowerUpErd,
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

static uint16_t TimeBetweenDefrostsInMinutesDependingOnCurrentSystemConditions(ReadyToDefrostImproved_t *instance)
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

static uint16_t TimeBetweenDefrostsInMinutes(ReadyToDefrostImproved_t *instance)
{
   uint16_t timeBetweenDefrostsInMinutes;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->timeBetweenDefrostsInMinutesErd,
      &timeBetweenDefrostsInMinutes);

   return timeBetweenDefrostsInMinutes;
}

static void SetTimeBetweenDefrostsInMinutesTo(ReadyToDefrostImproved_t *instance, uint16_t minutes)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->timeBetweenDefrostsInMinutesErd,
      &minutes);
}

static uint32_t DefrostCompressorOnTimeInSeconds(ReadyToDefrostImproved_t *instance)
{
   uint32_t seconds;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->defrostCompressorOnTimeInSecondsErd,
      &seconds);

   return seconds;
}

static uint32_t DoorAccelerationsInSeconds(ReadyToDefrostImproved_t *instance)
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

static bool CompressorIsOn(ReadyToDefrostImproved_t *instance)
{
   bool compressorIsOn;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->compressorIsOnErd,
      &compressorIsOn);

   return compressorIsOn;
}

static void SetWaitingDefrostTimeInSecondsTo(ReadyToDefrostImproved_t *instance, uint32_t seconds)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->waitingForDefrostTimeInSecondsErd,
      &seconds);
}

static void SetWaitingForDefrostTimeInSecondsToDefrostCompressorOnTimeInSeconds(ReadyToDefrostImproved_t *instance)
{
   SetWaitingDefrostTimeInSecondsTo(instance, DefrostCompressorOnTimeInSeconds(instance));
}

static void SetWaitingForDefrostTimeInSecondsToDefrostCompressorOnTimeInSecondsPlusDoorAccelerationsInSeconds(ReadyToDefrostImproved_t *instance)
{
   SetWaitingDefrostTimeInSecondsTo(instance, DefrostCompressorOnTimeInSeconds(instance) + DoorAccelerationsInSeconds(instance));
}

static bool ReadyToDefrost(ReadyToDefrostImproved_t *instance)
{
   uint16_t timeBetweenDefrostsInMinutes;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->timeBetweenDefrostsInMinutesErd,
      &timeBetweenDefrostsInMinutes);

   return ((DefrostCompressorOnTimeInSeconds(instance) >= (instance->_private.defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE)) &&
      ((DefrostCompressorOnTimeInSeconds(instance) + DoorAccelerationsInSeconds(instance)) >= timeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE));
}

static void SetReadyToDefrostTo(ReadyToDefrostImproved_t *instance, bool state)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->readyToDefrostErd,
      &state);
}

static bool DoorIsOpen(ReadyToDefrostImproved_t *instance, uint8_t doorIndex)
{
   bool doorIsOpen;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->doorsConfiguration[doorIndex].doorIsOpenErd,
      &doorIsOpen);

   return doorIsOpen;
}

static bool AtLeastOneDoorIsOpen(ReadyToDefrostImproved_t *instance)
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

static uint16_t DoorAccelerationFactor(ReadyToDefrostImproved_t *instance, uint8_t doorIndex)
{
   uint16_t factor;
   memcpy(
      &factor,
      ((const uint8_t *)instance->_private.defrostData) + instance->_private.config->doorsConfiguration[doorIndex].offsetInParametricForDoorFactor,
      sizeof(factor));

   return factor;
}

static uint32_t SumOfOpenDoorFactors(ReadyToDefrostImproved_t *instance)
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
   ReadyToDefrostImproved_t *instance = context;
   Hsm_SendSignal(&instance->_private.hsm, Signal_MinimumTimeBetweenDefrostsTimeExpired, NULL);
}

static TimerTicks_t RemainingTimeUntilMinimumTimeBetweenDefrostsIsReachedInMilliseconds(ReadyToDefrostImproved_t *instance)
{
   return (instance->_private.defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN) -
      (DefrostCompressorOnTimeInSeconds(instance) * MSEC_PER_SEC);
}

static void StartMinimumTimeBetweenDefrostsTimer(ReadyToDefrostImproved_t *instance)
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
   ReadyToDefrostImproved_t *instance = context;
   Hsm_SendSignal(&instance->_private.hsm, Signal_UpdatePeriodExpired, NULL);
}

static void StartPeriodicUpdateTimer(ReadyToDefrostImproved_t *instance)
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
   ReadyToDefrostImproved_t *instance = context;
   Hsm_SendSignal(&instance->_private.hsm, Signal_RemainingTimeBetweenDefrostsTimeExpired, NULL);
}

static void StartRemainingTimeBetweenDefrostsTimerFor(ReadyToDefrostImproved_t *instance, TimerTicks_t ticks)
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

static uint32_t RemainingTimeBetweenDefrostsInMilliseconds(ReadyToDefrostImproved_t *instance)
{
   return ((TimeBetweenDefrostsInMinutes(instance) * SECONDS_PER_MINUTE) -
             DefrostCompressorOnTimeInSeconds(instance) -
             DoorAccelerationsInSeconds(instance)) *
      MSEC_PER_SEC;
}

static TimerTicks_t RemainingTimeUntilReadyToDefrostInMilliseconds(ReadyToDefrostImproved_t *instance)
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

static TimerTicks_t ElapsedTicks(ReadyToDefrostImproved_t *instance)
{
   return TimerModule_TicksSinceLastStarted(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         Erd_TimerModule),
      &instance->_private.timer);
}

static void AddElapsedTicksToCompressorOnTime(ReadyToDefrostImproved_t *instance)
{
   uint32_t updatedTimeInSeconds = DefrostCompressorOnTimeInSeconds(instance) + (ElapsedTicks(instance) / MSEC_PER_SEC);

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->defrostCompressorOnTimeInSecondsErd,
      &updatedTimeInSeconds);
}

static void StopTimer(ReadyToDefrostImproved_t *instance)
{
   TimerModule_Stop(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         Erd_TimerModule),
      &instance->_private.timer);
}

static void StartTimerForRemainingTimeBetweenDefrostsIfRemainingTimeIsLessThanPeriodicUpdateTimeOtherwiseStartTimerForPeriodicUpdateTime(ReadyToDefrostImproved_t *instance)
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

static void IncrementDoorAccelerationByElapsedTicks(ReadyToDefrostImproved_t *instance, TimerTicks_t elapsedTicks, uint8_t doorIndex)
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

static void AddElapsedTicksToDoorAccelerationForDoorThatClosed(ReadyToDefrostImproved_t *instance, uint8_t doorIndex)
{
   IncrementDoorAccelerationByElapsedTicks(instance, ElapsedTicks(instance), doorIndex);
}

static void AddElapsedTicksToDoorAccelerationForOpenDoors(ReadyToDefrostImproved_t *instance)
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

static void AddElapsedTicksToDoorAccelerationForOpenDoorsExceptForDoorThatJustOpened(ReadyToDefrostImproved_t *instance, uint8_t doorIndex)
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

static void StartMinimumTimeBetweenDefrostsTimerIfCompressorIsOnAndMinimumTimeBetweenDefrostsIsLessThanPeriodicUpdateOtherwiseStartTimerForPeriodicUpdateTimeIfCompressorIsOnOrADoorIsOpen(ReadyToDefrostImproved_t *instance)
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

static void AddElapsedTicksToCompressorOnTimeIfCompressorIsOn(ReadyToDefrostImproved_t *instance)
{
   if(CompressorIsOn(instance))
   {
      AddElapsedTicksToCompressorOnTime(instance);
   }
}

static void SetWaitingForDefrostTimeInSecondsToDefrostCompressorOnTimeInSecondsIfCompressorIsOn(ReadyToDefrostImproved_t *instance)
{
   if(CompressorIsOn(instance))
   {
      SetWaitingDefrostTimeInSecondsTo(instance, DefrostCompressorOnTimeInSeconds(instance));
   }
}

static bool State_WaitingToGetReadyToDefrost(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   ReadyToDefrostImproved_t *instance = InstanceFromHsm(hsm);
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
   ReadyToDefrostImproved_t *instance = InstanceFromHsm(hsm);
   const uint8_t *doorIndex = data;

   switch(signal)
   {
      case Hsm_Entry:
         SetHsmStateTo(instance, ReadyToDefrostHsmState_WaitingForMinimumTimeBetweenDefrosts);
         SetReadyToDefrostTo(instance, false);
         SetWaitingForDefrostTimeInSecondsToDefrostCompressorOnTimeInSeconds(instance);

         if(instance->_private.useMinimumTimeBetweenDefrosts)
         {
            SetTimeBetweenDefrostsInMinutesTo(
               instance,
               instance->_private.defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes);
            instance->_private.useMinimumTimeBetweenDefrosts = false;
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

      case Signal_UseMinimumTimeBetweenDefrostsAndResetDefrostCounts:
         ResetWaitingForDefrostTimeInSecondsToZero(instance);
         ResetCompressorOnTimeInSecondsToZero(instance);
         ResetDoorAccelerationsInSecondsToZero(instance);
         SetTimeBetweenDefrostsInMinutesTo(
            instance,
            instance->_private.defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes);
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
   ReadyToDefrostImproved_t *instance = InstanceFromHsm(hsm);
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

      case Signal_UseMinimumTimeBetweenDefrostsAndResetDefrostCounts:
         ResetWaitingForDefrostTimeInSecondsToZero(instance);
         ResetCompressorOnTimeInSecondsToZero(instance);
         ResetDoorAccelerationsInSecondsToZero(instance);
         instance->_private.useMinimumTimeBetweenDefrosts = true;
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
   ReadyToDefrostImproved_t *instance = InstanceFromHsm(hsm);
   IGNORE(data);

   switch(signal)
   {
      case Hsm_Entry:
         SetHsmStateTo(instance, ReadyToDefrostHsmState_ReadyAndDefrosting);
         break;

      case Signal_WaitingToDefrost:
         Hsm_Transition(&instance->_private.hsm, State_WaitingForMinimumTimeBetweenDefrosts);
         break;

      case Signal_UseMinimumTimeBetweenDefrostsAndResetDefrostCounts:
         ResetCompressorOnTimeInSecondsToZero(instance);
         ResetDoorAccelerationsInSecondsToZero(instance);
         instance->_private.useMinimumTimeBetweenDefrosts = true;
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
   ReadyToDefrostImproved_t *instance = context;
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
   else if(erd == instance->_private.config->useMinimumReadyToDefrostTimeAndResetDefrostCountsErd)
   {
      const bool *useMinimumReadyToDefrostTimeAndResetCounts = onChangeData->data;

      if(*useMinimumReadyToDefrostTimeAndResetCounts)
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_UseMinimumTimeBetweenDefrostsAndResetDefrostCounts, NULL);
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

static HsmState_t InitialState(ReadyToDefrostImproved_t *instance)
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

void ReadyToDefrostImproved_Init(
   ReadyToDefrostImproved_t *instance,
   I_DataModel_t *dataModel,
   const ReadyToDefrostImprovedConfiguration_t *config,
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
