/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DATASOURCE_BSP_H
#define DATASOURCE_BSP_H

#include "I_DataSource.h"
#include "Timer.h"

/*!
 * @param timerModule
 * @return
 */
I_DataSource_t *DataSource_Bsp_Init(TimerModule_t *timerModule);

#endif
