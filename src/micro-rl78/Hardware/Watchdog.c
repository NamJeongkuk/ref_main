/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <stddef.h>
#include "Watchdog.h"
#include "iodefine.h"
#include "iodefine_ext.h"
#include "utils.h"

#define WDT_KICK_VALUE 0xAC

enum
{
   WatchdogKickPeriod = 10
};

static TinyTimer_t watchdogKickTimer;

static void KickWatchdog(void *context, TinyTimerModule_t *timerModule)
{
   IGNORE(context);

   WDTE = WDT_KICK_VALUE;

   TinyTimerModule_Start(
      timerModule,
      &watchdogKickTimer,
      WatchdogKickPeriod,
      KickWatchdog,
      NULL);
}

void Watchdog_Init(TinyTimerModule_t *timerModule)
{
   // The watchdog is turned on using option bytes

   KickWatchdog(NULL, timerModule);
}
