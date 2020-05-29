/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "Clk.h"
#include "Iwdg.h"
#include "Pd4Heartbeat.h"
#include "Rst.h"
#include "TinyTimeSource_Tim4SystemTick.h"
#include "TinyAdcGroup_Adc1.h"
#include "TinyUart_Uart1.h"
#include "TinyTimer.h"
#include "GeaStack.h"
#include "NanoSystemData.h"
#include "NanoApplication.h"
#include "Led.h"
#include "Button.h"
#include "I_TinyInterrupt.h"
#include "Gea2Addresses.h"
#include "Version.h"
#include "Ul.h"
#include "utils.h"

enum
{
   WatchdogKickPeriodInMsec = 1
};

static TinyTimerModule_t timerModule;
static TinyTimer_t periodicWatchdogTimer;
static GeaStack_t geaStack;
static NanoSystemData_t systemData;
static NanoApplication_t application;

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
   Iwdg_Init();
   Clk_Init();

   disableInterrupts();
   {
      TinyTimerModule_Init(
         &timerModule,
         TinyTimeSource_Tim4SystemTick_Init());

      Pd4Heartbeat_Init(&timerModule);

      NanoSystemData_Init(&systemData);
      I_TinyDataSource_t *dataSource = NanoSystemData_DataSource(&systemData);

      GeaStack_Init(
         &geaStack,
         TinyUart_Uart1_Init(),
         dataSource,
         &timerModule,
         Stm8sGea2Address);

      Button_Init(dataSource, &timerModule, Erd_ButtonState);
      Led_Init(dataSource, Erd_LedState);

      NanoApplication_Init(&application, dataSource);
   }
   enableInterrupts();

   KickWatchdog(NULL, &timerModule);

   while(1)
   {
      TinyTimerModule_Run(&timerModule);
      GeaStack_Run(&geaStack);
   }
}
