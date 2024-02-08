/*!
 * @file
 * @brief Initialize turbo mode for setpoint ERDs
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SINGLEDOORFREEZERTURBOMODESETPOINTPLUGIN_H
#define SINGLEDOORFREEZERTURBOMODESETPOINTPLUGIN_H

#include "I_DataModel.h"
#include "SetpointTurboMode.h"

typedef struct
{
   struct
   {
      SetpointTurboMode_t freezerSetpointTurboFreeze;
   } _private;
} SingleDoorFreezerTurboModeSetpointPlugin_t;

void SingleDoorFreezerTurboModeSetpointPlugin_Init(
   SingleDoorFreezerTurboModeSetpointPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
