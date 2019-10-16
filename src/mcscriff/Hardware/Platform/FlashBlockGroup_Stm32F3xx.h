/*!
 * @file
 * @brief Implementation of a FlashBlockGroup for the STM32F3xx
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FLASHBLOCKGROUP_STM32F3XX
#define FLASHBLOCKGROUP_STM32F3XX

#include "I_FlashBlockGroup.h"
#include "I_Action.h"
#include "I_ContextProtector.h"

/*!
 * Initialize the STM32F3xx FlashBlockGroup
 * @param flashBlockBlankAction
 * @param onErrorAction
 * @param blockAddresses
 * @return
 */
I_FlashBlockGroup_t *FlashBlockGroup_Stm32F3xx_Init(
   I_Action_t *flashBlockBlankAction,
   I_Action_t *onErrorAction,
   const uint32_t *blockAddresses);

#endif
