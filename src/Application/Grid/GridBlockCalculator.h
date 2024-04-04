/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef GRIDBLOCKCALCULATOR_H
#define GRIDBLOCKCALCULATOR_H

#include "GridData.h"
#include "I_DataModel.h"
#include "CalculatedGridLines.h"
#include "RingBuffer.h"

enum
{
   GridBlockCalculator_NumberOfPreviousGridBlocksToStore = 5
};

typedef struct
{
   Erd_t filteredResolvedTemperatureInDegFx100; // TemperatureDegFx100_t
   Erd_t thermistorIsValidResolvedErd; // TemperatureDegFx100_t
} GridBlockAdjustmentErds_t;

typedef struct
{
   Erd_t calculatedGridLinesErd; // CalculatedGridLines_t
   Erd_t currentGridBlockNumberErd; // GridBlockNumber_t
   Erd_t previousGridBlockNumbersErd; // PreviousGridBlockNumbers_t
   GridBlockAdjustmentErds_t gridBlockAdjustmentErds[CalculatedGridLines_MaxGridDimensionsCount];
} GridBlockCalculatorConfiguration_t;

typedef struct
{
   struct
   {
      EventSubscription_t subscription;
      const GridBlockCalculatorConfiguration_t *config;
      I_DataModel_t *dataModel;
      RingBuffer_t ringBuffer;
      uint8_t ringBufferArray[GridBlockCalculator_NumberOfPreviousGridBlocksToStore];
      const GridData_t *gridData;
   } _private;
} GridBlockCalculator_t;

void GridBlockCalculator_Init(
   GridBlockCalculator_t *instance,
   const GridBlockCalculatorConfiguration_t *config,
   I_DataModel_t *dataModel,
   const GridData_t *gridData);

#endif
