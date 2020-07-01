/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef PA5HEARTBEAT_H
#define PA5HEARTBEAT_H

#include "TinyTimer.h"

/*!
 * Begin heartbeat at 1 Hz.
 */
void Pa5Heartbeat_Init(TinyTimerModule_t *timerModule);

#endif
