/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ICECABINETADJUSTEDSETPOINTPLUGIN_H
#define ICECABINETADJUSTEDSETPOINTPLUGIN_H

#include "IceFormationOffsetWriter.h"
#include "IceCabinetShiftOffsetCalculatorPlugin.h"
#include "I16ErdAdder.h"
#include "ResolvedSetpointWriter.h"
#include "CrossAmbientOffsetCalculator.h"
#include "OverrideArbiter.h"

typedef struct
{
   struct
   {
      I16ErdAdder_t iceCabinetAdjustedSetpointErdAdder;
      I16ErdAdder_t iceCabinetAdjustedSetpointWithoutShiftErdAdder;
      ResolvedSetpointWriter_t iceCabinetResolvedSetpointWriter;
      IceCabinetShiftOffsetCalculatorPlugin_t iceCabinetShiftOffsetCalculatorPlugin;
      CrossAmbientOffsetCalculator_t iceCabinetOffsetCalculator;
      IceFormationOffsetWriter_t iceFormationOffsetWriter;
   } _private;
} IceCabinetAdjustedSetpointPlugin_t;

/*!
 * @param instance
 * @param dataModel
 */
void IceCabinetAdjustedSetpointPlugin_Init(
   IceCabinetAdjustedSetpointPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
