/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DISPENSINGINHIBITEDREASONBITMAP_H
#define DISPENSINGINHIBITEDREASONBITMAP_H

#include <stdint.h>

enum
{
   DispensingInhibitedReason_WaterDueToDoorOpen,
   DispensingInhibitedReason_IceDueToDoorOpen,
   DispensingInhibitedReason_WaterDueToRfidFilter
};
typedef uint8_t DispensingInhibitedReason_t;

typedef struct
{
   uint8_t bitmap[4];
} DispensingInhibitedReasonBitmap_t;

#endif
