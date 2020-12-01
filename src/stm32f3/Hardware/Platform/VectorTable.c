/* File: startup_ARMCM4.c
 * Purpose: startup file for Cortex-M4 devices.
 *          Should be used with GCC 'GNU Tools ARM Embedded'
 * Version: V1.01
 * Date: 12 June 2014
 *
 */
/* Copyright (c) 2011 - 2014 ARM LIMITED

   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of ARM nor the names of its contributors may be used
     to endorse or promote products derived from this software without
     specific prior written permission.
   *
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
   ---------------------------------------------------------------------------*/

#include <stdint.h>
#include "stm32f3xx.h"

#pragma GCC optimize("O0")

typedef void (*pFunc)(void);

void Default_Handler(void); /* Default empty handler */
void Reset_Handler(void); /* Reset Handler */

// clang-format off
/* Cortex-M4 Processor Exceptions */
void NMI_Handler         (void) __attribute__ ((weak, alias("Default_Handler")));
void HardFault_Handler   (void) __attribute__ ((weak, alias("Default_Handler")));
void MemManage_Handler   (void) __attribute__ ((weak, alias("Default_Handler")));
void BusFault_Handler    (void) __attribute__ ((weak, alias("Default_Handler")));
void UsageFault_Handler  (void) __attribute__ ((weak, alias("Default_Handler")));
void SVC_Handler         (void) __attribute__ ((weak, alias("Default_Handler")));
void DebugMon_Handler    (void) __attribute__ ((weak, alias("Default_Handler")));
void PendSV_Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void SysTick_Handler     (void) __attribute__ ((weak, alias("Default_Handler")));

