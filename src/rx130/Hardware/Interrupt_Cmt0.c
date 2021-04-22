/*!
 * @file
 * @brief Contains CMT0 system timer initialization and wrapper calls
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

#include <stdint.h>

#include "Interrupt_Cmt0.h"
#include "Event_SingleSubscriberSynchronous.h"
#include "I_Interrupt.h"
#include "iodefine.h"
#include "SystemClock.h"
#include "utils.h"

#define U8_CMT_MODULE_STOP_CANCELED ((uint8_t)0)
#define U8_CMT_MODULE_STOP_ENABLED ((uint8_t)1)

#define U8_PCLK_DIV_BY_8 ((uint8_t)0)
#define U8_PCLK_DIV_BY_32 ((uint8_t)1)
#define U8_PCLK_DIV_BY_128 ((uint8_t)2)
#define U8_PCLK_DIV_BY_512 ((uint8_t)3)

#define U8_COMP_MATCH_INT_ENABLED ((uint8_t)1)
#define U8_COMP_MATCH_INT_DISABLED ((uint8_t)0)
#define U8_CMT0_INT_ENABLE ((uint8_t)1)
#define U8_CMT0_INT_DISABLE ((uint8_t)0)

// Timer period, given PCLK = ICLK and timer clock source = PCLK/8
#define U32_CMT_0_TIMER_PERIOD ((uint32_t)((U32_PCLKB / 8) / 1000))
#define U16_RESET_CMCNT ((uint16_t)0x0000)
#define U8_INTERRUPT_PRIORITY_6 ((uint8_t)6)
#define U8_CMT_START ((uint8_t)1)

#define U16_PRCR_ENABLE ((uint16_t)0xA50F)
#define U16_PRCR_DISABLE ((uint16_t)0xA500)

typedef struct
{
   I_Interrupt_t interrupt;
   Event_SingleSubscriberSynchronous_t onInterrupt;
} Cmt0Interrupt_t;

static Cmt0Interrupt_t cmt0Interrupt;

/*!
 * Interrupt service routine for CMT0 - system tick
 */
void CMT0_CMI0(void) __attribute__((interrupt));
void CMT0_CMI0(void)
{
   Event_SingleSubscriberSynchronous_Publish(&cmt0Interrupt.onInterrupt, NULL);
}

I_Interrupt_t *Interrupt_Cmt0_Init(void)
{
   // PRCR write enabled
   SYSTEM.PRCR.WORD = U16_PRCR_ENABLE;
   // Take CMT0 out of stop mode
   SYSTEM.MSTPCRA.BIT.MSTPA15 = 0;
   // Lock PRCR again
   SYSTEM.PRCR.WORD = U16_PRCR_DISABLE;
   // Set input signal to be PCLK divided by 8
   CMT0.CMCR.BIT.CKS = U8_PCLK_DIV_BY_8;
   // Turn on Compare Match Interrupts
   CMT0.CMCR.BIT.CMIE = U8_COMP_MATCH_INT_ENABLED;
   //Enable CMT0 Interrupt Request
   IEN(CMT0, CMI0) = U8_CMT0_INT_ENABLE;
   //Set interrupt priority
   IPR(CMT0, CMI0) = U8_INTERRUPT_PRIORITY_6;
   // Set Compare Match Constant Register - this is the number of cycles between interrupts
   // PCLK is 32MHz, divided by 8 it is 4MHz so dividing by 4000 gives an accurate 1mS tick
   CMT0.CMCOR = U32_CMT_0_TIMER_PERIOD;
   // Reset Counter - it counts up until it hits the match constant register.
   CMT0.CMCNT = U16_RESET_CMCNT;
   CMT.CMSTR0.BIT.STR0 = U8_CMT_START;

   Event_SingleSubscriberSynchronous_Init(&cmt0Interrupt.onInterrupt);
   cmt0Interrupt.interrupt.OnInterrupt = &cmt0Interrupt.onInterrupt.interface;

   return &cmt0Interrupt.interrupt;
}
