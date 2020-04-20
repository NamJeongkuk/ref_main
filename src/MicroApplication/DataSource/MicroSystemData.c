/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "MicroSystemData.h"
#include "utils.h"

#define EXPAND_AS_OFFSET_STRUCT_MEMBER(Name, Number, DataType, Stream, RemoteErd) \
   uint8_t MACRO_SAFE_CONCATENATE(erd, Name)[sizeof(DataType)];

typedef struct
{
   ERD_TABLE(EXPAND_AS_OFFSET_STRUCT_MEMBER)
} TinyRamDataSourceStorage_t;

static struct
{
   TinyDataSource_Ram_t tinyRamDataSource;
   TinyRamDataSourceStorage_t erdRam;
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

void MicroSystemData_Init(TinyTimerModule_t *timerModule)
{
   TinyDataSource_Ram_Init(
      &instance.tinyRamDataSource,
      &configuration,
      &instance.erdRam);

   TinyDataSource_Write(
      &instance.tinyRamDataSource.interface,
      Erd_TimerModule,
      &timerModule);
}

I_TinyDataSource_t *MicroSystemData_DataSource(void)
{
   return &instance.tinyRamDataSource.interface;
}
