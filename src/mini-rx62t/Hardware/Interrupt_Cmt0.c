/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "iodefine.h"
#include "I_Interrupt.h"
#include "Interrupt_Cmt0.h"
#include "Event_SingleSubscriberSynchronous.h"
#include "Clock.h"
#include "utils.h"

STATIC_ASSERT(Clock_PclkFrequencyMhz == 50);

static Event_SingleSubscriberSynchronous_t interrupt;

void CMT0_CMI0(void) __attribute__((interrupt));
void CMT0_CMI0(void)
{
   Event_SingleSubscriberSynchronous_Publish(&interrupt, NULL);
}

I_Interrupt_t *Interrupt_Cmt0_Init(void)
{
   // Take CMT0 out of stop mode
   SYSTEM.MSTPCRA.BIT.MSTPA15 = 0;

   // Use PCLK / 8
   CMT0.CMCR.BIT.CKS = 0;

   // Enable compare match interrupt
   CMT0.CMCR.BIT.CMIE = 1;

   // Enable CMT0 interrupt
   IEN(CMT0, CMI0) = 1;

   // Interrupt priority 6
   IPR(CMT0, CMI0) = 6;

   // Frequency = (PCLK / PCLK divisor) / CMCOR
   // 1 KHz = (50 MHz / 8) / 6250
   CMT0.CMCOR = 6250;

   // Start CMT0
   CMT.CMSTR0.BIT.STR0 = 1;

   Event_SingleSubscriberSynchronous_Init(&interrupt);

   static I_Interrupt_t instance = { &interrupt.interface };
   return &instance;
}
