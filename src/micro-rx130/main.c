/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <stddef.h>
#include "Interrupts.h"
#include "Clock.h"
#include "Watchdog.h"
#include "TinyTimeSource_Cmt0.h"
#include "TinyTimer.h"
#include "Heartbeat.h"
#include "GeaStack.h"
#include "TinyUart_Uart6.h"
#include "MicroApplication.h"
#include "MicroSystemData.h"
#include "Gea2Addresses.h"
#include "utils.h"

enum
{
   WatchdogKickPeriodInMsec = 1
};

static TinyTimerModule_t timerModule;
static TinyTimer_t periodicWatchdogTimer;

static void KickWatchdog(void *context, struct TinyTimerModule_t *timerModule)
{
   IGNORE(context);
   Watchdog_Kick();

   TinyTimerModule_Start(
      timerModule,
      &periodicWatchdogTimer,
      WatchdogKickPeriodInMsec,
      KickWatchdog,
      NULL);
}

int main(void)
{
   Interrupts_Disable();
   {
      Clock_Init();

      TinyTimerModule_Init(
         &timerModule,
         TinyTimeSource_Cmt0_Init());

      Watchdog_Init();

      Heartbeat_Init(&timerModule);

      MicroSystemData_Init(&timerModule);

      GeaStack_Init(
         &timerModule,
         MicroSystemData_ExternalDataSource(),
         TinyUart_Uart6_Init(),
         Rx130Gea2Address);

      MicroApplication_Init(
         MicroSystemData_DataSource(),
         GeaStack_GetGea2Interface());
   }
   Interrupts_Enable();

   KickWatchdog(NULL, &timerModule);

   while(1)
   {
      TinyTimerModule_Run(&timerModule);
      GeaStack_Run();
   }
}
