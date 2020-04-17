/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SystemData.h"
#include "SystemErds.h"
#include "TinyDataSource_Ram.h"
#include "utils.h"

#define EXPAND_AS_OFFSET_STRUCT_MEMBER(Name, Number, DataType, Stream, RemoteErd) \
   uint8_t MACRO_SAFE_CONCATENATE(erd, Name)[sizeof(DataType)];

typedef struct
{
   ERD_TABLE(EXPAND_AS_OFFSET_STRUCT_MEMBER)
} DataSourceStorage_t;

static TinyDataSource_Ram_t dataSource;
// static TinyDataSource_EndiannessSwapped_t externalDataSource;
static DataSourceStorage_t erdRam;

#define EXPAND_AS_DATASOURCE_TINY_RAM_ERD_ENTRY(Name, Number, DataType, Stream, RemoteErd) \
   { Name, sizeof(DataType) },

static const TinyDataSource_RamErd_t erdTable[] = {
   ERD_TABLE(EXPAND_AS_DATASOURCE_TINY_RAM_ERD_ENTRY)
};

static const TinyDataSource_RamConfiguration_t configuration = {
   erdTable,
   NUM_ELEMENTS(erdTable)
};

void SystemData_Init(TinyTimerModule_t *timerModule)
{
   TinyDataSource_Ram_Init(
      &dataSource,
      &configuration,
      &erdRam);

   TinyDataSource_Write(
      &dataSource.interface,
      Erd_TimerModule,
      &timerModule);
}

I_TinyDataSource_t *SystemData_DataSource(void)
{
   return &dataSource.interface;
}

I_TinyDataSource_t *SystemData_ExternalDataSource(void)
{
   // return &externalDataSource.interface;
   return NULL;
}

// fixme need external data source
