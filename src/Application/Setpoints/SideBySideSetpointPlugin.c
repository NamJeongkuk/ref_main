/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SideBySideSetpointPlugin.h"

void SideBySideSetpointPlugin_Init(
   SideBySideSetpointPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   SideBySideSetpointResolverPlugin_Init(&instance->_private.setpointResolverPlugin, dataModel);
   SideBySideAdjustedSetpointPlugin_Init(&instance->_private.adjustedSetpointPlugin, dataModel);
   SideBySideUserSetpointPlugin_Init(&instance->_private.userSetpointPlugin, dataModel);
   SideBySideSetpointZonePlugin_Init(&instance->_private.setpointZonePlugin, dataModel);
}
