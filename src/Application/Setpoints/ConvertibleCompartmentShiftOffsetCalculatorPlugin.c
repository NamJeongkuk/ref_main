/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ConvertibleCompartmentShiftOffsetCalculatorPlugin.h"
#include "DataModelErdPointerAccess.h"
#include "SystemErds.h"

static const ShiftOffsetCalculatorConfig_t convertibleCompartmentShiftOffsetCalculatorConfig = {
   .filteredTemperatureInDegFx100Erd = Erd_ConvertibleCompartmentCabinet_FilteredTemperatureResolvedInDegFx100,
   .shiftOffsetErd = Erd_ConvertibleCompartment_ThermalShiftInDegFx100,
   .adjustedSetpointWithoutShiftErd = Erd_ConvertibleCompartment_AdjustedSetpointWithoutShiftInDegFx100,
   .postDwellCompletionSignalErd = Erd_PostDwellCompletionSignal,
   .resetThermalShiftOffsetSignalErd = Erd_ConvertibleCompartment_ResetThermalShiftOffsetSignal,
   .longTermAverageErd = Erd_ConvertibleCompartment_LongTermAverageInDegFx100,
};

void ConvertibleCompartmentShiftOffsetCalculatorPlugin_Init(
   ConvertibleCompartmentShiftOffsetCalculatorPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   const AdjustedSetpointData_t *adjustedSetpointData =
      PersonalityParametricData_Get(dataModel)->setpointData->adjustedSetpointData;

   Filter_LongTermAverage_Init(
      &instance->_private.convertibleCompartmentLongTermAverageFilter,
      adjustedSetpointData->shiftOffsetCalculatorData->longTermBeta);
   ShiftOffsetCalculator_Init(
      &instance->_private.convertibleCompartmentShiftOffsetCalculator,
      dataModel,
      &instance->_private.convertibleCompartmentLongTermAverageFilter.interface,
      DataModelErdPointerAccess_GetTimerModule(dataModel, Erd_TimerModule),
      &convertibleCompartmentShiftOffsetCalculatorConfig,
      adjustedSetpointData->convertibleCompartmentAdjustedSetpointData->shiftOffsetData);
}
