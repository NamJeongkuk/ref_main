/*!
 * @file
 * @brief Fault snapshot data definition
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FAULTSNAPSHOTDATA_H
#define FAULTSNAPSHOTDATA_H

#include <stdint.h>
#include <stdbool.h>
#include "FaultId.h"

typedef struct
{
   uint8_t someData[sizeof(uint32_t)];
} FaultSnapshotData_t;

#define SWAP_DEFINITION_FaultSnapshotData_t(_name, _type) \
   SWAPPED_FIELD(_name, _type, someData),

#endif
