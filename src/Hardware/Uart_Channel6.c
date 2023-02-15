/*!
 * @file
 * @brief Implementation of the GEA2 Uart configuration
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

#include <stdint.h>
#include <string.h>
#include "iodefine.h"
#include "I_Interrupt.h"
#include "uassert.h"
#include "Event_SingleSubscriberSynchronous.h"
#include "utils.h"
#include "Uart_Channel6.h"

#define U32_TARGET_BAUD_RATE ((uint32_t)19200)

#ifdef OLD_HW

static void Send(I_Uart_t *instance, const uint8_t byte);

typedef struct
{
   I_Uart_t interface;
   Event_SingleSubscriberSynchronous_t OnReceiveEvent;
   Event_SingleSubscriberSynchronous_t OnTransmitEvent;
   Event_SingleSubscriberSynchronous_t OnErrorEvent;
} UartData;

static UartData uartData;

/*!
 * Transmit a single byte out the UART6 port
 * @param instance
 * @param byte
 */
static void Send(I_Uart_t *instance, const uint8_t byte)
{
   IGNORE_ARG(instance);
   SCI6.TDR = byte;
}

static I_Event_t *GetOnReceiveEvent(I_Uart_t *instance)
{
   IGNORE(instance);
   return (&uartData.OnReceiveEvent.interface);
}

static I_Event_t *GetOnTransmitEvent(I_Uart_t *instance)
{
   IGNORE(instance);
   return (&uartData.OnTransmitEvent.interface);
}

static I_Event_t *GetOnErrorEvent(I_Uart_t *instance)
{
   IGNORE(instance);
   return (&uartData.OnErrorEvent.interface);
}

static const I_Uart_Api_t Uart_Api = {
   Send, GetOnReceiveEvent, GetOnTransmitEvent, GetOnErrorEvent
};

/*!
 * Interrupt service routine for UART6 rx data
 */
void SCI6_RXI6(void) __attribute__((interrupt));
void SCI6_RXI6(void)
{
   uint8_t receivedData;

   receivedData = SCI6.RDR;

   {
      UartOnReceiveArgs_t args = { receivedData };
      Event_SingleSubscriberSynchronous_Publish(&uartData.OnReceiveEvent, &args);
   }
}

/*!
 * Interrupt service routine for GEA2_UART rx error
 */
void SCI6_ERI6(void) __attribute__((interrupt));
void SCI6_ERI6(void)
{
   uint8_t received_data;

   received_data = SCI6.RDR;
   // Remove warning
   IGNORE_ARG(received_data);

   while(SCI6.SSR.BIT.PER | SCI6.SSR.BIT.FER | SCI6.SSR.BIT.ORER)
   {
      // Clear errors
      SCI6.SSR.BIT.PER = 0;
      SCI6.SSR.BIT.FER = 0;
      SCI6.SSR.BIT.ORER = 0;
   }

   MSTP_SCI6 = 1;
   MSTP_SCI6 = 0;

   Event_SingleSubscriberSynchronous_Publish(&uartData.OnErrorEvent, NULL);
}

/*!
 * Interrupt service routine for GEA2_UART tx error
 */
void SCI6_TEI6(void) __attribute__((interrupt));
void SCI6_TEI6(void)
{
   uint8_t received_data;

   received_data = SCI6.RDR;
   // Remove warning
   IGNORE_ARG(received_data);

   while(SCI6.SSR.BIT.PER | SCI6.SSR.BIT.FER | SCI6.SSR.BIT.ORER)
   {
      // Clear errors
      SCI6.SSR.BIT.PER = 0;
      SCI6.SSR.BIT.FER = 0;
      SCI6.SSR.BIT.ORER = 0;
   }

   MSTP_SCI6 = 1;
   MSTP_SCI6 = 0;

   Event_SingleSubscriberSynchronous_Publish(&uartData.OnErrorEvent, NULL);
}

