/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef WATCHDOG_H
#define WATCHDOG_H

/*!
 * Configures a 1 second watchdog timeout.
 */
void Watchdog_Init(void);

/*!
 */
void Watchdog_Kick(void);

#endif
