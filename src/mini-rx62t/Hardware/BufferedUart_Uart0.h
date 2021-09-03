/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef BUFFEREDUART_UART0_H
#define BUFFEREDUART_UART0_H

#include "I_BufferedUart.h"
#include "Dtc.h"

/*!
 * @return
 */
I_BufferedUart_t *BufferedUart_Uart0_Init(void);

/*!
 * Reference to DTC control block for receive
 */
extern volatile struct st_dtc_full uart0Receive;

/*!
 * Reference to DTC control block for transmit
 */
extern volatile struct st_dtc_full uart0Transmit;

#endif
