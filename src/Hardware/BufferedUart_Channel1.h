/*!
 * @file
 * @brief RX130 SCI1 Buffered uart
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef BUFFEREDUART_CHANNEL1_H
#define BUFFEREDUART_CHANNEL1_H

#include "I_BufferedUart.h"
#include "Dtc.h"

/*!
 * Initialize the SCI1 UART
 * @return I_BufferedUart_t for the SCI1 UART
 */
I_BufferedUart_t *BufferedUart_Channel1_Init(void);

/*!
 * Reference to DTC control block for receive
 */
extern volatile struct st_dtc_full uart1Receive;

/*!
 * Reference to DTC control block for transmit
 */
extern volatile struct st_dtc_full uart1Transmit;

#endif
