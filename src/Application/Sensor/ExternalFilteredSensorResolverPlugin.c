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
   .externalFilteredSensorErd = Erd_Ambient_ExternalTemperatureInDegFx100,
   .externalFilteredSensorIsValidErd = Erd_Ambient_ExternalThermistorIsValid,
   .resolvedFilteredSensorErd = Erd_Ambient_FilteredTemperatureResolvedInDegFx100,
   .resolvedFilteredSensorIsValidErd = Erd_Ambient_ResolvedThermistorIsValid,
   .sensorType = ExternalFilteredSensorResolver_SensorType_Temperature
};

static const ExternalFilteredSensorResolverConfig_t externalFilteredAmbientHumiditySensorResolver = {
   .internalFilteredSensorErd = Erd_AmbientHumidity_RelativeHumidityPercentx100,
   .internalFilteredSensorIsValidErd = Erd_AmbientHumidity_SensorIsValid,
   .externalFilteredSensorErd = Erd_Ambient_ExternalHumidityPercentx100,
   .externalFilteredSensorIsValidErd = Erd_Ambient_ExternalHumidityPercentx100SensorIsValid,
   .resolvedFilteredSensorErd = Erd_Ambient_ResolvedHumidityPercentx100,
   .resolvedFilteredSensorIsValidErd = Erd_Ambient_ResolvedHumidityPercentx100SensorIsValid,
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
