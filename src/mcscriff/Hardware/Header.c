/*!
 * @file
 * @brief Application header.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <stddef.h>
#include "Header.h"
#include "utils.h"

enum
{
   DummyCrc = 0x1234,
   TargetMemorySizeInK = 64,
   WriteAlignment = 8,

   ApplicationIncluded = true,
   ParametricIncluded = false,
   AuxiliaryIncluded = false,

   ApplicationImageId = 4,
   ParametricDataImageId = 4,
   AuxiliaryImageId = 4,
   HardwareVersion = 4,

   ImageParametricItemSize = 0,
   ImageParametricItemCount = 0,
   ImageParametricPointer = 0
};

typedef void (*BootLoaderJumpJunction_t)(void);

// clang-format off
static const ImageHeader_t applicationHeader __attribute__ ((used, section(".header"))) =
   {
      DummyCrc,
      DummyCrc,

      CRIT_VERSION_MAJOR,
      CRIT_VERSION_MINOR,
      NONCRIT_VERSION_MAJOR,
      NONCRIT_VERSION_MINOR,

      ApplicationImageId,
      ParametricDataImageId,
      AuxiliaryImageId,

      HardwareVersion,

      ImageType_Application,
      ImageHeader_FillValue,

      ImageParametricItemSize,
      ImageParametricItemCount,

      { ImageParametricPointer },

      { Reset_Handler },

      ImageHeader_FillValue,
      ImageHeader_FillValue,

      1,
      {
         {
            (uint32_t)ApplicationStartAddress,
            (uint32_t)ApplicationEndAddress
         }
      }
   };
// clang-format on

static void EnterBootLoader(I_Action_t *_instance)
{
   BootLoaderJumpJunction_t bootLoaderJumpFunction;
   const ImageHeader_t *bootLoaderHeader;

   IGNORE_ARG(_instance);

   bootLoaderHeader = Header_GetImageHeader(ImageType_BootLoader);
   bootLoaderJumpFunction = (BootLoaderJumpJunction_t)bootLoaderHeader->imageExecutionStart.pointer;
   bootLoaderJumpFunction();
}

static const I_Action_Api_t enterBootLoaderActionApi =
   { EnterBootLoader };

I_Action_t *Header_GetEnterBootLoaderAction(void)
{
   static I_Action_t action =
      { &enterBootLoaderActionApi };

   return &action;
}

const ImageHeader_t *Header_GetImageHeader(ImageType_t imageType)
{
   switch(imageType)
   {
      case ImageType_BootLoader:
         return (const ImageHeader_t *)BootLoaderHeaderAddress;

      case ImageType_Application:
         return &applicationHeader;

      case ImageType_Parametric:
         return (const ImageHeader_t *)ParametricHeaderAddress;

      default:
         return NULL;
   }
}
