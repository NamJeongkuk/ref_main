/*!
 * @file
 * @brief Kicks watchdog when super loop and system tick are operational.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef WATCHDOG_H
#define WATCHDOG_H

#include "TinyTimer.h"

/*!
 * @param timerModule
 */
void Watchdog_Init(TinyTimerModule_t *timerModule);

#endif
