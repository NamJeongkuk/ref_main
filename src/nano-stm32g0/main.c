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
#include "NanoSystemData.h"
#include "NanoApplication.h"
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
static GeaStack_t geaStack;
static NanoSystemData_t systemData;
static NanoApplication_t application;

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

      NanoSystemData_Init(&systemData);
      I_TinyDataSource_t *dataSource = NanoSystemData_DataSource(&systemData);

      GeaStack_Init(
         &geaStack,
         TinyUart_Usart2_Init(),
         dataSource,
         &timerModule,
         Stm8sGea2Address);

      NanoApplication_Init(&application, dataSource);
   }
   __enable_irq();

   KickWatchdog(NULL, &timerModule);

   while(1)
   {
      TinyTimerModule_Run(&timerModule);
      GeaStack_Run(&geaStack);
   }
}
