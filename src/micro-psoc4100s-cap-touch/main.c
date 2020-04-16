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
#include "TinySingleErdHeartbeatStream.h"
#include "TinyGea2Interface_FullDuplex.h"

enum
{
   WatchdogKickPeriodInMsec = 1
};

static TinyTimerModule_t timerModule;
static TinyTimer_t periodicWatchdogTimer;
static TinyGea2Interface_FullDuplex_t gea2Interface;
static uint8_t sendBuffer[100];
static uint8_t receiveBuffer[100];

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

   CyGlobalIntDisable;
   {
      TinyTimerModule_Init(&timerModule, TinyTimeSource_SysTick_Init());
      TinyGea2Interface_FullDuplex_Init(
         &gea2Interface,
         TinyUart_Gea3_Init(),
         0xBD,
         sendBuffer,
         sizeof(sendBuffer),
         receiveBuffer,
         sizeof(receiveBuffer),
         false);
      TinySingleErdHeartbeatStream_Init(&gea2Interface.interface, &timerModule);
   }
   CyGlobalIntEnable;

   CapTouchKeys_Init(&timerModule);

   KickWatchdog(NULL, &timerModule);

   while(1)
   {
      TinyTimerModule_Run(&timerModule);
      TinyGea2Interface_FullDuplex_Run(&gea2Interface);
   }
}
