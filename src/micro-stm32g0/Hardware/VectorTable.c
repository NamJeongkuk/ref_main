/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <stdint.h>
#include "stm32g0xx.h"

#pragma GCC optimize("O0")

void Default_Handler(void); /* Default empty handler */
void Reset_Handler(void); /* Reset Handler */

// Cortex-M0+
void NMI_Handler(void) __attribute__((weak, alias("Default_Handler")));
void HardFault_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SVC_Handler(void) __attribute__((weak, alias("Default_Handler")));
void PendSV_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SysTick_Handler(void) __attribute__((weak, alias("Default_Handler")));

// Peripherals
void WWDG_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void RTC_TAMP_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void FLASH_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void RCC_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void EXTI0_1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void EXTI2_3_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void EXTI4_15_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Channel1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Channel2_3_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Ch4_7_DMAMUX1_OVR_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void ADC1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIM1_BRK_UP_TRG_COM_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIM1_CC_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIM3_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIM6_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIM7_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIM14_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIM15_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIM16_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIM17_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void I2C1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void I2C2_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void SPI1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void SPI2_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void USART1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void USART2_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void USART3_4_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));

extern char __StackBottom;

typedef void (*Vector_t)(void);

const Vector_t __Vectors[] __attribute__((section(".vectors"))) = {
   // Cortex-M0+
   (Vector_t)&__StackBottom, // Initial Stack Pointer
   Reset_Handler,
   NMI_Handler,
   HardFault_Handler,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   SVC_Handler,
   0,
   0,
   PendSV_Handler,
   SysTick_Handler,

   // Peripherals
   WWDG_IRQHandler,
   0,
   RTC_TAMP_IRQHandler,
   FLASH_IRQHandler,
   RCC_IRQHandler,
   EXTI0_1_IRQHandler,
   EXTI2_3_IRQHandler,
   EXTI4_15_IRQHandler,
   0,
   DMA1_Channel1_IRQHandler,
   DMA1_Channel2_3_IRQHandler,
   DMA1_Ch4_7_DMAMUX1_OVR_IRQHandler,
   ADC1_IRQHandler,
   TIM1_BRK_UP_TRG_COM_IRQHandler,
   TIM1_CC_IRQHandler,
   0,
   TIM3_IRQHandler,
   TIM6_IRQHandler,
   TIM7_IRQHandler,
   TIM14_IRQHandler,
   TIM15_IRQHandler,
   TIM16_IRQHandler,
   TIM17_IRQHandler,
   I2C1_IRQHandler,
   I2C2_IRQHandler,
   SPI1_IRQHandler,
   SPI2_IRQHandler,
   USART1_IRQHandler,
   USART2_IRQHandler,
   USART3_4_IRQHandler
};

void Default_Handler(void)
{
   while(1)
   {
   }
}
