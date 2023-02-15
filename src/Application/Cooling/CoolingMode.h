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
   CoolingMode_Off,
   CoolingMode_FreshFood,
   CoolingMode_Freezer,
   CoolingMode_ConvertibleCompartment
};
typedef uint8_t CoolingMode_t;

#endif
