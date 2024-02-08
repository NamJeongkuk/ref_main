/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SINGLEDOORFRESHFOODSETPOINTZONEPLUGIN_H
#define SINGLEDOORFRESHFOODSETPOINTZONEPLUGIN_H

#include "I_DataModel.h"
#include "SetpointZoneResolver.h"

typedef struct
{
   struct
   {
      SetpointZoneResolver_t freshFoodSetpointZoneResolver;
   } _private;
} SingleDoorFreshFoodSetpointZonePlugin_t;

void SingleDoorFreshFoodSetpointZonePlugin_Init(
   SingleDoorFreshFoodSetpointZonePlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
