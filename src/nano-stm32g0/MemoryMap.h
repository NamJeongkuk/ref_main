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

#define ApplicationStartAddress (0x08003000)
#define ApplicationEndAddress (0x0801FFFF)
#define ApplicationHeaderAddress (&__Header)
#define ApplicationImageHeader (const ImageHeader_t *)(intptr_t)(ApplicationHeaderAddress)

#endif
