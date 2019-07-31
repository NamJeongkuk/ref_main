/*!
 * @file
 * @brief Clock module header file for RX130
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SYSTEMCLOCK_H
#define SYSTEMCLOCK_H

#include <stdint.h>
#include "I_Action.h"

#define U32_PCLKD                     ((uint32_t)27000000)   // Used for A/D clock, must be <= 54 mhz
#define U32_PCLKA                     ((uint32_t)27000000)   // MTU2, must be <= 54 mhz
#define U32_PCLKB                     ((uint32_t)27000000)   // All other peripherals, must be <= 32 mhz
#define U32_FCK                       ((uint32_t)27000000)   // Flash clock, must be <= 32 mhz

// Configured for 54 Mhz
#define U8_HOCOCR_REGISTER_MASK           ((uint8_t)3)
#define U8_HOCOWTCR2_REGISTER_MASK        ((uint8_t)3)

#define B_HOCO_START                      ((uint8_t)0)

#define U16_VRCR_REGISTER_INIT            ((uint16_t)0x00)

#define B_POWER_CONTROL_MODE_HIGH         ((uint8_t)0)

#define U16_PRCR_ENABLE                   ((uint16_t)0xA50F)
#define U16_PRCR_DISABLE                  ((uint16_t)0xA500)

#define U32_SCKCR_REGISTER_MASK_HIGHSPEED ((uint32_t)0x10811101)
// 00010000100000010001000100000001b
// |||||||||XXX||||||||||||XXXX|||*--> PCKD: x1/2
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
// |||||||||   |||*------------------> BCLK: x1/2 (not used)
// |||||||||   ||*-------------------> BCLK
// |||||||||   |*--------------------> BCLK
// |||||||||   *---------------------> BCLK
// ||||||||*-------------------------> PSTOP1: Disabled
// |||||||*--------------------------> ICLK: x1
// ||||||*---------------------------> ICLK
// |||||*----------------------------> ICLK
// ||||*-----------------------------> ICLK
// |||*------------------------------> FCLK: x1/2
// ||*-------------------------------> FCLK
// |*--------------------------------> FCLK
// *---------------------------------> FCLK

// Configured for HOCO
#define U16_SCKCR3_REGISTER_MASK          ((uint16_t)0x0100)
// 0000000100000000b
// XXXXX|||XXXXXXXX
//      ||*-------->  CKSEL[0]:  HOCO
//      |*--------->  CKSEL[1]:  ----
//      *---------->  CKSEL[2]:  ----

// Tuned to ~500 us based on scope measure (Renesas sample project is @ 350 us)
#define U16_DELAY_COUNT                   ((uint16_t)10)

/*!
 * Initialize and select the high-speed clock
 */
void SystemClock_Init(void);

#endif
