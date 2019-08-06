/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <stdint.h>
#include "iodefine.h"
#include "SystemClock.h"
#include "utils.h"

// Disables optimization for this file
#pragma GCC optimize ("O0")

enum
{
   Prcr_Enable = 0xA50F,
   Prcr_Disable = 0xA500
};

// Configured for 54 Mhz
#define U8_HOCOCR2_REGISTER_MASK          ((uint8_t)3)
#define U8_HOCOWTCR2_REGISTER_MASK        ((uint8_t)3)

#define B_HOCO_START                      ((uint8_t)0)

#define U16_VRCR_REGISTER_INIT            ((uint16_t)0x00)

#define B_POWER_CONTROL_MODE_HIGH         ((uint8_t)0)

#define U32_SCKCR_REGISTER_MASK           ((uint32_t)0x20811100)
// 00100000100000010001000100000000b
// |||||||||XXX||||||||||||XXXX|||*--> PCKD: x1
// |||||||||   ||||||||||||    ||*---> PCKD
// |||||||||   ||||||||||||    |*----> PCKD
// |||||||||   ||||||||||||    *-----> PCKD
// |||||||||   |||||||||||*----------> PCKB: x1/2
// |||||||||   ||||||||||*-----------> PCKB
// |||||||||   |||||||||*------------> PCKB
// |||||||||   ||||||||*-------------> PCKB
// |||||||||   |||||||*--------------> PCKA: x1/2
// |||||||||   ||||||*---------------> PCKA
// |||||||||   |||||*----------------> PCKA
// |||||||||   ||||*-----------------> PCKA
// |||||||||   |||*------------------> BCK: x1/2
// |||||||||   ||*-------------------> BCK
// |||||||||   |*--------------------> BCK
// |||||||||   *---------------------> BCK
// ||||||||*-------------------------> PSTOP1: Disabled
// |||||||*--------------------------> ICK: x1
// ||||||*---------------------------> ICK
// |||||*----------------------------> ICK
// ||||*-----------------------------> ICK
// |||*------------------------------> FCK
// ||*-------------------------------> FCK:
// |*--------------------------------> FCK
// *---------------------------------> FCK

// Configured for HOCO
#define U16_SCKCR3_REGISTER_MASK          ((uint16_t)0x100)
// 0000000100000000b
// XXXXX|||XXXXXXXX
//      ||*-------->  CKSEL[0]:  HOCO
//      |*--------->  CKSEL[1]:  ----
//      *---------->  CKSEL[2]:  ----

// Tuned to ~500 us based on scope measure (Renesas sample project is @ 350 us)
#define U16_DELAY_COUNT                   ((uint16_t)10)

// PCLK is set to 1/2 main clock, so 27MHz because for some damn reason the rx231 is different to the rx210
#define U32_PCLK                          ((uint32_t)27000000)

static void WaitForClockToStabilize(void)
{
   uint16_t i;

   for(i = 0; i < U16_DELAY_COUNT; i++)
   {
      __asm("nop");
   }
}

void SystemClock_Init(void)
{
   // Enable register modification
   SYSTEM.PRCR.WORD = Prcr_Enable;

   // Configure for 54 Mhz
   SYSTEM.HOCOCR2.BIT.HCFRQ = U8_HOCOCR2_REGISTER_MASK;

   // If ICLK > 32 MHz, you must set MEMWAIT bit
   SYSTEM.MEMWAIT.BIT.MEMWAIT = 1;

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

   // Set PCLKB to 1/2
   SYSTEM.SCKCR.LONG = U32_SCKCR_REGISTER_MASK;
   while(U32_SCKCR_REGISTER_MASK != SYSTEM.SCKCR.LONG)
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
   SYSTEM.PRCR.WORD = Prcr_Disable;
}
