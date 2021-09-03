/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <string.h>
#include "iodefine.h"
#include "Dtc.h"
#include "BufferedUart_Uart0.h"
#include "Event_SingleSubscriberSynchronous.h"
#include "uassert.h"
#include "utils.h"

// Set the DTC Receive Buffer Size: (230400 bits/sec) / 10 bits / 1000 msec = 23.04 bytes/msec ~= 25 bytes/msec
// The buffer is serviced every msec, so double this number for safety: 25 * 2 = 50 bytes
enum
{
   ClientReceiveBufferSize = 50,
   ReceiveBufferSize = 50,
   ReceiveRepeatCount = (uint16_t)((ReceiveBufferSize << 8) | ReceiveBufferSize)
};

volatile struct st_dtc_full uart0Receive __attribute__((section(".dtcTransferInformation")));
volatile struct st_dtc_full uart0Transmit __attribute__((section(".dtcTransferInformation")));

typedef struct
{
   I_BufferedUart_t interface;
   Event_SingleSubscriberSynchronous_t OnReceive;
   Event_SingleSubscriberSynchronous_t OnTransmit;
   bool waitingForTransmitToComplete;
   uint8_t dtcReceiveBuffer[ReceiveBufferSize];
   uint8_t dtcRingBufferTail;
} BufferedUartData_t;

static BufferedUartData_t instance __attribute__((section(".dtcTransferInformation")));

void SCI0_ERI0(void) __attribute__((interrupt));
void SCI0_ERI0(void)
{
   volatile uint8_t dummy = SCI0.RDR;
   IGNORE(dummy);

   if(SCI0.SSR.BIT.FER)
   {
      SCI0.SCR.BIT.RE = 0;
   }

   // Clear error flags
   while(0 != (0x30 & SCI0.SSR.BYTE))
   {
      SCI0.SSR.BYTE = 0xC0;
   }

   ICU.IR[IR_SCI0_RXI0].BIT.IR = 0;
   ICU.IR[IR_SCI0_ERI0].BIT.IR = 0;

   SCI0.SCR.BIT.RE = 1;
}

static void Transmit(I_BufferedUart_t *_instance, const uint8_t *data, const uint16_t byteCount)
{
   IGNORE(_instance);

   uassert(!instance.waitingForTransmitToComplete);
   instance.waitingForTransmitToComplete = true;

   uart0Transmit.CR.CRA = byteCount - 1;
   uart0Transmit.SAR = (void *)&data[1];

   // DTC activation by SCI0 TX interrupt request is enabled
   DTCE(SCI0, TXI0) = (byteCount > 1);

   // Start the transfer by sending the first byte in the buffer
   SCI0.TDR = *data;
}

static I_Event_t *GetOnReceiveEvent(I_BufferedUart_t *_instance)
{
   IGNORE(_instance);
   return &instance.OnReceive.interface;
}

static I_Event_t *GetOnTransmitCompleteEvent(I_BufferedUart_t *_instance)
{
   IGNORE(_instance);
   return &instance.OnTransmit.interface;
}

static uint16_t DtcRingBuffer_Count(void)
{
   uint16_t capacity = sizeof(instance.dtcReceiveBuffer);
   uint8_t dtcRingBufferHead = capacity - (uart0Receive.CR.CRA & 0xFF);

   if(dtcRingBufferHead >= instance.dtcRingBufferTail)
   {
      return (dtcRingBufferHead - instance.dtcRingBufferTail);
   }
   else
   {
      return capacity - (instance.dtcRingBufferTail - dtcRingBufferHead);
   }
}

static void Run(I_BufferedUart_t *_instance)
{
   IGNORE(_instance);

   if(instance.waitingForTransmitToComplete && !uart0Transmit.CR.CRA && SCI0.SSR.BIT.TDRE)
   {
      instance.waitingForTransmitToComplete = false;
      Event_SingleSubscriberSynchronous_Publish(&instance.OnTransmit, NULL);
   }

   {
      uint8_t clientReceiveBuffer[ClientReceiveBufferSize];
      uint16_t ringBufferCount = DtcRingBuffer_Count();
      uint8_t index = 0;

      while((ringBufferCount--) && (index < ClientReceiveBufferSize))
      {
         clientReceiveBuffer[index++] = instance.dtcReceiveBuffer[instance.dtcRingBufferTail];
         instance.dtcRingBufferTail = (instance.dtcRingBufferTail + 1) % sizeof(instance.dtcReceiveBuffer);
      }

      if(index > 0)
      {
         BufferedUartOnReceiveArgs_t args = { clientReceiveBuffer, index };
         Event_SingleSubscriberSynchronous_Publish(&instance.OnReceive, &args);
      }
   }
}

