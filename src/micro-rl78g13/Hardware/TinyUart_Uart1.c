/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

#include <stddef.h>
#include "iodefine.h"
#include "iodefine_ext.h"
#include "TinyUart_Uart1.h"
#include "I_TinyUart.h"
#include "TinyEvent_Synchronous.h"

#define UART_STOP_TX_RX_CHANNEL (0x000C)
#define UART_TX_CHANNEL_IS_SET_TO_ONE (0x0004)
#define UART_ENABLE_TX_CHANNEL (0x0004)
#define UART_RX_FLAG_ERROR_CLEAN (0x0007)
#define UART_ENABLE_RX_CHANNEL (0xFFF7)
#define UART_START_TX_RX_CHANNEL (0x000C)
#define UART_DISABLE_TX_CHANNEL (0xFFFB)

static I_TinyUart_t instance;
static TinyEvent_Synchronous_t receiveInterrupt;
static TinyEvent_Synchronous_t sendCompleteInterrupt;

void INT_SR1(void) __attribute__((interrupt));
void INT_SR1(void)
{
   uint8_t receivedData = RXD1;
   TinyUartOnReceiveArgs_t args = { receivedData };
   TinyEvent_Synchronous_Publish(&receiveInterrupt, &args);
}

void INT_ST1(void) __attribute__((interrupt));
void INT_ST1(void)
{
   TinyEvent_Synchronous_Publish(&sendCompleteInterrupt, NULL);
}

void INT_TM03H(void) __attribute__((interrupt));
void INT_TM03H(void)
{
   uint8_t dummy = RXD1;
   (void)dummy;

   // Disable the module and then re-init
   ST0 = UART_STOP_TX_RX_CHANNEL;
   SOE0 &= UART_DISABLE_TX_CHANNEL;

   // Disable the UART interrupt
   SRMK1 = 1; /* disable INTSR1 interrupt */
   SREMK1 = 1; /* disable INTSRE1 interrupt */

   // Cleaning the interrupt flags
   SRIF1 = 0;
   SREIF1 = 0;
   SIR03 = UART_RX_FLAG_ERROR_CLEAN;

   // Enable the UART interrupt
   SRMK1 = 0;
   SREMK1 = 0;

   SO0 |= UART_TX_CHANNEL_IS_SET_TO_ONE;
   SOE0 |= UART_ENABLE_TX_CHANNEL;
   SS0 = UART_START_TX_RX_CHANNEL;
}

static void Send(I_TinyUart_t *instance, uint8_t byte)
{
   (void)instance;
   TXD1 = byte;
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

I_TinyUart_t *TinyUart_Uart1_Init(void)
{
   // Initialize the events first to account for the possibility of the interrupt firing right after
   // interrupts are enabled
   TinyEvent_Synchronous_Init(&receiveInterrupt);
   TinyEvent_Synchronous_Init(&sendCompleteInterrupt);

   // Disabling TX, RX and error interrupts
   STMK1 = 1;
   SRMK1 = 1;
   SREMK1 = 1;

   // Enables the SAU0 Module. Supply clock from the CPU clock
   SAU0EN = 1;
   // Idle time to hardware response (4 instructions).
   NOP();
   NOP();
   NOP();
   NOP();

   // fclk = 32MHz, fCK00 = fclk / 1 = 32MHz
   SPS0 = 0x0000;

   // Disable RX, TX
   ST0 = UART_STOP_TX_RX_CHANNEL;

   // This section configures the TX //
   // Setup the channel 2 (TX)
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
   SMR02 = 0x0022;

   // Setup the channel 2 (TX) data format
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
   // 14-15 -    0b10 : Setting the UART channel 2 like transmission.
   SCR02 = 0x8497;

   // Setup the channel 2 (TX) data format
   // Bit   - Setting : Comment
   // 0-8   -       0 : Bits 8-0 (9 bits) transmit/receive data register.
   // 9-15  -      51 : Dividing the operation clock (CK00) to setting the TX UART speed (230400).
   SDR02 = 0x8800; // 32 MHz / (68 + 1) / 2 => 231884 bps;

   // Sets to logic one the serial data output of channel 2 (TX).
   SO0 |= UART_TX_CHANNEL_IS_SET_TO_ONE;

   // The serial data output of channel 2 (TX) is not inverted.
   SOL0 |= 0x0000;

   // Cleaning the framing, parity and overrun flag errors of channel 3 (RX)
   SIR03 = UART_RX_FLAG_ERROR_CLEAN;

   // Noise filter enable for the RX pin
   NFEN0 = 0x04;

   // Setup the channel 3 (RX)
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
   SMR03 = 0x0122;

   // Setup the channel 3 (RX) data format
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
   // 14-15 -    0b01 : Setting the UART channel 2 like reception.
   SCR03 = 0x4497;

   // Setup the channel 3 (RX) data format
   // Bit   - Setting : Comment
   // 0-8   -       0 : Bits 8-0 (9 bits) transmit/receive data register.
   // 9-15  -      51 : Dividing the operation clock (CK00) to setting the RX UART speed (230400).
   SDR03 = 0x8800; // 32 MHz / (68 + 1) / 2 => 231884 bps

   //Enables the UART output channel 2 (TX) and disable the output channel 3 (RX)
   SOE0 = UART_ENABLE_TX_CHANNEL & UART_ENABLE_RX_CHANNEL;

   // Turn on the TX pin
   P0_bit.no0 = 1;

   // TX and RX pin configuration like digital I/O
   PMC0_bit.no0 = 0;
   PMC0_bit.no1 = 0;

   // TX and RX pin configuration like output and input
   PM0_bit.no0 = 0;
   PM0_bit.no1 = 1;

   // Cleaning the TX, RX and error interrupts
   SRIF1 = 0;
   SREIF1 = 0;

   // Enable the RX, TX complete, and error interrupts
   SRMK1 = 0; // RX (INT_SR1)
   STMK1 = 0; // TX (INT_TM1)
   SREMK1 = 0; // Error (INT_SRE1)

   // Start the UART channels
   SS0 = UART_START_TX_RX_CHANNEL;

   // Set up for level 1 priority (system tick takes priority over this)
   SRPR01 = 1;
   SRPR11 = 0;

   instance.api = &api;
   return &instance;
}
