/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SINGLEDOORFREEZERSETPOINTZONEPLUGIN_H
#define SINGLEDOORFREEZERSETPOINTZONEPLUGIN_H

#include "I_DataModel.h"
#include "SetpointZoneResolver.h"

typedef struct
{
   struct
   {
      SetpointZoneResolver_t freezerSetpointZoneResolver;
   } _private;
} SingleDoorFreezerSetpointZonePlugin_t;

void SingleDoorFreezerSetpointZonePlugin_Init(
   SingleDoorFreezerSetpointZonePlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
