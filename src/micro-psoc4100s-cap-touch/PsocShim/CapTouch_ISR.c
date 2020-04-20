/*!
 * @file
 * @brief CapTouch Isr Glue Code
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "cydevice_trm.h"
#include "CyLib.h"
#include "CapTouch_ISR.h"
#include "utils.h"

void CapTouchIsrEnable(void)
{
   /* Enable the general interrupt. */
   *CapTouchIsrInterruptSetEnable = CapTouch_ISR__INTC_MASK;
}

void CapTouchIsrDisable(void)
{
   /* Disable the general interrupt. */
   *CapTouchIsrInterruptClearEnable = CapTouch_ISR__INTC_MASK;
}

void CapTouch_ISR_SetPending(void)
{
   *CapTouchIsrInterruptSetPending = CapTouch_ISR__INTC_MASK;
}

void CapTouch_ISR_ClearPending(void)
{
   *CapTouchIsrInterruptClearPending = CapTouch_ISR__INTC_MASK;
}

static void CapTouchIsrSetPriority(uint8 priority)
{
   uint8 interruptState;
   uint32 priorityOffset = ((CapTouch_ISR__INTC_NUMBER % 4u) * 8u) + 6u;

   interruptState = CyEnterCriticalSection();
   *CapTouchIsrInterruptPriority = (*CapTouchIsrInterruptPriority & (uint32)(~CapTouch_ISR__INTC_PRIOR_MASK)) |
      ((uint32)priority << priorityOffset);
   CyExitCriticalSection(interruptState);
}

void CapTouch_ISR_Start(void)
{
   /* For all we know the interrupt is active. */
   CapTouchIsrDisable();

   /* Set the priority. */
   CapTouchIsrSetPriority((uint8)CapTouchIsrInterruptPriorityNumber);

   /* Enable it. */
   CapTouchIsrEnable();
}

static cyisraddress isr;

CY_ISR(CapTouchIsrTrampoline)
{
   isr();
}

void CapTouch_ISR_StartEx(cyisraddress address)
{
   isr = address;

   /* For all we know the interrupt is active. */
   CapTouchIsrDisable();

   /* Set the priority. */
   CapTouchIsrSetPriority((uint8)CapTouchIsrInterruptPriorityNumber);

   /* Enable it. */
   CapTouchIsrEnable();
}
