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
#include "AsyncDataSource_Eeprom.h"
#include "DataSource_CachedAsyncDataSource.h"
#include "ConstArrayMap_BinarySearch.h"
#include "InputGroup_NonVolatileDataSourceDefaultData.h"

// clang-format off

#define EXPAND_AS_NON_VOLATILE_DATA_SOURCE_READ_WRITE_BUFFER_MEMBERS(Name, Number, DataType, Swap, Io, Sub, StorageType, NvDefaultData, FaultId) \
   CONCAT(INCLUDE_NV_, StorageType)(uint8_t CONCAT(erd, Name)[sizeof(DataType)];)

typedef struct
{
   union
   {
      ERD_TABLE(EXPAND_AS_NON_VOLATILE_DATA_SOURCE_READ_WRITE_BUFFER_MEMBERS)
   };

   uint8_t paddingToCoverAlignment[8];
} NonVolatileDataSourceReadWriteBuffer_t;

#define EXPAND_AS_NON_VOLATILE_DATA_SOURCE_OFFSET_STRUCT_MEMBER(Name, Number, DataType, Swap, Io, Sub, StorageType, NvDefaultData, FaultId) \
   CONCAT(INCLUDE_NV_, StorageType)(uint8_t CONCAT(erd, Name)[sizeof(DataType)];)

typedef struct
{
   ERD_TABLE(EXPAND_AS_NON_VOLATILE_DATA_SOURCE_OFFSET_STRUCT_MEMBER)
} NonVolatileDataSourceSyncCache_t;

#define EXPAND_AS_NON_VOLATILE_DATA_SOURCE_COUNT_STRUCT_MEMBER(Name, Number, DataType, Swap, Io, Sub, StorageType, NvDefaultData, FaultId) \
   CONCAT(INCLUDE_NV_, StorageType)(uint8_t CONCAT(erd, Name);)

typedef struct
{
   ERD_TABLE(EXPAND_AS_NON_VOLATILE_DATA_SOURCE_COUNT_STRUCT_MEMBER)
} NonVolatileDataSourceErdCount_t;

// clang-format on

typedef NonVolatileDataSourceReadWriteBuffer_t NonVolatileDataSourceSyncWriteCache_t;

typedef struct
{
   struct
   {
      AsyncDataSource_Eeprom_t eepromAsyncDataSource;
      ConstArrayMap_BinarySearch_t asyncMap;
      NonVolatileDataSourceReadWriteBuffer_t asyncReadWriteBuffer;
      DataSource_CachedAsyncDataSource_t sync;
      ConstArrayMap_BinarySearch_t syncMap;
      NonVolatileDataSourceSyncCache_t syncCache;
      NonVolatileDataSourceSyncWriteCache_t syncWriteCache;
      InputGroup_NonVolatileDataSourceDefaultData_t defaultDataInputGroup;
      uint8_t syncMetadataCache[sizeof(NonVolatileDataSourceErdCount_t) / 8 + 1];
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
   I_Eeprom_t *eeprom);

/*!
 * @param instance
 * @return
 */
I_DataSource_t *NonVolatileDataSource_DataSource(
   NonVolatileDataSource_t *instance);

#endif
