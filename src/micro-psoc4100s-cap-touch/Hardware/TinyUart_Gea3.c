/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "TinyUart_Gea3.h"
#include "Gea3.h"
#include "Gea3_SPI_UART.h"
#include "TinyEvent_SingleSubscriberSynchronous.h"
#include "utils.h"

static I_TinyUart_t interface;
static TinyEvent_SingleSubscriberSynchronous_t receiveInterrupt;
static TinyEvent_SingleSubscriberSynchronous_t sendCompleteInterrupt;

static void Send(I_TinyUart_t *_instance, const uint8_t byte)
{
   IGNORE(_instance);
   Gea3_UartPutChar(byte);
}

static I_TinyEvent_t *GetOnReceiveEvent(I_TinyUart_t *_instance)
{
   IGNORE(_instance);
   return &receiveInterrupt.interface;
}

static I_TinyEvent_t *GetOnTransmitEvent(I_TinyUart_t *_instance)
{
   IGNORE(_instance);
   return &sendCompleteInterrupt.interface;
}

static CY_ISR(Gea3Handler)
{
   if(Gea3_GetRxInterruptSourceMasked() == Gea3_INTR_RX_NOT_EMPTY)
   {
      uint8_t receivedData = Gea3_RX_FIFO_RD_REG;
      TinyUartOnReceiveArgs_t args = { receivedData };
      TinyEvent_SingleSubscriberSynchronous_Publish(&receiveInterrupt, &args);
   }
   else if(Gea3_GetTxInterruptSourceMasked() == Gea3_INTR_TX_UART_DONE)
   {
      Gea3_ClearTxInterruptSource(Gea3_INTR_TX_UART_DONE);
      TinyEvent_SingleSubscriberSynchronous_Publish(&sendCompleteInterrupt, NULL);
   }
}

static const I_TinyUart_Api_t api = { Send, GetOnReceiveEvent, GetOnTransmitEvent };

I_TinyUart_t *TinyUart_Gea3_Init(void)
{
   interface.api = &api;

   TinyEvent_SingleSubscriberSynchronous_Init(&receiveInterrupt);
   TinyEvent_SingleSubscriberSynchronous_Init(&sendCompleteInterrupt);

   Gea3_SetCustomInterruptHandler(Gea3Handler);
   Gea3_Start();

   return &interface;
}
