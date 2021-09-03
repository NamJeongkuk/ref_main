/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

#ifndef WATCHDOG_H
#define WATCHDOG_H

#include "I_Action.h"
#include "Timer.h"

/*!
 */
void Watchdog_Init(void);

/*!
 */
void Watchdog_Kick(void);

/*!
 * @param timerModule
 */
void Watchdog_EnableAutoKick(TimerModule_t *timerModule);

/*!
 * @return
 */
I_Action_t *Watchdog_KickAction(void);

#endif
