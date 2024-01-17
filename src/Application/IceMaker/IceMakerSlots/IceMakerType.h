/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ICEMAKERTYPE_H
#define ICEMAKERTYPE_H

#include <stdint.h>

enum
{
   IceMakerType_AluminumMold,
   IceMakerType_TwistTray,
   IceMakerType_Cartridge,
   IceMakerType_Nugget,
   IceMakerType_Disabled = 0xFF
};
typedef uint8_t IceMakerType_t;

#endif
