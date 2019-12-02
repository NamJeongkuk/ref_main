/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "Action_Stm32SystemReset.h"
#include "stm32f3xx.h"
#include "utils.h"
#include <stdint.h>

static void Invoke(I_Action_t *instance)
{
   IGNORE(instance);

   HAL_NVIC_SystemReset();
}

static const I_Action_Api_t api =
   { Invoke };

static I_Action_t instance;

I_Action_t *Action_Stm32SystemReset_Init(void)
{
   instance.api = &api;
   return &instance;
}
