/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef BUTTON_H
#define BUTTON_H

#include "I_TinyDataSource.h"
#include "TinyTimer.h"

/*!
 * @param dataSource
 * @param timerModule
 * @param buttonErd
 */
void Button_Init(I_TinyDataSource_t *dataSource, TinyTimerModule_t *timerModule, Erd_t buttonErd);

#endif