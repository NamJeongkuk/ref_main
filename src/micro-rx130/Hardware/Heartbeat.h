/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef HEARTBEAT_H
#define HEARTBEAT_H

#include "TinyTimer.h"

/*!
 * Begin heartbeat at 4 Hz.
 */
void Heartbeat_Init(TinyTimerModule_t *timerModule);

#endif