/* ARMCM4 Specific Interrupts */
void WWDG_IRQHandler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void PVD_IRQHandler                    (void) __attribute__ ((weak, alias("Default_Handler")));
void TAMPER_STAMP_IRQHandler           (void) __attribute__ ((weak, alias("Default_Handler")));
void RTC_WKUP_IRQHandler               (void) __attribute__ ((weak, alias("Default_Handler")));
void FLASH_IRQHandler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void RCC_IRQHandler                    (void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI0_IRQHandler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI1_IRQHandler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI2_IRQHandler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI3_IRQHandler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI4_IRQHandler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_Channel1_IRQHandler          (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_Channel2_IRQHandler          (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_Channel3_IRQHandler          (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_Channel4_IRQHandler          (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_Channel5_IRQHandler          (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_Channel6_IRQHandler          (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_Channel7_IRQHandler          (void) __attribute__ ((weak, alias("Default_Handler")));
void ADC1_2_IRQHandler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void USB_HP_CAN1_TX_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void USB_LP_CAN1_RX0_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void CAN1_RX1_IRQHandler               (void) __attribute__ ((weak, alias("Default_Handler")));
void CAN1_SCE_IRQHandler               (void) __attribute__ ((weak, alias("Default_Handler")));
void ExtI_Group9To5_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM1_BRK_TIM15_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM1_UP_TIM16_IRQHandler          (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM1_TRG_COM_TIM17_IRQHandler     (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM1_CC_IRQHandler                (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM2_IRQHandler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM3_IRQHandler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM4_IRQHandler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void I2C1_EV_IRQHandler                (void) __attribute__ ((weak, alias("Default_Handler")));
void I2C1_ER_IRQHandler                (void) __attribute__ ((weak, alias("Default_Handler")));
void I2C2_EV_IRQHandler                (void) __attribute__ ((weak, alias("Default_Handler")));
void I2C2_ER_IRQHandler                (void) __attribute__ ((weak, alias("Default_Handler")));
void SPI1_IRQHandler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void SPI2_IRQHandler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void USART1_IRQHandler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void USART2_IRQHandler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void USART3_IRQHandler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void ExtI_Group15To10_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void RTC_Alarm_IRQHandler              (void) __attribute__ ((weak, alias("Default_Handler")));
void USBWakeUp_IRQHandler              (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM8_BRK_IRQHandler               (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM8_UP_IRQHandler                (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM8_TRG_COM_IRQHandler           (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM8_CC_IRQHandler                (void) __attribute__ ((weak, alias("Default_Handler")));
void ADC3_IRQHandler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void SPI3_IRQHandler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void UART4_IRQHandler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void UART5_IRQHandler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM6_DAC_IRQHandler               (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM7_IRQHandler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA2_Channel1_IRQHandler          (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA2_Channel2_IRQHandler          (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA2_Channel3_IRQHandler          (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA2_Channel4_IRQHandler          (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA2_Channel5_IRQHandler          (void) __attribute__ ((weak, alias("Default_Handler")));
void ADC4_IRQHandler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void COMP1_2_3_IRQHandler              (void) __attribute__ ((weak, alias("Default_Handler")));
void COMP4_5_6_IRQHandler              (void) __attribute__ ((weak, alias("Default_Handler")));
void COMP7_IRQHandler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void USB_HP_IRQHandler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void USB_LP_IRQHandler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void FPU_IRQHandler                    (void) __attribute__ ((weak, alias("Default_Handler")));

extern char __StackBottom;

const pFunc __Vectors[] __attribute__ ((section(".vectors"))) = {
  /* Cortex-M4 Exceptions Handler */
  (pFunc)&__StackBottom,             // Initial Stack Pointer
  Reset_Handler,                     // Reset Handler
  NMI_Handler,                       // NMI Handler
  HardFault_Handler,                 // Hard Fault Handler
  MemManage_Handler,                 // MPU Fault Handler
  BusFault_Handler,                  // Bus Fault Handler
  UsageFault_Handler,                // Usage Fault Handler
  0,                                 // Reserved
  0,                                 // Reserved
  0,                                 // Reserved
  0,                                 // Reserved
  SVC_Handler,                       // SVCall Handler
  DebugMon_Handler,                  // Debug Monitor Handler
  0,                                 // Reserved
  PendSV_Handler,                    // PendSV Handler
  SysTick_Handler,                   // SysTick Handler

  /* External interrupts */
  WWDG_IRQHandler,                   // 0 Window WatchDog
  PVD_IRQHandler,                    // 1 PVD through EXTI Line detection
  TAMPER_STAMP_IRQHandler,           // 2 Tamper and TimeStamps through the EXTI line
  RTC_WKUP_IRQHandler,               // 3 RTC Wakeup through the EXTI line
  FLASH_IRQHandler,                  // 4 FLASH
  RCC_IRQHandler,                    // 5 RCC
  EXTI0_IRQHandler,                  // 6 EXTI Line0
  EXTI1_IRQHandler,                  // 7 EXTI Line1
  EXTI2_IRQHandler,                  // 8 EXTI Line2
  EXTI3_IRQHandler,                  // 9 EXTI Line3
  EXTI4_IRQHandler,                  // 10 EXTI Line4
  DMA1_Channel1_IRQHandler,          // 11 DMA1 Channel 1
  DMA1_Channel2_IRQHandler,          // 12 DMA1 Channel 2
  DMA1_Channel3_IRQHandler,          // 13 DMA1 Channel 3
  DMA1_Channel4_IRQHandler,          // 14 DMA1 Channel 4
  DMA1_Channel5_IRQHandler,          // 15 DMA1 Channel 5
  DMA1_Channel6_IRQHandler,          // 16 DMA1 Channel 6
  DMA1_Channel7_IRQHandler,          // 17 DMA1 Channel 7
  ADC1_2_IRQHandler,                 // 18 ADC1 and ADC2
  USB_HP_CAN1_TX_IRQHandler,         // 19 USB Device High Priority or CAN1 TX
  USB_LP_CAN1_RX0_IRQHandler,        // 20 USB Device Low Priority or CAN1 RX0
  CAN1_RX1_IRQHandler,               // 21 CAN1 RX1
  CAN1_SCE_IRQHandler,               // 22 CAN1 SCE
  ExtI_Group9To5_IRQHandler,         // 23 External Lines[9:5]
  TIM1_BRK_TIM15_IRQHandler,         // 24 TIM1 Break and TIM15
  TIM1_UP_TIM16_IRQHandler,          // 25 TIM1 Update and TIM16
  TIM1_TRG_COM_TIM17_IRQHandler,     // 26 TIM1 Trigger and Commutation and TIM11
  TIM1_CC_IRQHandler,                // 27 TIM1 Capture Compare
  TIM2_IRQHandler,                   // 28 TIM2
  TIM3_IRQHandler,                   // 29 TIM3
  TIM4_IRQHandler,                   // 30 TIM4
  I2C1_EV_IRQHandler,                // 31 I2C1 Event
  I2C1_ER_IRQHandler,                // 32 I2C1 Error
  I2C2_EV_IRQHandler,                // 33 I2C2 Event
  I2C2_ER_IRQHandler,                // 34 I2C2 Error
  SPI1_IRQHandler,                   // 35 SPI1
  SPI2_IRQHandler,                   // 36 SPI2
  USART1_IRQHandler,                 // 37 USART1
  USART2_IRQHandler,                 // 38 USART2
  USART3_IRQHandler,                 // 39 USART3
  ExtI_Group15To10_IRQHandler,       // 40 External Lines[15:10]
  RTC_Alarm_IRQHandler,              // 41 RTC Alarm (A and B) through EXTI Line
  USBWakeUp_IRQHandler,              // 42 USB Wakeup through EXTI line
  TIM8_BRK_IRQHandler,               // 43 TIM8 Break
  TIM8_UP_IRQHandler,                // 44 TIM8 Update
  TIM8_TRG_COM_IRQHandler,           // 45 TIM8 Trigger and Commutation
  TIM8_CC_IRQHandler,                // 46 TIM8 Capture Compare
  ADC3_IRQHandler,                   // 47 ADC3
  0,                                 // 48 Reserved
  0,                                 // 49 Reserved
  0,                                 // 50 Reserved
  SPI3_IRQHandler,                   // 51 SPI3
  UART4_IRQHandler,                  // 52 UART4
  UART5_IRQHandler,                  // 53 UART5
  TIM6_DAC_IRQHandler,               // 54 TIM6 and DAC1&2 underrun errors
  TIM7_IRQHandler,                   // 55 TIM7
  DMA2_Channel1_IRQHandler,          // 56 DMA2 Channel 1
  DMA2_Channel2_IRQHandler,          // 57 DMA2 Channel 2
  DMA2_Channel3_IRQHandler,          // 58 DMA2 Channel 3
  DMA2_Channel4_IRQHandler,          // 59 DMA2 Channel 4
  DMA2_Channel5_IRQHandler,          // 60 DMA2 Channel 5
  ADC4_IRQHandler,                   // 61 ADC4
  0,                                 // 62 Reserved
  0,                                 // 63 Reserved
  COMP1_2_3_IRQHandler,              // 64 COMP1, COMP2 and COMP3
  COMP4_5_6_IRQHandler,              // 65 COMP4, COMP5 and COMP6
  COMP7_IRQHandler,                  // 66 COMP7
  0,                                 // 67 Reserved
  0,                                 // 68 Reserved
  0,                                 // 69 Reserved
  0,                                 // 70 Reserved
  0,                                 // 71 Reserved
  0,                                 // 72 Reserved
  0,                                 // 73 Reserved
  USB_HP_IRQHandler,                 // 74 USB High Priority remap
  USB_LP_IRQHandler,                 // 75 USB Low Priority remap
  USBWakeUp_RMP_IRQHandler,          // 76 USB Wakeup remap through EXTI
  0,                                 // 77 Reserved
  0,                                 // 78 Reserved
  0,                                 // 79 Reserved
  0,                                 // 80 Reserved
  FPU_IRQHandler,                    // 81 FPU
  0,                                 // 82 Reserved
  0,                                 // 83 Reserved
  0,                                 // 84 Reserved
};
// clang-format on

void Default_Handler(void)
{
   while(1)
   {
   }
}
