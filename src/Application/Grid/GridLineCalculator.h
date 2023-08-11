/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef GRIDLINECALCULATOR_H
#define GRIDLINECALCULATOR_H

#include "EventSubscription.h"
#include "I_DataModel.h"
#include "TemperatureDegFx100.h"
#include "GridData.h"
#include "CalculatedGridLines.h"
#include "AdjustedSetpointData.h"

typedef struct
{
   Erd_t offsetInDegFx100Erd; // TemperatureDegFx100_t
   Erd_t adjustedSetpointInDegFx100Erd; // TemperatureDegFx100_t
} GridLineAdjustmentErds_t;

typedef struct
{
   Erd_t calculatedGridLineErd; // CalculatedGridLines_t
   Erd_t freshFoodFilteredTemperatureInDegFx100Erd; // TemperatureDegFx100_t
   Erd_t freezerFilteredTemperatureInDegFx100Erd; // TemperatureDegFx100_t
   Erd_t crossAmbientHysteresisAdjustmentErd; // TemperatureDegFx100_t
   GridLineAdjustmentErds_t gridLineAdjustmentErds[NumberGridDimensions];
} GridLineCalculatorConfiguration_t;

typedef struct
{
   struct
   {
      EventSubscription_t dataModelSubscription;
      const GridLineCalculatorConfiguration_t *config;
      const GridData_t *gridData;
      CalculatedGridLines_t calculatedGridLineOutput;
      I_DataModel_t *dataModel;
   } _private;
} GridLineCalculator_t;

void GridLineCalculator_Init(
   GridLineCalculator_t *instance,
   const GridLineCalculatorConfiguration_t *config,
   I_DataModel_t *dataModel);

#endif
