/*!
 * @file
 * @brief CapSense Isr Glue Code
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "cydevice_trm.h"
#include "CyLib.h"
#include "CapSense_ISR.h"
#include "utils.h"

void CapSenseIsrEnable(void)
{
   /* Enable the general interrupt. */
   *CapSenseIsrInterruptSetEnable = CapSense_ISR__INTC_MASK;
}

void CapSenseIsrDisable(void)
{
   /* Disable the general interrupt. */
   *CapSenseIsrInterruptClearEnable = CapSense_ISR__INTC_MASK;
}

void CapSense_ISR_SetPending(void)
{
   *CapSenseIsrInterruptSetPending = CapSense_ISR__INTC_MASK;
}

void CapSense_ISR_ClearPending(void)
{
   *CapSenseIsrInterruptClearPending = CapSense_ISR__INTC_MASK;
}

static void CapSenseIsrSetPriority(uint8 priority)
{
   uint8 interruptState;
   uint32 priorityOffset = ((CapSense_ISR__INTC_NUMBER % 4u) * 8u) + 6u;

   interruptState = CyEnterCriticalSection();
   *CapSenseIsrInterruptPriority = (*CapSenseIsrInterruptPriority & (uint32)(~CapSense_ISR__INTC_PRIOR_MASK)) |
      ((uint32)priority << priorityOffset);
   CyExitCriticalSection(interruptState);
}

void CapSense_ISR_Start(void)
{
   /* For all we know the interrupt is active. */
   CapSenseIsrDisable();

   /* Set the priority. */
   CapSenseIsrSetPriority((uint8)CapSenseIsrInterruptPriorityNumber);

   /* Enable it. */
   CapSenseIsrEnable();
}

static cyisraddress isr;

CY_ISR(CapSenseIsrTrampoline)
{
   isr();
}

void CapSense_ISR_StartEx(cyisraddress address)
{
   isr = address;

   /* For all we know the interrupt is active. */
   CapSenseIsrDisable();

   /* Set the priority. */
   CapSenseIsrSetPriority((uint8)CapSenseIsrInterruptPriorityNumber);

   /* Enable it. */
   CapSenseIsrEnable();
}
