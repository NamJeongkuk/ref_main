/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef CALENDARUSAGEINSECONDS_H
#define CALENDARUSAGEINSECONDS_H

#include <stdint.h>

typedef uint32_t CalendarUsageInSeconds_t;

#define SWAP_DEFINITION_CalendarUsageInSeconds_t(_name, _type) \
   SWAPPED_ERD(_name, sizeof(_type)),

#endif
