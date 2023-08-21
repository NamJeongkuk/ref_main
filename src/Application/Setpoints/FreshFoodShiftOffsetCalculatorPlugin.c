/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "FreshFoodShiftOffsetCalculatorPlugin.h"
#include "SystemErds.h"

static const ShiftOffsetCalculatorConfig_t freshFoodShiftOffsetCalculatorConfig = {
   .filteredTemperatureInDegFx100Erd = Erd_FreshFood_FilteredTemperatureResolvedInDegFx100,
   .adjustedSetpointErd = Erd_FreshFood_AdjustedSetpointInDegFx100,
   .shiftOffsetErd = Erd_FreshFood_ThermalShiftInDegFx100,
   .adjustedSetpointWithoutShiftErd = Erd_FreshFood_AdjustedSetpointWithoutShiftInDegFx100,
   .timerModuleErd = Erd_TimerModule,
   .postDwellCompletionSignalErd = Erd_PostDwellCompletionSignal,
   .resetThermalShiftOffsetSignalErd = Erd_FreshFood_ResetThermalShiftOffsetSignal,
   .longTermAverageErd = Erd_FreshFood_LongTermAverageInDegFx100,
   .adjustedSetpointPluginReadyErd = Erd_AdjustedSetpointPluginReady
};

void FreshFoodShiftOffsetCalculatorPlugin_Init(
   FreshFoodShiftOffsetCalculatorPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   const AdjustedSetpointData_t *adjustedSetpointData =
      PersonalityParametricData_Get(dataModel)->setpointData->adjustedSetpointData;

   Filter_LongTermAverage_Init(
      &instance->_private.freshFoodLongTermAverageFilter,
      adjustedSetpointData->shiftOffsetCalculatorData->longTermBeta);
   ShiftOffsetCalculator_Init(
      &instance->_private.freshFoodShiftOffsetCalculator,
      dataModel,
      &instance->_private.freshFoodLongTermAverageFilter.interface,
      &freshFoodShiftOffsetCalculatorConfig,
      adjustedSetpointData->freshFoodAdjustedSetpointData->shiftOffsetData);
}
