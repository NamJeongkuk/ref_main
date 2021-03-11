/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <stddef.h>
#include "Interrupts.h"
#include "RamParityCheck.h"
#include "Clock.h"
#include "Watchdog.h"
#include "TinyTimeSource_Tau0Channel3.h"
#include "TinyTimer.h"
#include "Heartbeat.h"
#include "GeaStack.h"
#include "TinyUart_Uart1.h"
#include "VectorTable.h"
#include "ApplicationVectorTable.h"
#include "MicroApplication.h"
#include "MicroSystemData.h"
#include "Gea2Addresses.h"
#include "MemoryMap.h"
#include "UlTestsPlugin.h"

enum
{
   UlTestPeriod = 10,
   UlTestResourceWatchdogTimeout = 1000,
   UlBytesToCheckPerRomCheck = 10,

#ifdef DEBUG
   RomCheckErrorEnabled = false,
#else
   RomCheckErrorEnabled = true
#endif
};

static TinyTimerModule_t timerModule;

int main(void)
{
   RamParityCheck_Disable();

   Interrupts_Disable();
   {
      VectorTable_SetVectorTable(appVectors);

      Clock_Init();

      TinyTimerModule_Init(
         &timerModule,
         TinyTimeSource_Tau0Channel3_Init());

      Watchdog_Init(&timerModule);

      Heartbeat_Init(&timerModule);

      MicroSystemData_Init(&timerModule);

      GeaStack_Init(
         &timerModule,
         MicroSystemData_ExternalDataSource(),
         TinyUart_Uart1_Init(),
         Rl78Gea2Address);

      MicroApplication_Init(
         MicroSystemData_DataSource(),
         GeaStack_GetGea2Interface());

      UlTestsPlugin_Init(
         &timerModule,
         ApplicationImageHeader,
         RomCheckErrorEnabled,
         UlTestPeriod,
         UlTestResourceWatchdogTimeout,
         UlBytesToCheckPerRomCheck);
   }
   Interrupts_Enable();

   while(1)
   {
      TinyTimerModule_Run(&timerModule);
      GeaStack_Run();
   }
}
