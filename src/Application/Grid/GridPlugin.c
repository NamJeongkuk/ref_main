/*!
 * @file
 * @brief Application Glue Code
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "GridPlugin.h"
#include "SystemErds.h"
#include "Constants_Time.h"
#include "Grid_MultiDoor.h"
#include "Grid_TripleEvap.h"
#include "Grid_DualEvap.h"

enum
{
   CabinetGridLineCount = 6
};

static const GridFunction_t grids[] = { Grid_MultiDoor, Grid_DualEvap, Grid_TripleEvap };

static const GridFunctionArray_t gridFunctionArray = {
   grids,
   NUM_ELEMENTS(grids)
};

static const GridConfiguration_t gridConfig = {
   .timerModuleErd = Erd_TimerModule,
   .gridFunctions = &gridFunctionArray,
};

static const GridLineErds_t freshFoodErds = {
   .rawSetpointErd = Erd_FreshFoodSetpoint_ResolvedVote,
   .offsetErd = Erd_FreshFood_Offset,
   .shiftErd = Erd_FreshFood_Shift,
   .adjustedSetpointErd = Erd_FreshFood_AdjustedSetpoint
};

static const GridLineErds_t freezerErds = {
   .rawSetpointErd = Erd_FreezerSetpoint_ResolvedVote,
   .offsetErd = Erd_Freezer_Offset,
   .shiftErd = Erd_Freezer_Shift,
   .adjustedSetpointErd = Erd_Freezer_AdjustedSetpoint
};

static TemperatureDegFx100_t freshFoodCalcAxisGridLines[CabinetGridLineCount] = { 0 };
static TemperatureDegFx100_t freezerCalcAxisGridLines[CabinetGridLineCount] = { 0 };

static const CalculatedAxisGridLines_t freshFoodCalcAxis = {
   .numberOfLines = ELEMENT_COUNT(freshFoodCalcAxisGridLines),
   .gridLinesDegFx100 = freshFoodCalcAxisGridLines
};

static const CalculatedAxisGridLines_t freezerCalcAxis = {
   .numberOfLines = ELEMENT_COUNT(freezerCalcAxisGridLines),
   .gridLinesDegFx100 = freezerCalcAxisGridLines
};

static CalculatedAxisGridLines_t calcGrid[] = { freshFoodCalcAxis, freezerCalcAxis };

static CalculatedGridLines_t calcGridLines = {
   .dimensions = ELEMENT_COUNT(calcGrid),
   .gridLines = calcGrid
};

static const GridBlockAndLinesConfig_t calcConfig = {
   .calculatedGridBlockErd = Erd_Grid_BlockNumber,
   .previousGridBlocksErd = Erd_Grid_PreviousBlocks,
   .calculatedGridLinesErd = Erd_Grid_CalculatedGridLines,
   .freshFoodFilteredTempErd = Erd_FreshFood_FilteredTemperatureResolved,
   .freezerFilteredTempErd = Erd_Freezer_FilteredTemperatureResolved,
   .timerModuleErd = Erd_TimerModule,
   .freezerErds = freezerErds,
   .freshFoodErds = freshFoodErds
};

static GridBlockNumber_t ringBufferNumbers[PreviousGridBlockNumbers_MaxBlockNumbers] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

static PreviousGridBlockNumbers_t prevGridBlocks = {
   .maxSize = PreviousGridBlockNumbers_MaxBlockNumbers,
   .currentNumberOfBlocksStored = 0
};

void GridPlugin_Init(
   GridPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   RingBuffer_Init(
      &instance->bufferInstance,
      ringBufferNumbers,
      prevGridBlocks.maxSize,
      ELEMENT_SIZE(ringBufferNumbers));

   CalcGridBlockAndGridLines_Init(
      &instance->calcGridBlockAndGridLinesInstance,
      &calcConfig,
      dataModel,
      &calcGridLines,
      &instance->bufferInstance,
      &prevGridBlocks);

   Grid_Init(
      &instance->gridInstance,
      &gridConfig,
      dataModel);
}
