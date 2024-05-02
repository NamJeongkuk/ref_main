/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "FeaturePanShiftOffsetCalculatorPlugin.h"
#include "DataModelErdPointerAccess.h"
#include "SystemErds.h"

static const ShiftOffsetCalculatorConfig_t featurePanShiftOffsetCalculatorConfig = {
   .filteredTemperatureInDegFx100Erd = Erd_FeaturePan_FilteredTemperatureResolvedInDegFx100,
   .shiftOffsetErd = Erd_FeaturePan_ThermalShiftInDegFx100,
   .adjustedSetpointWithoutShiftErd = Erd_FeaturePan_AdjustedSetpointWithoutShiftInDegFx100,
   .postDwellCompletionSignalErd = Erd_PostDwellCompletionSignal,
   .resetThermalShiftOffsetSignalErd = Erd_FeaturePan_ResetThermalShiftOffsetSignal,
   .longTermAverageErd = Erd_FeaturePan_LongTermAverageInDegFx100
};

void FeaturePanShiftOffsetCalculatorPlugin_Init(
   FeaturePanShiftOffsetCalculatorPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   const PersonalityParametricData_t *parametricData =
      PersonalityParametricData_Get(dataModel);

   Filter_LongTermAverage_Init(
      &instance->_private.featurePanLongTermAverageFilter,
      parametricData->shiftOffsetCalculatorData->longTermBeta);
   ShiftOffsetCalculator_Init(
      &instance->_private.featurePanShiftOffsetCalculator,
      dataModel,
      &instance->_private.featurePanLongTermAverageFilter.interface,
      DataModelErdPointerAccess_GetTimerModule(dataModel, Erd_TimerModule),
      &featurePanShiftOffsetCalculatorConfig,
      parametricData->featurePanThermalOffsetData->shiftOffsetData);
}
