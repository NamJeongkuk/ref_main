/*!
 * @file
 * @brief
 * @warning The order of these indices is extremely important and it needs to match the
 *          order of the AsyncDataSource_CompositeStatic components as well as the PartitionedEeprom Partitions
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef EEPROMPARTITIONINDEX_H
#define EEPROMPARTITIONINDEX_H

#include "EepromPartitionConfiguration.h"
#include "EepromPartitions.h"
#include <stdint.h>

// clang-format off

#define EXPAND_AS_PARTITION_ENUM(Name, StartAddress, Size, NvType) \
   EepromPartitionIndex_##NvType,

enum
{
   EEPROM_PARTITIONS(EXPAND_AS_PARTITION_ENUM)
   EepromPartitionIndex_PartitionCount,
   EepromPartitionIndex_NvAll = EepromPartitionIndex_PartitionCount
};
typedef uint8_t EepromPartitionIndex_t;

// clang-format on

#endif
