/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

#include <stddef.h>
#include "iodefine.h"
#include "iodefine_ext.h"
#include "TinyUart_Uart0.h"
#include "I_TinyUart.h"
#include "TinyEvent_Synchronous.h"

#define UART_STOP_TX_RX_CHANNEL (0x0003)
#define UART_TX_CHANNEL_IS_SET_TO_ONE (0x0001)
#define UART_ENABLE_TX_CHANNEL (0x0001)
#define UART_RX_FLAG_ERROR_CLEAN (0x0007)
#define UART_START_TX_RX_CHANNEL (0x0003)

static I_TinyUart_t instance;
static TinyEvent_Synchronous_t receiveInterrupt;
static TinyEvent_Synchronous_t sendCompleteInterrupt;

void INT_SR0(void) __attribute__((interrupt));
void INT_SR0(void)
{
   uint8_t receivedData = RXD0;
   TinyUartOnReceiveArgs_t args = { receivedData };
   TinyEvent_Synchronous_Publish(&receiveInterrupt, &args);
}

void INT_ST0(void) __attribute__((interrupt));
void INT_ST0(void)
{
   TinyEvent_Synchronous_Publish(&sendCompleteInterrupt, NULL);
}

void INT_SRE0(void) __attribute__((interrupt));
void INT_SRE0(void)
{
   volatile uint8_t dummy = RXD0;
   (void)dummy;

   SIR01 = UART_RX_FLAG_ERROR_CLEAN;
}

static void Send(I_TinyUart_t *instance, uint8_t byte)
{
   (void)instance;
   TXD0 = byte;
}

static I_TinyInterrupt_t *GetOnReceiveInterrupt(I_TinyUart_t *instance)
{
   (void)instance;
   return &receiveInterrupt.interface;
}

static I_TinyInterrupt_t *GetOnSendCompleteInterrupt(I_TinyUart_t *instance)
{
   (void)instance;
   return &sendCompleteInterrupt.interface;
}

static const I_TinyUart_Api_t api = { Send, GetOnReceiveInterrupt, GetOnSendCompleteInterrupt };

