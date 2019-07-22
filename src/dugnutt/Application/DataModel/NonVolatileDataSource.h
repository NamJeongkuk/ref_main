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

// fixme generate a union that can be used to size the RW buffer

#define EXPAND_AS_NON_VOLATILE_DATA_SOURCE_READ_WRITE_BUFFER_MEMBERS(Name, Number, DataType, Type, Swap, Io, O) \
   CONCAT(INCLUDE_NV_, Type)(uint8_t CONCAT(erd, Name)[sizeof(DataType)];)

typedef union
{
   ERD_TABLE(EXPAND_AS_NON_VOLATILE_DATA_SOURCE_READ_WRITE_BUFFER_MEMBERS)
} NonVolatileDataSourceReadWriteBuffer_t;

#define EXPAND_AS_NON_VOLATILE_DATA_SOURCE_OFFSET_STRUCT_MEMBER(Name, Number, DataType, Type, Swap, Io, O) \
   CONCAT(INCLUDE_NV_, Type)(uint8_t CONCAT(erd, Name)[sizeof(DataType)];)

typedef struct
{
   ERD_TABLE(EXPAND_AS_NON_VOLATILE_DATA_SOURCE_OFFSET_STRUCT_MEMBER)
} NonVolatileDataSourceSyncCache_t;

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
      uint8_t syncMetadataCache[1];
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
I_DataSource_t * NonVolatileDataSource_GetDataSource(
   NonVolatileDataSource_t *instance);

#endif
