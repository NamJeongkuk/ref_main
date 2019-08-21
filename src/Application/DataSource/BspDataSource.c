/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "BspDataSource.h"
#include "SystemErds.h"

static const DataSource_MappedErdPair_t applicationToBspMappedPairs[] =
   {
      { Erd_HeartbeatLed, Erd_BspGpio_HeartbeatLed },
      { Erd_OtherLed, Erd_BspGpio_OtherLed },
      { Erd_PushButtonSwitch, Erd_BspGpio_PushButtonSwitch },
      { Erd_SomeAnalogInput, Erd_BspAdc_SomeAnalogInput },
      { Erd_AnotherAnalogInput, Erd_BspAdc_AnotherAnalogInput }
   };

static const ConstArrayMap_BinarySearchConfiguration_t bspToApplicationMapConfiguration =
   {
      applicationToBspMappedPairs,
      NUM_ELEMENTS(applicationToBspMappedPairs),
      ELEMENT_SIZE(applicationToBspMappedPairs),
      MEMBER_SIZE(DataSource_MappedErdPair_t, baseErdId),
      OFFSET_OF(DataSource_MappedErdPair_t, baseErdId),
      IS_SIGNED(Erd_t)
   };

static const ConstArrayMap_LinearSearchConfiguration_t applicationToBspMapConfiguration =
   {
      applicationToBspMappedPairs,
      NUM_ELEMENTS(applicationToBspMappedPairs),
      ELEMENT_SIZE(applicationToBspMappedPairs),
      MEMBER_SIZE(DataSource_MappedErdPair_t, mappedErdId),
      OFFSET_OF(DataSource_MappedErdPair_t, mappedErdId)
   };

void BspDataSource_Init(
   BspDataSource_t *instance,
   TimerModule_t *timerModule)
{
   DataSource_Bsp_Init(
      &instance->_private.dataSource,
      timerModule);

   ConstArrayMap_LinearSearch_Init(
      &instance->_private.applicationToBspMap,
      &applicationToBspMapConfiguration);

   ConstArrayMap_BinarySearch_Init(
      &instance->_private.bspToApplicationMap,
      &bspToApplicationMapConfiguration);

   ConstBidirectionalMap_ConstArrayMap_Init(
      &instance->_private.applicationToBspBiDirectionalMap,
      &instance->_private.applicationToBspMap.interface,
      &instance->_private.bspToApplicationMap.interface);

   DataSource_Mapped_Init(
      &instance->_private.mappedDataSource,
      &instance->_private.dataSource.interface,
      &instance->_private.applicationToBspBiDirectionalMap.interface);
}

I_DataSource_t *BspDataSource_DataSource(BspDataSource_t *instance)
{
   return &instance->_private.mappedDataSource.interface;
}
