/*!
 * @file
 * @brief Hardware timer driver
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "HardwareTimer.h"
#include "iodefine.h"
#include "uassert.h"

void HardwareTimer_BlockingDelayInMicroSeconds(uint16_t microSecondsDelayTime)
{
   uassert(microSecondsDelayTime < (UINT16_MAX / 2)); // To prevent overflow of the timer

   CMT.CMSTR0.BIT.STR1 = 0;
   CMT1.CMCNT = 0x0000;
   CMT.CMSTR0.BIT.STR1 = 1;
   while(CMT1.CMCNT < (2 * microSecondsDelayTime))
   {
      ;
   }
}

void HardwareTimer_Init(void)
{
   // Take CMT1 out of stop mode
   SYSTEM.PRCR.WORD = 0xA50F;
   SYSTEM.MSTPCRA.BIT.MSTPA15 = 0;
   SYSTEM.PRCR.WORD = 0xA500;

   CMT1.CMCR.BIT.CKS = 0; // PCKB/8 for 0.5 microsecond resolution
   CMT1.CMCOR = UINT16_MAX; // Free running
}
