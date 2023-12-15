/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "UnmappedBspDataSource.h"
#include "SystemErds.h"

// clang-format off

#define EXPAND_AS_CONFIGURATION(Name, Number, DataType, Swap, Io, Sub, StorageType, NvDefaultData, YearlyWrites, FaultId) \
   CONCAT(INCLUDE_MAPPED_BSP_, StorageType)({ Name COMMA sizeof(Erd_t) } COMMA)

// clang-format on

static const DataSource_ZerosErdConfigurationElement_t erds[] = { ERD_TABLE(EXPAND_AS_CONFIGURATION) };

static const ConstArrayMap_BinarySearchConfiguration_t erdMapConfiguration = {
   erds,
   NUM_ELEMENTS(erds),
   ELEMENT_SIZE(erds),
   MEMBER_SIZE(DataSource_ZerosErdConfigurationElement_t, erd),
   OFFSET_OF(DataSource_ZerosErdConfigurationElement_t, erd),
   IS_SIGNED(Erd_t)
};

void UnmappedBspDataSource_Init(UnmappedBspDataSource_t *instance)
{
   ConstArrayMap_BinarySearch_Init(
      &instance->_private.erdMap,
      &erdMapConfiguration);

   DataSource_Zeros_Init(
      &instance->_private.dataSourceZeros,
      &instance->_private.erdMap.interface);
}

I_DataSource_t *UnmappedBspDataSource_DataSource(UnmappedBspDataSource_t *instance)
{
   return &instance->_private.dataSourceZeros.interface;
}
