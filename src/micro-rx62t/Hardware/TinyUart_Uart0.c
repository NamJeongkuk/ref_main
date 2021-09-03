/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <stdint.h>
#include <string.h>
#include "iodefine.h"
#include "Clock.h"
#include "TinyUart_Uart0.h"
#include "TinyEvent_SingleSubscriberSynchronous.h"
#include "utils.h"

STATIC_ASSERT(Clock_PclkFrequencyMhz == 50);

static TinyEvent_SingleSubscriberSynchronous_t onReceiveEvent;
static TinyEvent_SingleSubscriberSynchronous_t sendCompleteEvent;
static I_TinyUart_t uart;

static void Send(I_TinyUart_t *instance, const uint8_t byte)
{
   IGNORE(instance);
   SCI0.TDR = byte;
}

static I_TinyInterrupt_t *GetOnReceiveInterrupt(I_TinyUart_t *instance)
{
   IGNORE(instance);
   return &onReceiveEvent.interface;
}

static I_TinyInterrupt_t *GetOnSendCompleteInterrupt(I_TinyUart_t *instance)
{
   IGNORE(instance);
   return &sendCompleteEvent.interface;
}

void SCI0_RXI0(void) __attribute__((interrupt));
void SCI0_RXI0(void)
{
   TinyUartOnReceiveArgs_t args = { SCI0.RDR };
   TinyEvent_SingleSubscriberSynchronous_Publish(&onReceiveEvent, &args);
}

void SCI0_TXI0(void) __attribute__((interrupt));
void SCI0_TXI0(void)
{
   TinyEvent_SingleSubscriberSynchronous_Publish(&sendCompleteEvent, NULL);
}

void SCI0_ERI0(void) __attribute__((interrupt));
void SCI0_ERI0(void)
{
   // Turn off receive enable
   while(0 != SCI0.SCR.BIT.RE)
   {
      SCI0.SCR.BIT.RE = 0;
   }

   // Turn off transmit enable
   while(0 != SCI0.SCR.BIT.TE)
   {
      SCI0.SCR.BIT.TE = 0;
   }

   volatile uint8_t dummy = SCI0.RDR;
   IGNORE(dummy);

   // Clear error flags
   while(0 != (0x30 & SCI0.SSR.BYTE))
   {
      SCI0.SSR.BYTE = 0xC0;
   }

   // Clear receive interrupt flag
   while(0 != ICU.IR[IR_SCI0_RXI0].BIT.IR)
   {
      ICU.IR[IR_SCI0_RXI0].BIT.IR = 0;
   }

   // Clear error interrupt flag
   while(0 != ICU.IR[IR_SCI0_ERI0].BIT.IR)
   {
      ICU.IR[IR_SCI0_ERI0].BIT.IR = 0;
   }

   // Transmit enabled
   while(1 != SCI0.SCR.BIT.TE)
   {
      SCI0.SCR.BIT.TE = 1;
   }

   // Receive enabled
   while(1 != SCI0.SCR.BIT.RE)
   {
      SCI0.SCR.BIT.RE = 1;
   }
}

static const I_TinyUart_Api_t api = { Send, GetOnReceiveInterrupt, GetOnSendCompleteInterrupt };

I_TinyUart_t *TinyUart_Uart0_Init(void)
{
   TinyEvent_SingleSubscriberSynchronous_Init(&onReceiveEvent);
   TinyEvent_SingleSubscriberSynchronous_Init(&sendCompleteEvent);

   uart.api = &api;

   MSTP(SCI0) = 0;

   // Disable RX and TX while we fiddle with them
   SCI0.SCR.BYTE = 0;

   // Configure PB1 (RX) as an input
   PORTB.DDR.BIT.B1 = 0;

   // Enable ICR bit for PB1 (RX) so that it can be used by the UART
   PORTB.ICR.BIT.B1 = 1;

   // Configure PB2 (TX) as an output
   PORTB.DDR.BIT.B2 = 1;

   // Port multiplexing is pretty goofy, make sure you read the I/O Ports section in the hardware manual
   // before trying to do a find and replace port of this driver to another UART as you may need to play
   // with a port function register as well

   // No divisor on PCLK
   SCI0.SMR.BIT.CKS = 0;

   // Baud rate configured for ~19200 with 0.47% error (19530.3)
   SCI0.BRR = 80;

   // Set up interrupts in the Interrupt-Control-Unit
   // There is only one priority register per SCI port
   ICU.IPR[IPR_SCI0_RXI0].BIT.IPR = 13;

   // ...but there are 4 separate interrupt enable bits
   ICU.IER[IER_SCI0_RXI0].BIT.IEN_SCI0_RXI0 = 1;
   ICU.IER[IER_SCI0_TXI0].BIT.IEN_SCI0_TXI0 = 1;
   ICU.IER[IER_SCI0_ERI0].BIT.IEN_SCI0_ERI0 = 1;
   ICU.IER[IER_SCI0_TEI0].BIT.IEN_SCI0_TEI0 = 1;

   // Enable UART RX and TX interrupts
   SCI0.SCR.BIT.TIE = 1; // Transmit Interrupt Enabled
   SCI0.SCR.BIT.RIE = 1; // Receive Interrupt Enabled
   SCI0.SCR.BIT.TE = 1; // Transmit Enabled
   SCI0.SCR.BIT.RE = 1; // Receive Enabled

   return &uart;
}
