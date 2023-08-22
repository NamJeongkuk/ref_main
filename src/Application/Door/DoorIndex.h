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
   DoorIndex_LeftSideFreshFood,
   DoorIndex_RightSideFreshFood,
   DoorIndex_LeftSideFreezer,
   DoorIndex_RightSideFreezer,
   DoorIndex_ConvertibleCompartment,
   DoorIndex_DoorInDoor,
   DoorIndex_MaxNumberOfDoors
};
typedef uint8_t DoorIndex_t;

#endif
