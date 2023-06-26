/*!
 * @file
 * @brief Initialize turbo mode for setpoint ERDs
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SIDEBYSIDETURBOMODESETPOINTPLUGIN_H
#define SIDEBYSIDETURBOMODESETPOINTPLUGIN_H

#include "I_DataModel.h"
#include "SetpointTurboMode.h"

typedef struct
{
   struct
   {
      SetpointTurboMode_t freshFoodSetpointTurboCool;
   } _private;
} SideBySideTurboModeSetpointPlugin_t;

/*!
 *
 * @param instance
 * @param dataModel
 */
void SideBySideTurboModeSetpointPlugin_Init(
   SideBySideTurboModeSetpointPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
