/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef MEMORYMAP_H
#define MEMORYMAP_H

#include "ImageHeader.h"

extern char _bootLoaderHeaderStart;

extern char _applicationStart;
extern char _applicationEnd;
extern char _applicationHeaderStart;

extern char _parametricHeaderStart;

#define BootLoaderImageHeader ((const ImageHeader_t *)(intptr_t)&_bootLoaderHeaderStart)

#define ApplicationStartAddress ((uintptr_t)&_applicationStart)
#define ApplicationEndAddress ((uintptr_t)&_applicationEnd)
#define ApplicationImageHeader ((const ImageHeader_t *)(intptr_t)&_applicationHeaderStart)

#define ParametricImageHeader ((const ImageHeader_t *)(intptr_t)&_parametricHeaderStart)

#endif
