/*!
 * @file
 * @brief Application Glue Code
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "IceCabinetGridPlugin.h"
#include "SystemErds.h"
#include "Constants_Time.h"
#include "Grid_IceCabinet.h"
#include "uassert.h"
#include "Constants_Binary.h"
#include "ParametricData.h"
#include "DataModelErdPointerAccess.h"

static const GridConfiguration_t gridConfig = {
   .timerModuleErd = Erd_TimerModule,
   .gridOverrideSignalErd = Erd_IceCabinetGridOverrideSignal,
   .gridOverrideEnableErd = Erd_IceCabinetGridOverrideEnable,
};

static const GridLineCalculatorConfiguration_t gridLineCalculatorConfig = {
   .calculatedGridLineErd = Erd_IceCabinetGrid_CalculatedGridLines,
   .crossAmbientHysteresisAdjustmentErd = Erd_U8Zero,
   .gridDeltaOffsetErd = Erd_U8Zero,
   .gridLineAdjustmentErds = {
      {
         // This grid does not use the "offset" correction type, so this should never be accessed
         .offsetInDegFx100Erd = Erd_U8Zero,
         .adjustedSetpointInDegFx100Erd = Erd_IceCabinet_AdjustedSetpointInDegFx100,
      },
   }
};

static const GridBlockCalculatorConfiguration_t gridBlockCalculatorConfig = {
   .calculatedGridLinesErd = Erd_IceCabinetGrid_CalculatedGridLines,
   .currentGridBlockNumberErd = Erd_IceCabinetGrid_BlockNumber,
   .previousGridBlockNumbersErd = Erd_IceCabinetGrid_PreviousBlocks,
   .gridBlockAdjustmentErds = {
      {
         .filteredResolvedTemperatureInDegFx100 = Erd_IceCabinet_FilteredTemperatureResolvedInDegFx100,
         .thermistorIsValidResolvedErd = Erd_IceCabinetThermistor_IsValidResolved,
      },
   }
};

static const Erd_t gridBlockNumberOverrideRequestErdList[] = {
   Erd_IceCabinetGridBlockNumberOverrideRequest
};

static const Erd_t gridBlockNumberOverrideValueErdList[] = {
   Erd_IceCabinetGrid_BlockNumber,
   Erd_IceCabinetGridBlockNumberOverrideValue
};

static const OverrideArbiterConfiguration_t gridBlockNumberOverrideConfiguration = {
   gridBlockNumberOverrideRequestErdList,
   gridBlockNumberOverrideValueErdList,
   Erd_IceCabinetGridBlockNumberOverrideResolved,
   NUM_ELEMENTS(gridBlockNumberOverrideRequestErdList)
};

void IceCabinetGridPlugin_Init(IceCabinetGridPlugin_t *instance, I_DataModel_t *dataModel)
{
   OverrideArbiter_Init(
      &instance->gridBlockNumberArbiter,
      DataModel_AsDataSource(dataModel),
      &gridBlockNumberOverrideConfiguration);

   GridLineCalculator_Init(
      &instance->gridLineCalculator,
      &gridLineCalculatorConfig,
      PersonalityParametricData_Get(dataModel)->iceCabinetDeltaGridLines,
      dataModel);

   GridBlockCalculator_Init(
      &instance->gridBlockCalculator,
      &gridBlockCalculatorConfig,
      dataModel,
      PersonalityParametricData_Get(dataModel)->iceCabinetGridData,
      PersonalityParametricData_Get(dataModel)->iceCabinetDeltaGridLines);

   Grid_Init(
      &instance->grid,
      dataModel,
      &gridConfig,
      PersonalityParametricData_Get(dataModel)->iceCabinetGridData,
      Grid_IceCabinet);
}
