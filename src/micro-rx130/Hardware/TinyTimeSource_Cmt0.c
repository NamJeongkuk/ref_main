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

enum
{
   Prcr_Enable = 0xA50F,
   Prcr_Disable = 0xA500
};

#define U8_CMT_MODULE_STOP_CANCELED        ((uint8_t)0)
#define U8_CMT_MODULE_STOP_ENABLED         ((uint8_t)1)

#define U8_PCLK_DIV_BY_8                   ((uint8_t)0)
#define U8_PCLK_DIV_BY_32                  ((uint8_t)1)
#define U8_PCLK_DIV_BY_128                 ((uint8_t)2)
#define U8_PCLK_DIV_BY_512                 ((uint8_t)3)

#define U8_COMP_MATCH_INT_ENABLED          ((uint8_t)1)
#define U8_COMP_MATCH_INT_DISABLED         ((uint8_t)0)
#define U8_CMT0_INT_ENABLE                 ((uint8_t)1)
#define U8_CMT0_INT_DISABLE                ((uint8_t)0)

// We're using the PCLK / 8 setting, and we want a tick every 1000 uS
#define U32_CMT_0_TIMER_PERIOD             ((uint32_t)(PCLOCK_FREQUENCY / 8) / 1000)

#define U16_RESET_CMCNT                    ((uint16_t)0x0000)

#define U8_INTERRUPT_PRIORITY_6            ((uint8_t)6)

#define U8_CMT_START                       ((uint8_t)1)

static I_TinyTimeSource_t timeSource;
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

void CMT0_CMI0(void) __attribute__ ((interrupt));
void CMT0_CMI0(void)
{
   ticks++;
}

static const I_TinyTimeSource_Api_t api = { GetTicks };

I_TinyTimeSource_t * TinyTimeSource_Cmt0_Init(void)
{
   // PRCR write enabled
   SYSTEM .PRCR.WORD = Prcr_Enable;
   // Take CMT0 out of stop mode
   SYSTEM .MSTPCRA.BIT.MSTPA15 = 0;
   // Lock PRCR again
   SYSTEM .PRCR.WORD = Prcr_Disable;
   // Set input signal to be PCLK divided by 8
   CMT0 .CMCR.BIT.CKS = U8_PCLK_DIV_BY_8;
   // Turn on Compare Match Interrupts
   CMT0 .CMCR.BIT.CMIE = U8_COMP_MATCH_INT_ENABLED;
   //Enable CMT0 Interrupt Request
   IEN(CMT0, CMI0) = U8_CMT0_INT_ENABLE;
   //Set interrupt priority
   IPR(CMT0, CMI0) = U8_INTERRUPT_PRIORITY_6;
   // Set Compare Match Constant Register - this is the number of cycles between interrupts
   // PCLK is 27MHz, divided by 8 it is 3.375MHz so dividing by 3375 gives an accurate 1mS tick
   CMT0 .CMCOR = U32_CMT_0_TIMER_PERIOD;
   // Reset Counter - it counts up until it hits the match constant register.
   CMT0 .CMCNT = U16_RESET_CMCNT;
   //Start CMT0
   CMT .CMSTR0.BIT.STR0 = U8_CMT_START;

   timeSource.api = &api;

   return &timeSource;
}
