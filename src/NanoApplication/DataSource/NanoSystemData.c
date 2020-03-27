/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "NanoSystemData.h"

#define EXPAND_AS_DATASOURCE_TINY_RAM_ERD_ENTRY(Name, Number, DataType, Stream, RemoteErd) \
   { Name, sizeof(DataType) },

static const TinyDataSource_RamErd_t erdTable[] =
   {
      ERD_TABLE(EXPAND_AS_DATASOURCE_TINY_RAM_ERD_ENTRY)
   };

static const TinyDataSource_RamConfiguration_t configuration =
   {
      erdTable,
      NUM_ELEMENTS(erdTable)
   };

void NanoSystemData_Init(NanoSystemData_t *instance)
{
   TinyDataSource_Ram_Init(
      &instance->_private.tinyRamDataSource,
      &configuration,
      &instance->_private.erdRam);
}

I_TinyDataSource_t *NanoSystemData_DataSource(NanoSystemData_t *instance)
{
   return &instance->_private.tinyRamDataSource.interface;
}
