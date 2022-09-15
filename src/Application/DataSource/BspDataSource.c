/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "BspDataSource.h"
#include "SystemErds.h"
#include "PersonalityId.h"
#include "ParametricDataTableOfContents.h"
#include "ParametricData.h"

void BspDataSource_Init(
   BspDataSource_t *instance,
   TimerModule_t *timerModule)
{
   const ParametricDataTableOfContents_t *parametricData = ParametricData_GetParametricTableOfContents();

   instance->_private.dataSource = DataSource_Bsp_Init(timerModule);
   instance->_private.bspParametricConfiguration = parametricData->personalities[PersonalityId_Default]->bspConfigurationData;

   ConstArrayMap_LinearSearch_Init(
      &instance->_private.applicationToBspMap,
      instance->_private.bspParametricConfiguration->applicationToBspMapConfiguration);

   ConstArrayMap_BinarySearch_Init(
      &instance->_private.bspToApplicationMap,
      instance->_private.bspParametricConfiguration->bspToApplicationMapConfiguration);

   ConstBidirectionalMap_ConstArrayMap_Init(
      &instance->_private.applicationToBspBiDirectionalMap,
      &instance->_private.applicationToBspMap.interface,
      &instance->_private.bspToApplicationMap.interface);

   DataSource_Mapped_Init(
      &instance->_private.mappedDataSource,
      instance->_private.dataSource,
      &instance->_private.applicationToBspBiDirectionalMap.interface);
}

I_DataSource_t *BspDataSource_DataSource(BspDataSource_t *instance)
{
   return &instance->_private.mappedDataSource.interface;
}
