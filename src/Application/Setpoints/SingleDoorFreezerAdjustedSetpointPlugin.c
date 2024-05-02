/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SingleDoorFreezerAdjustedSetpointPlugin.h"
#include "SystemErds.h"
#include "Constants_Binary.h"

void SingleDoorFreezerAdjustedSetpointPlugin_Init(
   SingleDoorFreezerAdjustedSetpointPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   FreezerAdjustedSetpointPlugin_Init(&instance->_private.freezerAdjustedSetpointPlugin, dataModel);

   DataModel_Write(dataModel, Erd_AdjustedSetpointPluginReady, set);
}
