/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef MEMORYMAP_H
#define MEMORYMAP_H

#include "ImageHeader.h"

#define BootLoaderHeaderAddress ((const ImageHeader_t *)0x000000B0)
#define ApplicationHeaderAddress ((const ImageHeader_t *)0x00004000)

#endif
