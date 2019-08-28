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
#include "TinyGeaStack.h"
#include "TinySystemData.h"
#include "Led.h"
#include "Button.h"
#include "SwitchedLedPlugin.h"
#include "I_TinyInterrupt.h"
#include "Gea2Addresses.h"
#include "utils.h"

enum
{
   WatchdogKickPeriodInMsec = 1
};

static TinyTimerModule_t timerModule;
static TinyTimer_t periodicWatchdogTimer;
static TinyGeaStack_t geaStack;
static TinySystemData_t systemData;
static SwitchedLedPlugin_t ledPlugin;

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

static const SwitchedLedPluginConfiguration_t ledPluginConfiguration =
   {
      .ledStateErd = Erd_LedState,
      .buttonStateErd = Erd_ButtonState
   };

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

      TinySystemData_Init(&systemData);
      I_TinyDataSource_t *dataSource = TinySystemData_DataSource(&systemData);

      TinyGeaStack_Init(
         &geaStack,
         TinyUart_Uart1_Init(),
         dataSource,
         BonzalezGeaAddress);

      Button_Init(dataSource, &timerModule, Erd_ButtonState);
      Led_Init(dataSource, Erd_LedState);
      SwitchedLedPlugin_Init(&ledPlugin, dataSource, &ledPluginConfiguration);
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
