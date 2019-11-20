/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "Clk.h"
#include "Iwdg.h"
#include "Pd0Heartbeat.h"
#include "Rst.h"
#include "TinyTimeSource_Tim2SystemTick.h"
#include "TinyAdcGroup_Adc1.h"
#include "TinyUart_Uart2.h"
#include "TinyTimer.h"
#include "TinyGeaStack.h"
#include "TinySystemData.h"
#include "TinyApplication.h"
#include "Led.h"
#include "CapSenseButton.h"
#include "I_TinyInterrupt.h"
#include "Gea2Addresses.h"
#include "utils.h"
#include "stm8_tsl_timebase.h"
#include <string.h>

enum
{
   WatchdogKickPeriodInMsec = 1
};

static TinyTimerModule_t timerModule;
static TinyTimer_t periodicWatchdogTimer;
static TinyGeaStack_t geaStack;
static TinySystemData_t systemData;
static TinyApplication_t application;

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

static void SendStartupMessage(I_TinyGea2Interface_t *gea2Interface)
{
   TinyGea2Interface_Send(gea2Interface, 5, PopulateStartupMessage, NULL);
}

void main(void)
{
   Iwdg_Init();
   Clk_Init();

   disableInterrupts();
   {
      TinyTimerModule_Init(
         &timerModule,
         TinyTimeSource_Tim2SystemTick_Init());

      Pd0Heartbeat_Init(&timerModule);

      TinySystemData_Init(&systemData);
      I_TinyDataSource_t *dataSource = TinySystemData_DataSource(&systemData);

      TinyGeaStack_Init(
         &geaStack,
         TinyUart_Uart2_Init(),
         dataSource,
         &timerModule,
         BonzalezGeaAddress);

      CapSenseButton_Init(dataSource, &timerModule, Erd_ButtonState);
      Led_Init(dataSource, Erd_LedState);

      TinyApplication_Init(&application, dataSource);
   }
   enableInterrupts();

   SendStartupMessage(TinyGeaStack_GetGea2Interface(&geaStack));
   KickWatchdog(NULL, &timerModule);

   while(1)
   {
      TinyTimerModule_Run(&timerModule);
      TinyGeaStack_Run(&geaStack);
      TSL_Action();
   }
}

// CRC needs to be placed at the end of ROM and given a dummy value
static const uint16_t __at(0xFFFE) crc = 0xDEAD;
