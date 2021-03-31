/*!
 * @file
 * @brief
 *
 * Q. How can I configure the watchdog timeout?
 *
 * A. The IWDT is clocked by IWDTCLK which uses a fixed 15 KHz frequency. The clock divider and
 * timeout period for the IWDT are configurable and together determine the timeout:
 *
 *   Timeout = ClockDivider * TimeoutPeriod / IWDTCLK
 *
 * When the configuration mask is 0x3322 (windowing disabled, timeout of 1024 cycles, divide by 16),
 * the timeout is:
 *
 *   Timeout = 16 * 1024 / 15000 = ~1.09 seconds
 *
 * See Table 26.2 (RX130) or Table 31.2 (RX231) in the hardware manual for help setting the clock
 * divider and timeout.
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

#include "Watchdog.h"
#include "iodefine.h"
#include "utils.h"

#define PRCR_ENABLE (0xA50F)
#define PRCR_DISABLE (0xA500)

#define LOCO_IWDT_CLOCK_START (0)
#define LOCO_IWDT_CLOCK_STOP (1)

#define IWDTCR_CONFIG_MASK (0x3323)
// 0011001110100010B
// XX||XX||||||XX|+---------- Time-Out Period Selection            : 2048 cycles
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
   SYSTEM.PRCR.WORD = PRCR_ENABLE;

   // start low speed onchip clock for the independent watchdog timer
   SYSTEM.ILOCOCR.BIT.ILCSTP = LOCO_IWDT_CLOCK_START;

   // Disable register modification
   SYSTEM.PRCR.WORD = PRCR_DISABLE;

   // Write to the IWDT Control register
   // Timeout period selection
   // Clock cycle division ratio
   // Window Start position
   // Window End position
   IWDT.IWDTCR.WORD = IWDTCR_CONFIG_MASK;
   while(IWDT.IWDTCR.WORD != IWDTCR_CONFIG_MASK)
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

I_TinyAction_t *Watchdog_KickAction(void)
{
   static I_TinyAction_t instance;
   instance.api = &api;
   return &instance;
}
