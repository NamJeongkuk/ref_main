/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "stm32g0xx.h"
#include "Clock.h"
#include "Watchdog.h"
#include "Pa5Heartbeat.h"
#include "TinyTimeSource_SysTick.h"
#include "TinyUart_Usart2.h"
#include "TinyTimer.h"
#include "GeaStack.h"
#include "MicroApplication.h"
#include "MicroSystemData.h"
#include "I_TinyInterrupt.h"
#include "Gea2Addresses.h"
#include "Version.h"
#include "utils.h"

enum
{
   WatchdogKickPeriodInMsec = 50
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
   Clock_Init();

   __disable_irq();
   {
      TinyTimerModule_Init(
         &timerModule,
         TinyTimeSource_SysTick_Init());

      Pa5Heartbeat_Init(&timerModule);

      MicroSystemData_Init(&timerModule);

      GeaStack_Init(
         &timerModule,
         MicroSystemData_DataSource(),
         TinyUart_Usart2_Init(),
         Stm32g0Gea2Address);

      MicroApplication_Init(
         MicroSystemData_DataSource(),
         GeaStack_GetGea2Interface());
   }
   __enable_irq();

   KickWatchdog(NULL, &timerModule);

   while(1)
   {
      TinyTimerModule_Run(&timerModule);
      GeaStack_Run();
   }
}