#endif

/*!
 * Initialize GEA2_UART as GEA2 port - 19200, 8,n,1 with no Tx interrupts
 * @return
 */
I_Uart_t *Uart_Channel6_GetInstance(uint32_t clockFrequency)
{
#ifdef OLD_HW
   if(!uartData.interface.api)
   {
      uartData.interface.api = &Uart_Api;

      Event_SingleSubscriberSynchronous_Init(&uartData.OnReceiveEvent);
      Event_SingleSubscriberSynchronous_Init(&uartData.OnTransmitEvent);
      Event_SingleSubscriberSynchronous_Init(&uartData.OnErrorEvent);

      // Configure SCI6/UART6 on board
      // First, allow writes to Module-Stop-Control-Register
      SYSTEM.PRCR.WORD = 0xA502;
      // Now turn on SCI6
      MSTP_SCI6 = 0;
      SYSTEM.PRCR.WORD = 0xA500;

      // Disable UART in/out while we fiddle with it, set the clock to internal (CKE=0)
      SCI6.SCR.BYTE = 0;
      // Setup port PD1 (rx) and PD0 (tx) for use by UART
      PORTD.PMR.BIT.B1 = 1;
      PORTD.PMR.BIT.B0 = 1;
      PORTD.PDR.BIT.B0 = 1;
      PORTD.PODR.BIT.B0 = 1;
      // Setup the data format
      // Bit - Setting : Comment
      // 0:1 -     00  : Clock Select PCLK
      //   2 -      0  : No multi-processor mode
      //   3 -      0  : 1 Stop bit
      //   4 -      0  : Even Parity
      //   5 -      0  : No Parity
      //   6 -      0  : 8 Bytes
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
      // Setup BRR and the clock multiplier
      SCI6.SEMR.BIT.ABCS = 1; // 8 base clock cycles per 1 bit
      SCI6.BRR = (uint8_t)((clockFrequency / (32L * U32_TARGET_BAUD_RATE / 2L)) - 1);
      // Set up interrupts in the Interrupt-Control-Unit
      // There is only one priority register per SCI port
      ICU.IPR[IPR_SCI6_RXI6].BIT.IPR = 11;
      // But there are 4 separate interrupt enable bits
      ICU.IER[IER_SCI6_RXI6].BIT.IEN_SCI6_RXI6 = 1;
      ICU.IER[IER_SCI6_TXI6].BIT.IEN_SCI6_TXI6 = 0;
      ICU.IER[IER_SCI6_ERI6].BIT.IEN_SCI6_ERI6 = 1;
      ICU.IER[IER_SCI6_TEI6].BIT.IEN_SCI6_TEI6 = 1;
      // Enable the Interrupt should be deferred until the structure is enabled

      // Enable writing to the PFSWE bit
      MPC.PWPR.BIT.B0WI = 0;
      // Enable writing to the PFS register
      MPC.PWPR.BIT.PFSWE = 1;

      // Assign PD1 and PD0 to function as RXD0 and TXD0...
      MPC.PD1PFS.BYTE = 0x0B;
      MPC.PD0PFS.BYTE = 0x0B;

      // Disable writing to the PFS register
      MPC.PWPR.BIT.PFSWE = 0;
      // Disable writing to the PFSWE bit
      MPC.PWPR.BIT.B0WI = 1;

      ICU.IR[IR_SCI6_TXI6].BIT.IR = 0;

      // Finally, enable UART rx and interrupts
      SCI6.SCR.BIT.TIE = 0; // Transmit Interrupt Disabled
      SCI6.SCR.BIT.RIE = 1; // Receive Interrupt Enabled
      SCI6.SCR.BIT.TE = 1; // Transmit Enabled
      SCI6.SCR.BIT.RE = 1; // Receive Enabled
   }

   return &uartData.interface;
#else
   IGNORE(clockFrequency);
   return NULL;
#endif
}
