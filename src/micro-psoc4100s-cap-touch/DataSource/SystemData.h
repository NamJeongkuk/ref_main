/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SYSTEMDATA_H
#define SYSTEMDATA_H

#include "I_TinyDataSource.h"
#include "TinyTimer.h"

/*!
 * @param timerModule
 */
void SystemData_Init(TinyTimerModule_t *timerModule);

/*!
 * @return
 */
I_TinyDataSource_t *SystemData_DataSource(void);

/*!
 * @return
 */
I_TinyDataSource_t *SystemData_ExternalDataSource(void);

#endif
