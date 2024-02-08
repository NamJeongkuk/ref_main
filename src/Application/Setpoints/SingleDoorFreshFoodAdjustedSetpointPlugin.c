/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SingleDoorFreshFoodAdjustedSetpointPlugin.h"
#include "SystemErds.h"
#include "Constants_Binary.h"

void SingleDoorFreshFoodAdjustedSetpointPlugin_Init(
   SingleDoorFreshFoodAdjustedSetpointPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   DataModel_Write(
      dataModel, 
      Erd_AdjustedSetpointPluginReady, 
      set);

   FreshFoodAdjustedSetpointPlugin_Init(&instance->_private.freshFoodAdjustedSetpointPlugin, dataModel);
}
