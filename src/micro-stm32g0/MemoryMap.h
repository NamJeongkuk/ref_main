/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef MEMORYMAP_H
#define MEMORYMAP_H

#include "ImageHeader.h"

extern char __Header;
extern char __ApplicationStart;
extern char __ApplicationEnd;

#define BootLoaderImageHeader ((const ImageHeader_t *)(intptr_t)0x080000B8)

#define ApplicationStartAddress (&__ApplicationStart)
#define ApplicationEndAddress (&__ApplicationEnd)
#define ApplicationImageHeader (const ImageHeader_t *)(&__Header)

#endif
