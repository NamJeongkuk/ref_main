/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FEATUREPANADJUSTEDSETPOINTPLUGIN_H
#define FEATUREPANADJUSTEDSETPOINTPLUGIN_H

#include "FeaturePanShiftOffsetCalculatorPlugin.h"
#include "I16ErdAdder.h"
#include "ResolvedSetpointWriter.h"
#include "CrossAmbientOffsetCalculator.h"
#include "OverrideArbiter.h"

typedef struct
{
   struct
   {
      I16ErdAdder_t featurePanAdjustedSetpointWithoutShiftErdAdder;
      I16ErdAdder_t featurePanAdjustedSetpointErdAdder;
      ResolvedSetpointWriter_t featurePanResolvedSetpointWriter;
      FeaturePanShiftOffsetCalculatorPlugin_t featurePanShiftOffsetCalculatorPlugin;
      CrossAmbientOffsetCalculator_t convertibleCompartmentAsFreshFoodOffsetCalculator;
      CrossAmbientOffsetCalculator_t convertibleCompartmentAsFreezerOffsetCalculator;
      OverrideArbiter_t convertibleCompartmentOffsetOverrideArbiter;
   } _private;
} FeaturePanAdjustedSetpointPlugin_t;

/*!
 * @param instance
 * @param dataModel
 */
void FeaturePanAdjustedSetpointPlugin_Init(
   FeaturePanAdjustedSetpointPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
