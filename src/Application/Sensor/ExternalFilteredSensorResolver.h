/*!
 * @file
 * @brief create a fallback values from the external sensors if supported or if internal sensor is invalid.
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef EXTERNALFILTEREDSENSORRESOLVER_H
#define EXTERNALFILTEREDSENSORRESOLVER_H

#include "I_DataModel.h"
#include "SensorData.h"

enum
{
   ExternalFilteredSensorResolver_SensorType_Temperature,
   ExternalFilteredSensorResolver_SensorType_Humidity,
};

typedef struct
{
   Erd_t internalFilteredSensorErd; // TemperatureDegFx100_t or RelativeHumidityPercentx100_t
   Erd_t internalFilteredSensorIsValidErd; // bool
   Erd_t externalFilteredSensorErd; // TemperatureDegFx100_t or RelativeHumidityPercentx100_t
   Erd_t externalFilteredSensorIsValidErd; // bool
   Erd_t resolvedFilteredSensorErd; // TemperatureDegFx100_t or RelativeHumidityPercentx100_t
   Erd_t resolvedFilteredSensorIsValidErd; // bool
   uint8_t sensorType;
} ExternalFilteredSensorResolverConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      const ExternalFilteredSensorResolverConfig_t *config;
      const SensorDataSensorType_t *ambientSensorData;
      EventSubscription_t onDataModelChangeSubscription;
   } _private;
} ExternalFilteredSensorResolver_t;

/*!
 * @param instance
 * @param dataModel
 * @param config
 * @param ambientSensorData
 */
void ExternalFilteredSensorResolver_Init(
   ExternalFilteredSensorResolver_t *instance,
   I_DataModel_t *dataModel,
   const ExternalFilteredSensorResolverConfig_t *config,
   const SensorDataSensorType_t *ambientSensorData);

#endif
