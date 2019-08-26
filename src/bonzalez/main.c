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
#include "I_TinyInterrupt.h"
#include "TinyGeaStack.h"
#include "Gea2Addresses.h"
#include "utils.h"

enum
{
   WatchdogKickPeriodInMsec = 1
};

TinyTimerModule_t timerModule;
TinyTimer_t periodicWatchdogTimer;
TinyGeaStack_t geaStack;

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

static void SendStartupMessage(I_TinyGea2Interface_t *gea2Interface)
{
   STACK_ALLOC_GEA2PACKET(packet, 5);
   packet->destination = 0xFF;
   packet->payload[0] = 0x00;
   packet->payload[1] = 0xDE;
   packet->payload[2] = 0xAF;
   packet->payload[3] = 0xBA;
   packet->payload[4] = 0xBE;
  TinyGea2Interface_Send(gea2Interface, packet);
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

      TinyGeaStack_Init(&geaStack, TinyUart_Uart1_Init(), BonzalezGeaAddress);
   }
   enableInterrupts();

   SendStartupMessage(TinyGeaStack_GetGea2Interface(&geaStack));
   KickWatchdog(NULL, &timerModule);

   while(1)
   {
      TinyTimerModule_Run(&timerModule);
      TinyGeaStack_Run(&geaStack);
   }
}
