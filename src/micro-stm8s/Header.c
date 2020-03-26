/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ImageHeader.h"
#include "Parametric.h"
#include "ProgramLoader.h"
#include "MemoryMap.h"

enum
{
   DummyCrc = 0x1234,

   HardwareVersion = 1,
   ImageId = 1,

   ApplicationHeaderSize = sizeof(ImageHeader_t)
};

static const ImageHeader_t __at(ApplicationHeaderAddress) applicationHeader =
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
      ImageHeaderFillValue,

      0,
      0,
      0,

      // The application vector table is placed right after the application
      // header using the --code-loc flag in the makefile
      ApplicationStart + sizeof(ImageHeader_t),

      ImageHeaderFillValue,
      ImageHeaderFillValue,

      1,
      {
         {
            ApplicationStart,
            ApplicationEnd,
         },
      },
   };
