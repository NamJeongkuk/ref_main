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

static uint32_t TimeInSecondsWhenReadyToDefrost(ReadyToDefrost_t *instance)
{
   uint16_t timeInMinutesUntilReadyToDefrost;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->timeInMinutesUntilReadyToDefrostErd,
      &timeInMinutesUntilReadyToDefrost);

   return (uint32_t)timeInMinutesUntilReadyToDefrost * SECONDS_PER_MINUTE;
}

static uint32_t CompressorOnTimeInSeconds(ReadyToDefrost_t *instance)
{
   uint32_t compressorOnTimeInSeconds;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->compressorOnTimeInSecondsErd,
      &compressorOnTimeInSeconds);

   return compressorOnTimeInSeconds;
}

static bool ReadyToDefrost(ReadyToDefrost_t *instance)
{
   const DefrostData_t *defrostData = PersonalityParametricData_Get(instance->_private.dataModel)->defrostData;
   uint32_t compressorOnTimeInSeconds = CompressorOnTimeInSeconds(instance);

   return (compressorOnTimeInSeconds >= ((uint32_t)defrostData->minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE)) &&
      ((compressorOnTimeInSeconds + DoorAccelerations(instance)) >= TimeInSecondsWhenReadyToDefrost(instance));
}

static void UpdateReadyToDefrostErd(ReadyToDefrost_t *instance)
{
   bool readyToDefrost = ReadyToDefrost(instance);
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->readyToDefrostErd,
      &readyToDefrost);
}

static void DataModelChanged(void *context, const void *args)
{
   ReadyToDefrost_t *instance = context;

   const DataModelOnDataChangeArgs_t *onChangeData = args;
   Erd_t erd = onChangeData->erd;

   if(erd == instance->_private.config->compressorOnTimeInSecondsErd ||
      erd == instance->_private.config->freshFoodScaledDoorAccelerationInSecondsErd ||
      erd == instance->_private.config->freezerScaledDoorAccelerationInSecondsErd ||
      erd == instance->_private.config->convertibleCompartmentScaledDoorAccelerationInSecondsErd ||
      erd == instance->_private.config->timeInMinutesUntilReadyToDefrostErd)
   {
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

   UpdateReadyToDefrostErd(instance);

   EventSubscription_Init(
      &instance->_private.dataModelSubscription,
      instance,
      DataModelChanged);

   Event_Subscribe(
      instance->_private.dataModel->OnDataChange,
      &instance->_private.dataModelSubscription);
}