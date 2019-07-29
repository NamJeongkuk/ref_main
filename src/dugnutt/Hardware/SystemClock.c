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

// Configured for 32 Mhz
#define U8_HOCOCR_REGISTER_MASK           ((uint8_t)3)
#define U8_HOCOWTCR2_REGISTER_MASK        ((uint8_t)3)

#define B_HOCO_START                      ((uint8_t)0)

#define U16_VRCR_REGISTER_INIT            ((uint16_t)0x00)

#define B_POWER_CONTROL_MODE_HIGH         ((uint8_t)0)

#define U16_PRCR_ENABLE                   ((uint16_t)0xA50F)
#define U16_PRCR_DISABLE                  ((uint16_t)0xA500)

#define U32_SCKCR_REGISTER_MASK_HIGHSPEED ((uint32_t)0x00000001)
// 00000000000000000000000000000001b
// ||||||||XXXXXXXXXXXX||||XXXX||||
// ||||||||            ||||    |||#---> PCKD: x1/2
// ||||||||            ||||    ||#----> PCKD
// ||||||||            ||||    |#-----> PCKD
// ||||||||            ||||    #------> PCKD
// ||||||||            |||#-----------> PCKB: x1
// ||||||||            ||#------------> PCKB
// ||||||||            |#-------------> PCKB
// ||||||||            #--------------> PCKB
// |||||||#---------------------------> ICLK: x1
// ||||||#----------------------------> ICLK
// |||||#-----------------------------> ICLK
// ||||#------------------------------> ICLK
// |||#-------------------------------> FCLK: x1
// ||#--------------------------------> FCLK
// |#---------------------------------> FCLK
// #----------------------------------> FCLK

// Configured for HOCO
#define U16_SCKCR3_REGISTER_MASK          ((uint16_t)0x0100)
// 0000000100000000b
// XXXXX|||XXXXXXXX
//      ||*-------->  CKSEL[0]:  HOCO
//      |*--------->  CKSEL[1]:  ----
//      *---------->  CKSEL[2]:  ----

// Tuned to ~500 us based on scope measure (Renesas sample project is @ 350 us)
#define U16_DELAY_COUNT                   ((uint16_t)10)

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
