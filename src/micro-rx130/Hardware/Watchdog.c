/*!
 * @file
 * @brief
 *
 * Q.How can I calculate the expiration watchdog time?
 *
 * A. IWDTCLK = 125 KHz (Fixed independent clock value)
 * Clock division ratio = IWDTCLK/16 = 7.8 KHz
 * After setting IWDTRR to 0xFF and Window start position to 100%, end position to 0%,
 * Watchdog expiration time = 1024 * 256/IWDCLK ~= 2097 msec
 * So, the system will reboot 131msec after the last time it was reset.
 *
 * If you want to change the maximum expiration time for debugging issue,
 * you will be able to set U16_IWDTCR_CONFIG_MASK to 0x3353.
 * This gives you 256*16384/IWDTCLK = 33 second expiration time for watchdog.
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

#include <stdint.h>
#include <stdbool.h>
#include "Watchdog.h"
#include "iodefine.h"
#include "utils.h"

#define U16_PRCR_ENABLE ((uint16_t)0xA50F)
#define U16_PRCR_DISABLE ((uint16_t)0xA500)

#define B_LOCO_IWDT_CLOCK_START ((bool)0)
#define B_LOCO_IWDT_CLOCK_STOP ((bool)1)

#define U16_IWDTCR_CONFIG_MASK ((uint16_t)0x3352)
// 0011001110100010B
// XX||XX||||||XX|+---------- Time-Out Period Selection            : 1,024 cycles
// ||||||||||||||+----------- Time-Out Period Selection            : ---
// |||||||||||||+------------ Reserved                             : Write 0
// ||||||||||||+------------- Reserved                             : Write 0
// |||||||||||+-------------- Clock Division Ratio Selection       : IWDTCLK/16
// ||||||||||+--------------- Clock Division Ratio Selection       : ---
// |||||||||+---------------- Clock Division Ratio Selection       : ---
// ||||||||+----------------- Clock Division Ratio Selection       : ---
// |||||||+------------------ Window End Position Selection        : 0%
// ||||||+------------------- Window End Position Selection        : ---
// |||||+-------------------- Reserved                             : Write 0
// ||||+--------------------- Reserved                             : Write 0
// |||+---------------------- Window Start Position Selection      : 100%
// ||+----------------------- Window Start Position Selection      : ---
// |+------------------------ Reserved                             : Write 0
// +------------------------- Reserved                             : Write 0

// IWDTCSTPR.SLCSTP Bit Values
#define BIT_VALUE_IWDTCSTPR_SLCSTP_STOP_DISABLED 0x00
#define BIT_VALUE_IWDTCSTPR_SLCSTP_STOP_ENABLED 0x01

// IWDTRCR. Bit Values
#define BIT_VALUE_IWDTRCR_RSTIRQS_INTERRUPT 0x00
#define BIT_VALUE_IWDTRCR_RSTIRQS_RESET 0x01

// IWDTRR Refresh Count Values
#define WDT_REFRESH_COUNT_VALUE 0xFF

void Watchdog_Init(void)
{
   // Enable register modification
   SYSTEM.PRCR.WORD = U16_PRCR_ENABLE;

   // start low speed onchip clock for the independent watchdog timer
   SYSTEM.ILOCOCR.BIT.ILCSTP = B_LOCO_IWDT_CLOCK_START;

   // Disable register modification
   SYSTEM.PRCR.WORD = U16_PRCR_DISABLE;

   // Write to the IWDT Control register
   // Timeout period selection
   // Clock cycle division ratio
   // Window Start position
   // Window End position
   IWDT.IWDTCR.WORD = U16_IWDTCR_CONFIG_MASK;
   while(IWDT.IWDTCR.WORD != U16_IWDTCR_CONFIG_MASK)
   {
   }

   // Reset on the timeout
   IWDT.IWDTRCR.BIT.RSTIRQS = BIT_VALUE_IWDTRCR_RSTIRQS_RESET;

   // Sleep Mode Count Stop selection
   IWDT.IWDTCSTPR.BIT.SLCSTP = BIT_VALUE_IWDTCSTPR_SLCSTP_STOP_DISABLED;

   // Refresh the timer to start the watchdog
   IWDT.IWDTRR = 0x00;
   IWDT.IWDTRR = WDT_REFRESH_COUNT_VALUE;
}

void Watchdog_Kick(void)
{
   // Refresh the timer
   IWDT.IWDTRR = 0x00;
   IWDT.IWDTRR = WDT_REFRESH_COUNT_VALUE;
}

static void Invoke(I_TinyAction_t *instance)
{
   IGNORE(instance);
   Watchdog_Kick();
}

static const I_TinyAction_Api_t api = { Invoke };

I_TinyAction_t * Watchdog_KickAction(void)
{
   static I_TinyAction_t instance;
   instance.api = &api;
   return &instance;
}
