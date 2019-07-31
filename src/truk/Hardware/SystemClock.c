/*!
 * @file
 * @brief Contains all RX130 clock oscillator module function calls
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <stdint.h>
#include "iodefine.h"
#include "SystemClock.h"
#include "utils.h"

// Disables optimization for this file
#pragma GCC optimize ("O0")

static void WaitForClockToStabilize(void)
{
   for(uint16_t u16Index = 0; u16Index < U16_DELAY_COUNT; u16Index++)
   {
      __asm("nop");
   }
}

void SystemClock_Init(void)
{
   WaitForClockToStabilize();

   // Enable register modification
   SYSTEM.PRCR.WORD = U16_PRCR_ENABLE;

   // Start the HOCO clock
   SYSTEM.HOCOCR.BIT.HCSTP = B_HOCO_START;

   // Wait for the clock to stabilize (should be min 350 us)
   WaitForClockToStabilize();

   // Setup operating power control mode the operating power control mode to high
   SYSTEM.OPCCR.BIT.OPCM = B_POWER_CONTROL_MODE_HIGH;
   while(B_POWER_CONTROL_MODE_HIGH != SYSTEM.OPCCR.BIT.OPCMTSF)
   {
      // Wait until the power mode transitions
   }

   // Set clock dividers
   SYSTEM.SCKCR.LONG = U32_SCKCR_REGISTER_MASK_HIGHSPEED;
   while(U32_SCKCR_REGISTER_MASK_HIGHSPEED != SYSTEM.SCKCR.LONG)
   {
      // Confirm write success
   }

   // Select HOCO
   SYSTEM.SCKCR3.WORD = U16_SCKCR3_REGISTER_MASK;
   while (U16_SCKCR3_REGISTER_MASK != SYSTEM.SCKCR3.WORD)
   {
      // Confirm write success
   }

   // Disable register modification
   SYSTEM.PRCR.WORD = U16_PRCR_DISABLE;
}
