/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "CyLib.h"
#include "TinyTimer.h"
#include "TinyTimeSource_SysTick.h"
#include "Watchdog.h"
#include "TinyUart_Gea3.h"
#include "utils.h"
#include "GeaStack.h"
#include "Gea2Addresses.h"
#include "Application.h"
#include "SystemData.h"

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

void main(void)
{
   Watchdog_Init();

   CyGlobalIntDisable;
   {
      TinyTimerModule_Init(
         &timerModule,
         TinyTimeSource_SysTick_Init());

      SystemData_Init(&timerModule);

      GeaStack_Init(
         &timerModule,
         SystemData_ExternalDataSource(),
         TinyUart_Gea3_Init(),
         Psoc4100sGea2Address);
   }
   CyGlobalIntEnable;

   Application_Init(SystemData_DataSource());

   KickWatchdog(NULL, &timerModule);

   while(1)
   {
      TinyTimerModule_Run(&timerModule);
      GeaStack_Run();
   }
}
