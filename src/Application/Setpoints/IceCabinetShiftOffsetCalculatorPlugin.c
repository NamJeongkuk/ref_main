/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "DataModelErdPointerAccess.h"
#include "IceCabinetShiftOffsetCalculatorPlugin.h"
#include "SystemErds.h"

static const ShiftOffsetCalculatorConfig_t iceCabinetShiftOffsetCalculatorConfig = {
   .filteredTemperatureInDegFx100Erd = Erd_IceCabinet_FilteredTemperatureResolvedInDegFx100,
   .shiftOffsetErd = Erd_IceCabinet_ThermalShiftInDegFx100,
   .adjustedSetpointWithoutShiftErd = Erd_IceCabinet_AdjustedSetpointWithoutShiftInDegFx100,
   .postDwellCompletionSignalErd = Erd_PostDwellCompletionSignal,
   .resetThermalShiftOffsetSignalErd = Erd_IceCabinet_ResetThermalShiftOffsetSignal,
   .longTermAverageErd = Erd_IceCabinet_LongTermAverageInDegFx100,
};

void IceCabinetShiftOffsetCalculatorPlugin_Init(
   IceCabinetShiftOffsetCalculatorPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   const AdjustedSetpointData_t *adjustedSetpointData =
      PersonalityParametricData_Get(dataModel)->adjustedSetpointData;

   Filter_LongTermAverage_Init(
      &instance->_private.iceCabinetLongTermAverageFilter,
      adjustedSetpointData->shiftOffsetCalculatorData->longTermBeta);
   ShiftOffsetCalculator_Init(
      &instance->_private.iceCabinetShiftOffsetCalculator,
      dataModel,
      &instance->_private.iceCabinetLongTermAverageFilter.interface,
      DataModelErdPointerAccess_GetTimerModule(dataModel, Erd_TimerModule),
      &iceCabinetShiftOffsetCalculatorConfig,
      adjustedSetpointData->iceCabinetAdjustedSetpointData->shiftOffsetData);
}
