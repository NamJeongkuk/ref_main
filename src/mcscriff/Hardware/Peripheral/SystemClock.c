/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SystemClock.h"
#include "stm32f3xx.h"

void SystemClock_Init(void)
{
   HAL_RCC_DeInit();

   __HAL_RCC_SYSCFG_CLK_ENABLE();
   __HAL_RCC_PWR_CLK_ENABLE();

   RCC_OscInitTypeDef oscInitStructure = { 0 };
   RCC_ClkInitTypeDef clockInitStructure = { 0 };
   RCC_PeriphCLKInitTypeDef peripheralClockInitStructure = { 0 };

   oscInitStructure.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSI;
   oscInitStructure.HSIState = RCC_HSI_ON;
   oscInitStructure.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
   oscInitStructure.LSIState = RCC_LSI_ON;
   oscInitStructure.PLL.PLLState = RCC_PLL_ON;
   oscInitStructure.PLL.PLLSource = RCC_PLLSOURCE_HSI;
   oscInitStructure.PLL.PLLMUL = RCC_PLL_MUL16;
   if(HAL_RCC_OscConfig(&oscInitStructure) != HAL_OK)
   {
      HAL_NVIC_SystemReset();
   }
   clockInitStructure.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
   clockInitStructure.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
   clockInitStructure.AHBCLKDivider = RCC_SYSCLK_DIV1;
   clockInitStructure.APB1CLKDivider = RCC_HCLK_DIV2;
   clockInitStructure.APB2CLKDivider = RCC_HCLK_DIV1;

   if(HAL_RCC_ClockConfig(&clockInitStructure, FLASH_LATENCY_2) != HAL_OK)
   {
      HAL_NVIC_SystemReset();
   }
   peripheralClockInitStructure.PeriphClockSelection = RCC_PERIPHCLK_USART1;
   peripheralClockInitStructure.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
   if(HAL_RCCEx_PeriphCLKConfig(&peripheralClockInitStructure) != HAL_OK)
   {
      HAL_NVIC_SystemReset();
   }
}
