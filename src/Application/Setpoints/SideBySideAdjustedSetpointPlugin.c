/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SideBySideAdjustedSetpointPlugin.h"
#include "SystemErds.h"
#include "Constants_Binary.h"

void SideBySideAdjustedSetpointPlugin_Init(
   SideBySideAdjustedSetpointPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   FreezerAdjustedSetpointPlugin_Init(&instance->_private.freezerAdjustedSetpointPlugin, dataModel);
   FreshFoodAdjustedSetpointPlugin_Init(&instance->_private.freshFoodAdjustedSetpointPlugin, dataModel);

   DataModel_Write(dataModel, Erd_AdjustedSetpointPluginReady, set);
}
