/*!
 * @file
 * @brief Header file creates a AmbientHumidityPlugin type
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef AMBIENTHUMIDITYPLUGIN_H
#define AMBIENTHUMIDITYPLUGIN_H

#include "I_DataModel.h"
#include "OverrideArbiter.h"

typedef struct
{
   OverrideArbiter_t ambientHumiditySensorValidArbiter;
} AmbientHumidityPlugin_t;

/*!
 * Initialize Ambient Humidity Plugin
 * @param instance
 * @param dataModel
 */
void AmbientHumidityPlugin_Init(
   AmbientHumidityPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
