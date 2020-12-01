/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <string.h>
#include "BufferedUart_Uart3.h"
#include "stm32f3xx.h"
#include "Event_SingleSubscriberSynchronous.h"
#include "Event_Synchronous.h"
#include "utils.h"
#include "uassert.h"

enum
{
   ReceiveBufferSize = 50,
   BaudRate = 230400
};

static struct
{
   I_BufferedUart_t interface;
   Event_Synchronous_t receiveEvent;
   Event_SingleSubscriberSynchronous_t transmitEvent;
   bool waitingForTransmitToComplete;
   bool transmitComplete;
   bool errorOccurred;
   uint8_t receiveBuffer[ReceiveBufferSize];

   struct
   {
      UART_HandleTypeDef uart;
      DMA_HandleTypeDef dmaRx;
      DMA_HandleTypeDef dmaTx;
   } handles;

   struct
   {
      uint16_t tail;
      uint16_t head;
   } dmaRingBuffer;
} instance;

static void Initialize(void);

static void Transmit(I_BufferedUart_t *_instance, const uint8_t *data, const uint16_t byteCount)
{
   IGNORE(_instance);
   uassert(!instance.waitingForTransmitToComplete);

   instance.waitingForTransmitToComplete = true;
   instance.transmitComplete = false;
   HAL_UART_Transmit_DMA(&instance.handles.uart, (uint8_t *)data, byteCount);
}

static I_Event_t *GetOnReceiveEvent(I_BufferedUart_t *_instance)
{
   IGNORE(_instance);
   return &instance.receiveEvent.interface;
}

static I_Event_t *GetOnTransmitEvent(I_BufferedUart_t *_instance)
{
   IGNORE(_instance);
   return &instance.transmitEvent.interface;
}

static uint16_t DmaRingBuffer_GetCount(void)
{
   uint16_t capacity = sizeof(instance.receiveBuffer);
   uint16_t head = capacity - __HAL_DMA_GET_COUNTER(&instance.handles.dmaRx);

   if(head >= instance.dmaRingBuffer.tail)
   {
      return (head - instance.dmaRingBuffer.tail);
   }
   else
   {
      return capacity - (instance.dmaRingBuffer.tail - head);
   }
}

static void ProcessBytesReceivedByDma(void)
{
   uint8_t localData[ReceiveBufferSize];
   uint16_t queueCount = DmaRingBuffer_GetCount();
   uint8_t index = 0;

   while(queueCount--)
   {
      localData[index++] = instance.receiveBuffer[instance.dmaRingBuffer.tail];
      instance.dmaRingBuffer.tail = (instance.dmaRingBuffer.tail + 1) % sizeof(instance.receiveBuffer);
   }

   if(index > 0)
   {
      BufferedUartOnReceiveArgs_t args = { localData, index };
      Event_Synchronous_Publish(&instance.receiveEvent, &args);
   }
}

static void Run(I_BufferedUart_t *_instance)
{
   IGNORE(_instance);

   ProcessBytesReceivedByDma();

   if(instance.waitingForTransmitToComplete && instance.transmitComplete)
   {
      instance.waitingForTransmitToComplete = false;
      Event_SingleSubscriberSynchronous_Publish(&instance.transmitEvent, NULL);
   }
   else if(instance.errorOccurred)
   {
      Initialize();
   }
}

static const I_BufferedUart_Api_t uartApi = {
   .Transmit = Transmit,
   .GetOnReceiveEvent = GetOnReceiveEvent,
   .GetOnTransmitCompleteEvent = GetOnTransmitEvent,
   .Run = Run
};

static void InitializePins(void)
{
   __HAL_RCC_GPIOC_CLK_ENABLE();

   GPIO_InitTypeDef gpioInitStruct = { 0 };
   gpioInitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_11;
   gpioInitStruct.Mode = GPIO_MODE_AF_PP;
   gpioInitStruct.Pull = GPIO_NOPULL;
   gpioInitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
   gpioInitStruct.Alternate = GPIO_AF7_USART3;
   HAL_GPIO_Init(GPIOC, &gpioInitStruct);
}

