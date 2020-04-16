/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "CyLib.h"
#include "TinyTimer.h"
#include "TinyTimeSource_SysTick.h"
#include "Psoc4000Watchdog.h"
#include "CapTouchKeys.h"
#include "TinyUart_Gea3.h"
#include "utils.h"
#include "CapTouchKeys.h"
#include "MicroSystemData.h"
#include "MicroApplication.h"
#include "Gea2Addresses.h"
#include "GeaStack.h"

enum
{
   WatchdogKickPeriodInMsec = 1
};

static TinyTimerModule_t timerModule;
static TinyTimer_t periodicWatchdogTimer;
static MicroSystemData_t systemData;
static MicroApplication_t application;
static GeaStack_t geaStack;

static void KickWatchdog(void *context, struct TinyTimerModule_t *timerModule)
{
   IGNORE(context);
   Psoc4000Watchdog_Kick();

   TinyTimerModule_Start(
      timerModule,
      &periodicWatchdogTimer,
      WatchdogKickPeriodInMsec,
      KickWatchdog,
      NULL);
}

void main(void)
{
   Psoc4000Watchdog_InitWithDefaultConfiguration();
   I_TinyDataSource_t *dataSource = MicroSystemData_DataSource(&systemData);

   CyGlobalIntDisable;
   {
      TinyTimerModule_Init(&timerModule, TinyTimeSource_SysTick_Init());
      MicroSystemData_Init(&systemData);

      GeaStack_Init(
         &geaStack,
         TinyUart_Gea3_Init(),
         &timerModule,
         Psoc4100sGea2Address);

      MicroApplication_Init(&application, dataSource);
   }
   CyGlobalIntEnable;

   // CapTouchKeys_Init(dataSource, &timerModule, Erd_CapTouchKeysState);

   KickWatchdog(NULL, &timerModule);

   while(1)
   {
      TinyTimerModule_Run(&timerModule);
      GeaStack_Run(&geaStack);
   }
}

const Version_t version =
   {
      CRIT_VERSION_MAJOR,
      CRIT_VERSION_MINOR,
      NONCRIT_VERSION_MAJOR,
      NONCRIT_VERSION_MINOR
   };
