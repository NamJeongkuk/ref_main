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

   CrossAmbientOffsetCalculator_Init(
      &instance->_private.convertibleCompartmentAsFreshFoodOffsetCalculator,
      dataModel,
      PersonalityParametricData_Get(dataModel)->setpointData->adjustedSetpointData->convertibleCompartmentAdjustedSetpointData->crossAmbientAsFreshFoodOffsetData,
      &convertibleCompartmentAsFreshFoodOffsetCalculatorConfig);

   CrossAmbientOffsetCalculator_Init(
      &instance->_private.convertibleCompartmentAsFreezerOffsetCalculator,
      dataModel,
      PersonalityParametricData_Get(dataModel)->setpointData->adjustedSetpointData->convertibleCompartmentAdjustedSetpointData->crossAmbientAsFreezerOffsetData,
      &convertibleCompartmentAsFreezerOffsetCalculatorConfig);

   OverrideArbiter_Init(
      &instance->_private.convertibleCompartmentOffsetOverrideArbiter,
      DataModel_AsDataSource(dataModel),
      &convertibleCompartmentCrossAmbientOffsetArbiterConfiguration);
}
