/*!
 * @file
 * @brief Initializes TAU0 Channel 3 as the system timer
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <stddef.h>
#include "iodefine.h"
#include "iodefine_ext.h"
#include "TinyTimeSource_Tau0Channel3.h"
#include "TinyEvent_Synchronous.h"
#include "utils.h"

// Timer Mode Register mn (TMRmn)
// Selection of macro clock (MCK) of channel n (CKSmn)
#define _0000_TAU_CLOCK_SELECT_CKM0 0x0000 // operation clock CK0 set by PRS register
// Selection of count clock (CCK) of channel n (CCSmn)
#define _0000_TAU_CLOCK_MODE_CKS 0x0000 // macro clock MCK specified by CKSmn bit
// Operation explanation of channel 1 or 3 (SPLIT)
#define _0000_TAU_16BITS_MODE 0x0000 // operates as 16 bits timer
// Setting of start trigger or capture trigger of channel n (STSmn2 - STSmn0)
#define _0000_TAU_TRIGGER_SOFTWARE 0x0000 // only software trigger start is valid
// Operation mode of channel n (MDmn3 - MDmn0)
#define _0000_TAU_MODE_INTERVAL_TIMER 0x0000 // interval timer mode
// Setting of starting counting and interrupt (MDmn0)
#define _0000_TAU_START_INT_UNUSED 0x0000 // timer interrupt is not generated when counting is started (timer output does not change, either)

// Timer Channel Start Register m (TSm)
// Operation enable (start) trigger of channel 3 (TSm0)
#define _0001_TAU_CH3_START_TRG_ON 0x0008 // operation is enabled (start software trigger is generated)

// Timer Channel Stop Register m (TTm)
// Operation stop trigger of channel 0 (TTm0)
#define _0001_TAU_CH3_STOP_TRG_ON 0x0001 // operation is stopped (stop trigger is generated)

// Timer Output Enable Register m (TOEm)
// Timer output enable/disable of channel 0 (TOEm0)
#define _0001_TAU_CH3_OUTPUT_ENABLE 0x0001 // the TOm0 operation enabled by count operation (timer channel output bit)

// Timer Output Register m (TOm)
// Timer output of channel 0 (TOm0)
#define _0001_TAU_CH3_OUTPUT_VALUE_1 0x0001 // timer output value is "1"

#define ENABLE_INTTM00_INTERRUPT (0)
#define DISABLE_INTTM00_INTERRUPT (1)
#define CLEAR_INTTM00_INTERRUPT_FLAG (0)

static I_TinyTimeSource_t instance;
static volatile TinyTimeSourceTickCount_t ticks;
static TinyEvent_Synchronous_t interrupt;

void INT_TM03(void) __attribute__((interrupt));
void INT_TM03(void)
{
   ticks++;
   TinyEvent_Synchronous_Publish(&interrupt, NULL);
}

static TinyTimeSourceTickCount_t GetTicks(I_TinyTimeSource_t *instance)
{
   IGNORE(instance);

   TinyTimeSourceTickCount_t last;
   TinyTimeSourceTickCount_t current = ticks;

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

static const I_TinyTimeSource_Api_t api = { GetTicks };

I_TinyTimeSource_t *TinyTimeSource_Tau0Channel3_Init(void)
{
   TAU0EN = 1; // supplies input clock

   // Clock 0 for TAU = 1MHz Clock source
   // Clock 1 for TAU = 16Mhz
   // Clock 2 for TAU (Channel 0 - Channel 3) = 2MHz
   // Clock 3 for TAU (Channel 0 - Channel 3) = 125Kz
   //       Reset     CK3      CK2      CK1      CK0
   TPS0 = (0x0000 | 0x0000 | 0x0200 | 0x0010 | 0x0005);

   // Mask channel  interrupt
   TMMK03 = DISABLE_INTTM00_INTERRUPT;
   TMIF03 = CLEAR_INTTM00_INTERRUPT_FLAG;

   // Set INTTM03 high priority
   TMPR103 = 1;
   TMPR003 = 0;

   // Channel 3 used as interval timer
   TMR03 = _0000_TAU_CLOCK_SELECT_CKM0 | _0000_TAU_CLOCK_MODE_CKS | _0000_TAU_16BITS_MODE | _0000_TAU_TRIGGER_SOFTWARE | _0000_TAU_MODE_INTERVAL_TIMER | _0000_TAU_START_INT_UNUSED;
   TDR03 = 1000;

   TMIF03 = CLEAR_INTTM00_INTERRUPT_FLAG;
   TMMK03 = ENABLE_INTTM00_INTERRUPT;
   TS0 |= _0001_TAU_CH3_START_TRG_ON;

   TinyEvent_Synchronous_Init(&interrupt);

   instance.api = &api;

   return &instance;
}

I_TinyEvent_t *TinyTimeSource_Tau0Channel3_Interrupt(void)
{
   return &interrupt.interface;
}
