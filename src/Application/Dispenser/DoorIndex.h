/*!
 * @file
 * @brief Index for door bits
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DOORINDEX_H
#define DOORINDEX_H

#include <stdint.h>

enum
{
   DoorIndex_LeftHandFreshFood,
   DoorIndex_RightHandFreshFood,
   DoorIndex_LeftHandFreezer,
   DoorIndex_RightHandFreezer,
   DoorIndex_ConvertibleCompartment,
   DoorIndex_DoorInDoor
};
typedef uint8_t DoorIndex_t;

#endif
