/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FREEZERADJUSTEDSETPOINTPLUGIN_H
#define FREEZERADJUSTEDSETPOINTPLUGIN_H

#include "AdjustedSetpointData.h"
#include "ErdWriterOnCompareMatch.h"
#include "FreezerShiftOffsetCalculatorPlugin.h"
#include "CrossAmbientOffsetCalculator.h"
#include "HighAmbientHumidityOffsetCalculator.h"
#include "I16ErdAdder.h"
#include "ResolvedSetpointWriter.h"
#include "I_DataModel.h"

typedef struct
{
   struct
   {
      FreezerShiftOffsetCalculatorPlugin_t freezerShiftOffsetCalculatorPlugin;
      CrossAmbientCalculator_t freezerCrossAmbientOffsetCalculator;
      HighAmbientCalculator_t freezerHighAmbientOffsetCalculator;
      ErdWriterOnCompareMatch_t erdWriterOnCompareMatchForAdjustedSetpoint;
      I16ErdAdder_t freezerErdAdder;
      ResolvedSetpointWriter_t freezerResolvedSetpointWriter;
   } _private;
} FreezerAdjustedSetpointPlugin_t;

void FreezerAdjustedSetpointPlugin_Init(
   FreezerAdjustedSetpointPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
