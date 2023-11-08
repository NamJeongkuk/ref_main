/*!
 * @file
 * @brief Filters the sensor input through a scaled filter,
 * slew filter, EWMA filter, and a fallback filter and outputs
 * a filtered value in degFx100.
 *
 * If sensor becomes valid after being invalid on init and
 * it is discoverable, it sets the sensor as discovered.
 *
 * If sensor is not discoverable, it sets the sensor is invalid
 * fault if sensor is invalid and clears if sensor is valid but
 * if the sensor is discoverable, it must have been discovered before
 * setting/clearing the fault.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SENSORFILTERING_H
#define SENSORFILTERING_H

#include "I_DataModel.h"
#include "SensorData.h"
#include "Filter_ExponentiallyWeightedMovingAverage.h"
#include "Filter_Scaled.h"
#include "Filter_Slew.h"
#include "Filter_Fallback.h"
#include "Timer.h"
#include "Mapper_UnsignedSignedLookupTable.h"

typedef struct
{
   Erd_t sensorAdcCountErd;
   Erd_t sensorUnfilteredTemperatureInDegFx100Erd;
   Erd_t sensorFilteredTemperatureInDegFx100Erd;
   Erd_t sensorIsValidErd;
   Erd_t sensorIsInvalidFaultErd;
   Erd_t sensorDiscoveredErd;
   Erd_t timerModuleErd;
} SensorFilteringConfig_t;

typedef struct
{
   struct
   {
      Filter_ExponentiallyWeightedMovingAverage_t ewmaFilter;
      Filter_Scaled_t scaledFilter;
      Filter_Slew_t slewFilter;
      Filter_Fallback_t fallbackFilter;
      Timer_t filterTimer;
      Mapper_UnsignedSignedLookupTable_t lookupTable;
      I_DataModel_t *dataModel;
      const SensorDataSensorType_t *sensorParametricData;
      const SensorFilteringConfig_t *config;
   } _private;
} SensorFiltering_t;

void SensorFiltering_Init(
   SensorFiltering_t *instance,
   I_DataModel_t *dataModel,
   const SensorFilteringConfig_t *config,
   const SensorDataSensorType_t *sensorParametricData,
   uint16_t periodicUpdateRateInMs);

#endif
