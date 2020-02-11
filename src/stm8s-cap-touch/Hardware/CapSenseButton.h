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
#include "stm8_tsl_api.h"

/*!
 * @param dataSource
 * @param timerModule
 * @param buttonErd
 */
void CapSenseButton_Init(TinyTimerModule_t *timerModule);

#endif
