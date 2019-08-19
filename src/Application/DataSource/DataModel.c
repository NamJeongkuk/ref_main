/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "DataModel.h"

#define EXPAND_AS_INPUT_AND_INPUT_OUTPUT_RAM_ERDS(Name, Number, DataType, Swap, Io, StorageType, NvDefaultData, FaultIda) \
   CONCAT(INCLUDE_RAM_, StorageType)(CONCAT(INCLUDE_INPUT_OUTPUT_, Io)(Name COMMA))

#define EXPAND_AS_INPUT_AND_INPUT_OUTPUT_VIRTUAL_ERDS(Name, Number, DataType, Swap, Io, StorageType, NvDefaultData, FaultIda) \
   CONCAT(INCLUDE_VIRTUAL_, StorageType)(CONCAT(INCLUDE_INPUT_OUTPUT_, Io)(Name COMMA))

#define EXPAND_AS_INPUT_AND_INPUT_OUTPUT_NV_PUBLIC_ERDS(Name, Number, DataType, Swap, Io, StorageType, NvDefaultData, FaultIda) \
   CONCAT(INCLUDE_NV_PUBLIC_, StorageType)(CONCAT(INCLUDE_INPUT_OUTPUT_, Io)(Name COMMA))

#define EXPAND_AS_INPUT_AND_INPUT_OUTPUT_NV_PRIVATE_ERDS(Name, Number, DataType, Swap, Io, StorageType, NvDefaultData, FaultIda) \
   CONCAT(INCLUDE_NV_PRIVATE_, StorageType)(CONCAT(INCLUDE_INPUT_OUTPUT_, Io)(Name COMMA))

#define EXPAND_AS_INPUT_AND_INPUT_OUTPUT_FAULT_ERDS(Name, Number, DataType, Swap, Io, StorageType, NvDefaultData, FaultIda) \
   CONCAT(INCLUDE_FAULT_, StorageType)(CONCAT(INCLUDE_INPUT_OUTPUT_, Io)(Name COMMA))

#define EXPAND_AS_OUTPUT_RAM_ERDS(Name, Number, DataType, Swap, Io, StorageType, NvDefaultData, FaultId) \
   CONCAT(INCLUDE_RAM_, StorageType)(CONCAT(INCLUDE_OUTPUT_, Io)(Name COMMA))

#define EXPAND_AS_OUTPUT_VIRTUAL_ERDS(Name, Number, DataType, Swap, Io, StorageType, NvDefaultData, FaultId) \
   CONCAT(INCLUDE_VIRTUAL_, StorageType)(CONCAT(INCLUDE_OUTPUT_, Io)(Name COMMA))

#define EXPAND_AS_OUTPUT_NV_PUBLIC_ERDS(Name, Number, DataType, Swap, Io, StorageType, NvDefaultData, FaultId) \
   CONCAT(INCLUDE_NV_PUBLIC_, StorageType)(CONCAT(INCLUDE_OUTPUT_, Io)(Name COMMA))

#define EXPAND_AS_OUTPUT_NV_PRIVATE_ERDS(Name, Number, DataType, Swap, Io, StorageType, NvDefaultData, FaultId) \
   CONCAT(INCLUDE_NV_PRIVATE_, StorageType)(CONCAT(INCLUDE_OUTPUT_, Io)(Name COMMA))

#define EXPAND_AS_OUTPUT_FAULT_ERDS(Name, Number, DataType, Swap, Io, StorageType, NvDefaultData, FaultId) \
   CONCAT(INCLUDE_FAULT_, StorageType)(CONCAT(INCLUDE_OUTPUT_, Io)(Name COMMA))

static const Erd_t inputAndInputOutputErds[] =
   {
      ERD_TABLE(EXPAND_AS_INPUT_AND_INPUT_OUTPUT_NV_PUBLIC_ERDS)
      ERD_TABLE(EXPAND_AS_INPUT_AND_INPUT_OUTPUT_NV_PRIVATE_ERDS)
      ERD_TABLE(EXPAND_AS_INPUT_AND_INPUT_OUTPUT_RAM_ERDS)
      ERD_TABLE(EXPAND_AS_INPUT_AND_INPUT_OUTPUT_VIRTUAL_ERDS)
      ERD_TABLE(EXPAND_AS_INPUT_AND_INPUT_OUTPUT_FAULT_ERDS)
   };

static const Erd_t outputErds[] =
   {
      ERD_TABLE(EXPAND_AS_OUTPUT_NV_PUBLIC_ERDS)
      ERD_TABLE(EXPAND_AS_OUTPUT_NV_PRIVATE_ERDS)
      ERD_TABLE(EXPAND_AS_OUTPUT_RAM_ERDS)
      ERD_TABLE(EXPAND_AS_OUTPUT_VIRTUAL_ERDS)
      ERD_TABLE(EXPAND_AS_OUTPUT_FAULT_ERDS)
   };

static const ConstArrayMap_BinarySearchConfiguration_t outputErdList =
   {
      outputErds,
      NUM_ELEMENTS(outputErds),
      sizeof(Erd_t),
      sizeof(Erd_t),
      0,
      IS_SIGNED(Erd_t)
   };

static const ConstArrayMap_BinarySearchConfiguration_t inputAndInputOutputErdList =
   {
      inputAndInputOutputErds,
      NUM_ELEMENTS(inputAndInputOutputErds),
      sizeof(Erd_t),
      sizeof(Erd_t),
      0,
      IS_SIGNED(Erd_t)
   };

void DataModel_Init(DataModel_t *instance, I_DataSource_t *dataSource)
{
   ConstArrayMap_BinarySearch_Init(&instance->_private.outputErdMap, &outputErdList);
   ConstArrayMap_BinarySearch_Init(&instance->_private.inputAndInputOutputErdMap, &inputAndInputOutputErdList);

   DataModel_DataSource_Init(
      &instance->_private.dataModel,
      dataSource,
      &instance->_private.inputAndInputOutputErdMap.interface,
      &instance->_private.outputErdMap.interface,
      instance->_private.outputs,
      NUM_ELEMENTS(instance->_private.outputs),
      instance->_private.inputOutputs,
      NUM_ELEMENTS(instance->_private.inputOutputs),
      true,
      true);
}

I_DataModel_t *DataModel_DataModel(DataModel_t *instance)
{
   return &instance->_private.dataModel.interface;
}
