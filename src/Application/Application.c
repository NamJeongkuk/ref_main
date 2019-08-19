/*!
 * @file
 * @brief Application Glue Code
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <stdint.h>
#include "Application.h"
#include "SystemErds.h"
#include "DataModelErdPointerAccess.h"

void Application_Init(
   Application_t *instance,
   I_DataModel_t *dataModel)
{
   HeartbeatLedPlugin_Init(&instance->_private.heartbeatLedPlugin, dataModel);
   SwitchedLedPlugin_Init(&instance->_private.switchedLedPlugin, dataModel);

   ServiceDiagnosticsPlugin_Init(
      &instance->_private.serviceDiagnosticsPlugin,
      DataModel_AsDataSource(dataModel),
      DataModelErdPointerAccess_GetTimerModule(dataModel, Erd_TimerModule));
}
