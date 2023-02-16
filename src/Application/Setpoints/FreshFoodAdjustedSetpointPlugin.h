/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FRESHFOODADJUSTEDSETPOINTPLUGIN_H
#define FRESHFOODADJUSTEDSETPOINTPLUGIN_H

#include "AdjustedSetpointData.h"
#include "ErdWriterOnCompareMatch.h"
#include "FreshFoodShiftOffsetCalculatorPlugin.h"
#include "I16ErdAdder.h"
#include "ResolvedSetpointWriter.h"
#include "I_DataModel.h"

typedef struct
{
   struct
   {
      FreshFoodShiftOffsetCalculatorPlugin_t freshFoodShiftOffsetCalculatorPlugin;
      ErdWriterOnCompareMatch_t erdWriterOnCompareMatchForAdjustedSetpoint;
      I16ErdAdder_t freshFoodErdAdder;
      ResolvedSetpointWriter_t freshFoodResolvedSetpointWriter;
   } _private;
} FreshFoodAdjustedSetpointPlugin_t;

void FreshFoodAdjustedSetpointPlugin_Init(
   FreshFoodAdjustedSetpointPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
