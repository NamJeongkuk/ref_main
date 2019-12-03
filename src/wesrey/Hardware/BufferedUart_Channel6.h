/*!
 * @file
 * @brief RX130 SCI6 Buffered uart
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef BUFFEREDUART_CHANNEL6_H
#define BUFFEREDUART_CHANNEL6_H

#include "I_BufferedUart.h"
#include "Dtc.h"

/*!
 * Initialize the SCI6 UART
 * @return I_BufferedUart_t for the SCI6 UART
 */
I_BufferedUart_t * BufferedUart_Channel6_Init(void);

/*!
 * Reference to DTC control block for receive
 */
extern volatile struct st_dtc_full uart6Receive;

/*!
 * Reference to DTC control block for transmit
 */
extern volatile struct st_dtc_full uart6Transmit;

#endif
