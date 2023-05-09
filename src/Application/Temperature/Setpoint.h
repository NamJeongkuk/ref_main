/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SETPOINT_H
#define SETPOINT_H

#include <stdint.h>
#include "TemperatureDegFx100.h"
#include "Vote.h"

typedef int8_t Setpoint_t;

typedef struct
{
   TemperatureDegFx100_t temperatureInDegFx100;
   Vote_t care;
} SetpointVotedTemperature_t;

#define SWAP_DEFINITION_SetpointVotedTemperature_t(_name, _type) \
   SWAPPED_FIELD(_name, _type, temperatureInDegFx100),

#endif