I_TinyUart_t *TinyUart_Uart0_Init(void)
{
   // Initialize the events first to account for the possibility of the interrupt firing right after
   // interrupts are enabled
   TinyEvent_Synchronous_Init(&receiveInterrupt);
   TinyEvent_Synchronous_Init(&sendCompleteInterrupt);

   // Enable the SAU0 Module and supply the clock from the CPU clock
   SAU0EN = 1;
   // Idle time to hardware response (4 instructions).
   NOP();
   NOP();
   NOP();
   NOP();

   // fclk = 24MHz, fCK00 = fclk / 1 = 24MHz
   SPS0 = 0x0000;

   // Disable TX, RX, and error interrupts
   STMK0 = 1;
   SRMK0 = 1;
   SREMK0 = 1;

   // Clear the TX, RX, and error interrupt flags
   STIF0 = 0;
   SRIF0 = 0;
   SREIF0 = 0;

   // Set high priority for TX, RX, and error interrupts
   STPR10 = 0;
   STPR00 = 0;
   SRPR10 = 0;
   SRPR00 = 0;
   SREPR10 = 0;
   SREPR00 = 0;

   // Disable RX, TX
   ST0 = UART_STOP_TX_RX_CHANNEL;

   // This section configures the TX //
   // Setup channel 0 (TX)
   // Bit  - Setting : Comment
   //   0  -       0 : Transfer end interrupt.
   // 1-2  -     0b1 : UART mode operation.
   // 3-5  -  unused : Bits 4-3 defined as 0, bit 5 as 1.
   //   6  -       0 : The input communication data is captured as is.
   //   7  -  unused : Defined as 0.
   //   8  -       0 : Only software trigger is valid.
   // 9-13    unused : Defined as 0.
   //   14 -       0 : Divided operation clock fMCK specified by the bit 15 (CKS02 bit).
   //   15 -       0 : Operation clock CK00 is selected.
   SMR00 = 0x0022;

   // Setup channel 0 (TX) data format
   // Bit   - Setting : Comment
   // 0-1   -    0b11 : 8-bit data length.
   // 2-3   -  unused : Bit 2 defined as 1, bit 3 as 0.
   // 4-5   -    0b01 : 1 stop bit length.
   //   6   -  unused : Defined as 0.
   //   7   -       1 : Outputs data with LSB first.
   // 8-9   -    0b00 : No Parity
   //  10           1 : Enables generation of error interrupt.
   //  11   -  unused : Defined as 0.
   // 12-13 -    0b00 : 0 in the UART mode.
   // 14-15 -    0b10 : Setting the UART channel 0 like transmission.
   SCR00 = 0x8497;

   // Setup channel 0 (TX) data format
   // Bit   - Setting : Comment
   // 0-8   -       0 : Bits 8-0 (9 bits) transmit/receive data register.
   // 9-15  -      51 : Dividing the operation clock (CK00) to setting the TX UART speed (230400).
   SDR00 = 0x6600; // 24 MHz / (51 + 1) / 2 => 230769 bps;

   // Sets to logic one the serial data output of channel 0 (TX).
   SO0 |= UART_TX_CHANNEL_IS_SET_TO_ONE;

   // The serial data output of channel 0 (TX) is not inverted.
   SOL0 |= 0x0000;

   // Noise filter enable for the RX pin
   NFEN0 = 0x01;

   // Clean the framing, parity and overrun flag errors of channel 1 (RX)
   SIR01 = UART_RX_FLAG_ERROR_CLEAN;

   // Setup channel 1 (RX)
   // Bit  - Setting : Comment
   //   0  -       0 : Transfer end interrupt (Only for transmission).
   // 1-2  -     0b1 : UART mode operation.
   // 3-5  -  unused : Bits 4-3 defined as 0, bit 5 as 1.
   //   6  -       0 : The input communication data is captured as is.
   //   7  -  unused : Defined as 0.
   //   8  -       1 : A valid edge of the RXD is selected like a start trigger source
   // 9-13    unused : Defined as 0.
   //   14 -       0 : Divided operation clock fMCK specified by the bit 15 (CKS03 bit).
   //   15 -       0 : Operation clock CK00 is selected.
   SMR01 = 0x0122;

   // Setup channel 1 (RX) data format
   // Bit   - Setting : Comment
   // 0-1   -    0b11 : 8-bit data length.
   // 2-3   -  unused : Bit 2 defined as 1, bit 3 as 0.
   // 4-5   -    0b01 : 1 stop bit length.
   //   6   -  unused : Defined as 0.
   //   7   -       1 : Inputs data with LSB first.
   // 8-9   -    0b00 : No Parity
   //  10           1 : Enables generation of error interrupt.
   //  11   -  unused : Defined as 0.
   // 12-13 -    0b00 : 0 in the UART mode.
   // 14-15 -    0b01 : Setting the UART channel 0 like reception.
   SCR01 = 0x4497;

   // Setup channel 1 (RX) data format
   // Bit   - Setting : Comment
   // 0-8   -       0 : Bits 8-0 (9 bits) transmit/receive data register.
   // 9-15  -      51 : Dividing the operation clock (CK00) to setting the RX UART speed (230400).
   SDR01 = 0x6600; // 24 MHz / (51 + 1) / 2 => 230769 bps

   // Enables the UART output channel 0 (TX) and disable the output channel 1 (RX)
   SOE0 = UART_ENABLE_TX_CHANNEL;

   /* Set RxD0 pin */
   PMC1 &= 0xFDU;
   PM1 |= 0x02U;
   /* Set TxD0 pin */
   PMC1 &= 0xFBU;
   P1 |= 0x04U;
   PM1 &= 0xFBU;

   // Enable the RX, TX complete, and error interrupts
   SRMK0 = 0;
   STMK0 = 0;
   SREMK0 = 0;

   // Start the UART channels
   SS0 = UART_START_TX_RX_CHANNEL;

   instance.api = &api;
   return &instance;
}
