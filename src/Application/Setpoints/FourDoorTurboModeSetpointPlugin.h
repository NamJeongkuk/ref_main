/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FOURDOORTURBOMODESETPOINTPLUGIN_H
#define FOURDOORTURBOMODESETPOINTPLUGIN_H

#include "I_DataModel.h"
#include "SetpointTurboMode.h"

typedef struct
{
   struct
   {
      SetpointTurboMode_t freshFoodSetpointTurboCool;
      SetpointTurboMode_t freezerSetpointTurboFreeze;
   } _private;
} FourDoorTurboModeSetpointPlugin_t;

void FourDoorTurboModeSetpointPlugin_Init(FourDoorTurboModeSetpointPlugin_t *instance, I_DataModel_t *dataModel);

#endif
