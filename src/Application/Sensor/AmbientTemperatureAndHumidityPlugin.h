/*!
 * @file
 * @brief Header file creates a AmbientTemperaturePlugin type that stores an instance to access data in the AmbientTemperatureAverage type.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef AMBIENTTEMPERATUREPLUGIN_H
#define AMBIENTTEMPERATUREPLUGIN_H

#include "I_DataModel.h"
#include "OverrideArbiter.h"
#include "ExternalFilteredSensorResolver.h"
#include "AmbientTemperatureAverage.h"
#include "SensorFiltering.h"

typedef struct
{
   SensorFiltering_t ambientTemperatureSensorFiltering;
   SensorFiltering_t ambientHumiditySensorFiltering;
   OverrideArbiter_t ambientFilteredTemperatureArbiter;
   OverrideArbiter_t ambientThermistorValidArbiter;
   OverrideArbiter_t ambientHumiditySensorValidArbiter;
   OverrideArbiter_t ambientFilteredHumidityArbiter;
   ExternalFilteredSensorResolver_t externalFilteredAmbientTemperatureSensorResolver;
   ExternalFilteredSensorResolver_t externalFilteredAmbientHumiditySensorResolver;
   AmbientTemperatureAverage_t ambientTemperatureAverage;
} AmbientTemperaturePlugin_t;

/*!
 * Initialize Ambient Temperature Plugin
 * @param instance
 * @param dataModel
 */
void AmbientTemperatureAndHumidityPlugin_Init(AmbientTemperaturePlugin_t *instance, I_DataModel_t *dataModel);

#endif
