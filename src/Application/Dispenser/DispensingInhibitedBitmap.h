/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DISPENSINGINHIBITEDBITMAP_H
#define DISPENSINGINHIBITEDBITMAP_H

#include <stdint.h>

enum
{
   DispensingInhibitedBitmapIndex_WaterDueToDoorOpen,
   DispensingInhibitedBitmapIndex_IceDueToDoorOpen,
   DispensingInhibitedBitmapIndex_WaterDueToRfidFilter,
};
typedef uint8_t DispensingInhibitedBitmapIndex_t;

typedef uint32_t DispensingInhibitedBitmap_t;

#define SWAP_DEFINITION_DispensingInhibitedBitmap_t(_name, _type) \
   SWAPPED_ERD(_name, sizeof(_type)),

#endif