static const I_BufferedUart_Api_t api = { Transmit, GetOnTransmitCompleteEvent, GetOnReceiveEvent, Run };

I_BufferedUart_t *BufferedUart_Uart0_Init(void)
{
   uassert(sizeof(instance.dtcReceiveBuffer) >= ((ReceiveRepeatCount & 0xFF00) >> 8) && (sizeof(instance.dtcReceiveBuffer) >= (ReceiveRepeatCount & 0xFF)));

   Event_SingleSubscriberSynchronous_Init(&instance.OnReceive);
   Event_SingleSubscriberSynchronous_Init(&instance.OnTransmit);

   instance.interface.api = &api;
   instance.dtcRingBufferTail = 0;

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
   ICU.IER[IER_SCI0_TEI0].BIT.IEN_SCI0_TEI0 = 0;

   uart0Receive.MRA = 0x40;
   // 01000000b
   // ||||||XX
   // ||||##-----> MRA SM SAR value is fixed
   // ||##-------> MRA SZ Byte (8 bit) transfer
   // ##---------> MRA MD Repeat transfer mode

   uart0Receive.MRB = 0x08;
   // 00001000b
   // ||||||XX
   // ||||##---> MRB DM DAR value increments
   // |||#-----> MRB DTS Transfer destination side is repeat area
   // ||#------> MRB DISEL An interrupt request to the CPU is generated when specified data transfer is completed
   // |#-------> MRB CHNS
   // #--------> MRB CHNE Chain transfer is disabled

   uart0Receive.DAR = (void *)instance.dtcReceiveBuffer; // Transfer destination start address
   uart0Receive.SAR = (void *)&SCI0.RDR; // Transfer source start address
   uart0Receive.CR.CRA = ReceiveRepeatCount; // Transfer count: CRAL = CRAH = number of total byte transfers before wrapping round
   uart0Receive.CR.CRB = 0x0000; // Not used in repeat transfer.

   DTCE(SCI0, RXI0) = 1; // Enable DTC activation by RX interrupt

   SCI0.SCR.BIT.RIE = 1; // Receive Interrupt Enabled
   SCI0.SCR.BIT.RE = 1; // Receive Enabled

   uart0Transmit.MRA = 0x08;
   // 00001000b
   // ||||||XX
   // ||||##-----> MRA SM SAR value increments
   // ||##-------> MRA SZ Byte (8 bit) transfer
   // ##---------> MRA MD Normal transfer mode

   uart0Transmit.MRB = 0x00;
   // 00000000b
   // ||||||XX
   // ||||##---> MRB DM DAR value is fixed
   // |||#-----> MRB DTS Transfer destination side is repeat area
   // ||#------> MRB DISEL An interrupt request to the CPU is generated when specified data transfer is completed
   // |#-------> MRB CHNS
   // #--------> MRB CHNE Chain transfer is disabled

   uart0Transmit.SAR = NULL; // Transfer source start address - set up on send
   uart0Transmit.DAR = (void *)&SCI0.TDR; // Transfer destination start address
   uart0Transmit.CR.CRA = 0x0000; // Transfer count: normal mode this is a 16 bit number
   uart0Transmit.CR.CRB = 0x0000; // Not used in repeat transfer.

   DTCE(SCI0, TXI0) = 1; // Enable DTC activation by TX interrupt

   // To avoid generating a transmit interrupt here, we must enable transmit before enabling the transmit interrupt.
   SCI0.SCR.BIT.TE = 1; // Transmit Enabled
   SCI0.SCR.BIT.TIE = 1; // Transmit Interrupt Enabled

   return &instance.interface;
}
