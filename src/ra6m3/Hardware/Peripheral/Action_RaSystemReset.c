/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "Action_RaSystemReset.h"
#include "utils.h"
#include "bsp_api.h"

static void Invoke(I_Action_t *instance)
{
   IGNORE(instance);
   NVIC_SystemReset();
}

static const I_Action_Api_t api = { Invoke };
static I_Action_t instance;

I_Action_t *Action_RaSystemReset_Init(void)
{
   instance.api = &api;
   return &instance;
}
