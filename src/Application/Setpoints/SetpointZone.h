/*!
 * @file
 * @brief setpoint temperature zone type
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SETPOINTZONE_H
#define SETPOINTZONE_H

#include <stdint.h>

enum
{
   SetpointZone_Cold,
   SetpointZone_Middle,
   SetpointZone_Warm
};
typedef uint8_t SetpointZone_t;

#endif
