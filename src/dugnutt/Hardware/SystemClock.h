/*!
 * @file
 * @brief Clock module header file for RX130
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SYSTEMCLOCK_H
#define SYSTEMCLOCK_H

#include <stdint.h>
#include "I_Action.h"

#define U32_PCLKD                          ((uint32_t)32000000)   // Used for A/D clock, must be <= 32 mhz
#define U32_PCLKB                          ((uint32_t)32000000)   // All other peripherals, must be <= 32 mhz
#define U32_FCK                            ((uint32_t)32000000)   // Flash clock, must be <= 32 mhz

/*!
 * Initialize and select the high-speed clock
 */
void SystemClock_Init(void);

#endif
