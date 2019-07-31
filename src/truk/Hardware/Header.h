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
#include "Rx231FlashInfo.h"
#include "ImageHeader.h"

enum
{
   ApplicationStartBlock = 127,
   ApplicationEndBlock = 20,
   ParametricStartBlock = 19,
   ParametricEndBlock = 10,
   BootLoaderStartBlock = 9,
   BootLoaderEndBlock = 0
};

#define BootLoaderStartAddress FirstAddressInBlock(BootLoaderStartBlock)
#define BootLoaderEndAddress LastAddressInBlock(BootLoaderEndBlock)
#define BootLoaderHeaderAddress BootLoaderStartAddress
#define BootLoaderImageHeader (const ImageHeader_t *)(intptr_t)(BootLoaderHeaderAddress)

#define ParametricStartAddress FirstAddressInBlock(ParametricStartBlock)
#define ParametricEndAddress LastAddressInBlock(ParametricEndBlock)
#define ParametricHeaderAddress ParametricStartAddress
#define ParametricImageHeader (const ImageHeader_t *)(intptr_t)(ParametricHeaderAddress)

#define ApplicationStartAddress FirstAddressInBlock(ApplicationStartBlock)
#define ApplicationEndAddress LastAddressInBlock(ApplicationEndBlock)
#define ApplicationHeaderAddress ApplicationStartAddress
#define ApplicationImageHeader (const ImageHeader_t *)(intptr_t)(ApplicationHeaderAddress)

/*!
 * Get the Action that is used to enter the Boot Loader
 * @return I_Action_t that is used to enter the Boot Loader
 */
I_Action_t * Header_GetEnterBootLoaderAction(void);

/*!
 * Get the Image Header for a specified image
 * @param imageType The image type to be retrieved
 * @return ImageHeader_t for the specified image type
 */
const ImageHeader_t * Header_GetImageHeader(ImageType_t imageType);

/*!
 * Image execution start function
 */
void PowerON_Reset(void);

#endif
