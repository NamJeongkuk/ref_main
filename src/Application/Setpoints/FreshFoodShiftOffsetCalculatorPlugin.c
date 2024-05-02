/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "DataModelErdPointerAccess.h"
#include "FreshFoodShiftOffsetCalculatorPlugin.h"
#include "SystemErds.h"

static const ShiftOffsetCalculatorConfig_t freshFoodShiftOffsetCalculatorConfig = {
   .filteredTemperatureInDegFx100Erd = Erd_FreshFood_FilteredTemperatureResolvedInDegFx100,
   .shiftOffsetErd = Erd_FreshFood_ThermalShiftInDegFx100,
   .adjustedSetpointWithoutShiftErd = Erd_FreshFood_AdjustedSetpointWithoutShiftInDegFx100,
   .postDwellCompletionSignalErd = Erd_PostDwellCompletionSignal,
   .resetThermalShiftOffsetSignalErd = Erd_FreshFood_ResetThermalShiftOffsetSignal,
   .longTermAverageErd = Erd_FreshFood_LongTermAverageInDegFx100,
};

void FreshFoodShiftOffsetCalculatorPlugin_Init(
   FreshFoodShiftOffsetCalculatorPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   const PersonalityParametricData_t *parametricData =
      PersonalityParametricData_Get(dataModel);

   Filter_LongTermAverage_Init(
      &instance->_private.freshFoodLongTermAverageFilter,
      parametricData->shiftOffsetCalculatorData->longTermBeta);
   ShiftOffsetCalculator_Init(
      &instance->_private.freshFoodShiftOffsetCalculator,
      dataModel,
      &instance->_private.freshFoodLongTermAverageFilter.interface,
      DataModelErdPointerAccess_GetTimerModule(dataModel, Erd_TimerModule),
      &freshFoodShiftOffsetCalculatorConfig,
      parametricData->freshFoodThermalOffsetData->shiftOffsetData);
}
