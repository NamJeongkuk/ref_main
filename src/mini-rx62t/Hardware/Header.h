/*!
 * @file
 * @brief Provides access to the image headers.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef HEADER_H
#define HEADER_H

#include <stdbool.h>
#include "I_Action.h"
#include "ImageHeader.h"

extern char bootLoaderStart;
extern char bootLoaderEnd;

extern char applicationStart;
extern char applicationEnd;

#define BootLoaderStartAddress ((uintptr_t)&bootLoaderStart)
#define BootLoaderEndAddress ((uintptr_t)&bootLoaderEnd)
#define BootLoaderHeaderAddress BootLoaderStartAddress
#define BootLoaderImageHeader (const ImageHeader_t *)(intptr_t)(BootLoaderHeaderAddress)

#define ApplicationStartAddress ((uintptr_t)&applicationStart)
#define ApplicationEndAddress ((uintptr_t)&applicationEnd)
#define ApplicationHeaderAddress ApplicationStartAddress
#define ApplicationImageHeader (const ImageHeader_t *)(intptr_t)(ApplicationHeaderAddress)

#define ParametricHeaderAddress (0x00000000)
#define ParametricImageHeader (const ImageHeader_t *)(intptr_t)(ParametricHeaderAddress)

/*!
 * Get the Action that is used to enter the Boot Loader
 * @return I_Action_t that is used to enter the Boot Loader
 */
I_Action_t *Header_GetEnterBootLoaderAction(void);

/*!
 * Get the Image Header for a specified image
 * @param imageType The image type to be retrieved
 * @return ImageHeader_t for the specified image type
 */
const ImageHeader_t *Header_GetImageHeader(ImageType_t imageType);

/*!
 * Image execution start function
 */
void PowerON_Reset(void);

#endif
