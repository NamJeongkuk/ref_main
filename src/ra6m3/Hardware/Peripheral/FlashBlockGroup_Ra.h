/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

#ifndef FLASHBLOCKGROUP_RA_H
#define FLASHBLOCKGROUP_RA_H

#include "I_FlashBlockGroup.h"
#include "I_Action.h"
#include "Timer.h"

/*!
 * @param flashBlockBlankAction
 * @param onErrorAction
 * @param timerModule
 * @return
 */
I_FlashBlockGroup_t *FlashBlockGroup_Ra_Init(
   I_Action_t *flashBlockBlankAction,
   I_Action_t *onErrorAction,
   TimerModule_t *timerModule);

#endif
