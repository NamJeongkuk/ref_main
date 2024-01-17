/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ICEMAKERDATA_H
#define ICEMAKERDATA_H

#include "IceMakerFillBlockerData.h"
#include "IceMakerSlotsData.h"

typedef struct
{
   IceMakerSlotsData_t *iceMakerSlots;
   IceMakerFillBlockerData_t *iceMakerFillBlockerData;
} IceMakerData_t;

#endif
