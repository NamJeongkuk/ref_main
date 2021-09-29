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

   HardwareVersion = 1,
   ImageId = 1,

   ImageParametricItemSize = 0,
   ImageParametricItemCount = 0,
   ImageParametricPointer = 0
};

extern void start(void);

typedef struct
{
   const void *pointer;
   uint16_t zeros;
} ImageHeaderPointerHack_t;

typedef struct
{
   ImageHeaderPointerHack_t startAddress;
   ImageHeaderPointerHack_t endAddress;
} FlashPageRecordHack_t;

typedef struct
{
   uint16_t imageCrc;
   uint16_t headerCrc;

   uint8_t criticalMajorVersion;
   uint8_t criticalMinorVersion;
   uint8_t majorVersion;
   uint8_t minorVersion;

   uint16_t applicationImageId;
   uint16_t parametricImageId;
   uint16_t auxiliaryImageId;

   uint16_t hardwareVersion;

   uint8_t imageType;

   uint8_t filler1;

   uint8_t imageParametricSize;
   uint8_t imageParametricCount;
   ImageHeaderPointerHack_t imageParametricDataPointer;

   ImageHeaderPointerHack_t imageExecutionStart;

   uint16_t filler2;
   uint8_t filler3;

   uint8_t flashPageCount;
   FlashPageRecordHack_t flashPages[1];
} ImageHeaderHack_t;

static const ImageHeaderHack_t applicationHeader __attribute__((used, section(".header"))) = {
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

   { .pointer = NULL, .zeros = 0 },

   { .pointer = start, .zeros = 0 },

   ImageHeaderFillValue,
   ImageHeaderFillValue,

   1,
   {
      {
         { .pointer = ApplicationStartAddress, .zeros = 0 },
         { .pointer = ApplicationEndAddress, .zeros = 0 },
      },
   },
};
