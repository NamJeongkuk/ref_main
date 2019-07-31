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
   TargetMemorySizeInK = 256,

   ApplicationIncluded = true,
   ParametricIncluded = false,
   AuxiliaryIncluded = false,

   ApplicationImageId = 1,
   ParametricDataImageId = 1,
   AuxiliaryImageId = 1,

   HardwareVersion = 1,

   ImageParametricItemSize = 0,
   ImageParametricItemCount = 0,
   ImageParametricPointer = 0,
};

typedef void (*BootLoaderJumpJunction_t)(void);

static const ImageHeader_t applicationHeader __attribute__ ((section (".bootLoaderHeader"))) =
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

      { PowerON_Reset },

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

I_Action_t * Header_GetEnterBootLoaderAction(void)
{
   static I_Action_t action =
      { &enterBootLoaderActionApi };

   return &action;
}

const ImageHeader_t * Header_GetImageHeader(ImageType_t imageType)
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
