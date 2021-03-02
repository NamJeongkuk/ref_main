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

extern char __Header;
extern char __Boot_Header;

#define ApplicationStartAddress (&__Header)
#define ApplicationEndAddress (0x01FFFFF)
#define ApplicationHeaderAddress (&__Header)
#define ApplicationImageHeader (const ImageHeader_t *)(intptr_t)(ApplicationHeaderAddress)

#define ParametricStartAddress (0x00000000)
#define ParametricEndAddress (0x00000000)
#define ParametricHeaderAddress ParametricStartAddress
#define ParametricImageHeader (const ImageHeader_t *)(intptr_t)(ParametricHeaderAddress)

#define BootLoaderStartAddress (0x00000500)
#define BootLoaderEndAddress (0x00005FFF)
#define BootLoaderHeaderAddress (&__Boot_Header)
#define BootLoaderImageHeader (const ImageHeader_t *)(intptr_t)(BootLoaderHeaderAddress)

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
void Reset_Handler(void);

#endif
