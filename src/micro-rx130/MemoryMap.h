/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef MEMORYMAP_H
#define MEMORYMAP_H

#include "ImageHeader.h"

#define BootLoaderImageHeader ((const ImageHeader_t *)(intptr_t)0xFFFFB000)

#define ApplicationStartAddress (0xFFF80000)
#define ApplicationEndAddress (0xFFFF5FFF)
#define ApplicationImageHeader (const ImageHeader_t *)(intptr_t)(ApplicationStartAddress)

#endif
