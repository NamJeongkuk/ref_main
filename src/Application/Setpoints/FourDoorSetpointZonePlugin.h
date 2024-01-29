/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FOURDOORSETPOINTZONEPLUGIN_H
#define FOURDOORSETPOINTZONEPLUGIN_H

#include "I_DataModel.h"
#include "SetpointZoneResolver.h"

typedef struct
{
   struct
   {
      SetpointZoneResolver_t freezerSetpointZoneResolver;
      SetpointZoneResolver_t freshFoodSetpointZoneResolver;
   } _private;
} FourDoorSetpointZonePlugin_t;

void FourDoorSetpointZonePlugin_Init(FourDoorSetpointZonePlugin_t *instance, I_DataModel_t *dataModel);

#endif
