/*!
 * @file
 * @brief This file is to create a Ice Maker Test Request type.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ICEMAKERTESTREQUEST_H
#define ICEMAKERTESTREQUEST_H

#include <stdint.h>

enum
{
   IceMakerTestRequest_None,
   IceMakerTestRequest_Fill,
   IceMakerTestRequest_Harvest
};
typedef uint8_t IceMakerTestRequest_t;

#endif
