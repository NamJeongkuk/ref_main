/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "UlRomTest.h"
#include "TinyCrcCalculator.h"
#include "Reset.h"
#include "utils.h"

#define uassert(condition) \
   if(!condition)          \
   Reset()

static struct
{
   const ImageHeader_t *imageHeader;
   uint32_t currentAddress;
   uint16_t currentCrc;
   uint16_t bytesToCrc;
   uint8_t currentPage;
   bool errorEnabled;
} instance;

static uint32_t BytesLeftToCrcOnThisPage(void)
{
   return instance.imageHeader->flashPages[instance.currentPage].endAddress - instance.currentAddress + 1;
}

void UlRomTest_Init(
   const ImageHeader_t *imageHeader,
   uint16_t bytesToCrc,
   bool errorEnabled)
{
   instance.imageHeader = imageHeader;
   instance.bytesToCrc = bytesToCrc;
   instance.errorEnabled = errorEnabled;
   instance.currentCrc = CrcSeed;

   instance.currentPage = 0;
   instance.currentAddress = imageHeader->flashPages[0].startAddress;
}

void UlRomTest_Run(void)
{
   if(instance.currentAddress == (uint32_t)(uintptr_t)instance.imageHeader)
   {
      instance.currentAddress = (uint32_t)(uintptr_t)&instance.imageHeader->headerCrc;
   }

   uint32_t bytesLeftToCrc = BytesLeftToCrcOnThisPage();
   uint8_t bytesToCrc = instance.bytesToCrc;
   bool endOfPage = false;

   if(bytesLeftToCrc <= instance.bytesToCrc)
   {
      bytesToCrc = bytesLeftToCrc;
      endOfPage = true;
   }

   instance.currentCrc = TinyCrcCalculator_CalculateCrc(
      (const void *)(uintptr_t)instance.currentAddress,
      bytesToCrc,
      instance.currentCrc);

   instance.currentAddress += bytesToCrc;

   if(endOfPage)
   {
      instance.currentPage++;

      if(instance.currentPage == instance.imageHeader->flashPageCount)
      {
         if(instance.errorEnabled)
         {
            uassert(instance.imageHeader->imageCrc == instance.currentCrc);
         }

         instance.currentPage = 0;
         instance.currentCrc = CrcSeed;
      }

      instance.currentAddress = instance.imageHeader->flashPages[instance.currentPage].startAddress;
   }
}
