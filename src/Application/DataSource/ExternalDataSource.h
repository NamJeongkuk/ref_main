/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef EXTERNALDATASOURCE_H
#define EXTERNALDATASOURCE_H

#include "DataSource_Mapped.h"
#include "DataSource_SlowEndiannessSwapped.h"
#include "ConstBidirectionalMap_ConstArrayMap.h"
#include "ConstArrayMap_BinarySearch.h"
#include "ConstArrayMap_DirectIndex.h"

typedef struct
{
   struct
   {
      DataSource_Mapped_t mappedDataSource;
      DataSource_SlowEndiannessSwapped_t endiannessSwappedDataSource;

      ConstArrayMap_DirectIndex_t internalToExternalMap;
      ConstArrayMap_BinarySearch_t externalToInternalMap;
      ConstBidirectionalMap_ConstArrayMap_t internalExternalMap;
   } _private;
} ExternalDataSource_t;

/*!
 * @param instance
 * @param dataSource
 */
void ExternalDataSource_Init(
   ExternalDataSource_t *instance,
   I_DataSource_t *dataSource);

/*!
 * @param instance
 * @return
 */
I_DataSource_t * ExternalDataSource_DataSource(ExternalDataSource_t *instance);

#endif
