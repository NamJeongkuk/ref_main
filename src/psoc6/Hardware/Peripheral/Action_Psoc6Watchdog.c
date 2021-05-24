/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

#include "Action_Psoc6Watchdog.h"
#include "cy_pdl.h"
#include "utils.h"

static void Invoke(I_Action_t *instance)
{
   IGNORE(instance);
   Cy_WDT_ClearWatchdog();
}

static const I_Action_Api_t api = { Invoke };

I_Action_t *Action_Psoc6Watchdog_Init(void)
{
   Cy_WDT_Unlock();
   Cy_WDT_Disable();
   Cy_WDT_Init();
   Cy_WDT_SetMatch(0x8000);
   Cy_WDT_Lock();
   Cy_WDT_Enable();

   static I_Action_t instance;
   instance.api = &api;
   return &instance;
}
