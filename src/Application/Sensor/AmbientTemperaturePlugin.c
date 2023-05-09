/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "AmbientTemperaturePlugin.h"
#include "Constants_Binary.h"
#include "SystemErds.h"

static const Erd_t ambientFilteredTemperatureOverrideRequestErdList[] = {
   Erd_Ambient_FilteredInternalTemperatureOverrideRequest
};

static const Erd_t ambientFilteredTemperatureValueErdList[] = {
   Erd_Ambient_FilteredInternalTemperatureInDegFx100,
   Erd_Ambient_FilteredInternalTemperatureOverrideValueInDegFx100
};

static const OverrideArbiterConfiguration_t ambientFilteredTemperatureArbiterConfiguration = {
   ambientFilteredTemperatureOverrideRequestErdList,
   ambientFilteredTemperatureValueErdList,
   Erd_Ambient_FilteredInternalTemperatureResolvedInDegFx100,
   NUM_ELEMENTS(ambientFilteredTemperatureOverrideRequestErdList)
};

static const Erd_t ambientThermistorValidOverrideArbiterRequestErdList[] = {
   Erd_AmbientThermistor_IsValidOverrideRequest
};

static const Erd_t ambientThermistorValidValueErdList[] = {
   Erd_Ambient_ThermistorIsValid,
   Erd_AmbientThermistor_IsValidOverrideValue
};

static const OverrideArbiterConfiguration_t ambientThermistorValidArbiterConfiguration = {
   ambientThermistorValidOverrideArbiterRequestErdList,
   ambientThermistorValidValueErdList,
   Erd_AmbientThermistor_IsValidResolved,
   NUM_ELEMENTS(ambientThermistorValidOverrideArbiterRequestErdList)
};

void AmbientTemperaturePlugin_Init(AmbientTemperaturePlugin_t *instance, I_DataModel_t *dataModel)
{
   OverrideArbiter_Init(
      &instance->ambientFilteredTemperatureArbiter,
      DataModel_AsDataSource(dataModel),
      &ambientFilteredTemperatureArbiterConfiguration);

   OverrideArbiter_Init(
      &instance->ambientThermistorValidArbiter,
      DataModel_AsDataSource(dataModel),
      &ambientThermistorValidArbiterConfiguration);

   AmbientTemperatureAverage_Init(&instance->ambientTemperatureAverage, dataModel);

   DataModel_Write(
      dataModel,
      Erd_AmbientTemperaturePluginReady,
      set);
}
