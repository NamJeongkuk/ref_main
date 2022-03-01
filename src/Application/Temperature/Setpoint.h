/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SETPOINT_H
#define SETPOINT_H

#include "TemperatureDegFx100.h"

typedef struct
{
   TemperatureDegFx100_t temperature;
   bool care;
} SetpointVotedTemperature_t;

#endif
