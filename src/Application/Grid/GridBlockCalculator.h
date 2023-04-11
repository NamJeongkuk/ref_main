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
   NumberOfPreviousGridBlocksStored = 5,
   PreviousGridBlockDefaultValue = 0xFF
};

typedef struct
{
   Erd_t freshFoodFilteredResolvedTemperatureInDegFx100; // TemperatureDegFx100_t
   Erd_t freezerFilteredResolvedTemperatureInDegFx100; // TemperatureDegFx100_t
   Erd_t currentGridBlockNumberErd; // GridBlockNumber_t
   Erd_t calculatedGridLinesErd; // CalculatedGridLines_t
   Erd_t previousGridBlockNumbersErd; // PreviousGridBlockNumbers_t
   Erd_t freezerThermistorIsValidErd; // bool
   Erd_t freshFoodThermistorIsValidErd; // bool
} GridBlockCalculatorConfiguration_t;

typedef struct
{
   struct
   {
      EventSubscription_t dataModelOnChangedSubscription;
      const GridBlockCalculatorConfiguration_t *config;
      I_DataModel_t *dataModel;
      RingBuffer_t ringBuffer;
      uint8_t ringBufferArray[NumberOfPreviousGridBlocksStored];
      const GridData_t *gridData;
   } _private;
} GridBlockCalculator_t;

void GridBlockCalculator_Init(
   GridBlockCalculator_t *instance,
   const GridBlockCalculatorConfiguration_t *config,
   I_DataModel_t *dataModel,
   const GridData_t *gridData);

#endif
