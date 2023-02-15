/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FRESHFOODSHIFTOFFSETCALCULATORPLUGIN_H
#define FRESHFOODSHIFTOFFSETCALCULATORPLUGIN_H

#include "ShiftOffsetCalculator.h"
#include "Filter_LongTermAverage.h"

typedef struct
{
   struct
   {
      Filter_LongTermAverage_t freshFoodLongTermAverageFilter;
      ShiftOffsetCalculator_t freshFoodShiftOffsetCalculator;
   } _private;
} FreshFoodShiftOffsetCalculatorPlugin_t;

void FreshFoodShiftOffsetCalculatorPlugin_Init(
   FreshFoodShiftOffsetCalculatorPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
