/*!
 * @file
 * @brief Clock module header file for RX130
 *
 * Copyright GE Appliances - Confidential - All rights reserved.  
 */

#ifndef CLOCKCONFIG_H
#define CLOCKCONFIG_H

#include <stdint.h>
#include "I_Action.h"

#define U32_PCLKD                          ((uint32_t)32000000)   // Used for A/D clock, must be <= 32 mhz
#define U32_PCLKB                          ((uint32_t)32000000)   // All other peripherals, must be <= 32 mhz
#define U32_FCK                            ((uint32_t)32000000)   // Flash clock, must be <= 32 mhz

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

/*!
 * Sets RX130 CPU clock register
 * @param sckcrRegisterMask Clock settings
 */
void ClockConfig_SetHOCO(uint32_t sckcrRegisterMask);

/*!
 * Get action for setting clock speed to high.
 * @return Action for setting clock speed to high.
 */
I_Action_t * ClockConfig_GetSetClockSpeedHighAction(void);

#endif
