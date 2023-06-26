/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SIDEBYSIDESETPOINTPLUGIN_H
#define SIDEBYSIDESETPOINTPLUGIN_H

#include "I_DataModel.h"
#include "SideBySideAdjustedSetpointPlugin.h"
#include "SideBySideUserSetpointPlugin.h"
#include "SideBySideSetpointZonePlugin.h"
#include "SideBySideSetpointResolverPlugin.h"
#include "SideBySideTurboModeSetpointPlugin.h"

typedef struct
{
   struct
   {
      SideBySideAdjustedSetpointPlugin_t adjustedSetpointPlugin;
      SideBySideUserSetpointPlugin_t userSetpointPlugin;
      SideBySideSetpointZonePlugin_t setpointZonePlugin;
      SideBySideSetpointResolverPlugin_t setpointResolverPlugin;
      SideBySideTurboModeSetpointPlugin_t turboModeSetpointPlugin;
   } _private;
} SideBySideSetpointPlugin_t;

void SideBySideSetpointPlugin_Init(
   SideBySideSetpointPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
