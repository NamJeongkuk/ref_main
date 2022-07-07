/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "DefrostTimerIsSatisfied.h"
#include "PersonalityParametricData.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"

static uint32_t DoorAccelerations(DefrostTimerIsSatisfied_t *instance)
{
   uint32_t freshFoodDoorAccelerationCount;
   uint32_t freezerDoorAccelerationCount;
   uint32_t convertibleCompartmentDoorAccelerationCount;

   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freshFoodDoorAccelerationCountErd,
      &freshFoodDoorAccelerationCount);
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerDoorAccelerationCountErd,
      &freezerDoorAccelerationCount);
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->convertibleCompartmentDoorAccelerationCountErd,
      &convertibleCompartmentDoorAccelerationCount);

   return freshFoodDoorAccelerationCount + freezerDoorAccelerationCount + convertibleCompartmentDoorAccelerationCount;
}

static uint32_t TimeInSecondsWhenDefrostTimerIsSatisfied(DefrostTimerIsSatisfied_t *instance)
{
   uint16_t timeInMinutesWhenDefrostTimerIsSatisfied;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->timeInMinutesWhenDefrostTimerIsSatisfiedErd,
      &timeInMinutesWhenDefrostTimerIsSatisfied);

   return (uint32_t)timeInMinutesWhenDefrostTimerIsSatisfied * SECONDS_PER_MINUTE;
}

static uint32_t CompressorOnTimeInSeconds(DefrostTimerIsSatisfied_t *instance)
{
   uint32_t compressorOnTimeInSeconds;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->compressorOnTimeInSecondsErd,
      &compressorOnTimeInSeconds);

   return compressorOnTimeInSeconds;
}

static bool DefrostTimerIsSatisfied(DefrostTimerIsSatisfied_t *instance)
{
   const DefrostData_t *defrostData = PersonalityParametricData_Get(instance->_private.dataModel)->defrostData;
   uint32_t compressorOnTimeInSeconds = CompressorOnTimeInSeconds(instance);

   if(compressorOnTimeInSeconds >= ((uint32_t)defrostData->minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE))
   {
      if((compressorOnTimeInSeconds + DoorAccelerations(instance)) >= TimeInSecondsWhenDefrostTimerIsSatisfied(instance))
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

static void UpdateDefrostTimerIsSatisfiedErd(DefrostTimerIsSatisfied_t *instance)
{
   bool defrostTimerIsSatisfied = DefrostTimerIsSatisfied(instance);
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->defrostTimerIsSatisfiedErd,
      &defrostTimerIsSatisfied);
}

static void DataModelChanged(void *context, const void *args)
{
   DefrostTimerIsSatisfied_t *instance = context;

   const DataModelOnDataChangeArgs_t *onChangeData = args;
   Erd_t erd = onChangeData->erd;

   if(erd == instance->_private.config->compressorOnTimeInSecondsErd ||
      erd == instance->_private.config->freshFoodDoorAccelerationCountErd ||
      erd == instance->_private.config->freezerDoorAccelerationCountErd ||
      erd == instance->_private.config->convertibleCompartmentDoorAccelerationCountErd ||
      erd == instance->_private.config->timeInMinutesWhenDefrostTimerIsSatisfiedErd)
   {
      UpdateDefrostTimerIsSatisfiedErd(instance);
   }
}

void DefrostTimerIsSatisfied_Init(
   DefrostTimerIsSatisfied_t *instance,
   I_DataModel_t *dataModel,
   const DefrostTimerIsSatisfiedConfiguration_t *config)
{
   instance->_private.config = config;
   instance->_private.dataModel = dataModel;

   UpdateDefrostTimerIsSatisfiedErd(instance);

   EventSubscription_Init(
      &instance->_private.dataModelSubscription,
      instance,
      DataModelChanged);

   Event_Subscribe(
      instance->_private.dataModel->OnDataChange,
      &instance->_private.dataModelSubscription);
}
