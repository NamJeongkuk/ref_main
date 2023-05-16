/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ICEMAKERTYPEINFORMATION_H
#define ICEMAKERTYPEINFORMATION_H

#include <stdint.h>

enum
{
   IceMakerType_AluminumMold = 0,
   IceMakerType_Twist,
   IceMakerType_Cartridge,
   IceMakerType_Nugget
};
typedef uint8_t IceMakerType_t;

enum
{
   IceMakerLocation_Freezer = 0,
   IceMakerLocation_Fridge
};
typedef uint8_t IceMakerLocation_t;

typedef struct
{
   IceMakerType_t type;
   IceMakerLocation_t location;
} IceMakerTypeInformation_t;

#endif
