/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "TinySystemData.h"

#define EXPAND_AS_DATASOURCE_TINY_RAM_ERD_ENTRY(Name, Number, DataType) \
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

void TinySystemData_Init(TinySystemData_t *instance)
{
   TinyDataSource_Ram_Init(
      &instance->_private.tinyRamDataSource,
      &configuration,
      &instance->_private.erdRam);
}

I_TinyDataSource_t *TinySystemData_DataSource(TinySystemData_t *instance)
{
   return &instance->_private.tinyRamDataSource.interface;
}
