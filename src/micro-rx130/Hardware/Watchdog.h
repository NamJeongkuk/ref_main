/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

#ifndef WATCHDOG_H
#define WATCHDOG_H

#include "I_TinyAction.h"

/*!
 */
void Watchdog_Init(void);

/*!
 */
void Watchdog_Kick(void);

/*!
 * @return
 */
I_TinyAction_t * Watchdog_KickAction(void);

#endif
