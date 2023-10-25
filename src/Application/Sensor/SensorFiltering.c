/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SensorFiltering.h"
#include "PersonalityParametricData.h"
#include "DataModelErdPointerAccess.h"
#include <stdint.h>

static int16_t UnfilteredTemperature(SensorFiltering_t *instance)
{
   AdcCounts_t rawAdcValue;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->sensorAdcCountErd,
      &rawAdcValue);

   int32_t mappedAdcValue = (int32_t)Mapper_GetMappedValue(
      &instance->_private.lookupTable.interface,
      rawAdcValue);

   return (int16_t)mappedAdcValue;
}

static bool FallbackFilterHasBeenSeeded(SensorFiltering_t *instance)
{
   I_Input_t *readyInput = Filter_GetReadyInput(&instance->_private.fallbackFilter.interface);
   bool hasBeenSeeded;
   Input_Read(readyInput, &hasBeenSeeded);

   return hasBeenSeeded;
}

static int16_t FallbackFilterValue(SensorFiltering_t *instance)
{
   int16_t value;
   Filter_Read(&instance->_private.fallbackFilter.interface, &value);

   return value;
}

static void UpdateSensorValues(void *context)
{
   SensorFiltering_t *instance = context;

   int16_t unfilteredTemperature = UnfilteredTemperature(instance);

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->sensorUnfilteredTemperatureInDegFx100Erd,
      &unfilteredTemperature);

   if(!FallbackFilterHasBeenSeeded(instance))
   {
      Filter_Seed(&instance->_private.fallbackFilter.interface, &unfilteredTemperature);
   }
   else
   {
      Filter_Feed(&instance->_private.fallbackFilter.interface, &unfilteredTemperature);
   }

   int16_t fallbackFilterValue = FallbackFilterValue(instance);

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->sensorFilteredTemperatureInDegFx100Erd,
      &fallbackFilterValue);
}

void SensorFiltering_Init(
   SensorFiltering_t *instance,
   I_DataModel_t *dataModel,
   const SensorFilteringConfig_t *config,
   const SensorDataSensorType_t *sensorParametricData,
   uint16_t periodicUpdateTimeInMs)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;
   instance->_private.sensorParametricData = sensorParametricData;

   Mapper_UnsignedSignedLookupTable_Init(
      &instance->_private.lookupTable,
      instance->_private.sensorParametricData->lookupTable);

   Filter_ExponentiallyWeightedMovingAverage_Init(
      &instance->_private.ewmaFilter,
      instance->_private.sensorParametricData->windowSize,
      IS_SIGNED(int32_t),
      sizeof(int32_t),
      instance->_private.sensorParametricData->alphaDenom - instance->_private.sensorParametricData->alphaNum,
      instance->_private.sensorParametricData->alphaDenom);

   Filter_Scaled_Init(
      &instance->_private.scaledFilter,
      &instance->_private.ewmaFilter.interface,
      UINT8_MAX);

   if(sensorParametricData->clampData.slewRateFilterEnabled)
   {
      Filter_Slew_Init(
         &instance->_private.slewFilter,
         &instance->_private.scaledFilter.interface,
         instance->_private.sensorParametricData->clampData.slewRatePerSecondx100);

      Filter_Fallback_Init(
         &instance->_private.fallbackFilter,
         &instance->_private.slewFilter.interface,
         dataModel,
         instance->_private.sensorParametricData->lookupTable->mappings[instance->_private.sensorParametricData->lookupTable->mappingCount - 1].y,
         instance->_private.sensorParametricData->fallbackValueDegFx100,
         instance->_private.sensorParametricData->goodReadingCounterMax,
         instance->_private.sensorParametricData->badReadingCounterMax,
         instance->_private.config->sensorIsValidErd);
   }
   else
   {
      Filter_Fallback_Init(
         &instance->_private.fallbackFilter,
         &instance->_private.scaledFilter.interface,
         dataModel,
         instance->_private.sensorParametricData->lookupTable->mappings[instance->_private.sensorParametricData->lookupTable->mappingCount - 1].y,
         instance->_private.sensorParametricData->fallbackValueDegFx100,
         instance->_private.sensorParametricData->goodReadingCounterMax,
         instance->_private.sensorParametricData->badReadingCounterMax,
         instance->_private.config->sensorIsValidErd);
   }

   int16_t unfilteredTemperature = UnfilteredTemperature(instance);

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->sensorUnfilteredTemperatureInDegFx100Erd,
      &unfilteredTemperature);

   Filter_Seed(&instance->_private.fallbackFilter.interface, &unfilteredTemperature);

   int16_t fallbackFilterValue = FallbackFilterValue(instance);

   DataModel_Write(
      dataModel,
      instance->_private.config->sensorFilteredTemperatureInDegFx100Erd,
      &fallbackFilterValue);

   TimerModule_StartPeriodic(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         instance->_private.config->timerModuleErd),
      &instance->_private.filterTimer,
      periodicUpdateTimeInMs,
      UpdateSensorValues,
      instance);
}
