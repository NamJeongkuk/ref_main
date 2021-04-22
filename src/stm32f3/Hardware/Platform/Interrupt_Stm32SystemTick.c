/*!
 * @file
 * @brief System tick initialization
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "stm32f3xx.h"
#include "Event_SingleSubscriberSynchronous.h"
#include "Interrupt_Stm32SystemTick.h"

typedef struct
{
   I_Interrupt_t interrupt;
   Event_SingleSubscriberSynchronous_t interruptEvent;
} Interrupt_Stm32SystemTick_t;

static Interrupt_Stm32SystemTick_t instance;

void SysTick_Handler(void)
{
   Event_SingleSubscriberSynchronous_Publish(&instance.interruptEvent, NULL);
}

I_Interrupt_t *Interrupt_Stm32SystemTick_Init(void)
{
   Event_SingleSubscriberSynchronous_Init(&instance.interruptEvent);
   instance.interrupt.OnInterrupt = &instance.interruptEvent.interface;

   // Set the counter value for a 1ms system tick
   if(SysTick_Config(SystemCoreClock / 1000))
   {
      HAL_NVIC_SystemReset();
   }

   NVIC_SetPriority(SysTick_IRQn, 7);

   return &instance.interrupt;
}
