/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SystemClock.h"
#include "stm32f3xx_ll_rcc.h"
#include "stm32f3xx_ll_system.h"

void SystemClock_Init(void)
{
   LL_FLASH_EnablePrefetch();

   // 48-72 MHz requires 2 wait states
   LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);

   LL_RCC_HSI_Enable();
   while(!LL_RCC_HSI_IsReady())
   {
   };

   LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI_DIV_2, LL_RCC_PLL_MUL_16);

   LL_RCC_PLL_Enable();
   while(!LL_RCC_PLL_IsReady())
   {
   };

   LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
   LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
   while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
   {
   };

   LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
   LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

   SystemCoreClockUpdate();
}