static void InitializeUart(void)
{
   __HAL_RCC_USART3_CLK_ENABLE();

   instance.handles.uart.Instance = USART3;
   instance.handles.uart.Init.BaudRate = 230400;
   instance.handles.uart.Init.WordLength = UART_WORDLENGTH_8B;
   instance.handles.uart.Init.StopBits = UART_STOPBITS_1;
   instance.handles.uart.Init.Parity = UART_PARITY_NONE;
   instance.handles.uart.Init.Mode = UART_MODE_TX_RX;
   instance.handles.uart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
   instance.handles.uart.Init.OverSampling = UART_OVERSAMPLING_16;
   instance.handles.uart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_ENABLE;
   instance.handles.uart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
   if(HAL_UART_Init(&instance.handles.uart) != HAL_OK)
   {
      HAL_NVIC_SystemReset();
   }

   HAL_NVIC_SetPriority(USART3_IRQn, 10, 10);
   HAL_NVIC_EnableIRQ(USART3_IRQn);
}

static void InitializeDma(void)
{
   __HAL_RCC_DMA1_CLK_ENABLE();

   instance.handles.dmaRx.Instance = DMA1_Channel3;
   instance.handles.dmaRx.Init.Direction = DMA_PERIPH_TO_MEMORY;
   instance.handles.dmaRx.Init.PeriphInc = DMA_PINC_DISABLE;
   instance.handles.dmaRx.Init.MemInc = DMA_MINC_ENABLE;
   instance.handles.dmaRx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
   instance.handles.dmaRx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
   instance.handles.dmaRx.Init.Mode = DMA_CIRCULAR;
   instance.handles.dmaRx.Init.Priority = DMA_PRIORITY_MEDIUM;
   if(HAL_DMA_Init(&instance.handles.dmaRx) != HAL_OK)
   {
      HAL_NVIC_SystemReset();
   }
   __HAL_LINKDMA(&instance.handles.uart, hdmarx, instance.handles.dmaRx);

   instance.handles.dmaTx.Instance = DMA1_Channel2;
   instance.handles.dmaTx.Init.Direction = DMA_MEMORY_TO_PERIPH;
   instance.handles.dmaTx.Init.PeriphInc = DMA_PINC_DISABLE;
   instance.handles.dmaTx.Init.MemInc = DMA_MINC_ENABLE;
   instance.handles.dmaTx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
   instance.handles.dmaTx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
   instance.handles.dmaTx.Init.Mode = DMA_NORMAL;
   instance.handles.dmaTx.Init.Priority = DMA_PRIORITY_MEDIUM;
   if(HAL_DMA_Init(&instance.handles.dmaTx) != HAL_OK)
   {
      HAL_NVIC_SystemReset();
   }
   __HAL_LINKDMA(&instance.handles.uart, hdmatx, instance.handles.dmaTx);

   HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 0, 0);
   HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);
   HAL_NVIC_SetPriority(DMA1_Channel2_IRQn, 0, 0);
   HAL_NVIC_EnableIRQ(DMA1_Channel2_IRQn);
}

static void Initialize(void)
{
   InitializePins();
   InitializeUart();
   InitializeDma();

   instance.interface.api = &uartApi;
   instance.dmaRingBuffer.tail = 0;
   instance.waitingForTransmitToComplete = false;
   instance.errorOccurred = false;

   HAL_UART_Receive_DMA(&instance.handles.uart, instance.receiveBuffer, ReceiveBufferSize);
}

I_BufferedUart_t *BufferedUart_Uart3_Init(void)
{
   Event_Synchronous_Init(&instance.receiveEvent);
   Event_SingleSubscriberSynchronous_Init(&instance.transmitEvent);

   Initialize();

   return &instance.interface;
}

void BufferedUart_Uart3_Disable(void)
{
   HAL_DMA_DeInit(&instance.handles.dmaTx);
   HAL_DMA_DeInit(&instance.handles.dmaRx);
}

void DMA1_Channel2_IRQHandler(void)
{
   HAL_DMA_IRQHandler(instance.handles.uart.hdmatx);
}

void DMA1_Channel3_IRQHandler(void)
{
   HAL_DMA_IRQHandler(instance.handles.uart.hdmarx);
}

void USART3_IRQHandler(void)
{
   HAL_UART_IRQHandler(&instance.handles.uart);

   if(HAL_UART_GetError(&instance.handles.uart) != HAL_UART_ERROR_NONE)
   {
      instance.errorOccurred = true;
   }
   else
   {
      instance.transmitComplete = true;
   }
}
