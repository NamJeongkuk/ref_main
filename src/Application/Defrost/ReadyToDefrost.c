/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ReadyToDefrost.h"
#include "PersonalityParametricData.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"
#include "uassert.h"

static uint32_t DoorAccelerations(ReadyToDefrost_t *instance)
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

static bool ConvertibleCompartmentIsAbnormal(ReadyToDefrost_t *instance)
{
   bool hasConvertibleCompartment;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->hasConvertibleCompartment,
      &hasConvertibleCompartment);

   bool convertibleCompartmentDefrostWasAbnormal;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->convertibleCompartmentDefrostWasAbnormalErd,
      &convertibleCompartmentDefrostWasAbnormal);

   return hasConvertibleCompartment && convertibleCompartmentDefrostWasAbnormal;
}

static uint32_t TimeInSecondsWhenReadyToDefrost(ReadyToDefrost_t *instance)
{
   return instance->_private.timeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE;
}

static uint32_t CompressorOnTimeInSeconds(ReadyToDefrost_t *instance)
{
   uint32_t compressorOnTimeInSeconds;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->defrostCompressorOnTimeInSecondsErd,
      &compressorOnTimeInSeconds);

   return compressorOnTimeInSeconds;
}

static bool ReadyToDefrost(ReadyToDefrost_t *instance)
{
   const DefrostData_t *defrostData = PersonalityParametricData_Get(instance->_private.dataModel)->defrostData;
   uint32_t defrostCompressorOnTimeInSeconds = CompressorOnTimeInSeconds(instance);
   uint32_t waitingForDefrostTimeInSeconds = defrostCompressorOnTimeInSeconds;
   bool readyToDefrost = false;

   if(defrostCompressorOnTimeInSeconds >= (uint32_t)defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE)
   {
      waitingForDefrostTimeInSeconds += DoorAccelerations(instance);
      if(waitingForDefrostTimeInSeconds >= TimeInSecondsWhenReadyToDefrost(instance))
      {
         readyToDefrost = true;
      }
   }

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->waitingForDefrostTimeInSecondsErd,
      &waitingForDefrostTimeInSeconds);

   return readyToDefrost;
}

static void UpdateReadyToDefrostErd(ReadyToDefrost_t *instance)
{
   bool readyToDefrost = ReadyToDefrost(instance);
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->readyToDefrostErd,
      &readyToDefrost);
}

static void UpdateUseMinimumTimeErd(ReadyToDefrost_t *instance)
{
   if(ReadyToDefrost(instance))
   {
      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->freezerDefrostUseMinimumTimeErd,
         clear);
   }
}

static void DetermineTimeWhenReadyToDefrost(ReadyToDefrost_t *instance)
{
   const DefrostData_t *defrostData = PersonalityParametricData_Get(instance->_private.dataModel)->defrostData;

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

   bool filteredTempTooWarmOnPowerUp;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerFilteredTemperatureWasTooWarmOnPowerUpErd,
      &filteredTempTooWarmOnPowerUp);

   bool minimumTimeIsSet;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerDefrostUseMinimumTimeErd,
      &minimumTimeIsSet);

   bool eepromIsCleared;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->eepromClearedErd,
      &eepromIsCleared);

   if(freshFoodDefrostWasAbnormal ||
      freezerDefrostWasAbnormal ||
      ConvertibleCompartmentIsAbnormal(instance) ||
      invalidFreezerEvaporatorThermistorDuringDefrost ||
      filteredTempTooWarmOnPowerUp ||
      minimumTimeIsSet ||
      eepromIsCleared)
   {
      instance->_private.timeBetweenDefrostsInMinutes = defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes;
   }
   else
   {
      instance->_private.timeBetweenDefrostsInMinutes = defrostData->idleData.maxTimeBetweenDefrostsInMinutes;
   }
}

static void DataModelChanged(void *context, const void *args)
{
   ReadyToDefrost_t *instance = context;

   const DataModelOnDataChangeArgs_t *onChangeData = args;
   Erd_t erd = onChangeData->erd;

   if(erd == instance->_private.config->defrostCompressorOnTimeInSecondsErd ||
      erd == instance->_private.config->freshFoodScaledDoorAccelerationInSecondsErd ||
      erd == instance->_private.config->freezerScaledDoorAccelerationInSecondsErd ||
      erd == instance->_private.config->convertibleCompartmentScaledDoorAccelerationInSecondsErd ||
      erd == instance->_private.config->freezerDefrostWasAbnormalErd ||
      erd == instance->_private.config->freshFoodDefrostWasAbnormalErd ||
      erd == instance->_private.config->convertibleCompartmentDefrostWasAbnormalErd ||
      erd == instance->_private.config->invalidFreezerEvaporatorThermistorDuringDefrostErd ||
      erd == instance->_private.config->freezerFilteredTemperatureWasTooWarmOnPowerUpErd ||
      erd == instance->_private.config->freezerDefrostUseMinimumTimeErd ||
      erd == instance->_private.config->eepromClearedErd)
   {
      UpdateUseMinimumTimeErd(instance);
      DetermineTimeWhenReadyToDefrost(instance);
      UpdateReadyToDefrostErd(instance);
   }
}

void ReadyToDefrost_Init(
   ReadyToDefrost_t *instance,
   I_DataModel_t *dataModel,
   const ReadyToDefrostConfiguration_t *config)
{
   bool defrostCompressorOnTimeCounterReady;
   DataModel_Read(
      dataModel,
      config->defrostCompressorOnTimeCounterReadyErd,
      &defrostCompressorOnTimeCounterReady);

   bool doorAccelerationCounterReady;
   DataModel_Read(
      dataModel,
      config->doorAccelerationCounterReadyErd,
      &doorAccelerationCounterReady);

   uassert(defrostCompressorOnTimeCounterReady &&
      doorAccelerationCounterReady);

   instance->_private.config = config;
   instance->_private.dataModel = dataModel;

   UpdateUseMinimumTimeErd(instance);
   DetermineTimeWhenReadyToDefrost(instance);
   UpdateReadyToDefrostErd(instance);

   EventSubscription_Init(
      &instance->_private.dataModelSubscription,
      instance,
      DataModelChanged);

   Event_Subscribe(
      instance->_private.dataModel->OnDataChange,
      &instance->_private.dataModelSubscription);
}
