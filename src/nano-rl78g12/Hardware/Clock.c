/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "Clock.h"
#include "iodefine.h"
#include "iodefine_ext.h"

// Clock operation mode control register (CMC)
// High-speed system clock pin operation mode (EXCLK, OSCSEL)
#define _00_CG_HISYS_PORT 0x00U // X1, X2 as I/O port
// Control of X1 high-speed system clock oscillation frequency (AMPH)
#define _00_CG_SYSOSC_DEFAULT 0x00

// Operation speed mode control register (OSMC)
// RTC macro operation clock (WUTMMCK0)
#define _10_CG_RTC_CLK_FIL 0x10 // use fIL clock

void Clock_Init(void)
{
   // Configured for 24MHz
   HOCODIV = 0;
   // Set fMX
   CMC = _00_CG_HISYS_PORT | _00_CG_SYSOSC_DEFAULT;
   MSTOP = 1;
   // Set fMAIN
   MCM0 = 0;
   OSMC = _10_CG_RTC_CLK_FIL;
   // Set fIH
   HIOSTOP = 0;
}
