/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "stm32g0xx_ll_usart.h"
#include "stm32g0xx_ll_gpio.h"
#include "TinyUart_Usart2.h"
#include "TinyEvent_Synchronous.h"
#include "utils.h"

static struct
{
   I_TinyUart_t interface;
   TinyEvent_Synchronous_t receive;
   TinyEvent_Synchronous_t sendComplete;
} instance;

#define USART USART2

void USART2_IRQHandler(void)
{
   if(LL_USART_IsActiveFlag_PE(USART) ||
      LL_USART_IsActiveFlag_FE(USART) ||
      LL_USART_IsActiveFlag_NE(USART) ||
      LL_USART_IsActiveFlag_ORE(USART))
   {
      LL_USART_ClearFlag_PE(USART);
      LL_USART_ClearFlag_FE(USART);
      LL_USART_ClearFlag_NE(USART);
      LL_USART_ClearFlag_ORE(USART);

      if(LL_USART_IsActiveFlag_RXNE(USART))
      {
         uint8_t byte = LL_USART_ReceiveData8(USART);
      }
   }
   else if(LL_USART_IsActiveFlag_RXNE(USART))
   {
      uint8_t byte = LL_USART_ReceiveData8(USART);
      TinyEvent_Synchronous_Publish(&instance.receive, &byte);
   }
   else if(LL_USART_IsActiveFlag_TXE(USART))
   {
      LL_USART_DisableIT_TXE(USART);
      uint8_t byte = LL_USART_ReceiveData8(USART);
      TinyEvent_Synchronous_Publish(&instance.receive, &byte);
   }
}

static void Send(I_TinyUart_t *_instance, uint8_t byte)
{
   IGNORE(_instance);
   LL_USART_EnableIT_TXE(USART);
   LL_USART_TransmitData8(USART, byte);
}

static I_TinyInterrupt_t *GetOnReceiveInterrupt(I_TinyUart_t *_instance)
{
   IGNORE(_instance);
   return &instance.receive.interface;
}

static I_TinyInterrupt_t *GetOnSendCompleteInterrupt(I_TinyUart_t *_instance)
{
   IGNORE(_instance);
   return &instance.sendComplete.interface;
}

static const I_TinyUart_Api_t api = { Send, GetOnReceiveInterrupt, GetOnSendCompleteInterrupt };

static inline void InitializeGpio(void)
{
   SET_BIT(RCC->IOPENR, RCC_IOPENR_GPIOAEN);

   // TX: PA2, RX: PA3
   LL_GPIO_InitTypeDef gpio;
   gpio.Pin = LL_GPIO_PIN_2 | LL_GPIO_PIN_3;
   gpio.Mode = LL_GPIO_MODE_ALTERNATE;
   gpio.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
   gpio.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
   gpio.Pull = LL_GPIO_PULL_UP;
   gpio.Alternate = LL_GPIO_AF_1;
   LL_GPIO_Init(GPIOA, &gpio);
}

static inline void InitializeUsart2(void)
{
   LL_USART_Disable(USART);

   SET_BIT(RCC->APBENR1, RCC_APBENR1_USART2EN);

   LL_USART_InitTypeDef usart;
   LL_USART_StructInit(&usart);
   usart.PrescalerValue = LL_USART_PRESCALER_DIV1;
   usart.BaudRate = 19200;
   usart.DataWidth = LL_USART_DATAWIDTH_8B;
   usart.StopBits = LL_USART_STOPBITS_1;
   usart.Parity = LL_USART_PARITY_NONE;
   usart.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
   usart.TransferDirection = LL_USART_DIRECTION_TX_RX;
   usart.OverSampling = LL_USART_OVERSAMPLING_16;
   LL_USART_Init(USART, &usart);

   LL_USART_EnableIT_PE(USART);
   LL_USART_EnableIT_ERROR(USART);
   LL_USART_EnableIT_RXNE(USART);
   NVIC_EnableIRQ(USART2_IRQn);

   LL_USART_Enable(USART);
}

I_TinyUart_t *TinyUart_Usart2_Init(void)
{
   InitializeGpio();
   InitializeUsart2();

   TinyEvent_Synchronous_Init(&instance.receive);
   TinyEvent_Synchronous_Init(&instance.sendComplete);

   instance.interface.api = &api;

   return &instance.interface;
}
