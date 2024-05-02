/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ICECABINETSHIFTOFFSETCALCULATORPLUGIN_H
#define ICECABINETSHIFTOFFSETCALCULATORPLUGIN_H

#include "I_DataModel.h"
#include "Filter_LongTermAverage.h"
#include "ShiftOffsetCalculator.h"

typedef struct
{
   struct
   {
      Filter_LongTermAverage_t iceCabinetLongTermAverageFilter;
      ShiftOffsetCalculator_t iceCabinetShiftOffsetCalculator;
   } _private;
} IceCabinetShiftOffsetCalculatorPlugin_t;

void IceCabinetShiftOffsetCalculatorPlugin_Init(
   IceCabinetShiftOffsetCalculatorPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
