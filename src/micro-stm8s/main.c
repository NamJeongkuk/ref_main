/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <string.h>
#include "Clk.h"
#include "Iwdg.h"
#include "Pa4Heartbeat.h"
#include "TinyTimeSource_Tim4SystemTick.h"
#include "TinyUart_Uart2.h"
#include "TinyTimer.h"
#include "GeaStack.h"
#include "Gea2Addresses.h"
// #include "Ul.h"
#include "utils.h"

enum
{
   WatchdogKickPeriodInMsec = 1
};

static TinyTimerModule_t timerModule;
static TinyTimer_t periodicWatchdogTimer;
static GeaStack_t geaStack;

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

static void PopulateStartupMessage(void *context, Gea2Packet_t *packet)
{
   IGNORE(context);
   packet->destination = 0xFF;
   packet->payload[0] = 0x00;
   packet->payload[1] = 0xDE;
   packet->payload[2] = 0xAF;
   packet->payload[3] = 0xBA;
   packet->payload[4] = 0xBE;
}

// fixme marked for death
static void SendStartupMessage(I_TinyGea2Interface_t *gea2Interface)
{
   TinyGea2Interface_Send(gea2Interface, 5, PopulateStartupMessage, NULL);
}

void main(void)
{
#ifndef DISABLE_UL_CHECKS
   // Ul_Startup();
#endif
   Iwdg_Init();
   Clk_Init();

   disableInterrupts();
   {
      TinyTimerModule_Init(
         &timerModule,
         TinyTimeSource_Tim4SystemTick_Init());

      Pa4Heartbeat_Init(&timerModule);

      GeaStack_Init(
         &geaStack,
         TinyUart_Uart2_Init(),
         BonzalezGeaAddress);
   }
   enableInterrupts();

   SendStartupMessage(GeaStack_GetGea2Interface(&geaStack));
   KickWatchdog(NULL, &timerModule);

   while(1)
   {
      TinyTimerModule_Run(&timerModule);
      GeaStack_Run(&geaStack);
#ifndef DISABLE_UL_CHECKS
      // Ul_Run();
#endif
   }
}
