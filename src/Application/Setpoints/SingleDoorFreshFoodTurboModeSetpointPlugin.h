/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SINGLEDOORFRESHFOODTURBOMODESETPOINTPLUGIN_H
#define SINGLEDOORFRESHFOODTURBOMODESETPOINTPLUGIN_H

#include "SetpointTurboMode.h"

typedef struct
{
   struct
   {
      SetpointTurboMode_t freshFoodSetpointTurboCool;
   } _private;
} SingleDoorFreshFoodTurboModeSetpointPlugin_t;

void SingleDoorFreshFoodTurboModeSetpointPlugin_Init(
   SingleDoorFreshFoodTurboModeSetpointPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
