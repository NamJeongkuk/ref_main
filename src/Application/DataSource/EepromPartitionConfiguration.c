/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "EepromPartitionConfiguration.h"
#include "EepromPartitions.h"
#include "utils.h"

static const PartitionedEepromPartitionConfigurationElement_t eepromPartitions[] = { EEPROM_PARTITIONS(EXPAND_AS_PARTITION_CONFIGURATION) };

static const PartitionedEepromPartitionConfiguration_t partitionedEepromConfiguration = {
   .elements = eepromPartitions,
   .partitionCount = NUM_ELEMENTS(eepromPartitions)
};

const PartitionedEepromPartitionConfiguration_t *EepromPartitionConfiguration_Init(void)
{
   return &partitionedEepromConfiguration;
}
