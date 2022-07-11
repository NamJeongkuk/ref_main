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

typedef struct
{
   Erd_t rawSetpointErd; // TemperatureDegFx100_t
   Erd_t offsetErd; // TemperatureDegFx100_t
   Erd_t shiftErd; // TemperatureDegFx100_t
   Erd_t adjustedSetpointErd; // TemperatureDegFx100_t
} GridLineAdjustmentErds_t;

typedef struct
{
   Erd_t calculatedGridLineErd; // CalculatedGridLines_t
   Erd_t freshFoodFilteredTempErd; // TemperatureDegFx100_t
   Erd_t freezerFilteredTemperatureErd; // TemperatureDegFx100_t
   GridLineAdjustmentErds_t gridLineAdjustmentErds[NumberGridDimensions];
} GridLineCalculatorConfiguration_t;

typedef struct
{
   struct
   {
      EventSubscription_t dataModelSubscription;
      const GridLineCalculatorConfiguration_t *config;
      const GridData_t *gridData;
      CalculatedAxisGridLines_t freshFoodCalculatedAxis;
      CalculatedAxisGridLines_t freezerCalculatedAxis;
      CalculatedAxisGridLines_t calculatedGridLines[NumberGridDimensions];
      CalculatedGridLines_t calculatedGridLineOutput;
      TemperatureDegFx100_t calculatedFreshFoodGridLines[NumberOfGridLinesPerAxis];
      TemperatureDegFx100_t calculatedFreezerGridLines[NumberOfGridLinesPerAxis];
      I_DataModel_t *dataModel;
   } _private;
} GridLineCalculator_t;

void GridLineCalculator_Init(
   GridLineCalculator_t *instance,
   const GridLineCalculatorConfiguration_t *config,
   I_DataModel_t *dataModel);

#endif
