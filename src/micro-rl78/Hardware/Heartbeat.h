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
 * Begin heartbeat at 1 Hz on P12.0.
 */
void Heartbeat_Init(TinyTimerModule_t *timerModule);

#endif
