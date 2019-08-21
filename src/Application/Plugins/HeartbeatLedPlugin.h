/*!
 * @file
 * @brief Initialize the heartbeat LED
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef HEARTBEATLEDPLUGIN_H
#define HEARTBEATLEDPLUGIN_H

#include "DigitalOutputBlinker.h"
#include "DigitalOutput_OutputAdapter.h"
#include "I_DataModel.h"

typedef struct
{
   struct
   {
      DigitalOutputBlinker_t blinkHeartbeat;
      DigitalOutput_OutputAdapter_t heartbeatLed;
   } _private;
} HeartbeatLedPlugin_t;

/*!
 * Initialize the plugin
 * @param instance
 * @param dataModel
 */
void HeartbeatLedPlugin_Init(
   HeartbeatLedPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
