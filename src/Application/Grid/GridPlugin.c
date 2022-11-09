/*!
 * @file
 * @brief Application Glue Code
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "GridPlugin.h"
#include "SystemErds.h"
#include "Constants_Time.h"
#include "Grid_SingleEvap.h"
#include "Grid_DualEvap.h"
#include "Grid_TripleEvap.h"
#include "uassert.h"
#include "Constants_Binary.h"
#include "ParametricData.h"

typedef struct
{
   GridFunction_t singleEvap;
   GridFunction_t dualEvap;
   GridFunction_t tripleEvap;
} GridFunctions_t;

STATIC_ASSERT(OFFSET_OF(GridFunctions_t, singleEvap) / sizeof(GridFunction_t) == GridId_SingleEvap);
STATIC_ASSERT(OFFSET_OF(GridFunctions_t, dualEvap) / sizeof(GridFunction_t) == GridId_DualEvap);
STATIC_ASSERT(OFFSET_OF(GridFunctions_t, tripleEvap) / sizeof(GridFunction_t) == GridId_TripleEvap);

static const GridFunctions_t gridFunctions = {
   .singleEvap = Grid_SingleEvap,
   .dualEvap = Grid_DualEvap,
   .tripleEvap = Grid_TripleEvap
};

static const GridFunctionArray_t gridFunctionArray = {
   (const GridFunction_t *)&gridFunctions,
   sizeof(gridFunctions) / sizeof(GridFunction_t)
};

static const GridConfiguration_t gridConfig = {
   .timerModuleErd = Erd_TimerModule,
   .gridOverrideSignalErd = Erd_GridOverrideSignal,
   .gridOverrideEnableErd = Erd_GridOverrideEnable,
   .gridFunctions = &gridFunctionArray,
};

static const GridLineAdjustmentErds_t freshFoodGridLineAdjustmentErds = {
   .rawSetpointErd = Erd_FreshFoodSetpoint_ResolvedVote,
   .cabinetOffsetInDegFx100Erd = Erd_FreshFood_CabinetOffsetInDegFx100,
   .thermalShiftInDegFx100Erd = Erd_FreshFood_ThermalShiftInDegFx100,
   .adjustedSetpointInDegFx100Erd = Erd_FreshFood_AdjustedSetpointInDegFx100
};

static const GridLineAdjustmentErds_t freezerGridLineAdjustmentErds = {
   .rawSetpointErd = Erd_FreezerSetpoint_ResolvedVote,
   .cabinetOffsetInDegFx100Erd = Erd_Freezer_CabinetOffsetInDegFx100,
   .thermalShiftInDegFx100Erd = Erd_Freezer_ThermalShiftInDegFx100,
   .adjustedSetpointInDegFx100Erd = Erd_Freezer_AdjustedSetpointInDegFx100,
};

static const GridLineCalculatorConfiguration_t gridLineCalculatorConfig = {
   .calculatedGridLineErd = Erd_Grid_CalculatedGridLines,
   .freshFoodFilteredTemperatureInDegFx100Erd = Erd_FreshFood_FilteredTemperatureResolvedInDegFx100,
   .freezerFilteredTemperatureInDegFx100Erd = Erd_Freezer_FilteredTemperatureResolvedInDegFx100,
   .gridLineAdjustmentErds = {
      freshFoodGridLineAdjustmentErds,
      freezerGridLineAdjustmentErds,
   }
};

static const GridBlockCalculatorConfiguration_t gridBlockCalculatorConfig = {
   .freshFoodFilteredResolvedTemperatureInDegFx100 = Erd_FreshFood_FilteredTemperatureResolvedInDegFx100,
   .freezerFilteredResolvedTemperatureInDegFx100 = Erd_Freezer_FilteredTemperatureResolvedInDegFx100,
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

   DataModel_Write(
      dataModel,
      Erd_CompressorTripMitigationActive,
      &PersonalityParametricData_Get(dataModel)->pulldownData->compressorTripMitigationEnable);

   DataModel_Write(
      dataModel,
      Erd_PulldownInMediumCompressorSpeedEnabled,
      &PersonalityParametricData_Get(dataModel)->pulldownData->pulldownInMediumCompressorSpeed);

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
