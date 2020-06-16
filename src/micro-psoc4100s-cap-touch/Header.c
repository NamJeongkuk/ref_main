/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <stddef.h>
#include "ImageHeader.h"

enum
{
   DummyCrc = 0x1234,
   ImageId = 3,
   HardwareVersion = 1
};

extern void PowerOnReset(void);

static const ImageHeader_t applicationHeader __attribute__((used, section(".applicationHeader"))) = {
   DummyCrc,
   DummyCrc,

   CRIT_VERSION_MAJOR,
   CRIT_VERSION_MINOR,
   NONCRIT_VERSION_MAJOR,
   NONCRIT_VERSION_MINOR,

   ImageId,
   ImageId,
   ImageId,

   HardwareVersion,

   ImageType_Application,
   ImageHeaderFillValue,

   0,
   0,

   (ImageHeaderPointer_t)NULL,

   (ImageHeaderPointer_t)PowerOnReset,

   ImageHeaderFillValue,
   ImageHeaderFillValue,

   1,
   {
      {
         (ImageHeaderPointer_t)&applicationHeader,
         64 * 1024 - 1,
      },
   },
};
