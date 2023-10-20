/*!
 * @file
 * @brief The type for ERD 0x1237
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef WATERFILTERREMAININGUSAGE_H
#define WATERFILTERREMAININGUSAGE_H

#include <stdint.h>

typedef struct
{
   uint8_t percentUsageRemaining;
   uint8_t padding;
   uint16_t daysUsageRemaining;
} WaterFilterRemainingUsage_t;

#define SWAP_DEFINITION_WaterFilterRemainingUsage_t(_name, _type) \
   SWAPPED_FIELD(_name, _type, daysUsageRemaining),

#endif
