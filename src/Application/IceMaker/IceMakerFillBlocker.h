/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ICEMAKERFILLBLOCKER_H
#define ICEMAKERFILLBLOCKER_H

#include <stdint.h>

typedef struct
{
   uint32_t fillPurgeOzX100;
   uint16_t newFilterFillStateDelayInMinutes;
} IceMakerFillBlocker_t;

#endif
