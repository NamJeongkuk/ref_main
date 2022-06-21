/*!
 * @file
 * @brief main grid control
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "utils.h"
#include "CalcGridBlockAndGridLines.h"
#include "DataModelErdPointerAccess.h"
#include "Constants_Time.h"
#include "Constants_Binary.h"
#include "Setpoint.h"

enum
{
   InvertingMultiplier = -1,
   NotInverted = false,
   Inverted = true,
   CabinetGridLineCount = 6
};

static TemperatureDegFx100_t freshFoodCalculatedAxisGridLines[CabinetGridLineCount] = { 0 };
static TemperatureDegFx100_t freezerCalculatedAxisGridLines[CabinetGridLineCount] = { 0 };

static const CalculatedAxisGridLines_t freshFoodCalculatedAxis = {
   .numberOfLines = ELEMENT_COUNT(freshFoodCalculatedAxisGridLines),
   .gridLinesDegFx100 = freshFoodCalculatedAxisGridLines
};

static const CalculatedAxisGridLines_t freezerCalculatedAxis = {
   .numberOfLines = ELEMENT_COUNT(freezerCalculatedAxisGridLines),
   .gridLinesDegFx100 = freezerCalculatedAxisGridLines
};

static CalculatedAxisGridLines_t calculatedGrid[] = { freshFoodCalculatedAxis, freezerCalculatedAxis };

static CalculatedGridLines_t calculatedGridLines = {
   .dimensions = ELEMENT_COUNT(calculatedGrid),
   .gridLines = calculatedGrid
};

static PreviousGridBlockNumbers_t previousGridBlocks = {
   .currentNumberOfBlocksStored = 0
};

static GridBlockNumber_t ringBufferBuffer[PreviousGridBlockNumbers_MaxBlockNumbers] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

static void CalculateAxisGridLines(
   CalculatedAxisGridLines_t *calcAxisGridlines,
   const DeltaAxisGridLines_t *parametricAxisGridLines,
   I_DataModel_t *dataModel,
   GridLineErds_t erds)
{
   for(uint8_t i = 0; i < parametricAxisGridLines->numberOfLines; i++)
   {
      calcAxisGridlines->gridLinesDegFx100[i] = parametricAxisGridLines->gridLineData[i].gridLinesDegFx100;

      if(parametricAxisGridLines->gridLineData[i].bitMapping & DeltaGridLines_BitMapping_SetpointBitMask)
      {
         SetpointVotedTemperature_t rawSetpoint;
         DataModel_Read(dataModel, erds.rawSetpointErd, &rawSetpoint);
         calcAxisGridlines->gridLinesDegFx100[i] += rawSetpoint.temperature;
      }

      if(parametricAxisGridLines->gridLineData[i].bitMapping & DeltaGridLines_BitMapping_OffsetBitMask)
      {
         TemperatureDegFx100_t offset;
         DataModel_Read(dataModel, erds.offsetErd, &offset);
         calcAxisGridlines->gridLinesDegFx100[i] += offset;
      }

      if(parametricAxisGridLines->gridLineData[i].bitMapping & DeltaGridLines_BitMapping_ShiftBitMask)
      {
         TemperatureDegFx100_t shift;
         DataModel_Read(dataModel, erds.shiftErd, &shift);
         calcAxisGridlines->gridLinesDegFx100[i] += shift;
      }

      if(parametricAxisGridLines->gridLineData[i].bitMapping & DeltaGridLines_BitMapping_AdjSetpointBitMask)
      {
         TemperatureDegFx100_t adjSetpoint;
         DataModel_Read(dataModel, erds.adjustedSetpointErd, &adjSetpoint);
         calcAxisGridlines->gridLinesDegFx100[i] += adjSetpoint;
      }
   }
}

static void UpdateGridLines(CalcGridBlockAndLines_t *instance)
{
   CalculateAxisGridLines(
      &calculatedGridLines.gridLines[GridDelta_FreshFood],
      &instance->_private.gridData->deltaGridLines->gridLines[GridDelta_FreshFood],
      instance->_private.dataModel,
      instance->_private.config->freshFoodGridLineErds);
   CalculateAxisGridLines(
      &calculatedGridLines.gridLines[GridDelta_Freezer],
      &instance->_private.gridData->deltaGridLines->gridLines[GridDelta_Freezer],
      instance->_private.dataModel,
      instance->_private.config->freezerGridLineErds);

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->calculatedGridLinesErd,
      &calculatedGridLines);
}

static uint8_t GetLocationIndex(
   TemperatureDegFx100_t temperature,
   uint8_t compartmentType,
   CalculatedGridLines_t *gridLines,
   bool inverted)
{
   uint8_t index;

   for(index = 0; index < gridLines->gridLines[compartmentType].numberOfLines; index++)
   {
      if(temperature <= gridLines->gridLines[compartmentType].gridLinesDegFx100[index])
      {
         break;
      }
   }

   if(inverted)
   {
      index = gridLines->gridLines[compartmentType].numberOfLines - index;
   }

   return index;
}

static GridBlockNumber_t CalculateGridBlock(
   uint8_t rowIndex,
   uint8_t colIndex,
   uint8_t numCols)
{
   return rowIndex * numCols + colIndex % numCols;
}

static void UpdateGridBlock(CalcGridBlockAndLines_t *instance)
{
   TemperatureDegFx100_t freshFoodFilteredTemp;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freshFoodFilteredTempErd,
      &freshFoodFilteredTemp);

   TemperatureDegFx100_t freezerFilteredTemp;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerFilteredTempErd,
      &freezerFilteredTemp);

   uint8_t colIndex = GetLocationIndex(freshFoodFilteredTemp, GridDelta_FreshFood, &calculatedGridLines, NotInverted);
   uint8_t rowIndex = GetLocationIndex(freezerFilteredTemp, GridDelta_Freezer, &calculatedGridLines, Inverted);

   GridBlockNumber_t newBlockNumber = CalculateGridBlock(rowIndex,
      colIndex,
      instance->_private.gridData->deltaGridLines->gridLines[GridDelta_FreshFood].numberOfLines + 1);

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->calculatedGridBlockErd,
      &newBlockNumber);
}

static void UpdatePreviousGridBlocks(void *context, const void *_args)
{
   CalcGridBlockAndLines_t *instance = context;
   const GridBlockNumber_t *blockNumber = _args;

   if(instance->_private.lastBlock != GridBlockNumber_Max)
   {
      RingBuffer_Add(&instance->_private.bufferInstance, &instance->_private.lastBlock);
      previousGridBlocks.currentNumberOfBlocksStored = RingBuffer_Count(&instance->_private.bufferInstance);

      for(uint8_t i = 0; i < previousGridBlocks.currentNumberOfBlocksStored; i++)
      {
         GridBlockNumber_t number;
         RingBuffer_At(&instance->_private.bufferInstance, &number, i);
         previousGridBlocks.blockNumbers[i] = number;
      }

      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->previousGridBlocksErd,
         &previousGridBlocks);
   }

   instance->_private.lastBlock = *blockNumber;
}

static void UpdateGridBlockAndGridLines(void *context)
{
   CalcGridBlockAndLines_t *instance = context;

   UpdateGridLines(instance);
   UpdateGridBlock(instance);
}

static void ClearPreviousGridBlocks(void)
{
   previousGridBlocks.currentNumberOfBlocksStored = 0;

   for(uint8_t i = 0; i < NUM_ELEMENTS(previousGridBlocks.blockNumbers); i++)
   {
      previousGridBlocks.blockNumbers[i] = 0xFF;
   }
}

static void UpdateLastBlockWithCurrentBlock(CalcGridBlockAndLines_t *instance)
{
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->calculatedGridBlockErd,
      &instance->_private.lastBlock);
}

void CalcGridBlockAndGridLines_Init(
   CalcGridBlockAndLines_t *instance,
   const GridBlockAndLinesConfig_t *config,
   I_DataModel_t *dataModel)
{
   instance->_private.config = config;
   instance->_private.dataModel = dataModel;

   RingBuffer_Init(
      &instance->_private.bufferInstance,
      ringBufferBuffer,
      PreviousGridBlockNumbers_MaxBlockNumbers,
      ELEMENT_SIZE(ringBufferBuffer));

   instance->_private.gridData = PersonalityParametricData_Get(dataModel)->gridData;

   ClearPreviousGridBlocks();

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->previousGridBlocksErd,
      &previousGridBlocks);

   UpdateGridBlockAndGridLines(instance);
   UpdateLastBlockWithCurrentBlock(instance);

   EventSubscription_Init(
      &instance->_private.gridBlockSubscription,
      instance,
      UpdatePreviousGridBlocks);

   DataModel_Subscribe(
      instance->_private.dataModel,
      instance->_private.config->calculatedGridBlockErd,
      &instance->_private.gridBlockSubscription);

   TimerModule_StartPeriodic(
      DataModelErdPointerAccess_GetTimerModule(
         dataModel,
         instance->_private.config->timerModuleErd),
      &instance->_private.timer,
      instance->_private.gridData->gridPeriodicRunRateInMSec,
      UpdateGridBlockAndGridLines,
      instance);
}
