/*!
 * @file
 * @brief System tick initialization
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "stm32f3xx.h"
#include "Event_Simple.h"
#include "Interrupt_Stm32SystemTick.h"

static struct
{
   I_Interrupt_t interrupt;
   Event_Simple_t OnInterrupt;
} instance;

void SysTick_Handler(void)
{
   Event_Simple_Publish(&instance.OnInterrupt, NULL);
}

I_Interrupt_t *Interrupt_Stm32SystemTick_Init(void)
{
   instance.interrupt.OnInterrupt = &instance.OnInterrupt.interface;
   Event_Simple_Init(&instance.OnInterrupt);

   // Set the counter value for a 1ms system tick
   if(SysTick_Config(SystemCoreClock / 1000))
   {
      HAL_NVIC_SystemReset();
   }

   NVIC_SetPriority(SysTick_IRQn, 7);

   return &instance.interrupt;
}
