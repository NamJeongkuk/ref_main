/*!
 * @file
 * @brief Application Glue Code
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "GridPlugin.h"
#include "SystemErds.h"
#include "Constants_Time.h"
#include "Grid_ThreeDoor.h"
#include "Grid_QuadDoor.h"
#include "Grid_FourDoor.h"

enum
{
   GridRunCallbackRate = 1000,
   GridId = 1,
   FivePreviousBlocks = 5,
   Zero = 0,
   SixGridLines = 6,
   TwoDimensional = 2
};

static const GridFunction_t grids[] = { Grid_ThreeDoor, Grid_FourDoor, Grid_QuadDoor };

static const GridFunctionArray_t gridFunctionArray = {
   grids,
   NUM_ELEMENTS(grids)
};

static const GridConfiguration_t gridConfig = {
   .timerModuleErd = Erd_TimerModule,
   .personalityParametricDataErd = Erd_PersonalityParametricData,
   .periodicGridLineCalcRate = GridRunCallbackRate, // get this from parametric
   .gridFunctions = &gridFunctionArray,
   .gridId = GridId, // get this from parametric
};

static const GridLineErds_t ffErds = {
   .rawSetpointErd = Erd_FfSetpoint_ResolvedVote,
   .offsetErd = Erd_FreshFood_Offset,
   .shiftErd = Erd_FreshFood_Shift,
   .adjustedSetpointErd = Erd_FreshFood_AdjustedSetpoint
};

static const GridLineErds_t fzErds = {
   .rawSetpointErd = Erd_FzSetpoint_ResolvedVote,
   .offsetErd = Erd_Freezer_Offset,
   .shiftErd = Erd_Freezer_Shift,
   .adjustedSetpointErd = Erd_Freezer_AdjustedSetpoint
};

static const TemperatureDegFx100_t ffAxisGridLines[] = { 0, -450, 150, 450, 950, 1150 }; // from parametric
static const DeltaGridLines_BitMapping_t ffBitMap[] = { 0b0010, 0b1000, 0b1000, 0b1000, 0b1000, 0b1000 }; // from parametric

static const TemperatureDegFx100_t fzAxisGridLines[] = { -250, 0, 250, 600, 750, 5500 }; // from parametric
static const DeltaGridLines_BitMapping_t fzBitMap[] = { 0b1000, 0b1000, 0b1000, 0b1000, 0b1000, 0b0010 }; // from parametric

static const DeltaAxisGridLines_t ffAxis = {
   .numberOfLines = SixGridLines,
   .gridLinesDegFx100 = ffAxisGridLines,
   .bitMappings = ffBitMap
};

static const DeltaAxisGridLines_t fzAxis = {
   .numberOfLines = SixGridLines,
   .gridLinesDegFx100 = fzAxisGridLines,
   .bitMappings = fzBitMap
};

static const DeltaAxisGridLines_t parametricGrid[] = { ffAxis, fzAxis };
static const DeltaGridLines_t deltaGrid = {
   .dimensions = TwoDimensional,
   .gridLines = parametricGrid
};

static TemperatureDegFx100_t ffCalcAxisGridLines[SixGridLines] = { 0 };
static TemperatureDegFx100_t fzCalcAxisGridLines[SixGridLines] = { 0 };

static const CalculatedAxisGridLines_t ffCalcAxis = {
   .numberOfLines = SixGridLines,
   .gridLinesDegFx100 = ffCalcAxisGridLines
};

static const CalculatedAxisGridLines_t fzCalcAxis = {
   .numberOfLines = SixGridLines,
   .gridLinesDegFx100 = fzCalcAxisGridLines
};

static CalculatedAxisGridLines_t calcGrid[] = { ffCalcAxis, fzCalcAxis };

static CalculatedGridLines_t calcGridLines = {
   .dimensions = TwoDimensional,
   .gridLines = calcGrid
};

static const GridBlockAndLinesConfig_t calcConfig = {
   .calculatedGridBlockErd = Erd_Grid_BlockNumber,
   .previousGridBlocksErd = Erd_Grid_PreviousBlocks,
   .calculatedGridLinesErd = Erd_Grid_CalculatedGridLines,
   .ffFilteredTempErd = Erd_FreshFood_FilteredTemperature,
   .fzFilteredTempErd = Erd_Freezer_FilteredTemperature,
   .timerModuleErd = Erd_TimerModule,
   .fzErds = fzErds,
   .ffErds = ffErds,
   .parametricGridLines = deltaGrid,
   .gridBlockCalcRate = MSEC_PER_SEC // from parametric
};

static GridBlockNumber_t ringBufferNumbers[FivePreviousBlocks] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

static PreviousGridBlockNumbers_t prevGridBlocks = {
   .maxSize = FivePreviousBlocks,
   .currentNumberOfBlocksStored = Zero
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
