/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

#ifndef NONVOLATILEDATASOURCE_H
#define NONVOLATILEDATASOURCE_H

#include "Timer.h"
#include "I_Action.h"
#include "SystemErds.h"
#include "DataSource_CachedAsyncDataSource.h"
#include "NonVolatileDataSourceReadWriteBuffer.h"
#include "ConstArrayMap_BinarySearch.h"

// clang-format off

#define EXPAND_AS_NON_VOLATILE_DATA_SOURCE_OFFSET_STRUCT_MEMBER(Name, Number, DataType, Swap, Io, Sub, StorageType, NvDefaultData, FaultId) \
   CONCAT(INCLUDE_NVALL_, StorageType)(uint8_t CONCAT(erd, Name)[sizeof(DataType)];)
typedef struct
{
   ERD_TABLE(EXPAND_AS_NON_VOLATILE_DATA_SOURCE_OFFSET_STRUCT_MEMBER)
} NonVolatileDataSourceSyncCache_t;

#define EXPAND_AS_NON_VOLATILE_DATA_SOURCE_COUNT_STRUCT_MEMBER(Name, Number, DataType, Swap, Io, Sub, StorageType, NvDefaultData, FaultId) \
   CONCAT(INCLUDE_NVALL_, StorageType)(uint8_t CONCAT(erd, Name);)

typedef struct
{
   ERD_TABLE(EXPAND_AS_NON_VOLATILE_DATA_SOURCE_COUNT_STRUCT_MEMBER)
} NonVolatileDataSourceErdCount_t;

typedef NonVolatileDataSourceReadWriteBuffer_t NonVolatileDataSourceSyncWriteCache_t;

typedef struct
{
   struct
   {
      DataSource_CachedAsyncDataSource_t sync;
      ConstArrayMap_BinarySearch_t syncMap;
      NonVolatileDataSourceSyncCache_t syncCache;
      NonVolatileDataSourceSyncWriteCache_t syncWriteCache;
      uint8_t syncMetadataCache[sizeof(NonVolatileDataSourceErdCount_t) / 8 + 1];
      Timer_t timeout;
   } _private;
} NonVolatileDataSource_t;

// clang-format on

/*!
 * @param instance
 * @param timerModule
 * @param watchdog
 * @param async
 */
void NonVolatileDataSource_Init(
   NonVolatileDataSource_t *instance,
   TimerModule_t *timerModule,
   I_Action_t *watchdog,
   I_AsyncDataSource_t *async);

/*!
 * @param instance
 * @return
 */
I_DataSource_t *NonVolatileDataSource_DataSource(
   NonVolatileDataSource_t *instance);

#endif
