/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SYSTEMCLOCK_H
#define SYSTEMCLOCK_H

#define U32_PCLKB                          ((uint32_t)27000000)   // All other peripherals, must be <= 32 mhz
#define U32_PCLKD                          ((uint32_t)27000000)   // Used for A/D clock, must be <= 32 mhz
#define U32_FCK                            ((uint32_t)27000000)   // Flash clock, must be <= 32 mhz

/*!
 * Initialize and select the high-speed clock
 */
void SystemClock_Init(void);

#endif
