/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FEATUREPANSHIFTOFFSETCALCULATORPLUGIN_H
#define FEATUREPANSHIFTOFFSETCALCULATORPLUGIN_H

#include "I_DataModel.h"
#include "Filter_LongTermAverage.h"
#include "ShiftOffsetCalculator.h"

typedef struct
{
   struct
   {
      Filter_LongTermAverage_t featurePanLongTermAverageFilter;
      ShiftOffsetCalculator_t featurePanShiftOffsetCalculator;
   } _private;
} FeaturePanShiftOffsetCalculatorPlugin_t;

/*!
 * @param instance
 * @param dataModel
 */
void FeaturePanShiftOffsetCalculatorPlugin_Init(
   FeaturePanShiftOffsetCalculatorPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
