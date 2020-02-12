/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

#ifndef NONVOLATILEDATASOURCE_H
#define NONVOLATILEDATASOURCE_H

#include "I_FlashBlockGroup.h"
#include "Timer.h"
#include "I_Action.h"
#include "I_Crc16Calculator.h"
#include "I_FlashBlockGroup.h"
#include "SystemErds.h"
#include "AsyncDataSource_FlashBlockGroup.h"
#include "DataSource_CachedAsyncDataSource.h"
#include "ConstArrayMap_BinarySearch.h"
#include "InputGroup_NonVolatileDataSourceDefaultData.h"

#define EXPAND_AS_NON_VOLATILE_DATA_SOURCE_READ_WRITE_BUFFER_MEMBERS(Name, Number, DataType, Swap, Io, StorageType, NvDefaultData, FaultId) \
   CONCAT(INCLUDE_NV_, StorageType)(uint8_t CONCAT(erd, Name)[sizeof(DataType)];)

typedef union
{
   ERD_TABLE(EXPAND_AS_NON_VOLATILE_DATA_SOURCE_READ_WRITE_BUFFER_MEMBERS)
} NonVolatileDataSourceReadWriteBuffer_t;

#define EXPAND_AS_NON_VOLATILE_DATA_SOURCE_OFFSET_STRUCT_MEMBER(Name, Number, DataType, Swap, Io, StorageType, NvDefaultData, FaultId) \
   CONCAT(INCLUDE_NV_, StorageType)(uint8_t CONCAT(erd, Name)[sizeof(DataType)];)

typedef struct
{
   ERD_TABLE(EXPAND_AS_NON_VOLATILE_DATA_SOURCE_OFFSET_STRUCT_MEMBER)
} NonVolatileDataSourceSyncCache_t;

typedef NonVolatileDataSourceReadWriteBuffer_t NonVolatileDataSourceSyncWriteCache_t;

typedef struct
{
   struct
   {
      AsyncDataSource_FlashBlockGroup_t async;
      ConstArrayMap_BinarySearch_t asyncMap;
      NonVolatileDataSourceReadWriteBuffer_t asyncReadWriteBuffer;
      DataSource_CachedAsyncDataSource_t sync;
      ConstArrayMap_BinarySearch_t syncMap;
      NonVolatileDataSourceSyncCache_t syncCache;
      NonVolatileDataSourceSyncWriteCache_t writeCache;
      InputGroup_NonVolatileDataSourceDefaultData_t defaultDataInputGroup;
      uint8_t syncMetadataCache[sizeof(NonVolatileDataSourceSyncCache_t) / 8];
   } _private;
} NonVolatileDataSource_t;

/*!
 * @param instance
 * @param timerModule
 * @param watchdog
 * @param crc16Calculator
 * @param flashBlockGroup
 */
void NonVolatileDataSource_Init(
   NonVolatileDataSource_t *instance,
   TimerModule_t *timerModule,
   I_Action_t *watchdog,
   I_Crc16Calculator_t *crc16Calculator,
   I_FlashBlockGroup_t *flashBlockGroup);

/*!
 * @param instance
 * @return
 */
I_DataSource_t * NonVolatileDataSource_DataSource(
   NonVolatileDataSource_t *instance);

#endif
