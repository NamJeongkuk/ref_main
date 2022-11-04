/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <string.h>
#include "BufferedUart_Channel5.h"
#include "Event_SingleSubscriberSynchronous.h"
#include "iodefine.h"
#include "utils.h"
#include "uassert.h"
#include "Dtc.h"
#include "InterruptPriorityLevel.h"

// Set the DTC Receive Buffer Size: (230400 bits/sec) / 10 bits / 1000 msec = 23.04 bytes/msec ~= 25 bytes/msec
// The buffer is serviced every msec, so double this number for safety: 25 * 2 = 50 bytes
#define DTC_RECEIVE_BUFFER_SIZE_BYTES (50)
#define TRANSFER_BUFFER_SIZE_BYTES (50)
#define RECEIVE_REPEAT_COUNT (uint16_t)((DTC_RECEIVE_BUFFER_SIZE_BYTES << 8) | DTC_RECEIVE_BUFFER_SIZE_BYTES)

volatile struct st_dtc_full uart5Receive __attribute__((section(".dtcTransferInformation")));
volatile struct st_dtc_full uart5Transmit __attribute__((section(".dtcTransferInformation")));

typedef struct
{
   I_BufferedUart_t interface;
   Event_SingleSubscriberSynchronous_t OnReceive;
   Event_SingleSubscriberSynchronous_t OnTransmit;
   bool waitingForTransmitToComplete;
   uint8_t dtcReceiveBuffer[DTC_RECEIVE_BUFFER_SIZE_BYTES];
   uint8_t dtcRingBufferTail;
} BufferedUartData_t;

static BufferedUartData_t instance __attribute__((section(".dtcTransferInformation")));

void SCI5_ERI5(void) __attribute__((interrupt));
void SCI5_ERI5(void)
{
   volatile uint8_t received_data = 0;
   IGNORE(received_data);

   received_data = SCI5.RDR;

   if(SCI5.SSR.BIT.FER)
   {
      SCI5.SCR.BIT.RE = 0;
   }

   // Clear error flags
   while(0 != (0x38 & SCI5.SSR.BYTE))
   {
      SCI5.SSR.BYTE = 0xC0;
   }

   ICU.IR[IR_SCI5_RXI5].BIT.IR = 0;
   ICU.IR[IR_SCI5_ERI5].BIT.IR = 0;

   SCI5.SCR.BIT.RE = 1;
}

