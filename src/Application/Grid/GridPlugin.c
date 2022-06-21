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
#include "uassert.h"

static const GridFunction_t grids[] = {
   Grid_MultiDoor,
   Grid_DualEvap,
   Grid_TripleEvap
};

static const GridFunctionArray_t gridFunctionArray = {
   grids,
   NUM_ELEMENTS(grids)
};

static const GridConfiguration_t gridConfig = {
   .timerModuleErd = Erd_TimerModule,
   .gridFunctions = &gridFunctionArray,
};

static const GridLineErds_t freshFoodGridLineErds = {
   .rawSetpointErd = Erd_FreshFoodSetpoint_ResolvedVote,
   .offsetErd = Erd_FreshFood_Offset,
   .shiftErd = Erd_FreshFood_Shift,
   .adjustedSetpointErd = Erd_FreshFood_AdjustedSetpoint
};

static const GridLineErds_t freezerGridLineErds = {
   .rawSetpointErd = Erd_FreezerSetpoint_ResolvedVote,
   .offsetErd = Erd_Freezer_Offset,
   .shiftErd = Erd_Freezer_Shift,
   .adjustedSetpointErd = Erd_Freezer_AdjustedSetpoint
};

static const GridBlockAndLinesConfig_t calcConfig = {
   .calculatedGridBlockErd = Erd_Grid_BlockNumber,
   .previousGridBlocksErd = Erd_Grid_PreviousBlocks,
   .calculatedGridLinesErd = Erd_Grid_CalculatedGridLines,
   .freshFoodFilteredTempErd = Erd_FreshFood_FilteredTemperatureResolved,
   .freezerFilteredTempErd = Erd_Freezer_FilteredTemperatureResolved,
   .timerModuleErd = Erd_TimerModule,
   .freezerGridLineErds = freezerGridLineErds,
   .freshFoodGridLineErds = freshFoodGridLineErds
};

void GridPlugin_Init(
   GridPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   bool sensorsReadyToBeRead;
   DataModel_Read(
      dataModel,
      Erd_SensorsReadyToBeRead,
      &sensorsReadyToBeRead);

   uassert(sensorsReadyToBeRead);

   CalcGridBlockAndGridLines_Init(
      &instance->calcGridBlockAndGridLinesInstance,
      &calcConfig,
      dataModel);

   Grid_Init(
      &instance->gridInstance,
      &gridConfig,
      dataModel);
}
