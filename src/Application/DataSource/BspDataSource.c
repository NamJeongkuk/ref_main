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
   { Erd_RelayWatchdog, Erd_BspGpio_RLY_09 },
   { Erd_Adc_ADC_0, Erd_BspAdc_SomeAnalogInput },
   { Erd_Adc_ADC_1, Erd_BspAdc_AnotherAnalogInput },
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
   { Erd_RelayWatchdog, Erd_BspGpio_RLY_09 },
   { Erd_HeartbeatLed, Erd_BspGpio_HEARTBEAT },
   { Erd_Gpio_PWM_VAR_01, Erd_BspGpio_PWM_VAR_01 },
   { Erd_Gpio_GPIO_OUT_00, Erd_BspGpio_GPIO_OUT_00 },
   { Erd_Gpio_GPIO_OUT_01, Erd_BspGpio_GPIO_OUT_01 },
   { Erd_Gpio_GPIO_OUT_02, Erd_BspGpio_GPIO_OUT_02 },
   { Erd_Gpio_HTR_00, Erd_BspGpio_HTR_00 },
   { Erd_Gpio_HTR_01, Erd_BspGpio_HTR_01 },
   { Erd_Gpio_HTR_02, Erd_BspGpio_HTR_02 },
   { Erd_Gpio_HTR_03, Erd_BspGpio_HTR_03 },
   { Erd_Gpio_HTR_04, Erd_BspGpio_HTR_04 },
   { Erd_Gpio_MTR_DRV_EN_00, Erd_BspGpio_MTR_DRV_EN_00 },
   { Erd_Gpio_MTR_DRV_EN_01, Erd_BspGpio_MTR_DRV_EN_01 },
   { Erd_Gpio_MTR_DRV_00, Erd_BspGpio_MTR_DRV_00 },
   { Erd_Gpio_MTR_DRV_01, Erd_BspGpio_MTR_DRV_01 },
   { Erd_Gpio_MTR_DRV_02, Erd_BspGpio_MTR_DRV_02 },
   { Erd_Gpio_MTR_DRV_03, Erd_BspGpio_MTR_DRV_03 },
   { Erd_Gpio_REF_VAL_00, Erd_BspGpio_REF_VAL_00 },
   { Erd_Gpio_REF_VAL_01, Erd_BspGpio_REF_VAL_01 },
   { Erd_Gpio_REF_VAL_02, Erd_BspGpio_REF_VAL_02 },
   { Erd_Gpio_REF_VAL_03, Erd_BspGpio_REF_VAL_03 },
   { Erd_Gpio_RLY_00, Erd_BspGpio_RLY_00 },
   { Erd_Gpio_RLY_01, Erd_BspGpio_RLY_01 },
   { Erd_Gpio_RLY_02, Erd_BspGpio_RLY_02 },
   { Erd_Gpio_RLY_03, Erd_BspGpio_RLY_03 },
   { Erd_Gpio_RLY_04, Erd_BspGpio_RLY_04 },
   { Erd_Gpio_RLY_05, Erd_BspGpio_RLY_05 },
   { Erd_Gpio_RLY_06, Erd_BspGpio_RLY_06 },
   { Erd_Gpio_RLY_07, Erd_BspGpio_RLY_07 },
   { Erd_Gpio_RLY_08, Erd_BspGpio_RLY_08 },
   { Erd_Gpio_AUGER_PWR, Erd_BspGpio_AUGER_PWR },
   { Erd_Gpio_AUGER_DIRECTION, Erd_BspGpio_AUGER_DIRECTION },
   { Erd_Gpio_SBC_RESET, Erd_BspGpio_SBC_RESET },
   { Erd_Gpio_PERSONALITY, Erd_BspGpio_PERSONALITY },
   { Erd_Gpio_DOOR_INT, Erd_BspGpio_DOOR_INT },
   { Erd_Gpio_DOOR_01, Erd_BspGpio_DOOR_01 },
   { Erd_Gpio_DOOR_02, Erd_BspGpio_DOOR_02 },
   { Erd_Gpio_DOOR_03, Erd_BspGpio_DOOR_03 },
   { Erd_Gpio_DOOR_04, Erd_BspGpio_DOOR_04 },
   { Erd_Gpio_GPIO_IN_00, Erd_BspGpio_GPIO_IN_00 },
   { Erd_Gpio_GPIO_IN_01, Erd_BspGpio_GPIO_IN_01 },
   { Erd_Gpio_GPIO_IN_02, Erd_BspGpio_GPIO_IN_02 },
   { Erd_Gpio_GPIO_IN_03, Erd_BspGpio_GPIO_IN_03 },
   { Erd_Gpio_SABBATH, Erd_BspGpio_SABBATH },
   { Erd_Adc_ANALOG_475K_TH_00, Erd_BspAdc_ANALOG_475K_TH_00 },
   { Erd_Adc_ANALOG_TH_LOW_00, Erd_BspAdc_ANALOG_TH_LOW_00 },
   { Erd_Adc_ANALOG_TH_LOW_01, Erd_BspAdc_ANALOG_TH_LOW_01 },
   { Erd_Adc_ANALOG_TH_LOW_02, Erd_BspAdc_ANALOG_TH_LOW_02 },
   { Erd_Adc_ANALOG_TH_LOW_03, Erd_BspAdc_ANALOG_TH_LOW_03 },
   { Erd_Adc_ANALOG_TH_LOW_04, Erd_BspAdc_ANALOG_TH_LOW_04 },
   { Erd_Adc_ANALOG_TH_LOW_05, Erd_BspAdc_ANALOG_TH_LOW_05 },
   { Erd_Adc_ANALOG_TH_LOW_06, Erd_BspAdc_ANALOG_TH_LOW_06 },
   { Erd_Adc_ANALOG_TH_LOW_07, Erd_BspAdc_ANALOG_TH_LOW_07 },
   { Erd_Adc_ANALOG_TH_LOW_08, Erd_BspAdc_ANALOG_TH_LOW_08 },
   { Erd_Adc_LOAD_CURRENT, Erd_BspAdc_LOAD_CURRENT },
   { Erd_Adc_HW_PERSONALITY_00, Erd_BspAdc_HW_PERSONALITY_00 },
   { Erd_Adc_HW_PERSONALITY_01, Erd_BspAdc_HW_PERSONALITY_01 },
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
