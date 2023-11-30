/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ASYNCNVMAPCONFIGURATIONS_H
#define ASYNCNVMAPCONFIGURATIONS_H

#include "ConstArrayMap_BinarySearch.h"
#include "NonVolatileAsyncDataSource.h"

enum
{
   NumberOfRecordBackups = 1
};

#define EXPAND_AS_ASYNC_MAP_ELEMENTS(Name, Number, DataType, Swap, Io, Sub, StorageType, DefaultData, FaultId) \
   CONCAT(INCLUDE_NVALL_, StorageType)                                                                         \
   ({ Name COMMA sizeof(DataType) COMMA NumberOfRecordBackups } COMMA)

#define EXPAND_AS_PROTECTED_ASYNC_MAP_ELEMENTS(Name, Number, DataType, Swap, Io, Sub, StorageType, DefaultData, FaultId) \
   CONCAT(INCLUDE_NVPROTECTED_, StorageType)                                                                             \
   ({ Name COMMA sizeof(DataType) COMMA NumberOfRecordBackups } COMMA)

#define EXPAND_AS_UNITSETTING_ASYNC_MAP_ELEMENTS(Name, Number, DataType, Swap, Io, Sub, StorageType, DefaultData, FaultId) \
   CONCAT(INCLUDE_NVUNITSETTING_, StorageType)                                                                             \
   ({ Name COMMA sizeof(DataType) COMMA NumberOfRecordBackups } COMMA)

#define EXPAND_AS_USERSETTING_ASYNC_MAP_ELEMENTS(Name, Number, DataType, Swap, Io, Sub, StorageType, DefaultData, FaultId) \
   CONCAT(INCLUDE_NVUSERSETTING_, StorageType)                                                                             \
   ({ Name COMMA sizeof(DataType) COMMA NumberOfRecordBackups } COMMA)

#define EXPAND_AS_USAGEPROFILE_ASYNC_MAP_ELEMENTS(Name, Number, DataType, Swap, Io, Sub, StorageType, DefaultData, FaultId) \
   CONCAT(INCLUDE_NVUSAGEPROFILE_, StorageType)                                                                             \
   ({ Name COMMA sizeof(DataType) COMMA NumberOfRecordBackups } COMMA)

#define EXPAND_AS_RFID_ASYNC_MAP_ELEMENTS(Name, Number, DataType, Swap, Io, Sub, StorageType, DefaultData, FaultId) \
   CONCAT(INCLUDE_NVRFID_, StorageType)                                                                             \
   ({ Name COMMA sizeof(DataType) COMMA NumberOfRecordBackups } COMMA)

#define EXPAND_AS_FAULTSNAPSHOT_ASYNC_MAP_ELEMENTS(Name, Number, DataType, Swap, Io, Sub, StorageType, DefaultData, FaultId) \
   CONCAT(INCLUDE_NVFAULTSNAPSHOT_, StorageType)                                                                             \
   ({ Name COMMA sizeof(DataType) COMMA NumberOfRecordBackups } COMMA)

#define EXPAND_AS_CYCLEHISTORY_ASYNC_MAP_ELEMENTS(Name, Number, DataType, Swap, Io, Sub, StorageType, DefaultData, FaultId) \
   CONCAT(INCLUDE_NVCYCLEHISTORY_, StorageType)                                                                             \
   ({ Name COMMA sizeof(DataType) COMMA NumberOfRecordBackups } COMMA)

#define EXPAND_AS_PERSONALITYEEPROM_ASYNC_MAP_ELEMENTS(Name, Number, DataType, Swap, Io, Sub, StorageType, NvDefaultData, FaultId) \
   CONCAT(INCLUDE_PERSONALITYEEPROM_, StorageType)                                                                                 \
   ({ Name COMMA sizeof(DataType) COMMA NumberOfRecordBackups } COMMA)

#define EXPAND_AS_BINARY_SEARCH_CONFIGURATION(Elements)                                \
   Elements COMMA NUM_ELEMENTS(Elements)                                               \
   COMMA ELEMENT_SIZE(Elements)                                                        \
   COMMA sizeof(Elements[0].erd) COMMA OFFSET_OF(AsyncDataSource_EepromErdInfo_t, erd) \
   COMMA IS_SIGNED(Erd_t)

#define MAP_ELEMENTS_NvProtected EXPAND_AS_PROTECTED_ASYNC_MAP_ELEMENTS
#define MAP_ELEMENTS_NvUnitSetting EXPAND_AS_UNITSETTING_ASYNC_MAP_ELEMENTS
#define MAP_ELEMENTS_NvUserSetting EXPAND_AS_USERSETTING_ASYNC_MAP_ELEMENTS
#define MAP_ELEMENTS_NvUsageProfile EXPAND_AS_USAGEPROFILE_ASYNC_MAP_ELEMENTS
#define MAP_ELEMENTS_NvRfid EXPAND_AS_RFID_ASYNC_MAP_ELEMENTS
#define MAP_ELEMENTS_NvFaultSnapshot EXPAND_AS_FAULTSNAPSHOT_ASYNC_MAP_ELEMENTS
#define MAP_ELEMENTS_NvCycleHistory EXPAND_AS_CYCLEHISTORY_ASYNC_MAP_ELEMENTS

#define EXPAND_AS_PARTITION_ASYNC_ELEMENTS(Name, StartAddress, Size, NvType) \
   static const AsyncDataSource_EepromErdInfo_t async##NvType##Elements[] = { { .erd = Erd_NvMetadata, .size = sizeof(AsyncDataSource_EepromMetadata_t), .backupCount = 1 }, ERD_TABLE(MAP_ELEMENTS_##NvType) };

#define EXPAND_AS_PARTITION_ASYNC_MAP_CONFIG(Name, StartAddress, Size, NvType) \
   { EXPAND_AS_BINARY_SEARCH_CONFIGURATION(async##NvType##Elements) },

/*!
 *
 */
void AsyncNvMapConfigurations_Init(void);

/*!
 * @return the input group with the default data map for AsyncDataSource_Eeprom
 */
I_InputGroup_t *AsyncNvMapConfigurations_GetInputGroup(void);

/*!
 * @return the const array maps and readWriteBuffers for all AsyncDataSource_Eeprom's to be made
 */
NonVolatileAsyncDataSourceResources_t *AsyncNvMapConfigurations_GetAsyncDataSourceResources(void);

/*!
 * @return the metadata Erd and the number of maps
 */
const NonVolatileAsyncDataSourceConfiguration_t *AsyncNvMapConfigurations_GetAsyncDataSourceConfiguration(void);

#endif
