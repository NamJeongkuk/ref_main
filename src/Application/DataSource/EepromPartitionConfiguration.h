/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef EEPROMPARTITIONCONFIGURATION_H
#define EEPROMPARTITIONCONFIGURATION_H

#include "PartitionedEeprom.h"
#include "XMacroUtils.h"

#define EXPAND_AS_PARTITION_CONFIGURATION(Name, StartAddress, Size, NvType) \
   { .startAddress = StartAddress COMMA.size = Size } COMMA

/*!
 * Initialize the eeprom partitions
 * @return Eeprom Partition Configuration
 */
const PartitionedEepromPartitionConfiguration_t *EepromPartitionConfiguration_Init(void);

#endif
