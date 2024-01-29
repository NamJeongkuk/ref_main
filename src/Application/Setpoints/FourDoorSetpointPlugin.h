/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FOURDOORSETPOINTPLUGIN_H
#define FOURDOORSETPOINTPLUGIN_H

#include "I_DataModel.h"
#include "FourDoorAdjustedSetpointPlugin.h"
#include "FourDoorUserSetpointPlugin.h"
#include "FourDoorSetpointZonePlugin.h"
#include "FourDoorSetpointResolverPlugin.h"
#include "FourDoorTurboModeSetpointPlugin.h"

typedef struct
{
   struct
   {
      FourDoorAdjustedSetpointPlugin_t adjustedSetpointPlugin;
      FourDoorUserSetpointPlugin_t userSetpointPlugin;
      FourDoorSetpointZonePlugin_t setpointZonePlugin;
      FourDoorSetpointResolverPlugin_t setpointResolverPlugin;
      FourDoorTurboModeSetpointPlugin_t turboModeSetpointPlugin;
   } _private;
} FourDoorSetpointPlugin_t;

void FourDoorSetpointPlugin_Init(FourDoorSetpointPlugin_t *instance, I_DataModel_t *dataModel);

#endif
