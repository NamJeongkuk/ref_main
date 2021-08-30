/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <stdint.h>
#include "iodefine.h"
#include "TinyTimeSource_Cmt0.h"
#include "Clock.h"
#include "utils.h"

STATIC_ASSERT(Clock_PclkFrequencyMhz == 50);

static volatile TinyTimeSourceTickCount_t ticks;

static TinyTimeSourceTickCount_t GetTicks(I_TinyTimeSource_t *instance)
{
   IGNORE(instance);
   TinyTimeSourceTickCount_t last;
   TinyTimeSourceTickCount_t current;

   current = ticks;

   // Note: this provides interrupt-safety and means that we do not ever have to disable interrupts.
   // Keep reading the tick count until it reads the same two times in a row so that we know the value
   // is correct.
   // Unless the interrupt is coming so fast that the few cycles of the loop don't guarantee that we
   // can do two reads without the ISR coming this will work.  If the interrupt comes so fast that we
   // can't execute a few cycles in between invocations then we have a bigger problem.
   do
   {
      last = current;
      current = ticks;
   } while(last != current);

   return current;
}

void CMT0_CMI0(void) __attribute__((interrupt));
void CMT0_CMI0(void)
{
   ticks++;
}

static const I_TinyTimeSource_Api_t api = { GetTicks };

I_TinyTimeSource_t *TinyTimeSource_Cmt0_Init(void)
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

   static I_TinyTimeSource_t instance = { &api };
   return &instance;
}
