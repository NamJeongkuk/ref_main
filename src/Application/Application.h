/*!
 * @file
 * @brief Application Glue Code
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef APPLICATION_H
#define APPLICATION_H

#include "I_DataModel.h"
#include "HeartbeatLedPlugin.h"
#include "SwitchedLedPlugin.h"
#include "ServiceDiagnosticsPlugin.h"

typedef struct
{
   struct
   {
      HeartbeatLedPlugin_t heartbeatLedPlugin;
      SwitchedLedPlugin_t switchedLedPlugin;
      ServiceDiagnosticsPlugin_t serviceDiagnosticsPlugin;
   } _private;
} Application_t;

/*!
 * Initialize Application
 * @param instance
 * @param dataModel
 */
void Application_Init(
   Application_t *instance,
   I_DataModel_t *dataModel);

#endif
