/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SystemData.h"
#include "SystemErds.h"
#include "DataSource_TinyRam.h"
#include "DataSource_EndiannessSwapped.h"
#include "ConstMultiMap_BinarySearch.h"
#include "DataSource_Mapped.h"
#include "utils.h"

#define EXPAND_AS_OFFSET_STRUCT_MEMBER(Name, Number, DataType, Stream, RemoteErd) \
   uint8_t MACRO_SAFE_CONCATENATE(erd, Name)[sizeof(DataType)];

typedef struct
{
   ERD_TABLE(EXPAND_AS_OFFSET_STRUCT_MEMBER)
} RamDataSourceStorage_t;

typedef union
{
   ERD_TABLE(EXPAND_AS_OFFSET_STRUCT_MEMBER)
} EndiannessSwapBuffer_t;

static struct
{
   DataSource_TinyRam_t ramDataSource;
   RamDataSourceStorage_t erdRam;
   DataSource_EndiannessSwapped_t swappedDataSource;
   EndiannessSwapBuffer_t endiannessSwapBuffer;
   ConstMultiMap_BinarySearch_t swappedFieldsMap;
} instance;

#define EXPAND_AS_DATASOURCE__RAM_ERD_ENTRY(Name, Number, DataType, Stream, RemoteErd) \
   { Name, sizeof(DataType) },

static const DataSource_TinyRamErd_t erdTable[] = {
   ERD_TABLE(EXPAND_AS_DATASOURCE__RAM_ERD_ENTRY)
};

static const DataSource_TinyRamConfiguration_t configuration = {
   erdTable,
   NUM_ELEMENTS(erdTable)
};

static const DataSource_EndiannessSwappedSwappedField_t swappedFields[] = {
   SWAPPED_FIELDS
};

static const ConstMultiMap_BinarySearchConfiguration_t endiannessSwappedFieldsMapConfiguration = {
   swappedFields,
   NUM_ELEMENTS(swappedFields),
   sizeof(DataSource_EndiannessSwappedSwappedField_t),
   MEMBER_SIZE(DataSource_EndiannessSwappedSwappedField_t, erd),
   OFFSET_OF(DataSource_EndiannessSwappedSwappedField_t, erd),
   sizeof(DataSource_EndiannessSwappedSwappedField_t),
   0,
   IS_SIGNED(Erd_t)
};

static const DataSource_EndiannessSwappedSwappedRangesConfiguration_t swappedRangesConfiguration = {
   .swappedRanges = NULL,
   .swappedRangesCount = 0
};

void SystemData_Init(TimerModule_t *timerModule)
{
   DataSource_TinyRam_Init(
      &instance.ramDataSource,
      &configuration,
      &instance.erdRam);

   ConstMultiMap_BinarySearch_Init(
      &instance.swappedFieldsMap,
      &endiannessSwappedFieldsMapConfiguration);

   DataSource_EndiannessSwapped_Init(
      &instance.swappedDataSource,
      &instance.ramDataSource.interface,
      &instance.swappedFieldsMap.interface,
      &swappedRangesConfiguration,
      false);

   DataSource_Write(
      &instance.ramDataSource.interface,
      Erd_TimerModule,
      &timerModule);
}

I_DataSource_t *SystemData_DataSource(void)
{
   return &instance.ramDataSource.interface;
}

I_DataSource_t *SystemData_ExternalDataSource(void)
{
   return &instance.swappedDataSource.interface;
}
