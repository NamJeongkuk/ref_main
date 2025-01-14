/*!
 * @file
 * @brief Application Glue Code
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "FeaturePanGridPlugin.h"
#include "SystemErds.h"
#include "Constants_Time.h"
#include "Grid_FeaturePan.h"
#include "uassert.h"
#include "Constants_Binary.h"
#include "ParametricData.h"
#include "DataModelErdPointerAccess.h"

static const GridConfiguration_t gridConfig = {
   .timerModuleErd = Erd_TimerModule,
   .gridOverrideSignalErd = Erd_FeaturePanGridOverrideSignal,
   .gridOverrideEnableErd = Erd_FeaturePanGridOverrideEnable,
};

static const GridLineCalculatorConfiguration_t gridLineCalculatorConfig = {
   .calculatedGridLineErd = Erd_FeaturePanGrid_CalculatedGridLines,
   .crossAmbientHysteresisAdjustmentErd = Erd_CrossAmbientHysteresisAdjustmentInDegFx100,
   .gridDeltaOffsetErd = Erd_FeaturePanCurrentModeStatus,
   .gridLineAdjustmentErds = {
      {
         .offsetInDegFx100Erd = Erd_FeaturePan_CrossAmbientOffsetInDegFx100,
         .adjustedSetpointInDegFx100Erd = Erd_FeaturePan_AdjustedSetpointInDegFx100,
      },
   }
};

static const GridBlockCalculatorConfiguration_t gridBlockCalculatorConfig = {
   .calculatedGridLinesErd = Erd_FeaturePanGrid_CalculatedGridLines,
   .currentGridBlockNumberErd = Erd_FeaturePanGrid_BlockNumber,
   .previousGridBlockNumbersErd = Erd_FeaturePanGrid_PreviousBlocks,
   .gridBlockAdjustmentErds = {
      {
         .filteredResolvedTemperatureInDegFx100 = Erd_FeaturePan_FilteredTemperatureResolvedInDegFx100,
         .thermistorIsValidResolvedErd = Erd_FeaturePanCabinetThermistor_IsValidResolved,
      },
   }
};

static const Erd_t gridBlockNumberOverrideRequestErdList[] = {
   Erd_FeaturePanGridBlockNumberOverrideRequest
};

static const Erd_t gridBlockNumberOverrideValueErdList[] = {
   Erd_FeaturePanGrid_BlockNumber,
   Erd_FeaturePanGridBlockNumberOverrideValue
};

static const OverrideArbiterConfiguration_t gridBlockNumberOverrideConfiguration = {
   gridBlockNumberOverrideRequestErdList,
   gridBlockNumberOverrideValueErdList,
   Erd_FeaturePanGridBlockNumberOverrideResolved,
   NUM_ELEMENTS(gridBlockNumberOverrideRequestErdList)
};

void FeaturePanGridPlugin_Init(FeaturePanGridPlugin_t *instance, I_DataModel_t *dataModel)
{
   I_ConstArrayMap_t *constArrayMapInterface = ConstArrayMap_FeaturePan_Init(&instance->featurePanCoolingStateBasedGridVotesTable);
   DataModelErdPointerAccess_Write(dataModel, Erd_FeaturePanCoolingStatesGridVotesConstArrayMapInterface, constArrayMapInterface);

   OverrideArbiter_Init(
      &instance->gridBlockNumberArbiter,
      DataModel_AsDataSource(dataModel),
      &gridBlockNumberOverrideConfiguration);

   GridLineCalculator_Init(
      &instance->gridLineCalculator,
      &gridLineCalculatorConfig,
      PersonalityParametricData_Get(dataModel)->featurePanDeltaGridLines,
      dataModel);

   GridBlockCalculator_Init(
      &instance->gridBlockCalculator,
      &gridBlockCalculatorConfig,
      dataModel,
      PersonalityParametricData_Get(dataModel)->featurePanGridData,
      PersonalityParametricData_Get(dataModel)->featurePanDeltaGridLines);

   Grid_Init(
      &instance->grid,
      dataModel,
      &gridConfig,
      PersonalityParametricData_Get(dataModel)->featurePanGridData,
      Grid_FeaturePan);
}
