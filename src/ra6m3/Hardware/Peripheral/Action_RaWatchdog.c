/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

#include "Action_RaWatchdog.h"
#include "utils.h"
#include "r_iwdt.h"

// IWDTCLK = 15 kHz; Timeout =~ 1.09 sec
static const wdt_cfg_t watchdogConfig = {
   .timeout = WDT_TIMEOUT_16384,
   .clock_division = WDT_CLOCK_DIVISION_1,
   .window_start = WDT_WINDOW_START_100,
   .window_end = WDT_WINDOW_END_0,
   .reset_control = WDT_RESET_CONTROL_RESET,
   .stop_control = WDT_STOP_CONTROL_ENABLE,
   .p_callback = NULL,
   .p_context = NULL,
   .p_extend = NULL
};

static iwdt_instance_ctrl_t watchdogControl;

static void Invoke(I_Action_t *instance)
{
   IGNORE(instance);
   R_IWDT_Refresh(&watchdogControl);
}

static const I_Action_Api_t api = { Invoke };

I_Action_t *Action_RaWatchdog_Init(void)
{
   R_IWDT_Open(&watchdogControl, &watchdogConfig);

   static I_Action_t instance;
   instance.api = &api;
   return &instance;
}
