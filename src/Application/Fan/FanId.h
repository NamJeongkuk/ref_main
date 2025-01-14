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
   FanId_Condenser,
   FanId_IceCabinet,
   FanId_ConvertibleCompartment,
   FanId_Deli,
   FanId_FreezerEvap,
   FanId_FreshFoodEvap,
   FanId_NumerOfFans
};
typedef uint8_t FanId_t;

#endif
