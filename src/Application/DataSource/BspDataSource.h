/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef BSPDATASOURCE_H
#define BSPDATASOURCE_H

#include "DataSource_Bsp.h"
#include "ConstArrayMap_BinarySearch.h"

typedef struct
{
   struct
   {
      DataSource_Bsp_t dataSource;
      ConstArrayMap_BinarySearch_t erdGpioChannelMap;
   } _private;
} BspDataSource_t;

void BspDataSource_Init(BspDataSource_t *instance);

I_DataSource_t * BspDataSource_DataSource(BspDataSource_t *instance);

#endif
