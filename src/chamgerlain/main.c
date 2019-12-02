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
#include "Pd0Heartbeat.h"
#include "Rst.h"
#include "TinyTimeSource_Tim2SystemTick.h"
#include "TinyAdcGroup_Adc1.h"
#include "TinyUart_Uart2.h"
#include "TinyTimer.h"
#include "TinyGeaStackWithSingleErdHeartbeat.h"
#include "Led.h"
#include "CapSenseButton.h"
#include "I_TinyInterrupt.h"
#include "Gea2Addresses.h"
#include "utils.h"
#include "stm8_tsl_timebase.h"
#include "Version.h"
#include <string.h>

enum
{
   WatchdogKickPeriodInMsec = 1
};

static TinyTimerModule_t timerModule;
static TinyTimer_t periodicWatchdogTimer;
static TinyGeaStackWithSingleErdHeartbeat_t geaStack;

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
         TinyTimeSource_Tim2SystemTick_Init());

      Pd0Heartbeat_Init(&timerModule);

      TinyGeaStackWithSingleErdHeartbeat_Init(
         &geaStack,
         TinyUart_Uart2_Init(),
         &timerModule,
         ChamgerlainGeaAddress);

      CapSenseButton_Init(&timerModule);
   }
   enableInterrupts();

   KickWatchdog(NULL, &timerModule);

   while(1)
   {
      TinyTimerModule_Run(&timerModule);
      TinyGeaStackWithSingleErdHeartbeat_Run(&geaStack);
      TSL_Action();
   }
}

// CRC needs to be placed at the end of ROM and given a dummy value
static const uint16_t __at(FLASH_PROG_END_PHYSICAL_ADDRESS - 1) crc = 0xDEAD;

// ...and version is placed just before CRC
const Version_t __at(FLASH_PROG_END_PHYSICAL_ADDRESS - 5) version =
   {
      CRIT_VERSION_MAJOR,
      CRIT_VERSION_MINOR,
      NONCRIT_VERSION_MAJOR,
      NONCRIT_VERSION_MINOR
   };
