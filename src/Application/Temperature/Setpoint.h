/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SETPOINT_H
#define SETPOINT_H

#include "TemperatureDegFx100.h"
#include <stdint.h>

typedef int8_t Setpoint_t;

typedef struct
{
   TemperatureDegFx100_t temperature;
   bool care;
} SetpointVotedTemperature_t;

#endif
