/*!
 * @file
 * @brief plugin to create all instances of setpoint zone logic
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SETPOINTZONEPLUGIN_H
#define SETPOINTZONEPLUGIN_H

#include "I_DataModel.h"
#include "SetpointZoneResolver.h"

typedef struct
{
   struct
   {
      SetpointZoneResolver_t freezerSetpointZoneResolver;
      SetpointZoneResolver_t freshFoodSetpointZoneResolver;
      SetpointZoneResolver_t convertibleCompartmentSetpointZoneResolver;
   } _private;
} SetpointZonePlugin_t;

/*!
 * @param instance
 * @param dataModel
 */
void SetpointZonePlugin_Init(
   SetpointZonePlugin_t *instance,
   I_DataModel_t *dataModel);
#endif
