/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <stdint.h>
#include <string.h>
#include "iodefine.h"
#include "TinyUart_Uart6.h"
#include "TinyEvent_SingleSubscriberSynchronous.h"
#include "utils.h"

static TinyEvent_SingleSubscriberSynchronous_t onReceiveEvent;
static TinyEvent_SingleSubscriberSynchronous_t sendCompleteEvent;
static I_TinyUart_t uart;

static void Send(I_TinyUart_t *instance, const uint8_t byte)
{
   IGNORE(instance);
   SCI6.TDR = byte;
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

void SCI6_RXI6(void) __attribute__((interrupt));
void SCI6_RXI6(void)
{
   TinyUartOnReceiveArgs_t args = { SCI6.RDR };
   TinyEvent_SingleSubscriberSynchronous_Publish(&onReceiveEvent, &args);
}

void SCI6_TXI6(void) __attribute__((interrupt));
void SCI6_TXI6(void)
{
   TinyEvent_SingleSubscriberSynchronous_Publish(&sendCompleteEvent, NULL);
}

void SCI6_ERI6(void) __attribute__((interrupt));
void SCI6_ERI6(void)
{
   volatile uint8_t receivedData;

   // Turn off receive enable
   while(0 != SCI6.SCR.BIT.RE)
   {
      SCI6.SCR.BIT.RE = 0;
   }

   // Turn off transmit enable
   while(0 != SCI6.SCR.BIT.TE)
   {
      SCI6.SCR.BIT.TE = 0;
   }

   receivedData = SCI6.RDR;
   IGNORE(receivedData);

   // Clear error flags
   while(0 != (0x30 & SCI6.SSR.BYTE))
   {
      SCI6.SSR.BYTE = 0xC0;
   }

   // Clear receive interrupt flag
   while(0 != ICU.IR[IR_SCI6_RXI6].BIT.IR)
   {
      ICU.IR[IR_SCI6_RXI6].BIT.IR = 0;
   }

   // Clear error interrupt flag
   while(0 != ICU.IR[IR_SCI6_ERI6].BIT.IR)
   {
      ICU.IR[IR_SCI6_ERI6].BIT.IR = 0;
   }

   // Transmit enabled
   while(1 != SCI6.SCR.BIT.TE)
   {
      SCI6.SCR.BIT.TE = 1;
   }

   // Receive enabled
   while(1 != SCI6.SCR.BIT.RE)
   {
      SCI6.SCR.BIT.RE = 1;
   }
}

static const I_TinyUart_Api_t api = { Send, GetOnReceiveInterrupt, GetOnSendCompleteInterrupt };

I_TinyUart_t *TinyUart_Uart6_Init(void)
{
   TinyEvent_SingleSubscriberSynchronous_Init(&onReceiveEvent);
   TinyEvent_SingleSubscriberSynchronous_Init(&sendCompleteEvent);

   uart.api = &api;

   // First, allow writes to Module-Stop-Control-Register
   SYSTEM.PRCR.WORD = 0xA502;
   // Now turn on SCI6
   MSTP_SCI6 = 0;
   // Disable writes to Module-Stop-Control-Register
   SYSTEM.PRCR.WORD = 0xA500;

   // Disable UART in/out while we fiddle with it, set the clock to internal (CKE=0)
   SCI6.SCR.BYTE = 0;
   // Setup port PB0 (rx) and PB1 (tx) for use by UART
   PORTB.PMR.BIT.B0 = 1;
   PORTB.PMR.BIT.B1 = 1;
   PORTB.PDR.BIT.B1 = 1;
   PORTB.PODR.BIT.B1 = 1;
   // Setup the mode register data format
   // Bit - Setting : Comment
   // 0:1 -     00  : Clock Select PCLK (n=0)
   //   2 -      0  : No multi-processor mode
   //   3 -      0  : 1 Stop bit
   //   4 -      0  : Even Parity
   //   5 -      0  : No Parity
   //   6 -      0  : 8 bit data transfer
   //   7 -      0  : Async
   SCI6.SMR.BYTE = 0x00;
   // Setup the smart card mode register data format
   // Bit - Setting : Comment
   //   0 -      0  : Non-smart card mode
   //   1 -      1  : Always 1
   //   2 -      0  : Don't invert Tx/Rx
   //   3 -      0  : Transfer LSB first
   //   4 -      1  : 8 bit data transfer
   //   5 -      1  : Always 1
   //   6 -      1  : Always 1
   //   7 -      0  : Base clock pulse
   // Set data transfer length to 8 bits, and the UART to non-smart card mode
   SCI6.SCMR.BYTE = 0x72;
   // Setup the extended mode register data format
   // Bit - Setting : Comment
   //   0 -      0  : Async clock source
   //   1 -      0  : Always 0
   //   2 -      1  : Do bit rate modulation
   //   3 -      0  : Always 0
   //   4 -      1  : Async base clock = 8 cycles/bit
   //   5 -      0  : Noise filter off
   //   6 -      0  : Baud rate gen double speed off
   //   7 -      0  : Start bit level detect
   // Setup BRR and the clock multiplier
   SCI6.SEMR.BIT.ACS0 = 0;
   SCI6.SEMR.BIT.BRME = 1;
   SCI6.SEMR.BIT.ABCS = 1;
   SCI6.SEMR.BIT.NFEN = 0;
   SCI6.SEMR.BIT.BGDM = 0;
   SCI6.SEMR.BIT.RXDESEL = 0;
   // Baud rate configured for 230400 with 0.14% error
   SCI6.BRR = 5;
   SCI6.MDDR = 177;
   // Set up interrupts in the Interrupt-Control-Unit
   // There is only one priority register per SCI port
   ICU.IPR[IPR_SCI6_RXI6].BIT.IPR = 13;
   // But there are 4 separate interrupt enable bits
   ICU.IER[IER_SCI6_RXI6].BIT.IEN_SCI6_RXI6 = 1;
   ICU.IER[IER_SCI6_TXI6].BIT.IEN_SCI6_TXI6 = 1;
   ICU.IER[IER_SCI6_ERI6].BIT.IEN_SCI6_ERI6 = 1;
   ICU.IER[IER_SCI6_TEI6].BIT.IEN_SCI6_TEI6 = 1;

   // First, allow writing to the Multi-Function-Pin-Controller
   MPC.PWPR.BIT.B0WI = 0;
   MPC.PWPR.BIT.PFSWE = 1;

   // Assign PB0 and PB1 to function as RXD6 and TXD6...
   MPC.PB0PFS.BYTE = 0x0B;
   MPC.PB1PFS.BYTE = 0x0B;

   // Disable writing to the Multi-Function-Pin-Controller
   MPC.PWPR.BIT.PFSWE = 0;
   MPC.PWPR.BIT.B0WI = 1;

   // Finally, enable UART rx and interrupts
   SCI6.SCR.BIT.CKE = 0;
   SCI6.SCR.BIT.TIE = 1; // Transmit Interrupt Enabled
   SCI6.SCR.BIT.RIE = 1; // Receive Interrupt Enabled
   SCI6.SCR.BIT.TE = 1; // Transmit Enabled
   SCI6.SCR.BIT.RE = 1; // Receive Enabled

   return &uart;
}
