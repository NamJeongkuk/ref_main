/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef COOLINGMODE_H
#define COOLINGMODE_H

#include <stdint.h>

enum
{
   CoolingMode_FreshFood,
   CoolingMode_Freezer,
   CoolingMode_ConvertibleCompartment,
   CoolingMode_Unknown,
};
typedef uint8_t CoolingMode_t;

#endif
