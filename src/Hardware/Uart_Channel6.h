/*!
 * @file
 * @brief Header file for the channel 6 Uart configuration
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

#ifndef UART_Channel6_H
#define UART_Channel6_H

#include "I_Uart.h"
/*!
 * Get the Uart singleton for channel 6
 * @return Pointer to the I_Uart interface.
 */
I_Uart_t * Uart_Channel6_GetInstance(uint32_t clockFrequency);

#endif
