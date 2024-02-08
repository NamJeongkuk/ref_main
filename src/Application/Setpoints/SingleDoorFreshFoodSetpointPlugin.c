/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SingleDoorFreshFoodSetpointPlugin.h"

void SingleDoorFreshFoodSetpointPlugin_Init(
   SingleDoorFreshFoodSetpointPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   SingleDoorFreshFoodSetpointResolverPlugin_Init(&instance->_private.setpointResolverPlugin, dataModel);
   SingleDoorFreshFoodUserSetpointPlugin_Init(&instance->_private.userSetpointPlugin, dataModel);
   SingleDoorFreshFoodSetpointZonePlugin_Init(&instance->_private.setpointZonePlugin, dataModel);
   SingleDoorFreshFoodAdjustedSetpointPlugin_Init(&instance->_private.adjustedSetpointPlugin, dataModel);
   SingleDoorFreshFoodTurboModeSetpointPlugin_Init(&instance->_private.turboModeSetpointPlugin, dataModel);
}
