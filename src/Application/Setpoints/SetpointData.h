/*!
 * @file
 * @brief setpoint parametric data
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SETPOINTDATA_H
#define SETPOINTDATA_H

#include "SetpointZoneData.h"
#include "UserSetpointData.h"
#include "TurboModeSetpointData.h"

typedef struct
{
   const SetpointZoneData_t *setpointZoneData;
   const UserSetpointData_t *userSetpointData;
} SetpointData_t;

#endif
