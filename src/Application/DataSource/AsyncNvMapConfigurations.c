/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "AsyncNvMapConfigurations.h"
#include "AsyncDataSource_Eeprom.h"
#include "NonVolatileDataSourceReadWriteBuffer.h"
#include "EepromPartitions.h"

enum
{
   NumberOfAsyncDataSources = EepromPartitionIndex_PartitionCount
};

typedef struct
{
   struct
   {
      ConstArrayMap_BinarySearch_t binarySearchMaps[NumberOfAsyncDataSources];
      NonVolatileAsyncDataSourceResources_t resources[NumberOfAsyncDataSources];
      InputGroup_NonVolatileDataSourceDefaultData_t inputGroup;
      uint16_t mapCount;
   } _private;
} AsyncNvMapConfiguration_t;

static AsyncNvMapConfiguration_t instance;
static NonVolatileDataSourceReadWriteBuffer_t buffers[NumberOfAsyncDataSources];

static const AsyncDataSource_EepromErdInfo_t asyncNvAllElements[] = { ERD_TABLE(EXPAND_AS_ASYNC_MAP_ELEMENTS) };

EEPROM_PARTITIONS(EXPAND_AS_PARTITION_ASYNC_ELEMENTS)
static const ConstArrayMap_BinarySearchConfiguration_t asyncNvMapConfigurations[] = {
   EEPROM_PARTITIONS(EXPAND_AS_PARTITION_ASYNC_MAP_CONFIG)
};

static const NonVolatileAsyncDataSourceConfiguration_t configuration = {
   .metadataErd = Erd_NvMetadata,
   .mapCount = NUM_ELEMENTS(asyncNvMapConfigurations)
};

void AsyncNvMapConfigurations_Init(void)
{
   for(uint8_t i = 0; i < EepromPartitionIndex_PartitionCount; i++)
   {
      ConstArrayMap_BinarySearch_Init(&instance._private.binarySearchMaps[i], &asyncNvMapConfigurations[i]);
      instance._private.resources[i].map = &instance._private.binarySearchMaps[i].interface;
      instance._private.resources[i].readWriteBuffer = &buffers[i];
      instance._private.resources[i].readWriteBufferSize = sizeof(buffers[i]);
   }

   InputGroup_NonVolatileDataSourceDefaultData_Init(
      &instance._private.inputGroup,
      asyncNvAllElements,
      NUM_ELEMENTS(asyncNvAllElements));
}

I_InputGroup_t *AsyncNvMapConfigurations_GetInputGroup(void)
{
   return &instance._private.inputGroup.interface;
}

NonVolatileAsyncDataSourceResources_t *AsyncNvMapConfigurations_GetAsyncDataSourceResources(void)
{
   return instance._private.resources;
}

const NonVolatileAsyncDataSourceConfiguration_t *AsyncNvMapConfigurations_GetAsyncDataSourceConfiguration(void)
{
   return &configuration;
}
