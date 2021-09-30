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
#include "MicroSystemErds.h"
#include "I_TinyInterrupt.h"
#include "Gea2Addresses.h"
#include "Version.h"
#include "TinyStackUsageService.h"
#include "utils.h"

enum
{
   WatchdogKickPeriodInMsec = 50
};

static TinyTimerModule_t timerModule;
static TinyTimer_t periodicWatchdogTimer;
static TinyStackUsageService_t stackUsageService;

extern char __stackStart;
extern char __stackEnd;
static const TinyStackUsageServiceConfiguration_t stackUsageServiceConfiguration = {
   .start = (uintptr_t)&__stackStart,
   .end = (uintptr_t)&__stackEnd,
   .percentFreeErd = Erd_StackUsagePercentFree
};

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
   __disable_irq();
   {
      Watchdog_Init();
      Clock_Init();

      TinyStackUsageService_InitializeStack(
         &stackUsageServiceConfiguration);

      TinyTimerModule_Init(
         &timerModule,
         TinyTimeSource_SysTick_Init());

      Pa5Heartbeat_Init(&timerModule);

      MicroSystemData_Init(&timerModule);

      TinyStackUsageService_Init(
         &stackUsageService,
         &timerModule,
         MicroSystemData_DataSource(),
         &stackUsageServiceConfiguration);

      GeaStack_Init(
         &timerModule,
         MicroSystemData_ExternalDataSource(),
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
      GeaStack_Run();

      if(!TinyTimerModule_Run(&timerModule))
      {
         __WFI();
      }
   }
}
