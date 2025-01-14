/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

#include "NonVolatileDataSource.h"
#include "InputGroup_Null.h"
#include "Constants_Binary.h"
#include "utils.h"

enum
{
   BitsPerByte = 8,
   EepromMissingTimeoutTicks = 5000
};

#define EXPAND_AS_SYNC_CONFIGURATION(Name, Number, DataType, Swap, Io, Sub, StorageType, NvDefaultData, YearlyWrites, FaultId) \
   CONCAT(INCLUDE_NVALL_, StorageType)                                                                                         \
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
   NonVolatileDataSource_t *instance = context;
   instance->_private.ready = true;
}

STATIC_ASSERT(NUM_ELEMENTS(syncMapElements) < (MEMBER_SIZE(NonVolatileDataSource_t, _private.syncMetadataCache) * BitsPerByte));

void NonVolatileDataSource_Init(
   NonVolatileDataSource_t *instance,
   TimerModule_t *timerModule,
   I_Action_t *watchdog,
   I_AsyncDataSource_t *async)
{
   instance->_private.ready = false;

   Action_Context_Init(
      &instance->_private.onReadyAction,
      instance,
      MarkReady);

   ConstArrayMap_BinarySearch_Init(&instance->_private.syncMap, &syncMapConfiguration);

   TimerModule_StartOneShot(
      timerModule,
      &instance->_private.timeout,
      EepromMissingTimeoutTicks,
      MarkReady,
      instance);

   DataSource_CachedAsyncDataSource_Init(
      &instance->_private.sync,
      async,
      &instance->_private.syncMap.interface,
      &instance->_private.syncMetadataCache,
      &instance->_private.syncCache,
      sizeof(instance->_private.syncCache),
      &instance->_private.syncWriteCache,
      &instance->_private.onReadyAction.interface,
      Erd_NonVolatileDataSourceCacheSyncState);

   while(!instance->_private.ready)
   {
      Action_Invoke(watchdog);
      TimerModule_Run(timerModule);
   }

   TimerModule_Stop(timerModule, &instance->_private.timeout);
}

I_DataSource_t *NonVolatileDataSource_DataSource(
   NonVolatileDataSource_t *instance)
{
   return &instance->_private.sync.interface;
}
