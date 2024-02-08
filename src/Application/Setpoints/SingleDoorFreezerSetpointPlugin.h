/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SINGLEDOORFREEZERSETPOINTPLUGIN_H
#define SINGLEDOORFREEZERSETPOINTPLUGIN_H

#include "SingleDoorFreezerAdjustedSetpointPlugin.h"
#include "SingleDoorFreezerUserSetpointPlugin.h"
#include "SingleDoorFreezerSetpointZonePlugin.h"
#include "SingleDoorFreezerSetpointResolverPlugin.h"
#include "SingleDoorFreezerTurboModeSetpointPlugin.h"

typedef struct
{
   struct
   {
      SingleDoorFreezerAdjustedSetpointPlugin_t adjustedSetpointPlugin;
      SingleDoorFreezerUserSetpointPlugin_t userSetpointPlugin;
      SingleDoorFreezerSetpointZonePlugin_t setpointZonePlugin;
      SingleDoorFreezerSetpointResolverPlugin_t setpointResolverPlugin;
      SingleDoorFreezerTurboModeSetpointPlugin_t turboModeSetpointPlugin;
   } _private;
} SingleDoorFreezerSetpointPlugin_t;

void SingleDoorFreezerSetpointPlugin_Init(
   SingleDoorFreezerSetpointPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
