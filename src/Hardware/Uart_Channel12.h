/*!
 * @file
 * @brief Header file for the channel 12 Uart configuration
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

#ifndef UART_CHANNEL12_H
#define UART_CHANNEL12_H

#include "I_Uart.h"
/*!
 * Get the Uart singleton for channel 12
 * @return Pointer to the I_Uart interface.
 */
I_Uart_t *Uart_Channel12_GetInstance(uint32_t clockFrequency);

#endif
