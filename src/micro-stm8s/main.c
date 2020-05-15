/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

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

      MicroSystemData_Init(&timerModule);

      GeaStack_Init(
         &timerModule,
         MicroSystemData_DataSource(),
         TinyUart_Uart3_Init(),
         Stm8sGea2Address);

      MicroApplication_Init(
         MicroSystemData_DataSource(),
         GeaStack_GetGea2Interface());
   }
   enableInterrupts();

   KickWatchdog(NULL, &timerModule);

   while(1)
   {
      TinyTimerModule_Run(&timerModule);
      GeaStack_Run();
#ifndef DISABLE_UL_CHECKS
      Ul_Run();
#endif
   }
}
