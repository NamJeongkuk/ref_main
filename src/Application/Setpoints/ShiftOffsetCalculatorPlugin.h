/*!
 * @file
 * @brief Glue module of shift offset calculators for each compartment
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SHIFTOFFSETCALCULATORPLUGIN_H
#define SHIFTOFFSETCALCULATORPLUGIN_H

#include "I_DataModel.h"
#include "PostDefrostShiftSeedingRequester.h"

typedef struct
{
   struct
   {
      PostDefrostShiftSeedingRequester_t postDefrostShiftSeedingRequester;
   } _private;
} ShiftOffsetCalculatorPlugin_t;

void ShiftOffsetCalculatorPlugin_Init(
   ShiftOffsetCalculatorPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
