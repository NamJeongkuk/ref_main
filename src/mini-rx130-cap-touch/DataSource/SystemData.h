/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SYSTEMDATA_H
#define SYSTEMDATA_H

#include "I_DataSource.h"
#include "Timer.h"

/*!
 * @param timerModule
 */
void SystemData_Init(TimerModule_t *timerModule);

/*!
 * @return
 */
I_DataSource_t *SystemData_DataSource(void);

/*!
 * @return
 */
I_DataSource_t *SystemData_ExternalDataSource(void);

#endif
