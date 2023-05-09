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

#define SWAP_DEFINITION_SetpointZoneTemperatureBounds_t(_name, _type) \
   SWAPPED_FIELD(_name, _type, coldSetpointBoundInDegFx100),          \
      SWAPPED_FIELD(_name, _type, warmSetpointBoundInDegFx100),

#endif
