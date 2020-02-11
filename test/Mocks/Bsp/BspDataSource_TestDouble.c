/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <string.h>
#include <stdint.h>
#include "BspDataSource.h"
#include "DataSource_Ram.h"
#include "ConstArrayMap_LinearSearch.h"
#include "SystemErds.h"
#include "uassert.h"

#define EXPAND_AS_BSP_OFFSET_STRUCT_MEMBER(Name, Number, DataType, Swap, Io, StorageType, NvDefaultData, FaultId) \
   CONCAT(INCLUDE_BSP_, StorageType)                                                                              \
   (uint8_t MACRO_SAFE_CONCATENATE(erd, Name)[sizeof(DataType)];)

#define EXPAND_AS_BSP_CONFIGURATION(Name, Number, DataType, Swap, Io, StorageType, NvDefaultData, FaultId) \
   CONCAT(INCLUDE_BSP_, StorageType)                                                                       \
   ({ Name COMMA OFFSET_OF(DataSourceStorage_t, MACRO_SAFE_CONCATENATE(erd, Name)) } COMMA)

typedef struct
{
   ERD_TABLE(EXPAND_AS_BSP_OFFSET_STRUCT_MEMBER)
} DataSourceStorage_t;

static const DataSource_RamErdConfigurationElement_t erds[] =
   {
      ERD_TABLE(EXPAND_AS_BSP_CONFIGURATION)
   };

static const ConstArrayMap_LinearSearchConfiguration_t constArrayMapConfig =
   {
      erds,
      NUM_ELEMENTS(erds),
      ELEMENT_SIZE(erds),
      MEMBER_SIZE(DataSource_RamErdConfigurationElement_t, erd),
      OFFSET_OF(DataSource_RamErdConfigurationElement_t, erd)
   };

static DataSource_Ram_t dataSource;
static DataSourceStorage_t storage;
static ConstArrayMap_LinearSearch_t dataSourceConstArrayMap;

static struct
{
   I_DataSource_t interface;
} instance;

static void Read(I_DataSource_t *instance, const Erd_t erd, void *data)
{
   IGNORE(instance);
   DataSource_Read(&dataSource.interface, erd, data);
}

static void Write(I_DataSource_t *instance, const Erd_t erd, const void *data)
{
   IGNORE(instance);
   DataSource_Write(&dataSource.interface, erd, data);
}

static bool Has(const I_DataSource_t *instance, const Erd_t erd)
{
   IGNORE(instance);
   return DataSource_Has(&dataSource.interface, erd);
}

static uint8_t SizeOf(const I_DataSource_t *instance, const Erd_t erd)
{
   IGNORE(instance);
   return DataSource_SizeOf(&dataSource.interface, erd);
}

static const I_DataSource_Api_t api =
   { Read, Write, Has, SizeOf };

void BspDataSource_Init(
   BspDataSource_t *_instance,
   TimerModule_t *timerModule)
{
   IGNORE(_instance);
   IGNORE(timerModule);

   ConstArrayMap_LinearSearch_Init(&dataSourceConstArrayMap, &constArrayMapConfig);
   DataSource_Ram_Init(&dataSource, &storage, sizeof(storage), &dataSourceConstArrayMap.interface);

   instance.interface.api = &api;
   instance.interface.OnDataChange = dataSource.interface.OnDataChange;
}

I_DataSource_t *BspDataSource_DataSource(BspDataSource_t *_instance)
{
   IGNORE(_instance);

   return &instance.interface;
}
