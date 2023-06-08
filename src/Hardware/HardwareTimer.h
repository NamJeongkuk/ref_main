/*!
 * @file
 * @brief Hardware timer driver
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef HARDWARETIMER_H
#define HARDWARETIMER_H

#include <stdint.h>

/*!
 * Delay for a specific time. Should be called with interrupts disabled
 * @param microSecondsDelayTime
 */
void HardwareTimer_BlockingDelayInMicroSeconds(uint16_t microSecondsDelayTime);

/*!
 * Initialize hardware timer
 */
void HardwareTimer_Init(void);

#endif
