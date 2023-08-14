/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SETPOINTVOTEDTEMPERATURE_H
#define SETPOINTVOTEDTEMPERATURE_H

#include "TemperatureDegFx100.h"
#include "Vote.h"

typedef struct
{
   TemperatureDegFx100_t temperatureInDegFx100;
   Vote_t care;
} SetpointVotedTemperature_t;

#define SWAP_DEFINITION_SetpointVotedTemperature_t(_name, _type) \
   SWAPPED_FIELD(_name, _type, temperatureInDegFx100),

#endif
