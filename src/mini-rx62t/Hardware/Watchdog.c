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

static Timer_t timer;

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

static void KickWatchdog(void *context)
{
   IGNORE(context);
   Watchdog_Kick();
}

void Watchdog_EnableAutoKick(TimerModule_t *timerModule)
{
   TimerModule_StartPeriodic(
      timerModule,
      &timer,
      KickPeriodInMsec,
      KickWatchdog,
      NULL);
}

static void Invoke(I_Action_t *instance)
{
   IGNORE(instance);
   Watchdog_Kick();
}

static const I_Action_Api_t api = { Invoke };

I_Action_t *Watchdog_KickAction(void)
{
   static I_Action_t instance;
   instance.api = &api;
   return &instance;
}
