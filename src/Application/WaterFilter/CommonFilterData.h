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
   uint16_t minimumVolumeNeededToStartFilterLifeTimerInOunces;
   uint16_t filterMonthInMinutes;
} CommonFilterData_t;

#endif
