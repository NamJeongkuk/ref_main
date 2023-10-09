/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef COMMONFILTERDATA_H
#define COMMONFILTERDATA_H

#include <stdint.h>

typedef struct
{
   uint32_t maximumFilterVolumeInOuncesX100;
   uint32_t maximumFilterLifeInMinutes;
   uint16_t minimumVolumeNeededToStartFilterLifeTimerInOunces;
   uint16_t filterMonthInMinutes;
} CommonFilterData_t;

#endif
