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
   ImageId = 0xFFFF,
   HardwareVersion = 0xFF
};

extern void Reset(void);

// clang-format off
static const ImageHeader_t bootLoaderHeader __attribute__((used, section(".applicationHeader"))) =
   {
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
      ImageHeader_FillValue,

      0,
      0,

      { .pointer = NULL },

      { .pointer = Reset },

      ImageHeader_FillValue,
      ImageHeader_FillValue,

      1,
      {
         {
            0,
            (32 * 1024) - 1
         }
      }
   };
// clang-format on
