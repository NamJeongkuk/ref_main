/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "FreezerAdjustedSetpointPlugin.h"
#include "PersonalityParametricData.h"
#include "SetpointZone.h"
#include "SystemErds.h"

static const Erd_t freezerAdjustedSetpointErds[] = {
   Erd_Freezer_ResolvedSetpointInDegFx100,
   Erd_Freezer_CabinetOffsetInDegFx100,
   Erd_Freezer_CrossAmbientOffsetInDegFx100,
   Erd_Freezer_ThermalShiftInDegFx100
};

static const I16ErdAdderConfiguration_t freezerErdAdderConfig = {
   .resultErd = Erd_Freezer_AdjustedSetpointInDegFx100,
   .i16ErdsToBeAdded = {
      freezerAdjustedSetpointErds,
      NUM_ELEMENTS(freezerAdjustedSetpointErds),
   },
};

static const ResolvedSetpointWriterConfiguration_t resolvedSetpointWriterConfiguration = {
   .resolvedSetpointVoteErd = Erd_FreezerSetpoint_ResolvedVote,
   .resolvedSetpointErd = Erd_Freezer_ResolvedSetpointInDegFx100
};

static void InitializeFreezerCabinetOffsetErd(I_DataModel_t *dataModel)
{
   DataModel_Write(
      dataModel,
      Erd_Freezer_CabinetOffsetInDegFx100,
      &PersonalityParametricData_Get(dataModel)->setpointData->adjustedSetpointData->freezerAdjustedSetpointData->cabinetOffsetInDegFx100);
}

void FreezerAdjustedSetpointPlugin_Init(
   FreezerAdjustedSetpointPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   InitializeFreezerCabinetOffsetErd(dataModel);
   I16ErdAdder_Init(&instance->_private.freezerErdAdder, dataModel, &freezerErdAdderConfig);
   ResolvedSetpointWriter_Init(&instance->_private.resolvedSetpointWriter, dataModel, &resolvedSetpointWriterConfiguration);
   FreezerShiftOffsetCalculatorPlugin_Init(&instance->_private.freezerShiftOffsetCalculatorPlugin, dataModel);
}
