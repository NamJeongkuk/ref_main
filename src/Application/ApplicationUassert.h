/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef APPLICATIONUASSERT_H
#define APPLICATIONUASSERT_H

#include "I_Output.h"
#include "I_Action.h"
#include "Timer.h"

/*!
 * @param resetAction
 * @param programCounterAddressOutput
 * @param timerModule
 */
void ApplicationUassert_Init(
   I_Action_t *resetAction,
   I_Output_t *programCounterAddressOutput,
   TimerModule_t *timerModule);

#endif
