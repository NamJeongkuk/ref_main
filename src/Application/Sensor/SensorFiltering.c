/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SensorFiltering.h"
#include "PersonalityParametricData.h"
#include "DataModelErdPointerAccess.h"
#include "DataModelErdPointerAccessMacros.h"
#include "SystemErds.h"
#include "Constants_Binary.h"
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

static Filter_Fallback_Data_t FallbackFilterValue(SensorFiltering_t *instance)
{
   Filter_Fallback_Data_t value;
   Filter_Read(&instance->_private.fallbackFilter.interface, &value);

   return value;
}

static bool SensorIsDiscovered(SensorFiltering_t *instance)
{
   bool sensorIsDiscovered;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->sensorDiscoveredErd,
      &sensorIsDiscovered);

   return sensorIsDiscovered;
}

static void SetSensorIsValidTo(SensorFiltering_t *instance, bool state)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->sensorIsValidErd,
      &state);
}

static void SetSensorIsInvalidFaultTo(SensorFiltering_t *instance, bool state)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->sensorIsInvalidFaultErd,
      &state);
}

static void SetSensorDiscoveredTo(SensorFiltering_t *instance, bool state)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->sensorDiscoveredErd,
      &state);
}

static void SetUnfilteredSensorValueTo(SensorFiltering_t *instance, int16_t value)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->sensorUnfilteredTemperatureInDegFx100Erd,
      &value);
}

static void SetFilteredSensorValueTo(SensorFiltering_t *instance, int16_t value)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->sensorFilteredTemperatureInDegFx100Erd,
      &value);
}

static void UpdateSensorValues(void *context)
{
   SensorFiltering_t *instance = context;

   int16_t unfilteredTemperature = UnfilteredTemperature(instance);
   SetUnfilteredSensorValueTo(instance, unfilteredTemperature);

   Filter_Feed(&instance->_private.fallbackFilter.interface, &unfilteredTemperature);

   Filter_Fallback_Data_t fallbackFilterValue = FallbackFilterValue(instance);
   SetSensorIsValidTo(instance, fallbackFilterValue.isValid);
   SetFilteredSensorValueTo(instance, fallbackFilterValue.filteredValue);

   if(fallbackFilterValue.isValid)
   {
      SetSensorDiscoveredTo(instance, SET);
   }

   if(SensorIsDiscovered(instance))
   {
      SetSensorIsInvalidFaultTo(instance, !fallbackFilterValue.isValid);
   }
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

   I_DataSource_t *bspDataSource = DataModelErdPointerAccess_GetDataSource(dataModel, Erd_BspDataSource);

   bool bspMapped = DataSource_Has(
      bspDataSource,
      instance->_private.config->sensorAdcCountErd);

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
         instance->_private.sensorParametricData->badReadingCounterMax);
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
         instance->_private.sensorParametricData->badReadingCounterMax);
   }

   int16_t unfilteredTemperature = UnfilteredTemperature(instance);
   SetUnfilteredSensorValueTo(instance, unfilteredTemperature);

   Filter_Seed(&instance->_private.fallbackFilter.interface, &unfilteredTemperature);

   Filter_Fallback_Data_t fallbackFilterValue = FallbackFilterValue(instance);
   SetFilteredSensorValueTo(instance, fallbackFilterValue.filteredValue);

   if(fallbackFilterValue.isValid)
   {
      SetSensorDiscoveredTo(instance, SET);
      SetSensorIsValidTo(instance, SET);
      SetSensorIsInvalidFaultTo(instance, CLEAR);
   }
   else
   {
      SetSensorIsValidTo(instance, CLEAR);

      if(bspMapped && !instance->_private.sensorParametricData->discoverable)
      {
         SetSensorDiscoveredTo(instance, SET);
         SetSensorIsInvalidFaultTo(instance, SET);
      }
   }

   TimerModule_StartPeriodic(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         instance->_private.config->timerModuleErd),
      &instance->_private.filterTimer,
      periodicUpdateTimeInMs,
      UpdateSensorValues,
      instance);
}
