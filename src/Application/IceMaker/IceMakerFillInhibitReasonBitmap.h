/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ICEMAKERFILLINHIBITREASONBITMAP_H
#define ICEMAKERFILLINHIBITREASONBITMAP_H

#include <stdint.h>

enum
{
   IceMakerFillInhibitedReason_DueToRfidFilter
};
typedef uint8_t IceMakerFillInhibitedReason_t;

typedef struct
{
   uint8_t bitmap[1];
} IceMakerFillInhibitedReasonBitmap_t;

#endif
