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
#include "Constants_Binary.h"

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

static const GridLineAdjustmentErds_t freshFoodGridLineAdjustmentErds = {
   .rawSetpointErd = Erd_FreshFoodSetpoint_ResolvedVote,
   .offsetErd = Erd_FreshFood_Offset,
   .shiftErd = Erd_FreshFood_Shift,
   .adjustedSetpointErd = Erd_FreshFood_AdjustedSetpointInDegFx100
};

static const GridLineAdjustmentErds_t freezerGridLineAdjustmentErds = {
   .rawSetpointErd = Erd_FreezerSetpoint_ResolvedVote,
   .offsetErd = Erd_Freezer_Offset,
   .shiftErd = Erd_Freezer_Shift,
   .adjustedSetpointErd = Erd_Freezer_AdjustedSetpointInDegFx100,
};

static const GridLineCalculatorConfiguration_t gridLineCalculatorConfig = {
   .calculatedGridLineErd = Erd_Grid_CalculatedGridLines,
   .freshFoodFilteredTempErd = Erd_FreshFood_FilteredTemperatureResolved,
   .freezerFilteredTemperatureErd = Erd_Freezer_FilteredTemperatureResolved,
   .gridLineAdjustmentErds = {
      freshFoodGridLineAdjustmentErds,
      freezerGridLineAdjustmentErds,
   }
};

static const GridBlockCalculatorConfiguration_t gridBlockCalculatorConfig = {
   .freshFoodFilteredResolvedTemperature = Erd_FreshFood_FilteredTemperatureResolved,
   .freezerFilteredResolvedTemperature = Erd_Freezer_FilteredTemperatureResolved,
   .currentGridBlockNumberErd = Erd_Grid_BlockNumber,
   .calculatedGridLinesErd = Erd_Grid_CalculatedGridLines,
   .previousGridBlockNumbersErd = Erd_Grid_PreviousBlocks
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

   bool setpointResolverReady;
   DataModel_Read(
      dataModel,
      Erd_SetpointResolverReady,
      &setpointResolverReady);

   bool overrideArbiterReady;
   DataModel_Read(
      dataModel,
      Erd_OverrideArbiterReady,
      &overrideArbiterReady);

   uassert(sensorsReadyToBeRead && setpointResolverReady & overrideArbiterReady);

   GridLineCalculator_Init(
      &instance->gridLineCalculator,
      &gridLineCalculatorConfig,
      dataModel);

   GridBlockCalculator_Init(
      &instance->gridBlockCalculator,
      &gridBlockCalculatorConfig,
      dataModel);

   Grid_Init(
      &instance->gridInstance,
      &gridConfig,
      dataModel);

   DataModel_Write(
      dataModel,
      Erd_GridPluginReady,
      set);
}
