/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "Action_Stm32Watchdog.h"
#include "stm32f3xx.h"
#include "utils.h"

static struct
{
   I_Action_t interface;
   IWDG_HandleTypeDef peripheral;
} watchdog;

// Prescales the Watchdog timer (IWDGCLK) by the following equation
// WDT Clock = IWDGCLK/Prescaler
// WDT Clock = 40KHz/16 = 2500Hz => 400uS period.
#define WATCHDOG_PRESCALER (IWDG_PRESCALER_16)

// Counter = (Desired Timeout) / (Watchdog Clock Period) = 3000ms/400uS = 7500
#define WATCHDOG_COUNTER_VALUE (7500)

static void Invoke(I_Action_t *instance)
{
   IGNORE_ARG(instance);
   HAL_IWDG_Refresh(&watchdog.peripheral);
}

static const I_Action_Api_t api =
   {
      Invoke
   };

I_Action_t *Action_Stm32Watchdog_Init(void)
{
   watchdog.peripheral.Instance = IWDG;
   watchdog.peripheral.Init.Prescaler = WATCHDOG_PRESCALER;
   watchdog.peripheral.Init.Window = WATCHDOG_COUNTER_VALUE;
   watchdog.peripheral.Init.Reload = WATCHDOG_COUNTER_VALUE;
   if(HAL_IWDG_Init(&watchdog.peripheral) != HAL_OK)
   {
      HAL_NVIC_SystemReset();
   }

   watchdog.interface.api = &api;
   return &watchdog.interface;
}
