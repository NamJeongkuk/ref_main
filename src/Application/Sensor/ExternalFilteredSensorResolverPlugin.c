/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ExternalFilteredSensorResolverPlugin.h"
#include "SystemErds.h"
#include "Constants_Binary.h"

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
   .internalFilteredSensorIsValidErd = Erd_AmbientHumidity_SensorIsValid,
   .externalFilteredSensorErd = Erd_ExternalAmbientFilteredRelativeHumidityPercentx100,
   .externalFilteredSensorIsValidErd = Erd_ExternalAmbientHumidity_IsValid,
   .resolvedFilteredSensorErd = Erd_Ambient_FilteredHumidityResolvedPercentx100,
   .resolvedFilteredSensorIsValidErd = Erd_AmbientHumidity_IsValidResolved,
   .sensorType = ExternalFilteredSensorResolver_SensorType_Humidity
};

void ExternalFilteredSensorResolverPlugin_Init(ExternalFilteredSensorResolverPlugin_t *instance, I_DataModel_t *dataModel)
{
   ExternalFilteredSensorResolver_Init(
      &instance->externalFilteredAmbientTemperatureSensorResolver,
      dataModel,
      &externalFilteredAmbientTemperatureResolver,
      PersonalityParametricData_Get(dataModel)->sensorData->ambientThermistor);

   ExternalFilteredSensorResolver_Init(
      &instance->externalFilteredAmbientHumiditySensorResolver,
      dataModel,
      &externalFilteredAmbientHumiditySensorResolver,
      PersonalityParametricData_Get(dataModel)->sensorData->ambientHumidityThermistor);
}
