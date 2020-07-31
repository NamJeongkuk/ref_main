/*!
 * @file
 * @brief Determines the reset source
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

#include "Stm32F3xxResetSource.h"
#include "stm32f3xx.h"

Stm32F3xxResetSource_t Stm32F3xxResetSource_GetResetSource(void)
{
   Stm32F3xxResetSource_t resetSource = Stm32F3xxResetSource_Unknown;

   if(__HAL_RCC_GET_FLAG(RCC_FLAG_LPWRRST))
   {
      resetSource = Stm32F3xxResetSource_LowPower;
   }
   else if(__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST))
   {
      resetSource = Stm32F3xxResetSource_WindowWatchdog;
   }
   else if(__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST))
   {
      resetSource = Stm32F3xxResetSource_IndependentWindowWatchdog;
   }
   else if(__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST))
   {
      resetSource = Stm32F3xxResetSource_Software;
   }
   else if(__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST))
   {
      resetSource = Stm32F3xxResetSource_PowerOn;
   }
   else if(__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST))
   {
      resetSource = Stm32F3xxResetSource_NrstPin;
   }
   else if(__HAL_RCC_GET_FLAG(RCC_FLAG_OBLRST))
   {
      resetSource = Stm32F3xxResetSource_OptionByteLoader;
   }

   return resetSource;
}
