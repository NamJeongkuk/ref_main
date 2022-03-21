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
   Inverted = true
};

static void CalculateAxisGridLines(
   CalculatedAxisGridLines_t *calcAxisGridlines,
   const DeltaAxisGridLines_t *parametricAxisGridLines,
   I_DataModel_t *dataModel,
   GridLineErds_t erds)
{
   for(int i = 0; i < parametricAxisGridLines->numberOfLines; i++)
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
      &instance->_private.calcGridLines->gridLines[GridDelta_Ff],
      &instance->_private.gridData->deltaGridLines->gridLines[GridDelta_Ff],
      instance->_private.dataModel,
      instance->_private.config->ffErds);
   CalculateAxisGridLines(
      &instance->_private.calcGridLines->gridLines[GridDelta_Fz],
      &instance->_private.gridData->deltaGridLines->gridLines[GridDelta_Fz],
      instance->_private.dataModel,
      instance->_private.config->fzErds);

   DataModel_Write(instance->_private.dataModel, instance->_private.config->calculatedGridLinesErd, instance->_private.calcGridLines);
}

static uint8_t GetLocationIndex(TemperatureDegFx100_t temperature, uint8_t compartmentType, CalculatedGridLines_t *gridLines, bool inverted)
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

static GridBlockNumber_t CalculateGridBlock(uint8_t rowIndex, uint8_t colIndex, uint8_t numCols)
{
   return rowIndex * numCols + colIndex % numCols;
}

static void UpdateGridBlock(CalcGridBlockAndLines_t *instance)
{
   TemperatureDegFx100_t ffFilteredTemp;
   DataModel_Read(instance->_private.dataModel, instance->_private.config->ffFilteredTempErd, &ffFilteredTemp);

   TemperatureDegFx100_t fzFilteredTemp;
   DataModel_Read(instance->_private.dataModel, instance->_private.config->fzFilteredTempErd, &fzFilteredTemp);

   uint8_t colIndex = GetLocationIndex(ffFilteredTemp, GridDelta_Ff, instance->_private.calcGridLines, NotInverted);
   uint8_t rowIndex = GetLocationIndex(fzFilteredTemp, GridDelta_Fz, instance->_private.calcGridLines, Inverted);

   GridBlockNumber_t newBlockNumber = CalculateGridBlock(rowIndex, colIndex, instance->_private.gridData->deltaGridLines->gridLines[GridDelta_Ff].numberOfLines + 1);

   DataModel_Write(instance->_private.dataModel, instance->_private.config->calculatedGridBlockErd, &newBlockNumber);
}

static void UpdatePreviousGridBlocks(void *context, const void *_args)
{
   REINTERPRET(instance, context, CalcGridBlockAndLines_t *);
   REINTERPRET(blockNumber, _args, const GridBlockNumber_t *);

   if(instance->_private.lastBlock != GridBlockNumber_Max)
   {
      RingBuffer_Add(instance->_private.bufferInstance, &instance->_private.lastBlock);
      instance->_private.previousGridBlocks->currentNumberOfBlocksStored = RingBuffer_Count(instance->_private.bufferInstance);

      for(uint8_t i = 0; i < instance->_private.previousGridBlocks->currentNumberOfBlocksStored; i++)
      {
         GridBlockNumber_t number;
         RingBuffer_At(instance->_private.bufferInstance, &number, i);
         instance->_private.previousGridBlocks->blockNumbers[i] = number;
      }

      DataModel_Write(instance->_private.dataModel, instance->_private.config->previousGridBlocksErd, instance->_private.previousGridBlocks);
   }

   instance->_private.lastBlock = *blockNumber;
}

static void UpdateGridBlockAndGridLines(void *context)
{
   REINTERPRET(instance, context, CalcGridBlockAndLines_t *);

   UpdateGridLines(instance);
   UpdateGridBlock(instance);
}

static void ClearPreviousGridBlocks(PreviousGridBlockNumbers_t *prevBlocks)
{
   prevBlocks->currentNumberOfBlocksStored = 0;

   for(int i = 0; i < prevBlocks->maxSize; i++)
   {
      prevBlocks->blockNumbers[i] = 0xFF;
   }
}

void CalcGridBlockAndGridLines_Init(
   CalcGridBlockAndLines_t *instance,
   const GridBlockAndLinesConfig_t *config,
   I_DataModel_t *dataModel,
   CalculatedGridLines_t *calcGridLines,
   RingBuffer_t *bufferInstance,
   PreviousGridBlockNumbers_t *prevGridBlocks)
{
   instance->_private.config = config;
   instance->_private.dataModel = dataModel;
   instance->_private.calcGridLines = calcGridLines;
   instance->_private.bufferInstance = bufferInstance;
   instance->_private.previousGridBlocks = prevGridBlocks;
   instance->_private.lastBlock = GridBlockNumber_Max;
   instance->_private.gridData = PersonalityParametricData_Get(dataModel)->gridData;

   RingBuffer_Clear(instance->_private.bufferInstance);
   ClearPreviousGridBlocks(instance->_private.previousGridBlocks);

   DataModel_Write(instance->_private.dataModel, instance->_private.config->calculatedGridBlockErd, &instance->_private.lastBlock);
   DataModel_Write(instance->_private.dataModel, instance->_private.config->previousGridBlocksErd, instance->_private.previousGridBlocks);
   DataModel_Write(instance->_private.dataModel, instance->_private.config->calculatedGridLinesErd, instance->_private.calcGridLines);

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
