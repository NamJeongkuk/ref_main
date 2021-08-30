/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "iodefine.h"
#include "Reset.h"

void Reset(void)
{
   // Upper byte 0x5A specifies to write to RSTE bit of RSTCSR
   // Lower byte:
   // - 0bx1xxxxxx => Reset enabled
   WDT.WRITE.WINB = 0x5A40;

   // Upper byte 0xA5 specifies to write to TCSR
   // Lower byte:
   // - 0b0xxxxxxx => Can be whatever
   // - 0bx1xxxxxx => Watchdog timer mode
   // - 0bxx1xxxxx => Start counting
   // - 0bxxx11xxx => Must always be written as 1s
   // - 0bxxxxx000 => PCLK / 4 (overflows after 20 usec @ 50 MHz PCLK)
   WDT.WRITE.WINA = 0xA578;

   while(1)
   {
   }
}
