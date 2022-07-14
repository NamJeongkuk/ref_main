/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "DefrostReadyTimerIsSatisfied.h"
#include "PersonalityParametricData.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"

static uint32_t DoorAccelerations(DefrostReadyTimerIsSatisfied_t *instance)
{
   uint32_t freshFoodScaledDoorAccelerationInSeconds;
   uint32_t freezerScaledDoorAccelerationInSeconds;
   uint32_t convertibleCompartmentScaledDoorAccelerationInSeconds;

   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freshFoodScaledDoorAccelerationInSecondsErd,
      &freshFoodScaledDoorAccelerationInSeconds);
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerScaledDoorAccelerationInSecondsErd,
      &freezerScaledDoorAccelerationInSeconds);
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->convertibleCompartmentScaledDoorAccelerationInSecondsErd,
      &convertibleCompartmentScaledDoorAccelerationInSeconds);

   return freshFoodScaledDoorAccelerationInSeconds +
      freezerScaledDoorAccelerationInSeconds +
      convertibleCompartmentScaledDoorAccelerationInSeconds;
}

static uint32_t TimeInSecondsWhenDefrostReadyTimerIsSatisfied(DefrostReadyTimerIsSatisfied_t *instance)
{
   uint16_t timeInMinutesWhenDefrostReadyTimerIsSatisfied;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->timeInMinutesWhenDefrostReadyTimerIsSatisfiedErd,
      &timeInMinutesWhenDefrostReadyTimerIsSatisfied);

   return (uint32_t)timeInMinutesWhenDefrostReadyTimerIsSatisfied * SECONDS_PER_MINUTE;
}

static uint32_t CompressorOnTimeInSeconds(DefrostReadyTimerIsSatisfied_t *instance)
{
   uint32_t compressorOnTimeInSeconds;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->compressorOnTimeInSecondsErd,
      &compressorOnTimeInSeconds);

   return compressorOnTimeInSeconds;
}

static bool DefrostReadyTimerIsSatisfied(DefrostReadyTimerIsSatisfied_t *instance)
{
   const DefrostData_t *defrostData = PersonalityParametricData_Get(instance->_private.dataModel)->defrostData;
   uint32_t compressorOnTimeInSeconds = CompressorOnTimeInSeconds(instance);

   if(compressorOnTimeInSeconds >= ((uint32_t)defrostData->minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE))
   {
      if((compressorOnTimeInSeconds + DoorAccelerations(instance)) >= TimeInSecondsWhenDefrostReadyTimerIsSatisfied(instance))
      {
         return true;
      }
      else
      {
         return false;
      }
   }
   else
   {
      return false;
   }
}

static void UpdateDefrostReadyTimerIsSatisfiedErd(DefrostReadyTimerIsSatisfied_t *instance)
{
   bool defrostReadyTimerIsSatisfied = DefrostReadyTimerIsSatisfied(instance);
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->defrostReadyTimerIsSatisfiedErd,
      &defrostReadyTimerIsSatisfied);
}

static void DataModelChanged(void *context, const void *args)
{
   DefrostReadyTimerIsSatisfied_t *instance = context;

   const DataModelOnDataChangeArgs_t *onChangeData = args;
   Erd_t erd = onChangeData->erd;

   if(erd == instance->_private.config->compressorOnTimeInSecondsErd ||
      erd == instance->_private.config->freshFoodScaledDoorAccelerationInSecondsErd ||
      erd == instance->_private.config->freezerScaledDoorAccelerationInSecondsErd ||
      erd == instance->_private.config->convertibleCompartmentScaledDoorAccelerationInSecondsErd ||
      erd == instance->_private.config->timeInMinutesWhenDefrostReadyTimerIsSatisfiedErd)
   {
      UpdateDefrostReadyTimerIsSatisfiedErd(instance);
   }
}

void DefrostReadyTimerIsSatisfied_Init(
   DefrostReadyTimerIsSatisfied_t *instance,
   I_DataModel_t *dataModel,
   const DefrostReadyTimerIsSatisfiedConfiguration_t *config)
{
   instance->_private.config = config;
   instance->_private.dataModel = dataModel;

   UpdateDefrostReadyTimerIsSatisfiedErd(instance);

   EventSubscription_Init(
      &instance->_private.dataModelSubscription,
      instance,
      DataModelChanged);

   Event_Subscribe(
      instance->_private.dataModel->OnDataChange,
      &instance->_private.dataModelSubscription);
}
