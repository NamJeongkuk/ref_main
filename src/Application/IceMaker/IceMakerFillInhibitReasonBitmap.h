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
   IceMakerFillInhibitedReason_DueToRfidFilter,
   IceMakerFillInhibitedReason_DueToNewFilter
};
typedef uint8_t IceMakerFillInhibitedReason_t;

enum
{
   IceMakerFillInhibitedReasonBitmap_NumberOfBytes = 1
};

typedef struct
{
   uint8_t bitmap[IceMakerFillInhibitedReasonBitmap_NumberOfBytes];
} IceMakerFillInhibitedReasonBitmap_t;

#endif
