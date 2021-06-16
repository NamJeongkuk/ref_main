/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SystemData.h"
#include "SystemErds.h"
#include "TinyDataSource_Ram.h"
#include "TinyDataSource_EndiannessSwapped.h"
#include "utils.h"

#define EXPAND_AS_OFFSET_STRUCT_MEMBER(Name, Number, DataType, Stream, RemoteErd) \
   uint8_t MACRO_SAFE_CONCATENATE(erd, Name)[sizeof(DataType)];

typedef struct
{
   ERD_TABLE(EXPAND_AS_OFFSET_STRUCT_MEMBER)
} TinyRamDataSourceStorage_t;

#ifdef LITTLE_ENDIAN
typedef union
{
   ERD_TABLE(EXPAND_AS_OFFSET_STRUCT_MEMBER)
} EndiannessSwapBuffer_t;
#endif

static struct
{
   TinyDataSource_Ram_t tinyRamDataSource;
   TinyRamDataSourceStorage_t erdRam;
#ifdef LITTLE_ENDIAN
   TinyDataSource_EndiannessSwapped_t swappedDataSource;
   EndiannessSwapBuffer_t endiannessSwapBuffer;
#endif
} instance;

#define EXPAND_AS_DATASOURCE_TINY_RAM_ERD_ENTRY(Name, Number, DataType, Stream, RemoteErd) \
   { Name, sizeof(DataType) },

static const TinyDataSource_RamErd_t erdTable[] = {
   ERD_TABLE(EXPAND_AS_DATASOURCE_TINY_RAM_ERD_ENTRY)
};

static const TinyDataSource_RamConfiguration_t configuration = {
   erdTable,
   NUM_ELEMENTS(erdTable)
};

#ifdef LITTLE_ENDIAN
static const TinyDataSource_EndiannessSwappedSwappedField_t endiannessSwappedFields[] = {
   SWAPPED_FIELDS
};

static const TinyDataSource_EndiannessSwappedConfiguration_t endiannessSwappedConfiguration = {
   endiannessSwappedFields,
   NUM_ELEMENTS(endiannessSwappedFields)
};
#endif

void SystemData_Init(TinyTimerModule_t *timerModule)
{
   TinyDataSource_Ram_Init(
      &instance.tinyRamDataSource,
      &configuration,
      &instance.erdRam);

#ifdef LITTLE_ENDIAN
   TinyDataSource_EndiannessSwapped_Init(
      &instance.swappedDataSource,
      &instance.tinyRamDataSource.interface,
      &instance.endiannessSwapBuffer,
      &endiannessSwappedConfiguration);
#endif

   TinyDataSource_Write(
      &instance.tinyRamDataSource.interface,
      Erd_TimerModule,
      &timerModule);
}

I_TinyDataSource_t *SystemData_DataSource(void)
{
   return &instance.tinyRamDataSource.interface;
}

I_TinyDataSource_t *SystemData_ExternalDataSource(void)
{
#ifdef LITTLE_ENDIAN
   return &instance.swappedDataSource.interface;
#else
   return &instance.tinyRamDataSource.interface;
#endif
}
