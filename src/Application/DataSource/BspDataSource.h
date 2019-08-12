/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef BSPDATASOURCE_H
#define BSPDATASOURCE_H

#include "DataSource_Bsp.h"
#include "DataSource_Mapped.h"
#include "ConstArrayMap_LinearSearch.h"
#include "ConstArrayMap_BinarySearch.h"
#include "ConstBidirectionalMap_ConstArrayMap.h"

typedef struct
{
   struct
   {
      DataSource_Bsp_t dataSource;
      DataSource_Mapped_t mappedDataSource;

      ConstArrayMap_LinearSearch_t applicationToBspMap;
      ConstArrayMap_BinarySearch_t bspToApplicationMap;
      ConstBidirectionalMap_ConstArrayMap_t applicationToBspBiDirectionalMap;
   } _private;
} BspDataSource_t;

void BspDataSource_Init(BspDataSource_t *instance);

I_DataSource_t *BspDataSource_DataSource(BspDataSource_t *instance);

#endif
