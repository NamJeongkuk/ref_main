/*!
 * @file
 * @brief Contains all RX130 clock oscillator module function calls
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <stdint.h>
#include "iodefine.h"
#include "ClockConfig.h"
#include "utils.h"

// Disables optimization for this file
#pragma GCC optimize ("O0")

/*!
 * Delay to manage clock stabilization
 */
static void WaitForClockToStabilize(void)
{
   for(uint16_t u16Index = 0; u16Index < U16_DELAY_COUNT; u16Index++)
   {
      __asm("nop");
   }
}

/*!
 * Sets up the internal high speed clock
 */
void ClockConfig_SetHOCO(uint32_t sckcrRegisterMask)
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
   SYSTEM.SCKCR.LONG = sckcrRegisterMask;
   while(sckcrRegisterMask != SYSTEM.SCKCR.LONG)
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

static void SetClockSpeedHighDelegate(I_Action_t *context)
{
   IGNORE_ARG(context);
   ClockConfig_SetHOCO(U32_SCKCR_REGISTER_MASK_HIGHSPEED);
}

static I_Action_t setClockSpeedHighAction;
static const I_Action_Api_t setClockSpeedHighApi = { SetClockSpeedHighDelegate };

I_Action_t * ClockConfig_GetSetClockSpeedHighAction(void)
{
   setClockSpeedHighAction.api = &setClockSpeedHighApi;
   return &setClockSpeedHighAction;
}
