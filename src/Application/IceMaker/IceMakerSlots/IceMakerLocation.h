/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ICEMAKERLOCATION_H
#define ICEMAKERLOCATION_H

#include <stdint.h>

enum
{
   IceMakerLocation_Freezer,
   IceMakerLocation_FreshFood,
   IceMakerLocation_Disabled = 0xFF
};
typedef uint8_t IceMakerLocation_t;

#endif
