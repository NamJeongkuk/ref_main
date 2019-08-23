/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef PD4HEARTBEAT_H
#define PD4HEARTBEAT_H

#include "TinyTimer.h"

/*!
 * Begin heartbeat at 2 Hz.
 */
void Pd4Heartbeat_Init(TinyTimerModule_t *timerModule);

#endif
