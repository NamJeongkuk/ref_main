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
#include "DataSource_Gpio.h"
#include "DataSource_Adc.h"
#include "DataSource_OldHw_Pwm.h"
#include "DataSource_OldHw_InputCapture.h"
#include "DataSource_OldHw_Personality.h"

#ifdef OLD_HW
static const DataSource_MappedErdPair_t applicationBspToBspMappedPairs[] = {
   { Erd_GpioGroupInterface, Erd_GpioGroupInterface },
   { Erd_HeartbeatLed, Erd_BspGpio_HeartbeatLed },
   { Erd_RelayWatchdog, Erd_BspGpio_RelayWatchdog },
   { Erd_ApplicationToBspAdc_ADC_0, Erd_BspAdc_SomeAnalogInput },
   { Erd_ApplicationToBspAdc_ADC_1, Erd_BspAdc_AnotherAnalogInput },
   { Erd_ApplicationToBspPwm_PWM_0, Erd_BspPwm_PWM_0 },
   { Erd_ApplicationToBspPwm_PWM_1, Erd_BspPwm_PWM_1 },
   { Erd_ApplicationToBspPwm_PWM_2, Erd_BspPwm_PWM_2 },
   { Erd_ApplicationToBspPwm_PWM_4, Erd_BspPwm_PWM_4 },
   { Erd_ApplicationToBspPwm_PWM_5, Erd_BspPwm_PWM_5 },
   { Erd_ApplicationToBspInputCapture_CAPT_0, Erd_BspFanInputCapture_CAPT_0 },
   { Erd_ApplicationToBspInputCapture_CAPT_1, Erd_BspFanInputCapture_CAPT_1 },
   { Erd_ApplicationToBspInputCapture_CAPT_2, Erd_BspFanInputCapture_CAPT_2 },
   { Erd_ApplicationToBspInputCapture_CAPT_4, Erd_BspFanInputCapture_CAPT_4 },
   { Erd_ApplicationToBspInputCapture_CAPT_5, Erd_BspFanInputCapture_CAPT_5 },
};
#else
static const DataSource_MappedErdPair_t applicationBspToBspMappedPairs[] = {
   { Erd_GpioGroupInterface, Erd_GpioGroupInterface },
   { Erd_HeartbeatLed, Erd_BspGpio_HeartbeatLed },
};
#endif

static const ConstArrayMap_BinarySearchConfiguration_t bspToApplicationMapConfiguration = {
   applicationBspToBspMappedPairs,
   NUM_ELEMENTS(applicationBspToBspMappedPairs),
   ELEMENT_SIZE(applicationBspToBspMappedPairs),
   MEMBER_SIZE(DataSource_MappedErdPair_t, baseErdId),
   OFFSET_OF(DataSource_MappedErdPair_t, baseErdId),
   IS_SIGNED(Erd_t)
};

static const ConstArrayMap_LinearSearchConfiguration_t applicationToBspMapConfiguration = {
   applicationBspToBspMappedPairs,
   NUM_ELEMENTS(applicationBspToBspMappedPairs),
   ELEMENT_SIZE(applicationBspToBspMappedPairs),
   MEMBER_SIZE(DataSource_MappedErdPair_t, mappedErdId),
   OFFSET_OF(DataSource_MappedErdPair_t, mappedErdId)
};

static void InitializeApplicationToBspMappedDataSource(BspDataSource_t *instance)
{
   ConstArrayMap_LinearSearch_Init(
      &instance->_private.applicationToBspMap,
      &applicationToBspMapConfiguration);

   ConstArrayMap_BinarySearch_Init(
      &instance->_private.bspToApplicationMap,
      &bspToApplicationMapConfiguration);

   ConstBidirectionalMap_ConstArrayMap_Init(
      &instance->_private.applicationToBspBidirectionalMap,
      &instance->_private.applicationToBspMap.interface,
      &instance->_private.bspToApplicationMap.interface);

   DataSource_Mapped_Init(
      &instance->_private.applicationToBspMappedDataSource,
      instance->_private.dataSource,
      &instance->_private.applicationToBspBidirectionalMap.interface);
}

static void InitializeParametricToApplicationBspMappedDataSource(BspDataSource_t *instance)
{
   const ParametricDataTableOfContents_t *parametricData = ParametricData_GetParametricTableOfContents();
   instance->_private.bspParametricConfiguration = parametricData->personalities[PersonalityId_Default]->bspConfigurationData;

   ConstArrayMap_LinearSearch_Init(
      &instance->_private.parametricToApplicationBspMap,
      instance->_private.bspParametricConfiguration->parametricToBspMapConfiguration);

   ConstArrayMap_BinarySearch_Init(
      &instance->_private.applicationBspToParametricMap,
      instance->_private.bspParametricConfiguration->bspToParametricMapConfiguration);

   ConstBidirectionalMap_ConstArrayMap_Init(
      &instance->_private.parametricToApplicationBspBidirectionalMap,
      &instance->_private.parametricToApplicationBspMap.interface,
      &instance->_private.applicationBspToParametricMap.interface);

   DataSource_Mapped_InitWithPassThrough(
      &instance->_private.parametricToApplicationBspMappedDataSource,
      &instance->_private.applicationToBspMappedDataSource.interface,
      &instance->_private.parametricToApplicationBspBidirectionalMap.interface);
}

void BspDataSource_Init(
   BspDataSource_t *instance,
   TimerModule_t *timerModule)
{
   instance->_private.dataSource = DataSource_Bsp_Init(timerModule);

   InitializeApplicationToBspMappedDataSource(instance);
   InitializeParametricToApplicationBspMappedDataSource(instance);
}

I_DataSource_t *BspDataSource_DataSource(BspDataSource_t *instance)
{
   return &instance->_private.parametricToApplicationBspMappedDataSource.interface;
}
