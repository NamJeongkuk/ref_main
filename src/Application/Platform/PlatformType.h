/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef PLATFORMTYPE_H
#define PLATFORMTYPE_H

#include <stdint.h>

enum
{
   PlatformType_SideBySide,
   PlatformType_FourDoor,
   PlatformType_AllFreezer,
   PlatformType_AllFreshFood
};
typedef uint8_t PlatformType_t;

#endif