static void Transmit(I_BufferedUart_t *_instance, const uint8_t *data, const uint16_t byteCount)
{
   IGNORE(_instance);
   uassert(!instance.waitingForTransmitToComplete);

   instance.waitingForTransmitToComplete = true;

   uart5Transmit.CR.CRA = byteCount - 1;
   uart5Transmit.SAR = (void *)&data[1];
   // DTC activation by SCI5 Tx interrupt request is enabled
   DTCE(SCI5, TXI5) = (byteCount > 1);
   // Start the transfer by sending the first byte in the buffer
   SCI5.TDR = *data;
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

static uint16_t DtcRingBuffer_GetCount()
{
   uint16_t capacity = sizeof(instance.dtcReceiveBuffer);
   uint8_t dtcRingBufferHead = capacity - (uart5Receive.CR.CRA & 0xFF);

   if(dtcRingBufferHead >= instance.dtcRingBufferTail)
   {
      return (dtcRingBufferHead - instance.dtcRingBufferTail);
   }
   else
   {
      return capacity -
         (instance.dtcRingBufferTail - dtcRingBufferHead);
   }
}

static void Run(I_BufferedUart_t *_instance)
{
   IGNORE(_instance);
   // Transmit Data Ready?
   if(instance.waitingForTransmitToComplete && !uart5Transmit.CR.CRA && SCI5.SSR.BIT.TDRE)
   {
      instance.waitingForTransmitToComplete = false;
      Event_SingleSubscriberSynchronous_Publish(&instance.OnTransmit, NULL);
   }
   { // Any Received Data?
      uint8_t localData[TRANSFER_BUFFER_SIZE_BYTES];
      uint16_t queueCount = DtcRingBuffer_GetCount();
      uint8_t index = 0;

      while((queueCount--) && (index < TRANSFER_BUFFER_SIZE_BYTES))
      {
         localData[index++] = instance.dtcReceiveBuffer[instance.dtcRingBufferTail];
         instance.dtcRingBufferTail = (instance.dtcRingBufferTail + 1) % sizeof(instance.dtcReceiveBuffer);
      }

      if(index > 0)
      {
         BufferedUartOnReceiveArgs_t args = { localData, index };
         Event_SingleSubscriberSynchronous_Publish(&instance.OnReceive, &args);
      }
   }
}

static const I_BufferedUart_Api_t bufferedUartApi = {
   Transmit,
   GetOnTransmitCompleteEvent,
   GetOnReceiveEvent,
   Run
};

I_BufferedUart_t *BufferedUart_Channel5_Init(void)
{
   uassert(sizeof(instance.dtcReceiveBuffer) >= ((RECEIVE_REPEAT_COUNT & 0xFF00) >> 8) && (sizeof(instance.dtcReceiveBuffer) >= (RECEIVE_REPEAT_COUNT & 0xFF)));

   Event_SingleSubscriberSynchronous_Init(&instance.OnReceive);
   Event_SingleSubscriberSynchronous_Init(&instance.OnTransmit);

   instance.interface.api = &bufferedUartApi;
   instance.dtcRingBufferTail = 0;

   // First, allow writes to Module-Stop-Control-Register
   SYSTEM.PRCR.WORD = 0xA502;
   // Now turn on SCI5
   MSTP_SCI5 = 0;
   // Disable writes to the Module-Stop-Control-Register
   SYSTEM.PRCR.WORD = 0xA500;
   // Disable UART in/out while we fiddle with it, set the clock to internal (CKE=0)
   SCI5.SCR.BYTE = 0;
   // Setup port PC2 (rx) and PC3 (tx) for use by UART
   PORTC.PMR.BIT.B2 = 1;
   PORTC.PMR.BIT.B3 = 1;
   PORTC.PDR.BIT.B3 = 1;
   PORTC.PODR.BIT.B3 = 1;
   // Setup the mode register data format
   // Bit - Setting : Comment
   // 0:1 -     00  : Clock Select PCLK (n=0)
   //   2 -      0  : No multi-processor mode
   //   3 -      0  : 1 Stop bit
   //   4 -      0  : Even Parity
   //   5 -      0  : No Parity
   //   6 -      0  : 8 bit data transfer
   //   7 -      0  : Async
   SCI5.SMR.BYTE = 0x00;
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
   SCI5.SCMR.BYTE = 0x72;
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
   SCI5.SEMR.BIT.ACS0 = 0;
   SCI5.SEMR.BIT.BRME = 1;
   SCI5.SEMR.BIT.ABCS = 1;
   SCI5.SEMR.BIT.NFEN = 0;
   SCI5.SEMR.BIT.BGDM = 0;
   SCI5.SEMR.BIT.RXDESEL = 0;
   // Baud rate configured for 230400 with 0.03% error.
   // Set using Python tool in repo tools folder "CalculateBaudUsingMDDR.py"
   SCI5.BRR = 5;
   SCI5.MDDR = 177;

   // Set up interrupts in the Interrupt-Control-Unit
   // There is only one priority register per SCI port
   ICU.IPR[IPR_SCI5_RXI5].BIT.IPR = E_IRQ_PRIORITY_14;
   // But there are 4 separate interrupt enable bits
   ICU.IER[IER_SCI5_RXI5].BIT.IEN_SCI5_RXI5 = 1;
   ICU.IER[IER_SCI5_TXI5].BIT.IEN_SCI5_TXI5 = 1;
   ICU.IER[IER_SCI5_ERI5].BIT.IEN_SCI5_ERI5 = 1;
   ICU.IER[IER_SCI5_TEI5].BIT.IEN_SCI5_TEI5 = 0;

   // First, allow writing to the Multi-Function-Pin-Controller
   MPC.PWPR.BIT.B0WI = 0;
   MPC.PWPR.BIT.PFSWE = 1;

   // Assign PC2 and PC3 to function as RXD5 and TXD5...
   MPC.PC2PFS.BYTE = 0x0A;
   MPC.PC3PFS.BYTE = 0x0A;

   // Disable writing to the Multi-Function-Pin-Controller
   MPC.PWPR.BIT.PFSWE = 0;
   MPC.PWPR.BIT.B0WI = 1;

   // Finally, enable UART rx and interrupts
   SCI5.SCR.BIT.CKE = 0;

   uart5Receive.MRA = 0x40;
   // 01000000b
   // ||||||XX
   // ||||##-----> MRA SM SAR value is fixed
   // ||##-------> MRA SZ Byte (8 bit) transfer
   // ##---------> MRA MD Repeat transfer mode

   uart5Receive.MRB = 0x08;
   // 00001000b
   // ||||||XX
   // ||||##---> MRB DM DAR value increments
   // |||#-----> MRB DTS Transfer destination side is repeat area
   // ||#------> MRB DISEL An interrupt request to the CPU is generated when specified data transfer is completed
   // |#-------> MRB CHNS
   // #--------> MRB CHNE Chain transfer is disabled

   uart5Receive.SAR = (void *)&SCI5.RDR; // Transfer source start address
   uart5Receive.DAR = (void *)instance.dtcReceiveBuffer; // Transfer destination start address
   uart5Receive.CR.CRA = RECEIVE_REPEAT_COUNT; // Transfer count: CRAL = CRAH = number of total byte transfers before wrapping round
   uart5Receive.CR.CRB = 0x0000; // Not used in repeat transfer.

   DTCE(SCI5, RXI5) = 1; // DTC activation by SCI5 Rx interrupt request is enabled

   SCI5.SCR.BIT.RIE = 1; // Receive Interrupt Enabled
   SCI5.SCR.BIT.RE = 1; // Receive Enabled

   uart5Transmit.MRA = 0x08;
   // 00001000b
   // ||||||XX
   // ||||##-----> MRA SM SAR value increments
   // ||##-------> MRA SZ Byte (8 bit) transfer
   // ##---------> MRA MD Normal transfer mode

   uart5Transmit.MRB = 0x00;
   // 00000000b
   // ||||||XX
   // ||||##---> MRB DM DAR value is fixed
   // |||#-----> MRB DTS Transfer destination side is repeat area
   // ||#------> MRB DISEL An interrupt request to the CPU is generated when specified data transfer is completed
   // |#-------> MRB CHNS
   // #--------> MRB CHNE Chain transfer is disabled

   uart5Transmit.SAR = NULL; // Transfer source start address - set up on send
   uart5Transmit.DAR = (void *)&SCI5.TDR; // Transfer destination start address
   uart5Transmit.CR.CRA = 0x0000; // Transfer count: normal mode this is a 16 bit number
   uart5Transmit.CR.CRB = 0x0000; // Not used in repeat transfer.

   DTCE(SCI5, TXI5) = 1; // DTC activation by SCI5 Tx interrupt request is enabled

   // To avoid generating a transmit interrupt here, we must enable transmit before enabling the transmit interrupt.
   SCI5.SCR.BIT.TE = 1; // Transmit Enabled
   SCI5.SCR.BIT.TIE = 1; // Transmit Interrupt Enabled

   return &instance.interface;
}
