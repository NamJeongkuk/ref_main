/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <string.h>
#include "DataSource_Adc.h"
#include "I_Adc.h"
#include "Event_Null.h"
#include "uassert.h"
#include "utils.h"
#include "ConstArrayMap_LinearSearch.h"
#include "stm32f3xx.h"

#define ERD_IS_IN_RANGE(erd) (IN_RANGE(Erd_BspAdc_Start + 1, erd, Erd_BspAdc_End))

#define EXPAND_AS_ERD_ADC_CHANNEL_PAIRS(name, port, bit, channel) \
   {                                                              \
      Erd_BspAdc_##name, channel                                  \
   },

#define EXPAND_AS_PORTS_AND_PINS(name, port, pin, channel) \
   {                                                       \
      channel, port, pin                                   \
   },

enum
{
   ConversionTriggerPeriodInMSec = 5,
   HardwareChannelCount = 19
};

typedef struct
{
   Erd_t erd;
   AdcChannel_t channel;
} ErdAdcChannelPair_t;

static const ErdAdcChannelPair_t erdAdcChannelPairs[] =
   {
      ADC_TABLE(EXPAND_AS_ERD_ADC_CHANNEL_PAIRS)
   };

typedef struct
{
   AdcChannel_t channel;
   void *port;
   uint16_t pin;
} AdcPortsAndPins_t;

static const AdcPortsAndPins_t adcPortsAndPins[] =
   {
      ADC_TABLE(EXPAND_AS_PORTS_AND_PINS)
   };

static const ConstArrayMap_LinearSearchConfiguration_t erdToAdcChannelMapConfiguration =
   {
      erdAdcChannelPairs,
      NUM_ELEMENTS(erdAdcChannelPairs),
      ELEMENT_SIZE(erdAdcChannelPairs),
      MEMBER_SIZE(ErdAdcChannelPair_t, erd),
      OFFSET_OF(ErdAdcChannelPair_t, erd)
   };

static struct
{
   I_DataSource_t interface;
   TimerModule_t *timerModule;
   Timer_t timer;
   ADC_HandleTypeDef adcHandle;
   DMA_HandleTypeDef dmaHandle;
   ConstArrayMap_LinearSearch_t erdToChannelMap;
   AdcCounts_t buffer[NUM_ELEMENTS(erdAdcChannelPairs)];
   uint8_t channelIndex;
} instance;

static void ConfigurePins(void)
{
   GPIO_InitTypeDef gpioInitStruct = { 0 };

   __HAL_RCC_GPIOA_CLK_ENABLE();

   for(uint8_t i = 0; i < NUM_ELEMENTS(adcPortsAndPins); i++)
   {
      if(adcPortsAndPins[i].pin != ON_CHIP)
      {
         gpioInitStruct.Pin = adcPortsAndPins[i].pin;
         gpioInitStruct.Mode = GPIO_MODE_ANALOG;
         gpioInitStruct.Pull = GPIO_NOPULL;
         HAL_GPIO_Init(adcPortsAndPins[i].port, &gpioInitStruct);
      }
   }
}

static void ConfigureAdc(void)
{
   __HAL_RCC_ADC12_CLK_ENABLE();

   RCC_PeriphCLKInitTypeDef peripheralClockInit;
   peripheralClockInit.PeriphClockSelection = RCC_PERIPHCLK_ADC12;
   peripheralClockInit.Adc12ClockSelection = RCC_ADC12PLLCLK_DIV2;
   HAL_RCCEx_PeriphCLKConfig(&peripheralClockInit);

   instance.adcHandle.Instance = ADC2;
   instance.adcHandle.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
   instance.adcHandle.Init.Resolution = ADC_RESOLUTION_12B;
   instance.adcHandle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
   instance.adcHandle.Init.ScanConvMode = ADC_SCAN_ENABLE;
   instance.adcHandle.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
   instance.adcHandle.Init.LowPowerAutoWait = DISABLE;
   instance.adcHandle.Init.ContinuousConvMode = DISABLE;
   instance.adcHandle.Init.NbrOfConversion = NUM_ELEMENTS(erdAdcChannelPairs);
   instance.adcHandle.Init.DiscontinuousConvMode = ENABLE;
   instance.adcHandle.Init.NbrOfDiscConversion = NUM_ELEMENTS(erdAdcChannelPairs);
   instance.adcHandle.Init.ExternalTrigConv = ADC_SOFTWARE_START;
   instance.adcHandle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
   instance.adcHandle.Init.DMAContinuousRequests = ENABLE;
   instance.adcHandle.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
   if(HAL_ADC_Init(&instance.adcHandle) != HAL_OK)
   {
      HAL_NVIC_SystemReset();
   }

   uint8_t rank = ADC_REGULAR_RANK_1;
   ADC_ChannelConfTypeDef config = { 0 };
   config.SingleDiff = ADC_SINGLE_ENDED;
   config.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
   config.OffsetNumber = ADC_OFFSET_NONE;
   config.Offset = 0;

   for(uint8_t i = 0; i < NUM_ELEMENTS(adcPortsAndPins); i++)
   {
      config.Channel = adcPortsAndPins[i].channel;
      config.Rank = rank++;
      HAL_ADC_ConfigChannel(&instance.adcHandle, &config);
   }
}

