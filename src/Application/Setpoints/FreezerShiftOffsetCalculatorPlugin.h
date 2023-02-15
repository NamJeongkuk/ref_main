/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FREEZERSHIFTOFFSETCALCULATORPLUGIN_H
#define FREEZERSHIFTOFFSETCALCULATORPLUGIN_H

#include "I_DataModel.h"
#include "Filter_LongTermAverage.h"
#include "ShiftOffsetCalculator.h"

typedef struct
{
   struct
   {
      Filter_LongTermAverage_t freezerLongTermAverageFilter;
      ShiftOffsetCalculator_t freezerShiftOffsetCalculator;
   } _private;
} FreezerShiftOffsetCalculatorPlugin_t;

void FreezerShiftOffsetCalculatorPlugin_Init(
   FreezerShiftOffsetCalculatorPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
