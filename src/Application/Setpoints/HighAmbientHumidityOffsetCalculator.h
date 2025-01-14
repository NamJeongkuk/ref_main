/*!
 * @file
 * @brief - calculates high ambient offset for fresh food and freezer adjusted setpoints
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef HIGHAMBIENTOFFSETCALCULATOR_H
#define HIGHAMBIENTOFFSETCALCULATOR_H

#include "I_DataModel.h"
#include "EventSubscription.h"
#include "ThermalOffsetData.h"

typedef struct
{
   Erd_t setpointStatusErd; // Setpoint_t
   Erd_t highAmbientOffsetErd; // TemperatureDegFx100_t
   Erd_t ambientFilteredHumidityResolvedPercentx100Erd; // TemperatureDegFx100_t
   Erd_t ambientHumiditySensorIsValidResolvedErd; // bool
} HighAmbientOffsetCalculatorConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      EventSubscription_t setpointStatusSubscription;
      EventSubscription_t filteredHumidityResolvedPercentx100Subscription;
      const HighAmbientOffsetData_t *highAmbientOffsetData;
      const HighAmbientOffsetCalculatorConfig_t *config;
   } _private;
} HighAmbientOffsetCalculator_t;

/*!
 * Initialize High Ambient Offset Calculator
 * @param instance high ambient calculator
 * @param dataModel
 * @param data parametric high ambient offset data
 * @param config high ambient calculator config - Erds
 */
void HighAmbientHumidityOffsetCalculator_Init(
   HighAmbientOffsetCalculator_t *instance,
   I_DataModel_t *dataModel,
   const HighAmbientOffsetData_t *data,
   const HighAmbientOffsetCalculatorConfig_t *config);

#endif
