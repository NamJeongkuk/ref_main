/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <string.h>
#include "stm8s_flash.h"
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
#include "TinyApplication.h"
#include "Led.h"
#include "Button.h"
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
         TinyTimeSource_Tim4SystemTick_Init());

      Pd4Heartbeat_Init(&timerModule);

      TinySystemData_Init(&systemData);
      I_TinyDataSource_t *dataSource = TinySystemData_DataSource(&systemData);

      TinyGeaStack_Init(
         &geaStack,
         TinyUart_Uart1_Init(),
         dataSource,
         &timerModule,
         BonzalezGeaAddress);

      Button_Init(dataSource, &timerModule, Erd_ButtonState);
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
   }
}

const Version_t __at(FLASH_PROG_END_PHYSICAL_ADDRESS - 5) version =
   {
      CRIT_VERSION_MAJOR,
      CRIT_VERSION_MINOR,
      NONCRIT_VERSION_MAJOR,
      NONCRIT_VERSION_MINOR
   };

// CRC needs to be placed at the end of ROM and given a dummy value
static const uint16_t __at(FLASH_PROG_END_PHYSICAL_ADDRESS - 1) crc = 0xDEAD;
