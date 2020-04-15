/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <string.h>
#include "Clk.h"
#include "Iwdg.h"
#include "Pa3Heartbeat.h"
#include "TinyTimeSource_Tim4SystemTick.h"
#include "TinyUart_Uart3.h"
#include "TinyTimer.h"
#include "GeaStack.h"
#include "Gea2Addresses.h"
#include "Ul.h"
#include "MicroApplication.h"
#include "MicroSystemData.h"
#include "utils.h"

enum
{
   WatchdogKickPeriodInMsec = 1
};

static TinyTimerModule_t timerModule;
static TinyTimer_t periodicWatchdogTimer;
static GeaStack_t geaStack;
static MicroSystemData_t systemData;
static MicroApplication_t application;

static void KickWatchdog(void *context, struct TinyTimerModule_t *timerModule)
{
   IGNORE(context);
   Iwdg_Kick();

   TinyTimerModule_Start(
      timerModule,
      &periodicWatchdogTimer,
      WatchdogKickPeriodInMsec,
      KickWatchdog,
      NULL);
}

void main(void)
{
#ifndef DISABLE_UL_CHECKS
   Ul_Startup();
#endif
   Iwdg_Init();
   Clk_Init();

   disableInterrupts();
   {
      TinyTimerModule_Init(
         &timerModule,
         TinyTimeSource_Tim4SystemTick_Init());

      Pa3Heartbeat_Init(&timerModule);

      MicroSystemData_Init(&systemData, &timerModule);

      GeaStack_Init(
         &geaStack,
         &timerModule,
         MicroSystemData_DataSource(&systemData),
         TinyUart_Uart3_Init(),
         Stm8sGea2Address);

      MicroApplication_Init(&application, MicroSystemData_DataSource(&systemData));
   }
   enableInterrupts();

   KickWatchdog(NULL, &timerModule);

   while(1)
   {
      TinyTimerModule_Run(&timerModule);
      GeaStack_Run(&geaStack);
#ifndef DISABLE_UL_CHECKS
      Ul_Run();
#endif
   }
}
