/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ConvertibleCompartmentAdjustedSetpointPlugin.h"
#include "SystemErds.h"

static const Erd_t convertibleCompartmentAdjustedSetpointErds[] = {
   Erd_ConvertibleCompartment_ResolvedSetpointInDegFx100,
   Erd_ConvertibleCompartment_CrossAmbientOffsetInDegFx100,
   Erd_ConvertibleCompartment_ThermalShiftInDegFx100
};

static const I16ErdAdderConfiguration_t convertibleCompartmentErdAdderConfig = {
   .resultErd = Erd_ConvertibleCompartment_AdjustedSetpointInDegFx100,
   .i16ErdsToBeAdded = {
      convertibleCompartmentAdjustedSetpointErds,
      NUM_ELEMENTS(convertibleCompartmentAdjustedSetpointErds),
   },
};

static const ResolvedSetpointWriterConfiguration_t convertibleCompartmentResolvedSetpointWriterConfiguration = {
   .resolvedSetpointVoteErd = Erd_ConvertibleCompartmentSetpoint_ResolvedVote,
   .resolvedSetpointErd = Erd_ConvertibleCompartment_ResolvedSetpointInDegFx100,
   .userSetpointPluginReadyErd = Erd_UserSetpointPluginReady
};

void ConvertibleCompartmentAdjustedSetpointPlugin_Init(
   ConvertibleCompartmentAdjustedSetpointPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   I16ErdAdder_Init(
      &instance->_private.convertibleCompartmentErdAdderConfig,
      dataModel,
      &convertibleCompartmentErdAdderConfig);

   ResolvedSetpointWriter_Init(
      &instance->_private.convertibleCompartmentResolvedSetpointWriter,
      dataModel,
      &convertibleCompartmentResolvedSetpointWriterConfiguration);

   ConvertibleCompartmentShiftOffsetCalculatorPlugin_Init(
      &instance->_private.convertibleCompartmentShiftOffsetCalculatorPlugin,
      dataModel);
}
