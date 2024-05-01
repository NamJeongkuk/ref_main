/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "FeaturePanAdjustedSetpointPlugin.h"
#include "SystemErds.h"

static const Erd_t featurePanAdjustedSetpointWithoutShiftErds[] = {
   Erd_FeaturePan_ResolvedSetpointInDegFx100,
   Erd_FeaturePan_SetpointOffsetInDegFx100,
   Erd_FeaturePan_CrossAmbientOffsetInDegFx100,
};

static const I16ErdAdderConfiguration_t featurePanAdjustedSetpointWithoutShiftConfig = {
   .resultErd = Erd_FeaturePan_AdjustedSetpointWithoutShiftInDegFx100,
   .i16ErdsToBeAdded = {
      featurePanAdjustedSetpointWithoutShiftErds,
      NUM_ELEMENTS(featurePanAdjustedSetpointWithoutShiftErds),
   },
};

static const Erd_t featurePanAdjustedSetpointErds[] = {
   Erd_FeaturePan_AdjustedSetpointWithoutShiftInDegFx100,
   Erd_FeaturePan_ThermalShiftInDegFx100,
};

static const I16ErdAdderConfiguration_t featurePanAdjustedSetpointConfig = {
   .resultErd = Erd_FeaturePan_AdjustedSetpointInDegFx100,
   .i16ErdsToBeAdded = {
      featurePanAdjustedSetpointErds,
      NUM_ELEMENTS(featurePanAdjustedSetpointErds),
   },
};

static const ResolvedSetpointWriterConfiguration_t featurePanResolvedSetpointWriterConfiguration = {
   .setpointVoteErd = Erd_FeaturePanSetpoint_UserVote,
   .resolvedSetpointErd = Erd_FeaturePan_ResolvedSetpointInDegFx100,
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
   Erd_FeaturePan_CrossAmbientOffsetInDegFx100,
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

void FeaturePanAdjustedSetpointPlugin_Init(
   FeaturePanAdjustedSetpointPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   I16ErdAdder_Init(
      &instance->_private.featurePanAdjustedSetpointWithoutShiftErdAdder,
      dataModel,
      &featurePanAdjustedSetpointWithoutShiftConfig);

   I16ErdAdder_Init(
      &instance->_private.featurePanAdjustedSetpointErdAdder,
      dataModel,
      &featurePanAdjustedSetpointConfig);

   ResolvedSetpointWriter_Init(
      &instance->_private.featurePanResolvedSetpointWriter,
      dataModel,
      &featurePanResolvedSetpointWriterConfiguration);

   FeaturePanShiftOffsetCalculatorPlugin_Init(
      &instance->_private.featurePanShiftOffsetCalculatorPlugin,
      dataModel);

   CrossAmbientOffsetCalculator_Init(
      &instance->_private.convertibleCompartmentAsFreshFoodOffsetCalculator,
      dataModel,
      PersonalityParametricData_Get(dataModel)->featurePanThermalOffsetData->crossAmbientAsFreshFoodOffsetData,
      &convertibleCompartmentAsFreshFoodOffsetCalculatorConfig);

   CrossAmbientOffsetCalculator_Init(
      &instance->_private.convertibleCompartmentAsFreezerOffsetCalculator,
      dataModel,
      PersonalityParametricData_Get(dataModel)->featurePanThermalOffsetData->crossAmbientAsFreezerOffsetData,
      &convertibleCompartmentAsFreezerOffsetCalculatorConfig);

   OverrideArbiter_Init(
      &instance->_private.convertibleCompartmentOffsetOverrideArbiter,
      DataModel_AsDataSource(dataModel),
      &convertibleCompartmentCrossAmbientOffsetArbiterConfiguration);
}
