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

#define EXPAND_AS_CONFIGURATION(Name, Number, DataType, Swap, Io, StorageType, NvDefaultData) \
   CONCAT(INCLUDE_RAM_, StorageType)                                                                             \
   ({ Name COMMA OFFSET_OF(RamDataSourceStorage_t, MACRO_SAFE_CONCATENATE(erd, Name)) } COMMA)

static const DataSource_RamErdConfigurationElement_t erds[] =
   { ERD_TABLE(EXPAND_AS_CONFIGURATION) };

static const ConstArrayMap_BinarySearchConfiguration_t erdMapConfiguration =
   {
      erds,
      NUM_ELEMENTS(erds),
      ELEMENT_SIZE(erds),
      MEMBER_SIZE(DataSource_RamErdConfigurationElement_t, erd),
      OFFSET_OF(DataSource_RamErdConfigurationElement_t, erd),
      IS_SIGNED(Erd_t)
   };

void RamDataSource_Init(RamDataSource_t *instance)
{
   uassert(instance);

   ConstArrayMap_BinarySearch_Init(&instance->_private.erdMap, &erdMapConfiguration);

   DataSource_Ram_Init(
      &instance->_private.dataSource,
      &instance->_private.erdRam,
      sizeof(instance->_private.erdRam),
      &instance->_private.erdMap.interface);
}

I_DataSource_t * RamDataSource_DataSource(RamDataSource_t *instance)
{
   return &instance->_private.dataSource.interface;
}
