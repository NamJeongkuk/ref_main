/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ConvertedDataSource.h"
#include "SystemErds.h"

// clang-format off

#define EXPAND_AS_CONVERTED_ERD_SIZE(Name, Number, DataType, Swap, Io, Sub, StorageType, NvDefaultData, YearlyWrites, FaultId) \
   CONCAT(INCLUDE_CONVERTED_, StorageType)                                                                       \
   (SizeOf##Name = sizeof(DataType) COMMA)

enum
{
   ERD_TABLE(EXPAND_AS_CONVERTED_ERD_SIZE)
};

static const DataSource_ConvertedVirtualErd_t convertedVirtualTable[] = {
   CALLBACK_TABLE(EXPAND_AS_INDIRECT_ELEMENTS)
};

static const DataSource_ConvertedDependency_t convertedDependencyTable[] = {
   DEPENDENCY_TABLE(EXPAND_AS_DEPENDENCY_ELEMENTS)
};

// clang-format on

void ConvertedDataSource_Init(ConvertedDataSource_t *instance, I_DataSource_t *toConvert)
{
   DataSource_Converted_Init(
      &instance->_private.dataSource,
      toConvert,
      toConvert,
      convertedVirtualTable,
      NUM_ELEMENTS(convertedVirtualTable),
      convertedDependencyTable,
      NUM_ELEMENTS(convertedDependencyTable));
}

I_DataSource_t *ConvertedDataSource_DataSource(ConvertedDataSource_t *instance)
{
   return &instance->_private.dataSource.interface;
}
