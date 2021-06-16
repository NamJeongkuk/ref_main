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

enum
{
   KickPeriodInMsec = 20
};

enum
{
   PrcrEnable = 0xA50F,
   PrcrDisable = 0xA500,
};

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

static TinyTimer_t timer;

static inline void Kick(void)
{
   IWDT.IWDTRR = 0x00;
   IWDT.IWDTRR = 0xFF;
}

static void PeriodicWatchdogKick(void *context, struct TinyTimerModule_t *timerModule)
{
   IGNORE(context);
   Kick();

   TinyTimerModule_Start(
      timerModule,
      &timer,
      KickPeriodInMsec,
      PeriodicWatchdogKick,
      NULL);
}

static void InitializePeripheral(void)
{
   // Enable register modification
   SYSTEM.PRCR.WORD = PrcrEnable;

   // Start low speed onchip clock for the independent watchdog timer
   SYSTEM.ILOCOCR.BIT.ILCSTP = 0;

   // Disable register modification
   SYSTEM.PRCR.WORD = PrcrDisable;

   // Write to the IWDT Control register
   // Timeout period selection
   // Clock cycle division ratio
   // Window Start position
   // Window End position
   IWDT.IWDTCR.WORD = IWDTCR_CONFIG_MASK;
   while(IWDT.IWDTCR.WORD != IWDTCR_CONFIG_MASK)
   {
   }

   // Reset on timeout
   IWDT.IWDTRCR.BIT.RSTIRQS = 1;

   // Do not stop counting in sleep mode
   IWDT.IWDTCSTPR.BIT.SLCSTP = 0;

   // Refresh the timer to start the watchdog
   Kick();
}

void Watchdog_Init(TinyTimerModule_t *timerModule)
{
   InitializePeripheral();
   PeriodicWatchdogKick(NULL, timerModule);
}

static void Invoke(I_TinyAction_t *instance)
{
   IGNORE(instance);
   Kick();
}

static const I_TinyAction_Api_t api = { Invoke };

I_TinyAction_t *Watchdog_KickAction(void)
{
   static I_TinyAction_t instance;
   instance.api = &api;
   return &instance;
}
