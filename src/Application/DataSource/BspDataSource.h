/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef BSPDATASOURCE_H
#define BSPDATASOURCE_H

#include "Bsp.h"
#include "DataSource_Mapped.h"
#include "ConstArrayMap_LinearSearch.h"
#include "ConstArrayMap_BinarySearch.h"
#include "ConstBidirectionalMap_ConstArrayMap.h"

typedef struct
{
   struct
   {
      Bsp_t bsp;
      DataSource_Mapped_t mappedDataSource;

      ConstArrayMap_LinearSearch_t applicationToBspMap;
      ConstArrayMap_BinarySearch_t bspToApplicationMap;
      ConstBidirectionalMap_ConstArrayMap_t applicationToBspBiDirectionalMap;
   } _private;
} BspDataSource_t;

/*!
 * Initialize BSP data source components
 * @param instance
 * @param timerModule
 */
void BspDataSource_Init(
   BspDataSource_t *instance,
   TimerModule_t *timerModule);

/*!
 * Get the interface to the BSP data source
 * @param instance
 * @return
 */
I_DataSource_t *BspDataSource_DataSource(BspDataSource_t *instance);

#endif
