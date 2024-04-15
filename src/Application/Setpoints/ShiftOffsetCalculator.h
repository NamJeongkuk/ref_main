/*!
 * @file
 * @brief The shift offset calculator module updates the shift offset at regular time intervals,
 *        which is added to the resolved setpoint along with other offsets to determine the adjusted setpoint.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SHIFTOFFSETCALCULATOR_H
#define SHIFTOFFSETCALCULATOR_H

#include "AdjustedSetpointData.h"
#include "EventSubscription.h"
#include "I_DataModel.h"
#include "I_Filter.h"
#include "Timer.h"

typedef struct
{
   Erd_t filteredTemperatureInDegFx100Erd; // TemperatureDegFx100_t
   Erd_t adjustedSetpointErd; // TemperatureDegFx100_t
   Erd_t shiftOffsetErd; // TemperatureDegFx100_t
   Erd_t adjustedSetpointWithoutShiftErd; // TemperatureDegFx100_t
   Erd_t postDwellCompletionSignalErd; // Signal_t
   Erd_t resetThermalShiftOffsetSignalErd; // Signal_t
   Erd_t longTermAverageErd; // TemperatureDegFx100_t
} ShiftOffsetCalculatorConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      Timer_t longTermAverageUpdateTimer;
      Timer_t shiftOffsetCalculateTimer;
      I_Filter_t *longTermAverageFilter;
      EventSubscription_t dataModelSubscription;

      const ShiftOffsetCalculatorConfig_t *config;
      const ShiftOffsetData_t *shiftOffsetData;
   } _private;
} ShiftOffsetCalculator_t;

void ShiftOffsetCalculator_Init(
   ShiftOffsetCalculator_t *instance,
   I_DataModel_t *dataModel,
   I_Filter_t *longTermAverageFilter,
   TimerModule_t *timerModule,
   const ShiftOffsetCalculatorConfig_t *config,
   const ShiftOffsetData_t *shiftOffsetData);

#endif
