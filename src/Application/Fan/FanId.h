/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FANID_H
#define FANID_H

#include <stdint.h>

enum
{
   FanId_Freezer,
   FanId_Condenser,
   FanId_FreshFood,
   FanId_IceCabinet,
   FanId_ConvertibleCompartment,
   FanId_NumerOfFans
};
typedef uint8_t FanId_t;

#endif
