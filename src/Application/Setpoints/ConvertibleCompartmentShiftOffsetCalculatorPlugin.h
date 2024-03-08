/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef CONVERTIBLECOMPARTMENTSHIFTOFFSETCALCULATORPLUGIN_H
#define CONVERTIBLECOMPARTMENTSHIFTOFFSETCALCULATORPLUGIN_H

#include "I_DataModel.h"
#include "Filter_LongTermAverage.h"
#include "ShiftOffsetCalculator.h"

typedef struct
{
   struct
   {
      Filter_LongTermAverage_t convertibleCompartmentLongTermAverageFilter;
      ShiftOffsetCalculator_t convertibleCompartmentShiftOffsetCalculator;
   } _private;
} ConvertibleCompartmentShiftOffsetCalculatorPlugin_t;

/*!
 * @param instance
 * @param dataModel
 */
void ConvertibleCompartmentShiftOffsetCalculatorPlugin_Init(
   ConvertibleCompartmentShiftOffsetCalculatorPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
