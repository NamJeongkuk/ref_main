/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef MICROSYSTEMDATA_H
#define MICROSYSTEMDATA_H

#include "I_TinyDataSource.h"
#include "TinyTimer.h"

/*!
 * @param timerModule
 */
void MicroSystemData_Init(TinyTimerModule_t *timerModule);

/*!
 * @return
 */
I_TinyDataSource_t *MicroSystemData_DataSource(void);

/*!
 * @return
 */
I_TinyDataSource_t *MicroSystemData_ExternalDataSource(void);

#endif
