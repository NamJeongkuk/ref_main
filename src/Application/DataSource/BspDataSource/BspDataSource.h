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
#include "BspParametricConfiguration.h"
#include "Timer.h"
#include "I_Interrupt.h"

typedef struct
{
   struct
   {
      I_DataSource_t *dataSource;
      DataSource_Mapped_t applicationToBspMappedDataSource;
      DataSource_Mapped_t parametricToApplicationBspMappedDataSource;

      const BspParametricConfiguration_t *bspParametricConfiguration;
      ConstArrayMap_LinearSearch_t applicationToBspMap;
      ConstArrayMap_BinarySearch_t bspToApplicationMap;
      ConstBidirectionalMap_ConstArrayMap_t applicationToBspBidirectionalMap;

      ConstArrayMap_LinearSearch_t parametricToApplicationBspMap;
      ConstArrayMap_BinarySearch_t applicationBspToParametricMap;
      ConstBidirectionalMap_ConstArrayMap_t parametricToApplicationBspBidirectionalMap;
   } _private;
} BspDataSource_t;

/*!
 * Initialize BSP data source components
 * @param instance
 * @param timerModule
 * @param debounceInterrupt
 */
void BspDataSource_Init(
   BspDataSource_t *instance,
   TimerModule_t *timerModule,
   I_Interrupt_t *debounceInterrupt);

/*!
 * Get the interface to the BSP data source
 * @param instance
 * @return
 */
I_DataSource_t *BspDataSource_DataSource(BspDataSource_t *instance);

#endif
