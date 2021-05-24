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
extern char __Boot_Loader_Start;
extern char __Boot_Loader_End;

#define ApplicationStartAddress (0x10008000)
#define ApplicationEndAddress (0x101FFFFF)
#define ApplicationHeaderAddress ApplicationStartAddress
#define ApplicationImageHeader (const ImageHeader_t *)(intptr_t)(ApplicationHeaderAddress)

#define ParametricStartAddress (0x00000000)
#define ParametricEndAddress (0x00000000)
#define ParametricHeaderAddress ParametricStartAddress
#define ParametricImageHeader (const ImageHeader_t *)(intptr_t)(ParametricHeaderAddress)

#define BootLoaderStartAddress (0x10002000)
#define BootLoaderEndAddress (0x10007FFF)
#define BootLoaderHeaderAddress (0x100022e0)
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
