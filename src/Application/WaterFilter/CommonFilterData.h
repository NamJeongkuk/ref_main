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
   uint32_t filterRatedVolumeInOuncesX100;
   uint32_t filterRatedLifeInMinutes;
   uint16_t minimumVolumeNeededToStartFilterLifeTimerInOunces;
   uint16_t filterMonthInMinutes;
} CommonFilterData_t;

#endif