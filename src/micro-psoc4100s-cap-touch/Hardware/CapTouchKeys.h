/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef CAPTOUCHKEYS_H
#define CAPTOUCHKEYS_H

#include "TinyTimer.h"

/*!
 */
void CapTouchKeys_Init(TinyTimerModule_t *timerModule);

/*!
 * @return
 */
bool CapTouchKeys_BaselineHasDriftedOutOfBounds(void);

#endif
