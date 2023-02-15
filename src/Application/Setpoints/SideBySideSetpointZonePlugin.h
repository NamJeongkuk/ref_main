/*!
 * @file
 * @brief plugin to create all instances of setpoint zone logic
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SIDEBYSIDESETPOINTZONEPLUGIN_H
#define SIDEBYSIDESETPOINTZONEPLUGIN_H

#include "I_DataModel.h"
#include "SetpointZoneResolver.h"

typedef struct
{
   struct
   {
      SetpointZoneResolver_t freezerSetpointZoneResolver;
      SetpointZoneResolver_t freshFoodSetpointZoneResolver;
   } _private;
} SideBySideSetpointZonePlugin_t;

/*!
 * @param instance
 * @param dataModel
 */
void SideBySideSetpointZonePlugin_Init(
   SideBySideSetpointZonePlugin_t *instance,
   I_DataModel_t *dataModel);
#endif
