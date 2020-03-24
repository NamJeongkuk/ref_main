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
   ImageId = 1
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

      ImageType_BootLoader,
      ImageHeaderFillValue,

      0,
      0,
      0,

      // fixme this needs to point to the vector table
      // this can probably be done by placing the vector table right after
      // the header using the linker flag
      0,
      // fixme oh shit also the boot loader needs to relocate the vector table
      // in the boot loader we need to figure out how to not CRC this (can we exclude
      // it from the image somehow without it getting trimmed?) and then make sure we
      // verify that it has been relocated before jumping to the application

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
