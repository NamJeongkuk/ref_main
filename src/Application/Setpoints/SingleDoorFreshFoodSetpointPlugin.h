/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SINGLEDOORFRESHFOODSETPOINTPLUGIN_H
#define SINGLEDOORFRESHFOODSETPOINTPLUGIN_H

#include "SingleDoorFreshFoodAdjustedSetpointPlugin.h"
#include "SingleDoorFreshFoodUserSetpointPlugin.h"
#include "SingleDoorFreshFoodSetpointZonePlugin.h"
#include "SingleDoorFreshFoodSetpointResolverPlugin.h"
#include "SingleDoorFreshFoodTurboModeSetpointPlugin.h"

typedef struct
{
   struct
   {
      SingleDoorFreshFoodAdjustedSetpointPlugin_t adjustedSetpointPlugin;
      SingleDoorFreshFoodUserSetpointPlugin_t userSetpointPlugin;
      SingleDoorFreshFoodSetpointZonePlugin_t setpointZonePlugin;
      SingleDoorFreshFoodSetpointResolverPlugin_t setpointResolverPlugin;
      SingleDoorFreshFoodTurboModeSetpointPlugin_t turboModeSetpointPlugin;
   } _private;
} SingleDoorFreshFoodSetpointPlugin_t;

void SingleDoorFreshFoodSetpointPlugin_Init(
   SingleDoorFreshFoodSetpointPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
