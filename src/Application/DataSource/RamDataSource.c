/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <stdint.h>
#include "RamDataSource.h"
#include "utils.h"
#include "uassert.h"

// clang-format off

#define EXPAND_AS_CONFIGURATION(Name, Number, DataType, Swap, Io, Sub, StorageType, NvDefaultData, FaultId) \
   CONCAT(INCLUDE_RAM_, StorageType)({ Name COMMA OFFSET_OF(RamDataSourceStorage_t, MACRO_SAFE_CONCATENATE(erd, Name)) } COMMA)

// clang-format on

static const DataSource_RamErdConfigurationElement_t erds[] = { ERD_TABLE(EXPAND_AS_CONFIGURATION) };

static const ConstArrayMap_DirectIndexConfiguration_t erdMapConfiguration = {
   erds,
   ELEMENT_SIZE(erds),
   NUM_ELEMENTS(erds),
   RamErdBase
};

void RamDataSource_Init(RamDataSource_t *instance)
{
   uassert(instance);

   ConstArrayMap_DirectIndex_Init(&instance->_private.erdMap, &erdMapConfiguration);

   DataSource_Ram_Init(
      &instance->_private.dataSource,
      &instance->_private.erdRam,
      sizeof(instance->_private.erdRam),
      &instance->_private.erdMap.interface);
}

I_DataSource_t *RamDataSource_DataSource(RamDataSource_t *instance)
{
   return &instance->_private.dataSource.interface;
}
