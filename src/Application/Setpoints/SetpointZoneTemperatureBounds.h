/*!
 * @file
 * @brief setpoint temperature bounds type
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SETPOINTZONETEMPERATUREBOUNDS_H
#define SETPOINTZONETEMPERATUREBOUNDS_H

#include "TemperatureDegFx100.h"

typedef struct
{
   TemperatureDegFx100_t coldSetpointBoundInDegFx100;
   TemperatureDegFx100_t warmSetpointBoundInDegFx100;
} SetpointZoneTemperatureBounds_t;

#endif
