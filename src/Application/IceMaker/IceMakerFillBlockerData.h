/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ICEMAKERFILLBLOCKERDATA_H
#define ICEMAKERFILLBLOCKERDATA_H

#include <stdint.h>

typedef struct
{
   uint32_t newFilterDispenserPurgeVolumeInOuncesX100;
   uint16_t newFilterFillStateDelayInMinutes;
} IceMakerFillBlockerData_t;

#endif
