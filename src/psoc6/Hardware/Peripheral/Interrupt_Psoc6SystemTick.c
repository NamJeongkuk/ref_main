/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "Interrupt_Psoc6SystemTick.h"
#include "Event_SingleSubscriberSynchronous.h"
#include "uassert.h"
#include "utils.h"
#include "cy_pdl.h"

struct
{
   I_Interrupt_t interrupt;
   Event_SingleSubscriberSynchronous_t interruptEvent;
} instance;

static void Tick(void)
{
   Event_SingleSubscriberSynchronous_Publish(&instance.interruptEvent, NULL);
}

I_Interrupt_t *Interrupt_Psoc6SystemTick_Init(void)
{
   Event_SingleSubscriberSynchronous_Init(&instance.interruptEvent);
   instance.interrupt.OnInterrupt = &instance.interruptEvent.interface;

   Cy_SysTick_Init(CY_SYSTICK_CLOCK_SOURCE_CLK_CPU, SystemCoreClock / 1000);
   Cy_SysTick_SetCallback(0, Tick);

   return &instance.interrupt;
}
