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
#include "utils.h"
#include "TinyTimer.h"
#include "I_TinyInterrupt.h"

enum
{
   WatchdogKickPeriodInMsec = 1
};

TinyTimerModule_t timerModule;
TinyTimer_t periodicWatchdogTimer;

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
   }
   enableInterrupts();

   KickWatchdog(NULL, &timerModule);

   while(1)
   {
      TinyTimerModule_Run(&timerModule);
   }
}
