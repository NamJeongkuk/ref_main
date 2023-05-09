/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef CYCLEHISTORYRECORD_H
#define CYCLEHISTORYRECORD_H

#include <stdint.h>

typedef struct
{
   uint8_t someData[sizeof(uint32_t)];
} CycleHistoryRecord_t;

#define SWAP_DEFINITION_CycleHistoryRecord_t(_name, _type) \
   SWAPPED_FIELD(_name, _type, someData),

#endif
