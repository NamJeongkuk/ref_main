/*!
 * @file Grid Block Calculator
 * @brief Calculates main grid blocks based on if grid lines or first/second dimension resolved cabinet temperatures change.
 * Also updates previous grid blocks on change.
 *
 * Note that currently this assumes all one-dimensional grids will be inverted
 * (i.e. a higher adjusted setpoint temperature results in a lower grid block number)
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <string.h>
#include "GridBlockCalculator.h"
#include "GridBlockNumber.h"
#include "SystemErds.h"
#include "utils.h"
#include "uassert.h"

enum
{
   FirstDimension,
   SecondDimension,

   TwoDimensions = 2,

   GridLineIsNotInverted = false,
   GridLineIsInverted = true,

   PreviousGridBlockDefaultValue = 0xFF,
};

static bool ThermistorIsValid(GridBlockCalculator_t *instance, uint8_t dimension)
{
   bool state;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->gridBlockAdjustmentErds[dimension].thermistorIsValidResolvedErd,
      &state);
   return state;
}

static uint8_t GridLineIndex(
   GridBlockCalculator_t *instance,
   bool gridLineIsInverted,
   uint8_t dimension)
{
   TemperatureDegFx100_t temperature;

   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->gridBlockAdjustmentErds[dimension].filteredResolvedTemperatureInDegFx100,
      &temperature);

   TwoDimensionalCalculatedGridLines_t calculatedGridLines;

   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->calculatedGridLinesErd,
      &calculatedGridLines);

   CalculatedAxisGridLines_t calculatedGridLine =
      (dimension == FirstDimension) ? calculatedGridLines.firstDimensionGridLines : calculatedGridLines.secondDimensionGridLines;

   uint8_t index;

   for(index = 0; index < (instance->_private.gridData->deltaGridLines->gridLines->numberOfLines); index++)
   {
      if(temperature <= calculatedGridLine.gridLinesDegFx100[index])
      {
         break;
      }
   }

   if(gridLineIsInverted)
   {
      index = (instance->_private.gridData->deltaGridLines->gridLines->numberOfLines) - index;
   }

   return index;
}

static GridBlockNumber_t GridBlock(
   uint8_t firstDimensionIndex,
   uint8_t secondDimensionIndex,
   uint8_t numCols)
{
   return (secondDimensionIndex * numCols) + firstDimensionIndex % numCols;
}

static void AddGridBlockToRingBufferIfDifferent(
   GridBlockCalculator_t *instance,
   GridBlockNumber_t currentGridBlock)
{
   uint8_t ringBufferCount = RingBuffer_Count(&instance->_private.ringBuffer);

   if(ringBufferCount > 0)
   {
      GridBlockNumber_t newestPreviousGridBlockNumber;
      RingBuffer_At(
         &instance->_private.ringBuffer,
         &newestPreviousGridBlockNumber,
         (ringBufferCount - 1));

      if(newestPreviousGridBlockNumber != currentGridBlock)
      {
         RingBuffer_Add(&instance->_private.ringBuffer, &currentGridBlock);
      }
      return;
   }

   RingBuffer_Add(&instance->_private.ringBuffer, &currentGridBlock);
}

static GridBlockNumber_t GetCalculatedGridBlockNumber(GridBlockCalculator_t *instance)
{
   uint8_t firstDimensionGridLinesIndex = 0;
   uint8_t secondDimensionGridLinesIndex = 0;

   if(instance->_private.gridData->deltaGridLines->dimensions == TwoDimensions)
   {
      if(ThermistorIsValid(instance, FirstDimension))
      {
         firstDimensionGridLinesIndex = GridLineIndex(
            instance,
            GridLineIsNotInverted,
            FirstDimension);
      }
      else
      {
         firstDimensionGridLinesIndex = instance->_private.gridData->gridInvalidFirstDimensionThermistorColumn;
      }

      if(ThermistorIsValid(instance, SecondDimension))
      {
         secondDimensionGridLinesIndex = GridLineIndex(
            instance,
            GridLineIsInverted,
            SecondDimension);
      }
      else
      {
         secondDimensionGridLinesIndex = instance->_private.gridData->gridInvalidSecondDimensionThermistorRow;
      }
   }
   else
   {
      if(ThermistorIsValid(instance, FirstDimension))
      {
         firstDimensionGridLinesIndex = GridLineIndex(
            instance,
            GridLineIsInverted,
            FirstDimension);
      }
      else
      {
         firstDimensionGridLinesIndex = instance->_private.gridData->gridInvalidFirstDimensionThermistorColumn;
      }
   }

   GridBlockNumber_t calculatedBlockNumber =
      GridBlock(
         firstDimensionGridLinesIndex,
         secondDimensionGridLinesIndex,
         (instance->_private.gridData->deltaGridLines->gridLines->numberOfLines + 1));

   return calculatedBlockNumber;
}

static void UpdateGridBlock(GridBlockCalculator_t *instance)
{
   GridBlockNumber_t calculatedBlockNumber = GetCalculatedGridBlockNumber(instance);

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->currentGridBlockNumberErd,
      &calculatedBlockNumber);
}

static void CopyFifoOrder(GridBlockCalculator_t *instance, PreviousGridBlockNumbers_t *outputBlocks)
{
   uint8_t j = GridBlockCalculator_NumberOfPreviousGridBlocksToStore - 1;
   for(uint8_t i = 0; i < outputBlocks->count; i++)
   {
      RingBuffer_At(&instance->_private.ringBuffer, &outputBlocks->blockNumbers[j--], i);
   }
}

static void UpdatePreviousGridBlocks(GridBlockCalculator_t *instance)
{
   PreviousGridBlockNumbers_t previousGridBlockNumbers;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->previousGridBlockNumbersErd,
      &previousGridBlockNumbers);

   previousGridBlockNumbers.count = MIN(
      GridBlockCalculator_NumberOfPreviousGridBlocksToStore,
      RingBuffer_Count(&instance->_private.ringBuffer));

   CopyFifoOrder(instance, &previousGridBlockNumbers);

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->previousGridBlockNumbersErd,
      &previousGridBlockNumbers);
}

static void UpdateGridBlockIfDifferent(GridBlockCalculator_t *instance)
{
   GridBlockNumber_t calculatedBlockNumber = GetCalculatedGridBlockNumber(instance);

   GridBlockNumber_t savedBlockNumber;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->currentGridBlockNumberErd,
      &savedBlockNumber);

   if(calculatedBlockNumber != savedBlockNumber)
   {
      AddGridBlockToRingBufferIfDifferent(instance, savedBlockNumber);
      UpdatePreviousGridBlocks(instance);

      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->currentGridBlockNumberErd,
         &calculatedBlockNumber);
   }
}

static void WriteDefaultValuesToPreviousGridBlockErd(GridBlockCalculator_t *instance)
{
   PreviousGridBlockNumbers_t defaultValue = {
      .count = 0,
   };

   memset(defaultValue.blockNumbers, PreviousGridBlockDefaultValue, sizeof(defaultValue.blockNumbers));

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->previousGridBlockNumbersErd,
      &defaultValue);
}

static void OnDataModelChanged(void *context, const void *args)
{
   GridBlockCalculator_t *instance = context;
   const DataModelOnDataChangeArgs_t *onChangeData = args;
   const Erd_t erd = onChangeData->erd;

   if(erd == instance->_private.config->calculatedGridLinesErd)
   {
      if(ThermistorIsValid(instance, FirstDimension) || ((instance->_private.gridData->deltaGridLines->dimensions == TwoDimensions) && ThermistorIsValid(instance, SecondDimension)))
      {
         UpdateGridBlockIfDifferent(instance);
      }
   }
   else
   {
      for(uint8_t i = 0; i < instance->_private.gridData->deltaGridLines->dimensions; i++)
      {
         if((erd == instance->_private.config->gridBlockAdjustmentErds[i].filteredResolvedTemperatureInDegFx100) ||
            (erd == instance->_private.config->gridBlockAdjustmentErds[i].thermistorIsValidResolvedErd))
         {
            if(ThermistorIsValid(instance, FirstDimension) || ((instance->_private.gridData->deltaGridLines->dimensions == TwoDimensions) && ThermistorIsValid(instance, SecondDimension)))
            {
               UpdateGridBlockIfDifferent(instance);
               break;
            }
         }
      }
   }
}

void GridBlockCalculator_Init(
   GridBlockCalculator_t *instance,
   const GridBlockCalculatorConfiguration_t *config,
   I_DataModel_t *dataModel,
   const GridData_t *gridData)
{
   bool gridLineCalculatorReady;
   DataModel_Read(
      dataModel,
      Erd_GridLineCalculatorIsReady,
      &gridLineCalculatorReady);
   uassert(gridLineCalculatorReady);

   instance->_private.config = config;
   instance->_private.dataModel = dataModel;
   instance->_private.gridData = gridData;

   RingBuffer_Init(
      &instance->_private.ringBuffer,
      &instance->_private.ringBufferArray,
      GridBlockCalculator_NumberOfPreviousGridBlocksToStore,
      sizeof(GridBlockNumber_t));

   WriteDefaultValuesToPreviousGridBlockErd(instance);

   UpdateGridBlock(instance);

   EventSubscription_Init(
      &instance->_private.subscription,
      instance,
      OnDataModelChanged);

   DataModel_SubscribeAll(
      dataModel,
      &instance->_private.subscription);
}
