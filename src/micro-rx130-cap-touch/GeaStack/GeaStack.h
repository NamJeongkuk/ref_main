/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef GEASTACK_H
#define GEASTACK_H

#include "I_TinyDataSource.h"
#include "I_TinyUart.h"
#include "I_TinyGea2Interface.h"
#include "TinyTimer.h"

/*!
 * @param timerModule
 * @param dataSource
 * @param uart
 * @param geaAddress
 */
void GeaStack_Init(
   TinyTimerModule_t *timerModule,
   I_TinyDataSource_t *dataSource,
   I_TinyUart_t *uart,
   uint8_t geaAddress);

/*!
 * @return
 */
I_TinyGea2Interface_t *GeaStack_Gea2Interface(void);

/*!
 */
void GeaStack_Run(void);

#endif
