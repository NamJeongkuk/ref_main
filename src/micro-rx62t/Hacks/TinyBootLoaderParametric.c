/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "TinyBootLoaderParametric.h"
#include "TinyCrcCalculator.h"
#include "TinyString.h"
#include "Crc16.h"
#include "utils.h"

typedef struct
{
   uint8_t length;
   uint8_t padding;
   Crc16_t crc;
} ParametricItemHeader_t;

static uintptr_t ParametricItemAddress(const ImageHeader_t *bootLoaderHeader, uint8_t itemId)
{
   // Hack - disregard item size advertised in header and use 256
   return bootLoaderHeader->imageParametricDataPointer + (itemId - 1) * 256;
}

bool TinyBootLoaderParametric_ReadItem(
   const ImageHeader_t *bootLoaderHeader,
   uint8_t itemId,
   void *buffer,
   uint8_t *length)
{
   ParametricItemHeader_t header;

   TinyMemcpy(&header, (const void *)ParametricItemAddress(bootLoaderHeader, itemId), sizeof(header));
   TinyMemcpy(
      buffer,
      (const void *)(ParametricItemAddress(bootLoaderHeader, itemId) + sizeof(ParametricItemHeader_t)),
      bootLoaderHeader->imageParametricSize - sizeof(ParametricItemHeader_t));

   *length = header.length;

   if(*length > bootLoaderHeader->imageParametricSize)
   {
      return false;
   }
   else
   {
      return header.crc == TinyCrcCalculator_CalculateCrc(buffer, header.length, CrcSeed);
   }
}
