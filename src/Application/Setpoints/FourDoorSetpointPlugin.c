/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "FourDoorSetpointPlugin.h"

void FourDoorSetpointPlugin_Init(FourDoorSetpointPlugin_t *instance, I_DataModel_t *dataModel)
{
   FourDoorSetpointResolverPlugin_Init(&instance->_private.setpointResolverPlugin, dataModel);
   FourDoorUserSetpointPlugin_Init(&instance->_private.userSetpointPlugin, dataModel);
   FourDoorSetpointZonePlugin_Init(&instance->_private.setpointZonePlugin, dataModel);
   FourDoorAdjustedSetpointPlugin_Init(&instance->_private.adjustedSetpointPlugin, dataModel);
   FourDoorTurboModeSetpointPlugin_Init(&instance->_private.turboModeSetpointPlugin, dataModel);
}
