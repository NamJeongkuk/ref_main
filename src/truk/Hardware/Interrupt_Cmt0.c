/*!
 * @file
 * @brief CMT0 setup
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <stdint.h>
#include "iodefine.h"
#include "I_Interrupt.h"
#include "Interrupt_Cmt0.h"
#include "Event_Synchronous.h"
#include "utils.h"
#include "InterruptPriorityLevel.h"
#include "SystemClock.h"
#include "Rx231Defines.h"

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

#define U32_CMT_0_TIMER_PERIOD ((uint32_t)((U32_PCLKB / 8000) - 1))

#define U16_RESET_CMCNT ((uint16_t)0x0000)

#define U8_CMT_START ((uint8_t)1)

typedef struct
{
   I_Interrupt_t interrupt;
   Event_Synchronous_t OnInterrupt;
} Cmt0Interrupt_t;

static Cmt0Interrupt_t cmt0Interrupt;

void CMT0_CMI0(void) __attribute__((interrupt));
void CMT0_CMI0(void)
{
   // Please see P488/1622 of HW manual rev 1.40 for details.
   // Generate A2D conversion start event
   // Enable writing to ELSEGR register. The bit pattern enables writing to the bit and writes to the bit.
   ELC.ELSEGR.BYTE = 0x7F;
   // Disable writing to the ELSEGR register. The bit pattern disables writing to the bit and writes to the bit.
   ELC.ELSEGR.BYTE = 0xBE;

   Event_Synchronous_Publish(&cmt0Interrupt.OnInterrupt, NULL);
}

I_Interrupt_t *Interrupt_Cmt0_Init(void)
{
   // Initialize the event first to account for the possibility of the interrupt firing right after
   // interrupts are enabled
   Event_Synchronous_Init(&cmt0Interrupt.OnInterrupt);
   cmt0Interrupt.interrupt.OnInterrupt = &cmt0Interrupt.OnInterrupt.interface;

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
   IPR(CMT0, CMI0) = E_IRQ_PRIORITY_15;
   // Set Compare Match Constant Register - this is the number of cycles between interrupts
   CMT0.CMCOR = U32_CMT_0_TIMER_PERIOD;
   // Reset Counter - it counts up until it hits the match constant register.
   CMT0.CMCNT = U16_RESET_CMCNT;
   //Start CMT0
   CMT.CMSTR0.BIT.STR0 = U8_CMT_START;

   return &cmt0Interrupt.interrupt;
}
