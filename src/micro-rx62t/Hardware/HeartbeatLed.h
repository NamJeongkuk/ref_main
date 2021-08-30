/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef HEARTBEATLED_H
#define HEARTBEATLED_H

#include "TinyTimer.h"

/*!
 * Begin heartbeat at 1 Hz.
 */
void HeartbeatLed_Init(TinyTimerModule_t *timerModule);

#endif
