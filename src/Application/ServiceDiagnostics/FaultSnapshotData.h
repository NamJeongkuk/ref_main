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
#include "FaultIds.h"

typedef struct
{
   uint8_t someData[sizeof(uint32_t)];
} FaultSnapshotData_t;

#endif
