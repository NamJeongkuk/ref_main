/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef MICROSYSTEMDATA_H
#define MICROSYSTEMDATA_H

#include "TinyDataSource_Ram.h"
#include "TinyTimer.h"
#include "MicroSystemErds.h"

/*!
 * @param timerModule
 */
void MicroSystemData_Init(TinyTimerModule_t *timerModule);

/*!
 * @return
 */
I_TinyDataSource_t *MicroSystemData_DataSource(void);

#endif
