/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef CROSSAMBIENTOFFSETCALCULATOR_H
#define CROSSAMBIENTOFFSETCALCULATOR_H

#include "I_DataModel.h"
#include "EventSubscription.h"
#include "ThermalOffsetData.h"

typedef struct
{
   Erd_t ambientWindowAveragedTemperatureInDegFx100Erd; // TemperatureDegFx100_t
   Erd_t crossAmbientOffsetInDegFx100Erd; // TemperatureDegFx100_t
} CrossAmbientOffsetCalculatorConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      EventSubscription_t crossAmbientWindowAveragedTemperatureSubscription;
      const CrossAmbientOffsetData_t *crossAmbientOffsetData;
      const CrossAmbientOffsetCalculatorConfig_t *config;
   } _private;
} CrossAmbientOffsetCalculator_t;

void CrossAmbientOffsetCalculator_Init(
   CrossAmbientOffsetCalculator_t *instance,
   I_DataModel_t *dataModel,
   const CrossAmbientOffsetData_t *data,
   const CrossAmbientOffsetCalculatorConfig_t *config);

#endif
