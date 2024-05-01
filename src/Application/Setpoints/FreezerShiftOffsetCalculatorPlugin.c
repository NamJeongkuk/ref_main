/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "DataModelErdPointerAccess.h"
#include "FreezerShiftOffsetCalculatorPlugin.h"
#include "SystemErds.h"

static const ShiftOffsetCalculatorConfig_t freezerShiftOffsetCalculatorConfig = {
   .filteredTemperatureInDegFx100Erd = Erd_Freezer_FilteredTemperatureResolvedInDegFx100,
   .shiftOffsetErd = Erd_Freezer_ThermalShiftInDegFx100,
   .adjustedSetpointWithoutShiftErd = Erd_Freezer_AdjustedSetpointWithoutShiftInDegFx100,
   .postDwellCompletionSignalErd = Erd_PostDwellCompletionSignal,
   .resetThermalShiftOffsetSignalErd = Erd_Freezer_ResetThermalShiftOffsetSignal,
   .longTermAverageErd = Erd_Freezer_LongTermAverageInDegFx100,
};

void FreezerShiftOffsetCalculatorPlugin_Init(
   FreezerShiftOffsetCalculatorPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   const AdjustedSetpointData_t *adjustedSetpointData =
      PersonalityParametricData_Get(dataModel)->adjustedSetpointData;

   Filter_LongTermAverage_Init(
      &instance->_private.freezerLongTermAverageFilter,
      adjustedSetpointData->shiftOffsetCalculatorData->longTermBeta);
   ShiftOffsetCalculator_Init(
      &instance->_private.freezerShiftOffsetCalculator,
      dataModel,
      &instance->_private.freezerLongTermAverageFilter.interface,
      DataModelErdPointerAccess_GetTimerModule(dataModel, Erd_TimerModule),
      &freezerShiftOffsetCalculatorConfig,
      adjustedSetpointData->freezerAdjustedSetpointData->shiftOffsetData);
}
