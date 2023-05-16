/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SIGNOFLIFEPLUGIN_H
#define SIGNOFLIFEPLUGIN_H

#include "UiHeartbeatTickMonitor.h"

typedef struct
{
   struct
   {
      UiHeartbeatTickMonitor_t uiHeartbeatTickMonitor;
   } _private;
} SignOfLifePlugin_t;

/*!
 * Plugin for Heartbeat tick and sign of life
 * @param instance
 * @param dataModel
 * @return
 */
void SignOfLifePlugin_Init(
   SignOfLifePlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
