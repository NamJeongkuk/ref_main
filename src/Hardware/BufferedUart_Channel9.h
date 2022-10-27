/*!
 * @file
 * @brief RX130 SCI9 Buffered uart
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef BUFFEREDUART_CHANNEL9_H
#define BUFFEREDUART_CHANNEL9_H

#include "I_BufferedUart.h"
#include "Dtc.h"

/*!
 * Initialize the SCI9 UART
 * @return I_BufferedUart_t for the SCI9 UART
 */
I_BufferedUart_t *BufferedUart_Channel9_Init(void);

/*!
 * Reference to DTC control block for receive
 */
extern volatile struct st_dtc_full uart9Receive;

/*!
 * Reference to DTC control block for transmit
 */
extern volatile struct st_dtc_full uart9Transmit;

#endif
