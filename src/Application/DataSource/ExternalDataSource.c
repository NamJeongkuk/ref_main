/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ExternalDataSource.h"
#include "SystemErds.h"
#include "DeltaGridLines.h"
#include "StdintSwapDefinitions.h"

#define EXPAND_AS_MAPPED_ERD_PAIR(Name, Number, DataType, Swap, Io, Sub, StorageType, NvDefaultData, FaultId) \
   CONCAT(INCLUDE_RAM_, StorageType)                                                                          \
   ({ Number COMMA Name } COMMA)

static const DataSource_MappedErdPair_t internalExternalMappings[] = {
   ERD_TABLE(EXPAND_AS_MAPPED_ERD_PAIR)
};

static const ConstArrayMap_DirectIndexConfiguration_t internalToExternalMapConfiguration = {
   internalExternalMappings,
   ELEMENT_SIZE(internalExternalMappings),
   NUM_ELEMENTS(internalExternalMappings),
   RamErdBase
};

static const ConstArrayMap_BinarySearchConfiguration_t externalToInternalMapConfiguration = {
   internalExternalMappings,
   NUM_ELEMENTS(internalExternalMappings),
   ELEMENT_SIZE(internalExternalMappings),
   MEMBER_SIZE(DataSource_MappedErdPair_t, mappedErdId),
   OFFSET_OF(DataSource_MappedErdPair_t, mappedErdId),
   IS_SIGNED(Erd_t)
};

#define EXPAND_AS_SWAPPED_FIELDS(Name, Number, DataType, Swap, Io, Sub, StorageType, NvDefaultData, FaultId) \
   CONCAT(INCLUDE_SWAP_, Swap)                                                                               \
   (CONCAT(SWAP_DEFINITION_, DataType)(Public##Name, DataType))

static const DataSource_EndiannessSwappedSwappedField_t swappedFields[] = {
   ERD_TABLE(EXPAND_AS_SWAPPED_FIELDS)
};

#define EXPAND_AS_SWAPPED_RANGE(Name, Number, DataType, Swap, Io, Sub, StorageType, NvDefaultData, FaultId) \
   CONCAT(INCLUDE_SWAP_RANGE_START_, Swap)                                                                  \
   ({ Public##Name COMMA)                                                                                   \
   CONCAT(INCLUDE_SWAP_RANGE_END_, Swap)                                                                    \
   ( Public##Name } COMMA)

static const DataSource_EndiannessSwappedSwappedFieldRange_t swappedRanges[] = {
   ERD_TABLE(EXPAND_AS_SWAPPED_RANGE)
};

static const DataSource_EndiannessSwappedSwappedRangesConfiguration_t endianessSwappedSwappedRangesConfiguration = {
   swappedRanges,
   NUM_ELEMENTS(swappedRanges)
};

static const ConstMultiMap_BinarySearchConfiguration_t endiannessSwappedFieldsMapConfiguration = {
   swappedFields,
   NUM_ELEMENTS(swappedFields),
   sizeof(DataSource_EndiannessSwappedSwappedField_t),
   MEMBER_SIZE(DataSource_EndiannessSwappedSwappedField_t, erd),
   OFFSET_OF(DataSource_EndiannessSwappedSwappedField_t, erd),
   sizeof(DataSource_EndiannessSwappedSwappedField_t),
   0,
   IS_SIGNED(Erd_t)
};

void ExternalDataSource_Init(
   ExternalDataSource_t *instance,
   I_DataSource_t *dataSource)
{
   ConstArrayMap_DirectIndex_Init(&instance->_private.internalToExternalMap, &internalToExternalMapConfiguration);
   ConstArrayMap_BinarySearch_Init(&instance->_private.externalToInternalMap, &externalToInternalMapConfiguration);
   ConstBidirectionalMap_ConstArrayMap_Init(
      &instance->_private.internalExternalMap,
      &instance->_private.externalToInternalMap.interface,
      &instance->_private.internalToExternalMap.interface);

   ConstMultiMap_BinarySearch_Init(&instance->_private.swappedFieldsMap, &endiannessSwappedFieldsMapConfiguration);

   DataSource_Mapped_InitWithPassThrough(
      &instance->_private.mappedDataSource,
      dataSource,
      &instance->_private.internalExternalMap.interface);

   DataSource_EndiannessSwapped_Init(
      &instance->_private.endiannessSwappedDataSource,
      &instance->_private.mappedDataSource.interface,
      &instance->_private.swappedFieldsMap.interface,
      &endianessSwappedSwappedRangesConfiguration,
      false);
}

I_DataSource_t *ExternalDataSource_DataSource(ExternalDataSource_t *instance)
{
   return &instance->_private.endiannessSwappedDataSource.interface;
}
