/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef RAMDATASOURCE_H
#define RAMDATASOURCE_H

#include "DataSource_Ram.h"
#include "SystemErds.h"
#include "ConstArrayMap_BinarySearch.h"
#include "XMacroUtils.h"

#define EXPAND_AS_OFFSET_STRUCT_MEMBER(Name, Number, DataType, Swap, Io, StorageType, NvDefaultData, FaultId) \
   CONCAT(INCLUDE_RAM_, StorageType)                                                                                    \
   (uint8_t MACRO_SAFE_CONCATENATE(erd, Name)[sizeof(DataType)];)

typedef struct
{
   ERD_TABLE(EXPAND_AS_OFFSET_STRUCT_MEMBER)
} RamDataSourceStorage_t;

typedef struct
{
   struct
   {
      RamDataSourceStorage_t erdRam;
      DataSource_Ram_t dataSource;
      ConstArrayMap_BinarySearch_t erdMap;
   } _private;
} RamDataSource_t;

/*!
 * @param instance
 */
void RamDataSource_Init(RamDataSource_t *instance);

/*!
 * @return
 */
I_DataSource_t * RamDataSource_DataSource(RamDataSource_t *instance);

#endif
