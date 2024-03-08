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
   Erd_Freezer_HighAmbientOffsetInDegFx100,
   Erd_Freezer_ThermalShiftInDegFx100
};

static const I16ErdAdderConfiguration_t freezerErdAdderConfig = {
   .resultErd = Erd_Freezer_AdjustedSetpointInDegFx100,
   .i16ErdsToBeAdded = {
      freezerAdjustedSetpointErds,
      NUM_ELEMENTS(freezerAdjustedSetpointErds),
   },
};

static const ResolvedSetpointWriterConfiguration_t freezerResolvedSetpointWriterConfiguration = {
   .resolvedSetpointVoteErd = Erd_FreezerSetpoint_ResolvedVote,
   .resolvedSetpointErd = Erd_Freezer_ResolvedSetpointInDegFx100,
   .userSetpointPluginReadyErd = Erd_UserSetpointPluginReady
};

static const CrossAmbientOffsetCalculatorConfig_t freezerCrossAmbientOffsetCalculatorConfig = {
   .crossAmbientWindowAveragedTemperatureInDegFx100Erd = Erd_Ambient_WindowAveragedTemperatureInDegFx100,
   .crossAmbientOffsetInDegFx100Erd = Erd_Freezer_CrossAmbientOffsetInDegFx100,
};

static const HighAmbientOffsetCalculatorConfig_t freezerHighAmbientOffsetCalculatorConfig = {
   .ambientFilteredHumidityResolvedPercentx100Erd = Erd_Ambient_FilteredHumidityResolvedPercentx100,
   .setpointStatusErd = Erd_FreezerSetpointStatus,
   .highAmbientOffsetErd = Erd_Freezer_HighAmbientOffsetInDegFx100,
   .ambientHumiditySensorIsValidResolvedErd = Erd_AmbientHumidity_IsValidResolved
};

static void InitializeFreezerCabinetOffsetErd(I_DataModel_t *dataModel)
{
   DataModel_Write(
      dataModel,
      Erd_Freezer_CabinetOffsetInDegFx100,
      &PersonalityParametricData_Get(dataModel)->cabinetOffsetData->freezerOffsetInDegFx100);
}

void FreezerAdjustedSetpointPlugin_Init(
   FreezerAdjustedSetpointPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   InitializeFreezerCabinetOffsetErd(dataModel);
   I16ErdAdder_Init(&instance->_private.freezerErdAdder, dataModel, &freezerErdAdderConfig);
   ResolvedSetpointWriter_Init(
      &instance->_private.freezerResolvedSetpointWriter,
      dataModel,
      &freezerResolvedSetpointWriterConfiguration);
   CrossAmbientOffsetCalculator_Init(
      &instance->_private.freezerCrossAmbientOffsetCalculator,
      dataModel,
      PersonalityParametricData_Get(dataModel)->setpointData->adjustedSetpointData->freezerAdjustedSetpointData->crossAmbientOffsetData,
      &freezerCrossAmbientOffsetCalculatorConfig);
   HighAmbientHumidityOffsetCalculator_Init(
      &instance->_private.freezerHighAmbientOffsetCalculator,
      dataModel,
      PersonalityParametricData_Get(dataModel)->setpointData->adjustedSetpointData->freezerAdjustedSetpointData->highAmbientOffsetData,
      &freezerHighAmbientOffsetCalculatorConfig);
   FreezerShiftOffsetCalculatorPlugin_Init(&instance->_private.freezerShiftOffsetCalculatorPlugin, dataModel);
}
