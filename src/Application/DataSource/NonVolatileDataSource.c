/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

#include "NonVolatileDataSource.h"
#include "Action_Context.h"
#include "InputGroup_Null.h"
#include "Constants_Binary.h"
#include "utils.h"

// clang-format off

#define EXPAND_AS_ASYNC_MAP_ELEMENTS(Name, Number, DataType, Swap, Io, Sub, StorageType, NvDefaultData, FaultId) \
   CONCAT(INCLUDE_NV_, StorageType)({ Name COMMA sizeof(DataType) } COMMA)

// clang-format on

enum
{
   ClientVersion = 1,
   BitsPerByte = 8,
   TimerTicksBetweenRetriesInMsec = 100,
   NumberOfRetriesBeforeErase = 10
};

static const AsyncDataSource_FlashBlockGroupErdInfo_t asyncMapElements[] = { ERD_TABLE(EXPAND_AS_ASYNC_MAP_ELEMENTS) };

static const ConstArrayMap_BinarySearchConfiguration_t asyncMapConfiguration = {
   asyncMapElements,
   NUM_ELEMENTS(asyncMapElements),
   ELEMENT_SIZE(asyncMapElements),
   sizeof(asyncMapElements[0].erd),
   OFFSET_OF(AsyncDataSource_FlashBlockGroupErdInfo_t, erd),
   IS_SIGNED(Erd_t)
};

#define EXPAND_AS_SYNC_CONFIGURATION(Name, Number, DataType, Swap, Io, Sub, StorageType, NvDefaultData, FaultId) \
   CONCAT(INCLUDE_NV_, StorageType)                                                                              \
   ({ Name COMMA OFFSET_OF(NonVolatileDataSourceSyncCache_t, CONCAT(erd, Name)) } COMMA)

static const DataSource_CachedAsyncDataSourceErdInfo_t syncMapElements[] = { ERD_TABLE(EXPAND_AS_SYNC_CONFIGURATION) };

static const ConstArrayMap_BinarySearchConfiguration_t syncMapConfiguration = {
   syncMapElements,
   NUM_ELEMENTS(syncMapElements),
   ELEMENT_SIZE(syncMapElements),
   sizeof(syncMapElements[0].erd),
   OFFSET_OF(DataSource_CachedAsyncDataSourceErdInfo_t, erd),
   IS_SIGNED(Erd_t)
};

static void MarkReady(void *context)
{
   REINTERPRET(ready, context, bool *);
   *ready = true;
}

STATIC_ASSERT(NUM_ELEMENTS(syncMapElements) < (MEMBER_SIZE(NonVolatileDataSource_t, _private.syncMetadataCache) * BitsPerByte));

void NonVolatileDataSource_Init(
   NonVolatileDataSource_t *instance,
   TimerModule_t *timerModule,
   I_Action_t *watchdog,
   I_Crc16Calculator_t *crc16Calculator,
   I_Eeprom_t *eeprom)
{
   bool ready = false;
   Action_Context_t onReadyAction;
   Action_Context_Init(&onReadyAction, &ready, MarkReady);

   ConstArrayMap_BinarySearch_Init(&instance->_private.asyncMap, &asyncMapConfiguration);

   InputGroup_NonVolatileDataSourceDefaultData_Init(
      &instance->_private.defaultDataInputGroup,
      asyncMapElements,
      NUM_ELEMENTS(asyncMapElements));

   AsyncDataSource_Eeprom_Init(
      &instance->_private.eepromAsyncDataSource,
      &instance->_private.asyncMap.interface,
      eeprom,
      crc16Calculator,
      &instance->_private.defaultDataInputGroup.interface,
      &instance->_private.asyncReadWriteBuffer,
      sizeof(instance->_private.asyncReadWriteBuffer),
      Erd_NvMetadata,
      ClientVersion,
      timerModule,
      TimerTicksBetweenRetriesInMsec,
      NumberOfRetriesBeforeErase);

   ConstArrayMap_BinarySearch_Init(&instance->_private.syncMap, &syncMapConfiguration);

   DataSource_CachedAsyncDataSource_Init(
      &instance->_private.sync,
      &instance->_private.eepromAsyncDataSource.interface,
      &instance->_private.syncMap.interface,
      &instance->_private.syncMetadataCache,
      &instance->_private.syncCache,
      sizeof(instance->_private.syncCache),
      &instance->_private.syncWriteCache,
      &onReadyAction.interface,
      Erd_NonVolatileDataSourceCacheSyncState);

   while(!ready)
   {
      Action_Invoke(watchdog);
      TimerModule_Run(timerModule);
   }
}

I_DataSource_t *NonVolatileDataSource_DataSource(
   NonVolatileDataSource_t *instance)
{
   return &instance->_private.sync.interface;
}
