/*!
 * @file
 * @brief Header file for the channel 0 Uart configuration
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

#ifndef UART_Channel0_H
#define UART_Channel0_H

#include "I_Uart.h"
/*!
 * Get the Uart singleton for channel 0
 * @return Pointer to the I_Uart interface.
 */
I_Uart_t * Uart_Channel0_GetInstance(uint32_t clockFrequency);

#endif
