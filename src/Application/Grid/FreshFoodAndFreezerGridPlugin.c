/*!
 * @file
 * @brief Application Glue Code
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "FreshFoodAndFreezerGridPlugin.h"
#include "SystemErds.h"
#include "Constants_Time.h"
#include "Grid_SingleEvap.h"
#include "Grid_DualEvap.h"
#include "Grid_TripleEvap.h"
#include "Grid_SingleDoorSingleEvap.h"
#include "uassert.h"
#include "Constants_Binary.h"
#include "ParametricData.h"

typedef struct
{
   GridFunction_t singleEvap;
   GridFunction_t dualEvap;
   GridFunction_t tripleEvap;
   GridFunction_t singleDoorSingleEvap;
} GridFunctions_t;

STATIC_ASSERT(OFFSET_OF(GridFunctions_t, singleEvap) / sizeof(GridFunction_t) == GridId_SingleEvap);
STATIC_ASSERT(OFFSET_OF(GridFunctions_t, dualEvap) / sizeof(GridFunction_t) == GridId_DualEvap);
STATIC_ASSERT(OFFSET_OF(GridFunctions_t, tripleEvap) / sizeof(GridFunction_t) == GridId_TripleEvap);
STATIC_ASSERT(OFFSET_OF(GridFunctions_t, singleDoorSingleEvap) / sizeof(GridFunction_t) == GridId_SingleDoorSingleEvap);

static const GridFunctions_t gridFunctions = {
   .singleEvap = Grid_SingleEvap,
   .dualEvap = Grid_DualEvap,
   .tripleEvap = Grid_TripleEvap,
   .singleDoorSingleEvap = Grid_SingleDoorSingleEvap
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

static const Erd_t freshFoodErdAdderList[] = {
   Erd_FreshFood_CabinetOffsetInDegFx100,
   Erd_FreshFood_CrossAmbientOffsetInDegFx100
};

static const Erd_t freezerErdAdderList[] = {
   Erd_Freezer_CabinetOffsetInDegFx100,
   Erd_Freezer_CrossAmbientOffsetInDegFx100
};

static const I16ErdAdderConfiguration_t freshFoodErdAdderI16Config = {
   .resultErd = Erd_FreshFood_CabinetPlusCrossAmbientOffsetInDegFx100,
   .i16ErdsToBeAdded = { freshFoodErdAdderList, NUM_ELEMENTS(freshFoodErdAdderList) }
};

static const I16ErdAdderConfiguration_t freezerErdAdderI16Config = {
   .resultErd = Erd_Freezer_CabinetPlusCrossAmbientOffsetInDegFx100,
   .i16ErdsToBeAdded = { freezerErdAdderList, NUM_ELEMENTS(freezerErdAdderList) }
};

static const GridOffsetAdderErdConfiguration_t freshFoodGridOffsetAdderConfig = {
   .I16ErdAdderConfig = &freshFoodErdAdderI16Config
};

static const GridOffsetAdderErdConfiguration_t freezerGridOffsetAdderConfig = {
   .I16ErdAdderConfig = &freezerErdAdderI16Config
};

static const GridLineCalculatorConfiguration_t gridLineCalculatorConfig = {
   .calculatedGridLineErd = Erd_FreshFoodAndFreezerGrid_CalculatedGridLines,
   .crossAmbientHysteresisAdjustmentErd = Erd_CrossAmbientHysteresisAdjustmentInDegFx100,
   .gridLineAdjustmentErds = {
      {
         .offsetInDegFx100Erd = Erd_FreshFood_CabinetPlusCrossAmbientOffsetInDegFx100,
         .adjustedSetpointInDegFx100Erd = Erd_FreshFood_AdjustedSetpointInDegFx100,
      },
      {
         .offsetInDegFx100Erd = Erd_Freezer_CabinetPlusCrossAmbientOffsetInDegFx100,
         .adjustedSetpointInDegFx100Erd = Erd_Freezer_AdjustedSetpointInDegFx100,
      },
   }
};

static const GridBlockCalculatorConfiguration_t gridBlockCalculatorConfig = {
   .freshFoodFilteredResolvedTemperatureInDegFx100 = Erd_FreshFood_FilteredTemperatureResolvedInDegFx100,
   .freezerFilteredResolvedTemperatureInDegFx100 = Erd_Freezer_FilteredTemperatureResolvedInDegFx100,
   .currentGridBlockNumberErd = Erd_FreshFoodAndFreezerGrid_BlockNumber,
   .calculatedGridLinesErd = Erd_FreshFoodAndFreezerGrid_CalculatedGridLines,
   .previousGridBlockNumbersErd = Erd_FreshFoodAndFreezerGrid_PreviousBlocks,
   .freezerThermistorIsValidResolvedErd = Erd_FreezerThermistor_IsValidResolved,
   .freshFoodThermistorIsValidResolvedErd = Erd_FreshFoodThermistor_IsValidResolved
};

static const Erd_t gridBlockNumberOverrideRequestErdList[] = {
   Erd_GridBlockNumberOverrideRequest
};

static const Erd_t gridBlockNumberOverrideValueErdList[] = {
   Erd_FreshFoodAndFreezerGrid_BlockNumber,
   Erd_GridBlockNumberOverrideValue
};

static const OverrideArbiterConfiguration_t gridBlockNumberOverrideConfiguration = {
   gridBlockNumberOverrideRequestErdList,
   gridBlockNumberOverrideValueErdList,
   Erd_GridBlockNumberOverrideResolved,
   NUM_ELEMENTS(gridBlockNumberOverrideRequestErdList)
};

static const CrossAmbientHysteresisAdjustmentCalculatorConfig_t crossAmbientHysteresisAdjustmentCalculatorConfig = {
   .crossAmbientHysteresisAdjustmentErd = Erd_CrossAmbientHysteresisAdjustmentInDegFx100,
   .crossAmbientWindowAveragedTemperatureErd = Erd_Ambient_WindowAveragedTemperatureInDegFx100,
};

void FreshFoodAndFreezerGridPlugin_Init(
   FreshFoodAndFreezerGridPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   OverrideArbiter_Init(
      &instance->gridBlockNumberArbiter,
      DataModel_AsDataSource(dataModel),
      &gridBlockNumberOverrideConfiguration);

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

   bool ambientTemperaturePluginReady;
   DataModel_Read(
      dataModel,
      Erd_AmbientTemperaturePluginReady,
      &ambientTemperaturePluginReady);

   uassert(sensorsReadyToBeRead && setpointResolverReady && ambientTemperaturePluginReady);

   DataModel_Write(
      dataModel,
      Erd_CompressorTripMitigationActive,
      &PersonalityParametricData_Get(dataModel)->pulldownData->compressorTripMitigationEnable);

   DataModel_Write(
      dataModel,
      Erd_PulldownInMediumCompressorSpeedEnabled,
      &PersonalityParametricData_Get(dataModel)->pulldownData->pulldownInMediumCompressorSpeed);

   GridOffsetAdder_Init(
      &instance->freshFoodGridOffsetAdder,
      dataModel,
      &freshFoodGridOffsetAdderConfig);

   GridOffsetAdder_Init(
      &instance->freezerGridOffsetAdder,
      dataModel,
      &freezerGridOffsetAdderConfig);

   CrossAmbientHysteresisAdjustmentCalculator_Init(
      &instance->crossAmbientHysteresisAdjustmentCalculator,
      dataModel,
      PersonalityParametricData_Get(dataModel)->freshFoodAndFreezerGridData,
      &crossAmbientHysteresisAdjustmentCalculatorConfig);

   GridLineCalculator_Init(
      &instance->gridLineCalculator,
      &gridLineCalculatorConfig,
      PersonalityParametricData_Get(dataModel)->freshFoodAndFreezerGridData,
      dataModel);

   GridBlockCalculator_Init(
      &instance->gridBlockCalculator,
      &gridBlockCalculatorConfig,
      dataModel,
      PersonalityParametricData_Get(dataModel)->freshFoodAndFreezerGridData);

   Grid_Init(
      &instance->gridInstance,
      &gridConfig,
      dataModel);

   DataModel_Write(
      dataModel,
      Erd_FreshFoodAndFreezerGridPluginReady,
      set);
}