/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef MEMORYMAP_H
#define MEMORYMAP_H

#include "ImageHeader.h"

extern char bootLoaderStart;

extern char applicationStart;
extern char applicationEnd;

#define BootLoaderImageHeader ((const ImageHeader_t *)&bootLoaderStart)

#define ApplicationStartAddress ((uintptr_t)&applicationStart)
#define ApplicationEndAddress ((uintptr_t)&applicationEnd)
#define ApplicationImageHeader (const ImageHeader_t *)(intptr_t)(ApplicationStartAddress)

#endif
