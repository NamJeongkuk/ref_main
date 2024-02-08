/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SingleDoorFreezerSetpointPlugin.h"

void SingleDoorFreezerSetpointPlugin_Init(
   SingleDoorFreezerSetpointPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   SingleDoorFreezerSetpointResolverPlugin_Init(&instance->_private.setpointResolverPlugin, dataModel);
   SingleDoorFreezerUserSetpointPlugin_Init(&instance->_private.userSetpointPlugin, dataModel);
   SingleDoorFreezerSetpointZonePlugin_Init(&instance->_private.setpointZonePlugin, dataModel);
   SingleDoorFreezerAdjustedSetpointPlugin_Init(&instance->_private.adjustedSetpointPlugin, dataModel);
   SingleDoorFreezerTurboModeSetpointPlugin_Init(&instance->_private.turboModeSetpointPlugin, dataModel);
}
