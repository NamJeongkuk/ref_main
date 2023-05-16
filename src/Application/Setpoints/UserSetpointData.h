/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef USERSETPOINTDATA_H
#define USERSETPOINTDATA_H

#include "Setpoint.h"

typedef struct
{
   Setpoint_t lowTemperatureSetpoint;
   Setpoint_t highTemperatureSetpoint;
   Setpoint_t defaultTemperatureSetpoint;
} UserSetpointRangeData_t;

typedef struct
{
   const UserSetpointRangeData_t *freshFoodUserSetpoint;
   const UserSetpointRangeData_t *freezerUserSetpoint;
} UserSetpointData_t;

#endif
