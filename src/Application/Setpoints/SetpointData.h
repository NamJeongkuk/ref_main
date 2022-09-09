/*!
 * @file
 * @brief setpoint parametric data
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SETPOINTDATA_H
#define SETPOINTDATA_H

#include "AdjustedSetpointData.h"
#include "SetpointZoneData.h"

typedef struct
{
   const SetpointZoneData_t *setpointZoneData;
   const AdjustedSetpointData_t *adjustedSetpointData;
} SetpointData_t;

#endif
