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
#include "Rx130FlashInfo.h"
#include "ImageHeader.h"
#include "MemoryMap.h"

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
