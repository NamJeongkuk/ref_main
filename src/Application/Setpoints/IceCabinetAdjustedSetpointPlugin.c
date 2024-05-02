/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "IceCabinetAdjustedSetpointPlugin.h"
#include "SystemErds.h"

static const Erd_t iceCabinetAdjustedSetpointWithoutShiftErds[] = {
   Erd_IceCabinet_SetpointInDegFx100,
   Erd_IceCabinet_CabinetOffsetInDegFx100,
   Erd_IceCabinet_CrossAmbientOffsetInDegFx100,
   Erd_IceCabinet_IceFormationOffsetInDegFx100,
};

static const Erd_t iceCabinetAdjustedSetpointErds[] = {
   Erd_IceCabinet_AdjustedSetpointWithoutShiftInDegFx100,
   Erd_IceCabinet_ThermalShiftInDegFx100,
};

static const I16ErdAdderConfiguration_t iceCabinetAdjustedSetpointWithoutShiftConfig = {
   .resultErd = Erd_IceCabinet_AdjustedSetpointWithoutShiftInDegFx100,
   .i16ErdsToBeAdded = {
      iceCabinetAdjustedSetpointWithoutShiftErds,
      NUM_ELEMENTS(iceCabinetAdjustedSetpointWithoutShiftErds),
   },
};

static const I16ErdAdderConfiguration_t iceCabinetAdjustedSetpointConfig = {
   .resultErd = Erd_IceCabinet_AdjustedSetpointInDegFx100,
   .i16ErdsToBeAdded = {
      iceCabinetAdjustedSetpointErds,
      NUM_ELEMENTS(iceCabinetAdjustedSetpointErds),
   },
};

static const CrossAmbientOffsetCalculatorConfig_t iceCabinetOffsetCalculatorConfig = {
   .crossAmbientWindowAveragedTemperatureInDegFx100Erd = Erd_Ambient_WindowAveragedTemperatureInDegFx100,
   .crossAmbientOffsetInDegFx100Erd = Erd_IceCabinet_CrossAmbientOffsetInDegFx100
};

static void InitializeCabinetOffsetErd(I_DataModel_t *dataModel)
{
   DataModel_Write(
      dataModel,
      Erd_IceCabinet_CabinetOffsetInDegFx100,
      &PersonalityParametricData_Get(dataModel)->iceCabinetThermalOffsetData->cabinetOffsetInDegFx100);
}

void IceCabinetAdjustedSetpointPlugin_Init(
   IceCabinetAdjustedSetpointPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   InitializeCabinetOffsetErd(dataModel);
   I16ErdAdder_Init(
      &instance->_private.iceCabinetAdjustedSetpointWithoutShiftErdAdder,
      dataModel,
      &iceCabinetAdjustedSetpointWithoutShiftConfig);

   I16ErdAdder_Init(
      &instance->_private.iceCabinetAdjustedSetpointErdAdder,
      dataModel,
      &iceCabinetAdjustedSetpointConfig);

   IceCabinetShiftOffsetCalculatorPlugin_Init(
      &instance->_private.iceCabinetShiftOffsetCalculatorPlugin,
      dataModel);

   CrossAmbientOffsetCalculator_Init(
      &instance->_private.iceCabinetOffsetCalculator,
      dataModel,
      PersonalityParametricData_Get(dataModel)->iceCabinetThermalOffsetData->crossAmbientOffsetData,
      &iceCabinetOffsetCalculatorConfig);

   IceFormationOffsetWriter_Init(&instance->_private.iceFormationOffsetWriter, dataModel);
}
