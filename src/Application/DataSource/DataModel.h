/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DATAMODEL_H
#define DATAMODEL_H

#include "ConstArrayMap_BinarySearch.h"
#include "DataModel_DataSource.h"
#include "SystemErds.h"
#include "XMacroUtils.h"

// clang-format off

#define EXPAND_AS_INPUT_AND_INPUT_OUTPUT_ERDS_TYPE(Name, Number, DataType, Swap, Io, Sub, StorageType, NvDefaultData, YearlyWrites, FaultId) \
   CONCAT(INCLUDE_INPUT_OUTPUT_, Io)(uint8_t Name;)

#define EXPAND_AS_OUTPUT_ERDS_TYPE(Name, Number, DataType, Swap, Io, Sub, StorageType, NvDefaultData, YearlyWrites, FaultId) \
   CONCAT(INCLUDE_OUTPUT_, Io)(uint8_t Name;)

#define EXPAND_AS_SUBSCRIPTION_ERDS_TYPE(Name, Number, DataType, Swap, Io, Sub, StorageType, NvDefaultData, YearlyWrites, FaultId) \
   CONCAT(INCLUDE_SUBSCRIPTION_, Sub)(uint8_t Name;)

// clang-format on

typedef struct
{
   ERD_TABLE(EXPAND_AS_INPUT_AND_INPUT_OUTPUT_ERDS_TYPE)
} SystemDataInputAndInputOutputList_t;

typedef struct
{
   ERD_TABLE(EXPAND_AS_OUTPUT_ERDS_TYPE)
} SystemDataOutputList_t;

typedef struct
{
   ERD_TABLE(EXPAND_AS_SUBSCRIPTION_ERDS_TYPE)
} SystemDataSubscriptionListList_t;

typedef struct
{
   struct
   {
      DataModel_DataSource_t dataModel;
      ConstArrayMap_BinarySearch_t outputErdMap;
      ConstArrayMap_BinarySearch_t inputAndInputOutputErdMap;
      ConstArrayMap_BinarySearch_t subscriptionErdMap;
      DataModel_DataSourceOutputForSingleton_t outputs[sizeof(SystemDataOutputList_t)];
      DataModel_DataSourceInputOutputForSingleton_t inputOutputs[sizeof(SystemDataInputAndInputOutputList_t)];
      DataModel_DataSourceSubscriptionList_t subscriptionLists[sizeof(SystemDataSubscriptionListList_t)];
   } _private;
} DataModel_t;

/*!
 * @param instance
 * @param dataSource
 */
void DataModel_Init(DataModel_t *instance, I_DataSource_t *dataSource);

/*!
 * @param instance
 * @return
 */
I_DataModel_t *DataModel_DataModel(DataModel_t *instance);

#endif
