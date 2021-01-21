/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef MEMORYMAP_H
#define MEMORYMAP_H

#include "ImageHeader.h"

extern char _Header;
extern char _ApplicationStart;
extern char _ApplicationEnd;

#define BootLoaderImageHeader ((const ImageHeader_t *)(intptr_t)0x3348)

#define ApplicationStartAddress (&_ApplicationStart)
#define ApplicationEndAddress (&_ApplicationEnd)
#define ApplicationImageHeader (const ImageHeader_t *)(&_Header)

#endif
