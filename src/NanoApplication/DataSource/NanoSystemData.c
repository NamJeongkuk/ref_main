/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "NanoSystemData.h"

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

void NanoSystemData_Init(NanoSystemData_t *instance)
{
   TinyDataSource_Ram_Init(
      &instance->_private.tinyRamDataSource,
      &configuration,
      &instance->_private.erdRam);

#ifdef LITTLE_ENDIAN
   TinyDataSource_EndiannessSwapped_Init(
      &instance->_private.swappedDataSource,
      &instance->_private.tinyRamDataSource.interface,
      &instance->_private.endiannessSwapBuffer,
      &endiannessSwappedConfiguration);
#endif
}

I_TinyDataSource_t *NanoSystemData_DataSource(NanoSystemData_t *instance)
{
   return &instance->_private.tinyRamDataSource.interface;
}

I_TinyDataSource_t *NanoSystemData_ExternalDataSource(NanoSystemData_t *instance)
{
#ifdef LITTLE_ENDIAN
   return &instance->_private.swappedDataSource.interface;
#else
   return &instance->_private.tinyRamDataSource.interface;
#endif
}
