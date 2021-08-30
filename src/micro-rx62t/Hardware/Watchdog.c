/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

#include "Watchdog.h"
#include "iodefine.h"
#include "utils.h"

enum
{
   KickPeriodInMsec = 50
};

void Watchdog_Init(void)
{
   // OCOCLK / divisor / time out cycles => time out frequency
   // 125 KHz / 256 / 1024 => 0.477 Hz == ~2.1 seconds

   // IWDTCR can only be written once so we have to write the whole word at once
   // Upper byte:
   // - 0b00110011 => Required
   // Lower byte:
   // - 0b1111xxxx => OCOCLK / 256
   // - 0bxxxx00xx => Must be written as 0
   // - 0bxxxxxx00 => Time out after 1024 cycles
   IWDT.IWDTCR.WORD = 0x33F0;

   // Start the watchdog
   Watchdog_Kick();
}

void Watchdog_Kick(void)
{
   IWDT.IWDTRR = 0x00;
   IWDT.IWDTRR = 0xFF;
}

static void KickWatchdog(void *context, struct TinyTimerModule_t *timerModule)
{
   static TinyTimer_t timer;

   IGNORE(context);
   Watchdog_Kick();

   TinyTimerModule_Start(
      timerModule,
      &timer,
      KickPeriodInMsec,
      KickWatchdog,
      NULL);
}

void Watchdog_EnableAutoKick(TinyTimerModule_t *timerModule)
{
   KickWatchdog(NULL, timerModule);
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
