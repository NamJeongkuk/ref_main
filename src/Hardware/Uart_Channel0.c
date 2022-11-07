/*!
 * @file
 * @brief Implementation of the GEA2 Uart configuration
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

#include <stdint.h>
#include <string.h>
#include "iodefine.h"
#include "uassert.h"
#include "Event_SingleSubscriberSynchronous.h"
#include "utils.h"
#include "Uart_Channel0.h"

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
 * Transmit a single byte out the UART0 port
 * @param instance
 * @param byte
 */
static void Send(I_Uart_t *instance, const uint8_t byte)
{
   IGNORE_ARG(instance);
   SCI0.TDR = byte;
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
 * Interrupt service routine for UART0 rx data
 */
void SCI0_RXI0(void) __attribute__((interrupt));
void SCI0_RXI0(void)
{
   uint8_t receivedData;

   receivedData = SCI0.RDR;

   {
      UartOnReceiveArgs_t args = { receivedData };
      Event_SingleSubscriberSynchronous_Publish(&uartData.OnReceiveEvent, &args);
   }
}

/*!
 * Interrupt service routine for GEA2_UART rx error
 */
void SCI0_ERI0(void) __attribute__((interrupt));
void SCI0_ERI0(void)
{
   uint8_t received_data;

   received_data = SCI0.RDR;
   // Remove warning
   IGNORE_ARG(received_data);

   while(SCI0.SSR.BIT.PER | SCI0.SSR.BIT.FER | SCI0.SSR.BIT.ORER)
   {
      // Clear errors
      SCI0.SSR.BIT.PER = 0;
      SCI0.SSR.BIT.FER = 0;
      SCI0.SSR.BIT.ORER = 0;
   }
   MSTP_SCI0 = 1;
   MSTP_SCI0 = 0;

   Event_SingleSubscriberSynchronous_Publish(&uartData.OnErrorEvent, NULL);
}

/*!
 * Interrupt service routine for GEA2_UART tx error
 */
void SCI0_TEI0(void) __attribute__((interrupt));
void SCI0_TEI0(void)
{
   uint8_t received_data;

   received_data = SCI0.RDR;
   // Remove warning
   IGNORE_ARG(received_data);

   while(SCI0.SSR.BIT.PER | SCI0.SSR.BIT.FER | SCI0.SSR.BIT.ORER)
   {
      // Clear errors
      SCI0.SSR.BIT.PER = 0;
      SCI0.SSR.BIT.FER = 0;
      SCI0.SSR.BIT.ORER = 0;
   }
   MSTP_SCI0 = 1;
   MSTP_SCI0 = 0;

   Event_SingleSubscriberSynchronous_Publish(&uartData.OnErrorEvent, NULL);
}

#endif

/*!
 * Initialize GEA2_UART as GEA2 port - 19200, 8,n,1 with no Tx interrupts
 * @return
 */
I_Uart_t *Uart_Channel0_GetInstance(uint32_t clockFrequency)
{
#ifdef OLD_HW
   if(!uartData.interface.api)
   {
      uartData.interface.api = &Uart_Api;

      Event_SingleSubscriberSynchronous_Init(&uartData.OnReceiveEvent);
      Event_SingleSubscriberSynchronous_Init(&uartData.OnTransmitEvent);
      Event_SingleSubscriberSynchronous_Init(&uartData.OnErrorEvent);

      // Configure SCI0/UART0 on board
      // First, allow writes to Module-Stop-Control-Register
      SYSTEM.PRCR.WORD = 0xA502;
      // Now turn on SCI0
      MSTP_SCI0 = 0;
      SYSTEM.PRCR.WORD = 0xA500;

      // Disable UART in/out while we fiddle with it, set the clock to internal (CKE=0)
      SCI0.SCR.BYTE = 0;
      // Setup port P21 (rx) and P20 (tx) for use by UART
      PORT2.PMR.BIT.B1 = 1;
      PORT2.PMR.BIT.B0 = 1;
      PORT2.PDR.BIT.B0 = 1;
      PORT2.PODR.BIT.B0 = 1;
      // Setup the data format
      // Bit - Setting : Comment
      // 0:1 -     00  : Clock Select PCLK
      //   2 -      0  : No multi-processor mode
      //   3 -      0  : 1 Stop bit
      //   4 -      0  : Even Parity
      //   5 -      0  : No Parity
      //   6 -      0  : 8 Bytes
      //   7 -      0  : Async
      SCI0.SMR.BYTE = 0x00;
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
      SCI0.SCMR.BYTE = 0x72;
      // Setup BRR and the clock multiplier
      SCI0.SEMR.BIT.ABCS = 1; // 8 base clock cycles per 1 bit
      SCI0.BRR = (uint8_t)((clockFrequency / (32L * U32_TARGET_BAUD_RATE / 2L)) - 1);
      // Set up interrupts in the Interrupt-Control-Unit
      // There is only one priority register per SCI port
      ICU.IPR[IPR_SCI0_RXI0].BIT.IPR = 10;
      // But there are 4 separate interrupt enable bits
      ICU.IER[IER_SCI0_RXI0].BIT.IEN_SCI0_RXI0 = 1;
      ICU.IER[IER_SCI0_TXI0].BIT.IEN_SCI0_TXI0 = 0;
      ICU.IER[IER_SCI0_ERI0].BIT.IEN_SCI0_ERI0 = 1;
      ICU.IER[IER_SCI0_TEI0].BIT.IEN_SCI0_TEI0 = 1;
      // Enable the Interrupt should be deferred until the structure is enabled

      // Enable writing to the PFSWE bit
      MPC.PWPR.BIT.B0WI = 0;
      // Enable writing to the PFS register
      MPC.PWPR.BIT.PFSWE = 1;

      // Assign P21 and P20 to function as RXD0 and TXD0...
      MPC.P21PFS.BYTE = 0x0A;
      MPC.P20PFS.BYTE = 0x0A;

      // Disable writing to the PFS register
      MPC.PWPR.BIT.PFSWE = 0;
      // Disable writing to the PFSWE bit
      MPC.PWPR.BIT.B0WI = 1;

      ICU.IR[IR_SCI0_TXI0].BIT.IR = 0;

      // Finally, enable UART rx and interrupts
      SCI0.SCR.BIT.TIE = 0; // Transmit Interrupt Disabled
      SCI0.SCR.BIT.RIE = 1; // Receive Interrupt Enabled
      SCI0.SCR.BIT.TE = 1; // Transmit Enabled
      SCI0.SCR.BIT.RE = 1; // Receive Enabled
   }

   return &uartData.interface;
#else
   IGNORE(clockFrequency);
   return NULL;
#endif
}
