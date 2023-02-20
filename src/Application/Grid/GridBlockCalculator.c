/*!
 * @file Grid Block Calculator
 * @brief Calculates main grid blocks based on if grid lines or fresh Food / freezer resolved cabinet temperatures change
 * Also updates previous grid blocks on change
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "GridBlockCalculator.h"
#include "GridBlockNumber.h"
#include "SystemErds.h"
#include "utils.h"
#include "uassert.h"

enum
{
   NonInvertedGridLine = 0,
   InvertedGridLine = 1,
};

static void UpdatePreviousGridBlocks(GridBlockCalculator_t *instance);

static CalculatedAxisGridLines_t CalculatedGridLine(
   GridBlockCalculator_t *instance,
   uint8_t gridLineDimension)
{
   CalculatedGridLines_t calculatedGridLines;

   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->calculatedGridLinesErd,
      &calculatedGridLines);

   if(gridLineDimension == FreshFoodGridLineDimension)
   {
      return calculatedGridLines.freshFoodGridLine;
   }
   else
   {
      return calculatedGridLines.freezerGridLine;
   }
}

static uint8_t GridLineIndex(
   GridBlockCalculator_t *instance,
   TemperatureDegFx100_t temperature,
   bool gridLineIsInverted,
   uint8_t gridLineDimension)
{
   CalculatedAxisGridLines_t calculatedGridLine = CalculatedGridLine(instance, gridLineDimension);
   uint8_t index;

   for(index = 0; index < NumberOfGridLinesPerAxis; index++)
   {
      if(temperature <= calculatedGridLine.gridLinesDegFx100[index])
      {
         break;
      }
   }

   if(gridLineIsInverted)
   {
      index = NumberOfGridLinesPerAxis - index;
   }

   return index;
}

static GridBlockNumber_t GridBlock(
   uint8_t freshFoodIndex,
   uint8_t freezerIndex,
   uint8_t numCols)
{
   return (freezerIndex * numCols) + freshFoodIndex % numCols;
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
   TemperatureDegFx100_t freshFoodTemp;
   TemperatureDegFx100_t freezerTemp;

   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freshFoodFilteredResolvedTemperatureInDegFx100,
      &freshFoodTemp);

   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerFilteredResolvedTemperatureInDegFx100,
      &freezerTemp);

   uint8_t freshFoodGridLineIndex = GridLineIndex(
      instance,
      freshFoodTemp,
      NonInvertedGridLine,
      FreshFoodGridLineDimension);

   uint8_t freezerGridLineIndex = GridLineIndex(
      instance,
      freezerTemp,
      InvertedGridLine,
      FreezerGridLineDimension);

   GridBlockNumber_t calculatedBlockNumber =
      GridBlock(
         freshFoodGridLineIndex,
         freezerGridLineIndex,
         (NumberOfGridLinesPerAxis + 1));

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

static void CopyFifoOrder(GridBlockCalculator_t *instance, PreviousGridBlockNumbers_t *outputBlocks)
{
   uint8_t j = NumberOfPreviousGridBlocksStored - 1;
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
      NumberOfPreviousGridBlocksStored,
      RingBuffer_Count(&instance->_private.ringBuffer));

   CopyFifoOrder(instance, &previousGridBlockNumbers);

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->previousGridBlockNumbersErd,
      &previousGridBlockNumbers);
}

static void OnDataModelChanged(void *context, const void *args)
{
   const DataModelOnDataChangeArgs_t *arguments = args;
   GridBlockCalculator_t *instance = context;
   Erd_t erd = arguments->erd;

   if((erd == instance->_private.config->freshFoodFilteredResolvedTemperatureInDegFx100) ||
      (erd == instance->_private.config->freezerFilteredResolvedTemperatureInDegFx100) ||
      (erd == instance->_private.config->calculatedGridLinesErd))
   {
      UpdateGridBlockIfDifferent(instance);
   }
}

void GridBlockCalculator_Init(
   GridBlockCalculator_t *instance,
   const GridBlockCalculatorConfiguration_t *config,
   I_DataModel_t *dataModel)
{
   bool gridLineCalculatorReady;
   DataModel_Read(
      dataModel,
      Erd_GridLineCalculatorIsReady,
      &gridLineCalculatorReady);
   uassert(gridLineCalculatorReady);

   instance->_private.config = config;
   instance->_private.dataModel = dataModel;

   RingBuffer_Init(
      &instance->_private.ringBuffer,
      &instance->_private.ringBufferArray,
      NumberOfPreviousGridBlocksStored,
      sizeof(GridBlockNumber_t));

   WriteDefaultValuesToPreviousGridBlockErd(instance);

   UpdateGridBlock(instance);

   EventSubscription_Init(
      &instance->_private.dataModelOnChangedSubscription,
      instance,
      OnDataModelChanged);

   DataModel_SubscribeAll(
      dataModel,
      &instance->_private.dataModelOnChangedSubscription);
}
