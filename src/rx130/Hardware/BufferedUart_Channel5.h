/*!
 * @file
 * @brief RX130 SCI5 Buffered uart
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef BUFFEREDUART_CHANNEL5_H
#define BUFFEREDUART_CHANNEL5_H

#include "I_BufferedUart.h"
#include "Dtc.h"

/*!
 * Initialize the SCI5 UART
 * @return I_BufferedUart_t for the SCI5 UART
 */
I_BufferedUart_t * BufferedUart_Channel5_Init(void);

/*!
 * Reference to DTC control block for receive
 */
extern volatile struct st_dtc_full uart5Receive;

/*!
 * Reference to DTC control block for transmit
 */
extern volatile struct st_dtc_full uart5Transmit;

#endif
