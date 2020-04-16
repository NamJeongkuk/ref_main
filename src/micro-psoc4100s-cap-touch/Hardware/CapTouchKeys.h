/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef CAPTOUCHKEYS_H
#define CAPTOUCHKEYS_H

#include "TinyTimer.h"
#include "I_TinyDataSource.h"

/*!
 * @param dataSource
 * @param timerModule
 * @param capTouchKeysErd
 */
void CapTouchKeys_Init(I_TinyDataSource_t *dataSource, TinyTimerModule_t *timerModule, Erd_t capTouchKeysErd);

/*!
 * @return
 */
bool CapTouchKeys_BaselineHasDriftedOutOfBounds(void);

#endif
