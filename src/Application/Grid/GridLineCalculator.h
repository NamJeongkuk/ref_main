/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef GRIDLINECALCULATOR_H
#define GRIDLINECALCULATOR_H

#include "ThermalOffsetData.h"
#include "CalculatedGridLines.h"
#include "EventSubscription.h"
#include "DeltaGridLines.h"
#include "I_DataModel.h"
#include "TemperatureDegFx100.h"

typedef struct
{
   Erd_t offsetInDegFx100Erd; // TemperatureDegFx100_t
   Erd_t adjustedSetpointInDegFx100Erd; // TemperatureDegFx100_t
} GridLineAdjustmentErds_t;

typedef struct
{
   Erd_t calculatedGridLineErd; // TwoDimensionalCalculatedGridLines_t
   Erd_t crossAmbientHysteresisAdjustmentErd; // TemperatureDegFx100_t
   Erd_t gridDeltaOffsetErd; // uint8_t
   const GridLineAdjustmentErds_t gridLineAdjustmentErds[CalculatedGridLines_MaxGridDimensionsCount];
} GridLineCalculatorConfiguration_t;

typedef struct
{
   struct
   {
      EventSubscription_t dataModelSubscription;
      const GridLineCalculatorConfiguration_t *config;
      const DeltaGridLines_t *deltaGridLines;
      I_DataModel_t *dataModel;
      union
      {
         TwoDimensionalCalculatedGridLines_t twoDimensionalCalculatedGridLines;
         OneDimensionalCalculatedGridLines_t oneDimensionalCalculatedGridLines;
      };
   } _private;
} GridLineCalculator_t;

void GridLineCalculator_Init(
   GridLineCalculator_t *instance,
   const GridLineCalculatorConfiguration_t *config,
   const DeltaGridLines_t *deltaGridLines,
   I_DataModel_t *dataModel);

#endif
