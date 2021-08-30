/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "iodefine.h"
#include "Clock.h"

void Clock_Init(void)
{
   // Input is 12.5 MHz

   // ICLCK is input x 8 (100 MHz)
   SYSTEM.SCKCR.BIT.ICK = 0;

   // PCLCK is input x 4 (50 MHz)
   SYSTEM.SCKCR.BIT.PCK = 1;
}
