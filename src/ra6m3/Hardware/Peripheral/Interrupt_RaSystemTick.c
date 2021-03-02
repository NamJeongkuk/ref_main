/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "Interrupt_RaSystemTick.h"
#include "Event_SingleSubscriberSynchronous.h"
#include "uassert.h"
#include "utils.h"
#include "bsp_api.h"

struct
{
   I_Interrupt_t interrupt;
   Event_SingleSubscriberSynchronous_t interruptEvent;
} instance;

void SysTick_Handler(void)
{
   Event_SingleSubscriberSynchronous_Publish(&instance.interruptEvent, NULL);
}

I_Interrupt_t *Interrupt_RaSystemTick_Init(void)
{
   Event_SingleSubscriberSynchronous_Init(&instance.interruptEvent);
   instance.interrupt.OnInterrupt = &instance.interruptEvent.interface;

   SysTick_Config(R_FSP_SystemClockHzGet(FSP_PRIV_CLOCK_ICLK) / 1000);

   return &instance.interrupt;
}
