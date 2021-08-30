/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef MEMORYMAP_H
#define MEMORYMAP_H

#include "ImageHeader.h"

#define BootLoaderImageHeader ((const ImageHeader_t *)(intptr_t)0xFFFFD000)

#define ApplicationStartAddress (0xFFFC0000)
#define ApplicationEndAddress (0xFFFFCFFF)
#define ApplicationImageHeader (const ImageHeader_t *)(intptr_t)(ApplicationStartAddress)

#endif
