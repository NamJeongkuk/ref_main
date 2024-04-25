/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ConvertibleCompartmentAdjustedSetpointPlugin.h"
#include "SystemErds.h"

static const Erd_t convertibleCompartmentAdjustedSetpointWithoutShiftErds[] = {
   Erd_ConvertibleCompartment_ResolvedSetpointInDegFx100,
   Erd_FeaturePan_SetpointOffsetInDegFx100,
   Erd_ConvertibleCompartment_CrossAmbientOffsetInDegFx100,
};

static const I16ErdAdderConfiguration_t convertibleCompartmentAdjustedSetpointWithoutShiftConfig = {
   .resultErd = Erd_FeaturePan_AdjustedSetpointWithoutShiftInDegFx100,
   .i16ErdsToBeAdded = {
      convertibleCompartmentAdjustedSetpointWithoutShiftErds,
      NUM_ELEMENTS(convertibleCompartmentAdjustedSetpointWithoutShiftErds),
   },
};

static const Erd_t convertibleCompartmentAdjustedSetpointErds[] = {
   Erd_FeaturePan_AdjustedSetpointWithoutShiftInDegFx100,
   Erd_ConvertibleCompartment_ThermalShiftInDegFx100,
};

static const I16ErdAdderConfiguration_t convertibleCompartmentAdjustedSetpointConfig = {
   .resultErd = Erd_FeaturePan_AdjustedSetpointInDegFx100,
   .i16ErdsToBeAdded = {
      convertibleCompartmentAdjustedSetpointErds,
      NUM_ELEMENTS(convertibleCompartmentAdjustedSetpointErds),
   },
};

static const ResolvedSetpointWriterConfiguration_t convertibleCompartmentResolvedSetpointWriterConfiguration = {
   .setpointVoteErd = Erd_FeaturePanSetpoint_UserVote,
   .resolvedSetpointErd = Erd_ConvertibleCompartment_ResolvedSetpointInDegFx100,
   .userSetpointPluginReadyErd = Erd_UserSetpointPluginReady
};

static const Erd_t convertibleCompartmentCrossAmbientOffsetOverrideRequestErdList[] = {
   Erd_ConvertibleCompartmentState
};

static const Erd_t convertibleCompartmentCrossAmbientOffsetValueErdList[] = {
   Erd_ConvertibleCompartmentAsFreshFood_CrossAmbientOffsetInDegFx100,
   Erd_ConvertibleCompartmentAsFreezer_CrossAmbientOffsetInDegFx100
};

static const OverrideArbiterConfiguration_t convertibleCompartmentCrossAmbientOffsetArbiterConfiguration = {
   convertibleCompartmentCrossAmbientOffsetOverrideRequestErdList,
   convertibleCompartmentCrossAmbientOffsetValueErdList,
   Erd_ConvertibleCompartment_CrossAmbientOffsetInDegFx100,
   NUM_ELEMENTS(convertibleCompartmentCrossAmbientOffsetOverrideRequestErdList)
};

static const CrossAmbientOffsetCalculatorConfig_t convertibleCompartmentAsFreshFoodOffsetCalculatorConfig = {
   .crossAmbientWindowAveragedTemperatureInDegFx100Erd = Erd_Ambient_WindowAveragedTemperatureInDegFx100,
   .crossAmbientOffsetInDegFx100Erd = Erd_ConvertibleCompartmentAsFreshFood_CrossAmbientOffsetInDegFx100
};

static const CrossAmbientOffsetCalculatorConfig_t convertibleCompartmentAsFreezerOffsetCalculatorConfig = {
   .crossAmbientWindowAveragedTemperatureInDegFx100Erd = Erd_Ambient_WindowAveragedTemperatureInDegFx100,
   .crossAmbientOffsetInDegFx100Erd = Erd_ConvertibleCompartmentAsFreezer_CrossAmbientOffsetInDegFx100
};

void ConvertibleCompartmentAdjustedSetpointPlugin_Init(
   ConvertibleCompartmentAdjustedSetpointPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   I16ErdAdder_Init(
      &instance->_private.convertibleCompartmentAdjustedSetpointWithoutShiftErdAdder,
      dataModel,
      &convertibleCompartmentAdjustedSetpointWithoutShiftConfig);

   I16ErdAdder_Init(
      &instance->_private.convertibleCompartmentAdjustedSetpointErdAdder,
      dataModel,
      &convertibleCompartmentAdjustedSetpointConfig);

   ResolvedSetpointWriter_Init(
      &instance->_private.convertibleCompartmentResolvedSetpointWriter,
      dataModel,
      &convertibleCompartmentResolvedSetpointWriterConfiguration);

   ConvertibleCompartmentShiftOffsetCalculatorPlugin_Init(
      &instance->_private.convertibleCompartmentShiftOffsetCalculatorPlugin,
      dataModel);

   CrossAmbientOffsetCalculator_Init(
      &instance->_private.convertibleCompartmentAsFreshFoodOffsetCalculator,
      dataModel,
      PersonalityParametricData_Get(dataModel)->setpointData->adjustedSetpointData->featurePanAdjustedSetpointData->crossAmbientAsFreshFoodOffsetData,
      &convertibleCompartmentAsFreshFoodOffsetCalculatorConfig);

   CrossAmbientOffsetCalculator_Init(
      &instance->_private.convertibleCompartmentAsFreezerOffsetCalculator,
      dataModel,
      PersonalityParametricData_Get(dataModel)->setpointData->adjustedSetpointData->featurePanAdjustedSetpointData->crossAmbientAsFreezerOffsetData,
      &convertibleCompartmentAsFreezerOffsetCalculatorConfig);

   OverrideArbiter_Init(
      &instance->_private.convertibleCompartmentOffsetOverrideArbiter,
      DataModel_AsDataSource(dataModel),
      &convertibleCompartmentCrossAmbientOffsetArbiterConfiguration);
}