static void ConfigureDma(void)
{
   __HAL_RCC_DMA2_CLK_ENABLE();

   instance.dmaHandle.Instance = DMA2_Channel1;
   instance.dmaHandle.Init.Direction = DMA_PERIPH_TO_MEMORY;
   instance.dmaHandle.Init.PeriphInc = DMA_PINC_DISABLE;
   instance.dmaHandle.Init.MemInc = DMA_MINC_ENABLE;
   instance.dmaHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
   instance.dmaHandle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
   instance.dmaHandle.Init.Mode = DMA_CIRCULAR;
   instance.dmaHandle.Init.Priority = DMA_PRIORITY_LOW;

   HAL_DMA_DeInit(&instance.dmaHandle);
   HAL_DMA_Init(&instance.dmaHandle);

   __HAL_LINKDMA(&instance.adcHandle, DMA_Handle, instance.dmaHandle);
}

static AdcCounts_t ReadAdcChannel(uint8_t channel)
{
   IGNORE(channel);

   for(uint8_t i = 0; i < NUM_ELEMENTS(erdAdcChannelPairs); i++)
   {
      if(adcPortsAndPins[i].channel == channel)
      {
         return (AdcCounts_t)instance.buffer[i];
      }
   }
   return 0;
}

static void Read(I_DataSource_t *_instance, const Erd_t erd, void *data)
{
   IGNORE(_instance);
   uint16_t index;
   ErdAdcChannelPair_t pair;

   uassert(ERD_IS_IN_RANGE(erd));

   if(ConstArrayMap_Find(&instance.erdToChannelMap.interface, &erd, &index, &pair))
   {
      AdcCounts_t value = ReadAdcChannel(pair.channel);
      memcpy(data, &value, sizeof(AdcCounts_t));
   }
   else
   {
      uassert(!"ERD not found");
   }
}

static void Write(I_DataSource_t *instance, const Erd_t erd, const void *data)
{
   IGNORE(instance);
   IGNORE(erd);
   IGNORE(data);
}

static bool Has(const I_DataSource_t *instance, const Erd_t erd)
{
   IGNORE(instance);
   return ERD_IS_IN_RANGE(erd);
}

static uint8_t SizeOf(const I_DataSource_t *instance, const Erd_t erd)
{
   IGNORE(instance);
   uassert(ERD_IS_IN_RANGE(erd));

   return sizeof(AdcCounts_t);
}

static const I_DataSource_Api_t api =
   { Read, Write, Has, SizeOf };

static void StartDmaConversion(void *context)
{
   IGNORE(context);
   HAL_ADC_Start_DMA(&instance.adcHandle, (uint32_t *)instance.buffer, NUM_ELEMENTS(erdAdcChannelPairs));
}

I_DataSource_t *DataSource_Adc_Init(TimerModule_t *timerModule)
{
   instance.timerModule = timerModule;
   instance.interface.api = &api;
   instance.interface.OnDataChange = Event_Null_GetInstance();

   ConstArrayMap_LinearSearch_Init(
      &instance.erdToChannelMap,
      &erdToAdcChannelMapConfiguration);

   ConfigurePins();
   ConfigureAdc();
   ConfigureDma();

   HAL_NVIC_SetPriority(DMA2_Channel1_IRQn, 0, 0);
   HAL_NVIC_EnableIRQ(DMA2_Channel1_IRQn);
   HAL_NVIC_SetPriority(ADC1_2_IRQn, 0, 0);
   HAL_NVIC_EnableIRQ(ADC1_2_IRQn);

   if(HAL_ADCEx_Calibration_Start(&instance.adcHandle, ADC_SINGLE_ENDED) != HAL_OK)
   {
      HAL_NVIC_SystemReset();
   }

   TimerModule_StartPeriodic(
      instance.timerModule,
      &instance.timer,
      ConversionTriggerPeriodInMSec,
      StartDmaConversion,
      NULL);

   return &instance.interface;
}

void DMA2_Channel1_IRQHandler(void)
{
   HAL_DMA_IRQHandler(instance.adcHandle.DMA_Handle);
}
