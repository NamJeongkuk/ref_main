/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "DataModel.h"

// clang-format off

#define EXPAND_AS_INPUT_AND_INPUT_OUTPUT_NON_RAM_ERDS(Name, Number, DataType, Swap, Io, Sub, StorageType, NvDefaultData, YearlyWrites, FaultId) \
   CONCAT(INCLUDE_NON_RAM_, StorageType)(CONCAT(INCLUDE_INPUT_OUTPUT_, Io)(Name COMMA))

#define EXPAND_AS_INPUT_AND_INPUT_OUTPUT_RAM_ERDS(Name, Number, DataType, Swap, Io, Sub, StorageType, NvDefaultData, YearlyWrites, FaultId) \
   CONCAT(INCLUDE_RAM_, StorageType)(CONCAT(INCLUDE_INPUT_OUTPUT_, Io)(Name COMMA))

#define EXPAND_AS_OUTPUT_NON_RAM_ERDS(Name, Number, DataType, Swap, Io, Sub, StorageType, NvDefaultData, YearlyWrites, FaultId) \
   CONCAT(INCLUDE_NON_RAM_, StorageType)(CONCAT(INCLUDE_OUTPUT_, Io)(Name COMMA))

#define EXPAND_AS_OUTPUT_RAM_ERDS(Name, Number, DataType, Swap, Io, Sub, StorageType, NvDefaultData, YearlyWrites, FaultId) \
   CONCAT(INCLUDE_RAM_, StorageType)(CONCAT(INCLUDE_OUTPUT_, Io)(Name COMMA))

#define EXPAND_AS_SUBSCRIPTION_NON_RAM_ERDS(Name, Number, DataType, Swap, Io, Sub, StorageType, NvDefaultData, YearlyWrites, FaultId) \
   CONCAT(INCLUDE_NON_RAM_, StorageType)(CONCAT(INCLUDE_SUBSCRIPTION_, Sub)(Name COMMA))

#define EXPAND_AS_SUBSCRIPTION_RAM_ERDS(Name, Number, DataType, Swap, Io, Sub, StorageType, NvDefaultData, YearlyWrites, FaultId) \
   CONCAT(INCLUDE_RAM_, StorageType)(CONCAT(INCLUDE_SUBSCRIPTION_, Sub)(Name COMMA))

static const Erd_t inputAndInputOutputErds[] = {
   ERD_TABLE(EXPAND_AS_INPUT_AND_INPUT_OUTPUT_NON_RAM_ERDS)
   ERD_TABLE(EXPAND_AS_INPUT_AND_INPUT_OUTPUT_RAM_ERDS)
};

static const Erd_t outputErds[] = {
   ERD_TABLE(EXPAND_AS_OUTPUT_NON_RAM_ERDS)
   ERD_TABLE(EXPAND_AS_OUTPUT_RAM_ERDS)
};

static const Erd_t subscriptionErds[] = {
   ERD_TABLE(EXPAND_AS_SUBSCRIPTION_NON_RAM_ERDS)
   ERD_TABLE(EXPAND_AS_SUBSCRIPTION_RAM_ERDS)
};

// clang-format on

static const ConstArrayMap_BinarySearchConfiguration_t inputAndInputOutputErdList = {
   inputAndInputOutputErds,
   NUM_ELEMENTS(inputAndInputOutputErds),
   sizeof(Erd_t),
   sizeof(Erd_t),
   0,
   IS_SIGNED(Erd_t)
};

static const ConstArrayMap_BinarySearchConfiguration_t outputErdList = {
   outputErds,
   NUM_ELEMENTS(outputErds),
   sizeof(Erd_t),
   sizeof(Erd_t),
   0,
   IS_SIGNED(Erd_t)
};

static const ConstArrayMap_BinarySearchConfiguration_t subscriptionErdList = {
   subscriptionErds,
   NUM_ELEMENTS(subscriptionErds),
   sizeof(Erd_t),
   sizeof(Erd_t),
   0,
   IS_SIGNED(Erd_t)
};

void DataModel_Init(DataModel_t *instance, I_DataSource_t *dataSource)
{
   ConstArrayMap_BinarySearch_Init(&instance->_private.inputAndInputOutputErdMap, &inputAndInputOutputErdList);
   ConstArrayMap_BinarySearch_Init(&instance->_private.outputErdMap, &outputErdList);
   ConstArrayMap_BinarySearch_Init(&instance->_private.subscriptionErdMap, &subscriptionErdList);

   DataModel_DataSource_Init(
      &instance->_private.dataModel,
      dataSource,
      &instance->_private.inputAndInputOutputErdMap.interface,
      &instance->_private.outputErdMap.interface,
      &instance->_private.subscriptionErdMap.interface,
      instance->_private.outputs,
      NUM_ELEMENTS(instance->_private.outputs),
      instance->_private.inputOutputs,
      NUM_ELEMENTS(instance->_private.inputOutputs),
      instance->_private.subscriptionLists,
      NUM_ELEMENTS(instance->_private.subscriptionLists),
      true,
      true);
}

I_DataModel_t *DataModel_DataModel(DataModel_t *instance)
{
   return &instance->_private.dataModel.interface;
}
