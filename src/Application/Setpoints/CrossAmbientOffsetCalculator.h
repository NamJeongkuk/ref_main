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
#include "AdjustedSetpointData.h"

typedef struct
{
   Erd_t crossAmbientWindowAveragedTemperatureInDegFx100Erd; // TemperatureDegFx100_t
   Erd_t crossAmbientOffsetErd; // TemperatureDegFx100_t
} CrossAmbientCalculatorConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      EventSubscription_t crossAmbientWindowAveragedTemperatureSubscription;
      const CrossAmbientOffsetData_t *crossAmbientOffsetData;
      const CrossAmbientCalculatorConfig_t *config;
   } _private;
} CrossAmbientCalculator_t;

void CrossAmbientOffsetCalculator_Init(
   CrossAmbientCalculator_t *instance,
   I_DataModel_t *dataModel,
   const CrossAmbientOffsetData_t *data,
   const CrossAmbientCalculatorConfig_t *config);

#endif
