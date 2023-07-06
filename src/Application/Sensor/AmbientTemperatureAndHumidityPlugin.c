/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "AmbientTemperatureAndHumidityPlugin.h"
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

static const Erd_t ambientHumiditySensorValidOverrideArbiterRequestErdList[] = {
   Erd_AmbientHumiditySensor_IsValidOverrideRequest
};

static const Erd_t ambientHumiditySensorValidValueErdList[] = {
   Erd_Ambient_HumiditySensorIsValid,
   Erd_AmbientHumiditySensor_IsValidOverrideValue
};

static const OverrideArbiterConfiguration_t ambientHumiditySensorValidArbiterConfiguration = {
   ambientHumiditySensorValidOverrideArbiterRequestErdList,
   ambientHumiditySensorValidValueErdList,
   Erd_AmbientHumiditySensor_IsValidResolved,
   NUM_ELEMENTS(ambientHumiditySensorValidOverrideArbiterRequestErdList)
};

static const ExternalFilteredSensorResolverConfig_t externalFilteredAmbientTemperatureResolver = {
   .internalFilteredSensorErd = Erd_Ambient_FilteredInternalTemperatureResolvedInDegFx100,
   .internalFilteredSensorIsValidErd = Erd_AmbientThermistor_IsValidResolved,
   .externalFilteredSensorErd = Erd_ExternalAmbientFilteredTemperatureInDegFx100,
   .externalFilteredSensorIsValidErd = Erd_ExternalAmbientThermistor_IsValid,
   .resolvedFilteredSensorErd = Erd_Ambient_FilteredTemperatureResolvedInDegFx100,
   .resolvedFilteredSensorIsValidErd = Erd_AmbientTemperature_IsValidResolved,
   .sensorType = ExternalFilteredSensorResolver_SensorType_Temperature
};

static const ExternalFilteredSensorResolverConfig_t externalFilteredAmbientHumiditySensorResolver = {
   .internalFilteredSensorErd = Erd_AmbientHumidity_FilteredRelativeHumidityPercentx100,
   .internalFilteredSensorIsValidErd = Erd_AmbientHumiditySensor_IsValidResolved,
   .externalFilteredSensorErd = Erd_ExternalAmbientFilteredRelativeHumidityPercentx100,
   .externalFilteredSensorIsValidErd = Erd_ExternalAmbientHumidity_IsValid,
   .resolvedFilteredSensorErd = Erd_Ambient_FilteredHumidityResolvedPercentx100,
   .resolvedFilteredSensorIsValidErd = Erd_AmbientHumidity_IsValidResolved,
   .sensorType = ExternalFilteredSensorResolver_SensorType_Humidity
};

void AmbientTemperatureAndHumidityPlugin_Init(AmbientTemperaturePlugin_t *instance, I_DataModel_t *dataModel)
{
   OverrideArbiter_Init(
      &instance->ambientFilteredTemperatureArbiter,
      DataModel_AsDataSource(dataModel),
      &ambientFilteredTemperatureArbiterConfiguration);

   OverrideArbiter_Init(
      &instance->ambientThermistorValidArbiter,
      DataModel_AsDataSource(dataModel),
      &ambientThermistorValidArbiterConfiguration);

   OverrideArbiter_Init(
      &instance->ambientHumiditySensorValidArbiter,
      DataModel_AsDataSource(dataModel),
      &ambientHumiditySensorValidArbiterConfiguration);

   ExternalFilteredSensorResolver_Init(
      &instance->externalFilteredAmbientTemperatureSensorResolver,
      dataModel,
      &externalFilteredAmbientTemperatureResolver,
      PersonalityParametricData_Get(dataModel)->sensorData->ambientThermistor);

   AmbientTemperatureAverage_Init(
      &instance->ambientTemperatureAverage,
      dataModel);

   ExternalFilteredSensorResolver_Init(
      &instance->externalFilteredAmbientHumiditySensorResolver,
      dataModel,
      &externalFilteredAmbientHumiditySensorResolver,
      PersonalityParametricData_Get(dataModel)->sensorData->ambientHumidityThermistor);

   DataModel_Write(
      dataModel,
      Erd_AmbientTemperaturePluginReady,
      set);
}
