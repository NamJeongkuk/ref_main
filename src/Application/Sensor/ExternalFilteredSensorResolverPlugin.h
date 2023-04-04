/*!
 * @file
 * @brief External Filtered Sensor Plugin
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef EXTERNALFILTEREDSENSORRESOLVERPLUGIN_H
#define EXTERNALFILTEREDSENSORRESOLVERPLUGIN_H

#include "ExternalFilteredSensorResolver.h"

typedef struct
{
   ExternalFilteredSensorResolver_t externalFilteredAmbientTemperatureSensorResolver;
   ExternalFilteredSensorResolver_t externalFilteredAmbientHumiditySensorResolver;
} ExternalFilteredSensorResolverPlugin_t;

/*!
 * Initialize External Filtered Sensor Plugin
 * @param instance
 * @param dataModel
 */
void ExternalFilteredSensorResolverPlugin_Init(
   ExternalFilteredSensorResolverPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
