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
   HarnessEepromIndex = EepromPartitionIndex_PartitionCount,
   NumberOfAsyncDataSources,
   Erd_NvMetadata = 0,
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

static const AsyncDataSource_EepromErdInfo_t asyncNvAllElements[] = { { .erd = Erd_NvMetadata, .size = sizeof(AsyncDataSource_EepromMetadata_t), .backupCount = 1 },
   ERD_TABLE(EXPAND_AS_ASYNC_MAP_ELEMENTS) };

EEPROM_PARTITIONS(EXPAND_AS_PARTITION_ASYNC_ELEMENTS)
static const ConstArrayMap_BinarySearchConfiguration_t asyncNvMapConfigurations[] = {
   EEPROM_PARTITIONS(EXPAND_AS_PARTITION_ASYNC_MAP_CONFIG)
};

static const AsyncDataSource_EepromErdInfo_t eepromAsyncMapElements[] = { ERD_TABLE(EXPAND_AS_PERSONALITYEEPROM_ASYNC_MAP_ELEMENTS) };

static const ConstArrayMap_BinarySearchConfiguration_t eepromAsyncMapConfiguration = {
   eepromAsyncMapElements,
   NUM_ELEMENTS(eepromAsyncMapElements),
   ELEMENT_SIZE(eepromAsyncMapElements),
   sizeof(eepromAsyncMapElements[0].erd),
   OFFSET_OF(AsyncDataSource_EepromErdInfo_t, erd),
   IS_SIGNED(Erd_t)
};

static const NonVolatileAsyncDataSourceConfiguration_t configuration = {
   .metadataErd = Erd_NvMetadata,
   .harnessEepromMetadataErd = Erd_PersonalityEepromMetadata,
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

   ConstArrayMap_BinarySearch_Init(&instance._private.binarySearchMaps[HarnessEepromIndex], &eepromAsyncMapConfiguration);
   instance._private.resources[HarnessEepromIndex].map = &instance._private.binarySearchMaps[HarnessEepromIndex].interface;
   instance._private.resources[HarnessEepromIndex].readWriteBuffer = &buffers[HarnessEepromIndex];
   instance._private.resources[HarnessEepromIndex].readWriteBufferSize = sizeof(buffers[EepromPartitionIndex_PartitionCount]);

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
