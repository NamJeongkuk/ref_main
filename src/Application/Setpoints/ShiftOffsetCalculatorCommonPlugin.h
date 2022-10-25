/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SHIFTOFFSETCALCULATORCOMMONPLUGIN_H
#define SHIFTOFFSETCALCULATORCOMMONPLUGIN_H

#include "I_DataModel.h"
#include "PostDwellCompletionSignalRequester.h"

typedef struct
{
   struct
   {
      PostDwellCompletionSignalRequester_t postDwellCompletionSignalRequester;
   } _private;
} ShiftOffsetCalculatorCommonPlugin_t;

void ShiftOffsetCalculatorCommonPlugin_Init(
   ShiftOffsetCalculatorCommonPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
