/*!
 * @file
 * @brief Application header.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <stddef.h>
#include "Header.h"
#include "utils.h"

#define BootLoaderStartAddress FirstAddressInBlock(19)
#define BootLoaderEndAddress LastAddressInBlock(0)
#define BootLoaderHeaderAddress BootLoaderStartAddress

#define ParametricStartAddress 0
#define ParametricEndAddress 0
#define ParametricHeaderAddress 0

#define ApplicationStartAddress FirstAddressInBlock(127)
#define ApplicationEndAddress LastAddressInBlock(20)
#define ApplicationHeaderAddress ApplicationStartAddress

enum
{
   DummyCrc = 0x1234,
   TargetMemorySizeInK = 128,

   ApplicationIncluded = true,
   ParametricIncluded = false,
   AuxiliaryIncluded = false,

   ApplicationImageId = 126,
   ParametricDataImageId = 126,
   AuxiliaryImageId = 126,

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
