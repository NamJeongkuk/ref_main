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
#include "HeartbeatPin5Port5.h"
#include "Application.h"
#include "SystemData.h"
#include "MemoryMap.h"
#include "UlTestsPlugin.h"

enum
{
   WatchdogKickPeriodInMsec = 1,
   OneHzHalfPeriod = 500
};

enum
{
   UlTestPeriod = 5,
   UlBytesPerRomCheckStep = 10,
   UlResourceWatchdogTimeout = 100,
#if DEBUG
   UlRomCheckErrorEnabled = false,
#else
   UlRomCheckErrorEnabled = true,
#endif
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
   Watchdog_Init(0);

   CyGlobalIntDisable;
   {
      TinyTimerModule_Init(
         &timerModule,
         TinyTimeSource_SysTick_Init());

      HeartbeatPin5Port5_Init(&timerModule, OneHzHalfPeriod);

      SystemData_Init(&timerModule);

      GeaStack_Init(
         &timerModule,
         SystemData_ExternalDataSource(),
         TinyUart_Gea3_Init(),
         Psoc4100sGea2Address);
   }
   CyGlobalIntEnable;

   UlTestsPlugin_Init(
      &timerModule,
      ApplicationHeaderAddress,
      UlRomCheckErrorEnabled,
      UlTestPeriod,
      UlResourceWatchdogTimeout,
      UlBytesPerRomCheckStep);

   Application_Init(SystemData_DataSource());

   KickWatchdog(NULL, &timerModule);

   while(1)
   {
      TinyTimerModule_Run(&timerModule);
      GeaStack_Run();
   }
}
