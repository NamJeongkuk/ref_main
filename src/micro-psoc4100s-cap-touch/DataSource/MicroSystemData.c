/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "MicroSystemData.h"

#define EXPAND_AS_DATASOURCE_TINY_RAM_ERD_ENTRY(Name, Number, DataType, Stream, RemoteErd) \
   { Name, sizeof(DataType) },

static const TinyDataSource_RamErd_t erdTable[] = {
   ERD_TABLE(EXPAND_AS_DATASOURCE_TINY_RAM_ERD_ENTRY)
};

static const TinyDataSource_RamConfiguration_t configuration = {
   erdTable,
   NUM_ELEMENTS(erdTable)
};

void MicroSystemData_Init(MicroSystemData_t *instance, TinyTimerModule_t *timerModule)
{
   TinyDataSource_Ram_Init(
      &instance->_private.tinyRamDataSource,
      &configuration,
      &instance->_private.erdRam);

   TinyDataSource_Write(
      &instance->_private.tinyRamDataSource.interface,
      Erd_TimerModule,
      &timerModule);
}

I_TinyDataSource_t *MicroSystemData_DataSource(MicroSystemData_t *instance)
{
   return &instance->_private.tinyRamDataSource.interface;
}
