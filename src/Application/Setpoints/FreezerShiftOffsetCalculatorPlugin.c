/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "FreezerShiftOffsetCalculatorPlugin.h"
#include "SystemErds.h"

static const ShiftOffsetCalculatorConfig_t freezerShiftOffsetCalculatorConfig = {
   .filteredTemperatureInDegFx100Erd = Erd_Freezer_FilteredTemperatureResolvedInDegFx100,
   .adjustedSetpointErd = Erd_Freezer_AdjustedSetpointInDegFx100,
   .shiftOffsetErd = Erd_Freezer_ThermalShiftInDegFx100,
   .adjustedSetpointWithoutShiftErd = Erd_Freezer_AdjustedSetpointWithoutShiftInDegFx100,
   .timerModuleErd = Erd_TimerModule,
   .postDwellCompletionSignalErd = Erd_PostDwellCompletionSignal,
   .resetThermalShiftOffsetSignalErd = Erd_Freezer_ResetThermalShiftOffsetSignal,
   .longTermAverageErd = Erd_Freezer_LongTermAverageInDegFx100,
   .adjustedSetpointPluginReadyErd = Erd_AdjustedSetpointPluginReady
};

void FreezerShiftOffsetCalculatorPlugin_Init(
   FreezerShiftOffsetCalculatorPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   const AdjustedSetpointData_t *adjustedSetpointData =
      PersonalityParametricData_Get(dataModel)->setpointData->adjustedSetpointData;

   Filter_LongTermAverage_Init(
      &instance->_private.freezerLongTermAverageFilter,
      adjustedSetpointData->shiftOffsetCalculatorData->longTermBeta);
   ShiftOffsetCalculator_Init(
      &instance->_private.freezerShiftOffsetCalculator,
      dataModel,
      &instance->_private.freezerLongTermAverageFilter.interface,
      &freezerShiftOffsetCalculatorConfig,
      adjustedSetpointData->freezerAdjustedSetpointData->shiftOffsetData);
}
