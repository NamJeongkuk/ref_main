/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <stddef.h>
#include "ImageHeader.h"
#include "MemoryMap.h"
#include "utils.h"

enum
{
   DummyCrc = 0x1234,

   HardwareVersion = 42,
   ImageId = 42,

   ImageParametricItemSize = 0,
   ImageParametricItemCount = 0,
   ImageParametricPointer = 0
};

extern void Reset_Handler(void);

static const ImageHeader_t applicationHeader __attribute__((used, section(".header"))) = {
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

   ImageParametricItemSize,
   ImageParametricItemCount,

   ImageParametricPointer,

   (ImageHeaderPointer_t)Reset_Handler,

   ImageHeaderFillValue,
   ImageHeaderFillValue,

   1,
   {
      {
         (uint32_t)ApplicationStartAddress,
         (uint32_t)ApplicationEndAddress,
      },
   },
};
