/*!
 * @file
 * @brief Header file creates a AmbientTemperaturePlugin type that stores an instance to access data in the AmbientTemperatureAverage type.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef AMBIENTTEMPERATUREPLUGIN_H
#define AMBIENTTEMPERATUREPLUGIN_H

#include "I_DataModel.h"
#include "AmbientTemperatureAverage.h"
#include "OverrideArbiter.h"

typedef struct
{
   OverrideArbiter_t ambientFilteredTemperatureArbiter;
   OverrideArbiter_t ambientThermistorValidArbiter;
   AmbientTemperatureAverage_t ambientTemperatureAverage;
} AmbientTemperaturePlugin_t;

/*!
 * Initialize Ambient Temperature Plugin
 * @param instance
 * @param dataModel
 */
void AmbientTemperaturePlugin_Init(AmbientTemperaturePlugin_t *instance, I_DataModel_t *dataModel);

#endif
