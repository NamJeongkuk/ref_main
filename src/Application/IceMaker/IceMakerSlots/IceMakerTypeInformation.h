/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ICEMAKERTYPEINFORMATION_H
#define ICEMAKERTYPEINFORMATION_H

#include "IceMakerLocation.h"
#include "IceMakerType.h"

typedef struct
{
   IceMakerType_t type;
   IceMakerLocation_t location;
} IceMakerTypeInformation_t;

#endif
