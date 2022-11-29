/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

#ifndef EEPROMSTACK_H
#define EEPROMSTACK_H

#include "I_Action.h"
#include "I_Eeprom.h"
#include "I_Input.h"
#include "Timer.h"
#include "TimeSource_Rockhopper.h"

/*!
 * @param watchdogKickAction
 */
void EepromStack_Init(
   I_Action_t *watchdogKickAction,
   TimerModule_t *timerModule,
   TimeSource_Interrupt_t *timeSourceInterrupt);

/*!
 * Returns I_Eeprom_t interfaces
 * @return eeprom interface
 */
I_Eeprom_t *EepromStack_GetEeprom(void);

/*!
 * Returns I_Input_t of EepromReadFault
 * @return Input of readFaultStorage
 */
I_Input_t *EepromStack_GetReadFaultInput(void);

/*!
 * Returns I_Input_t for EepromWriteFault
 * @return Input of writeFaultStorage
 */
I_Input_t *EepromStack_GetWriteFaultInput(void);

/*!
 * Returns I_Input_t for EepromEraseFault
 * @return Input of eraseFaultStorage
 */
I_Input_t *EepromStack_GetEraseFaultInput(void);

#endif