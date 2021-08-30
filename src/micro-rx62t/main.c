/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <stddef.h>
#include "Interrupts.h"
#include "Clock.h"
#include "Watchdog.h"
#include "TinyTimeSource_Cmt0.h"
#include "TinyTimer.h"
#include "HeartbeatLed.h"
#include "GeaStack.h"
#include "TinyUart_Uart0.h"
#include "MicroApplication.h"
#include "MicroSystemData.h"
#include "Gea2Addresses.h"
#include "utils.h"

static TinyTimerModule_t timerModule;

int main(void)
{
   Interrupts_Disable();
   {
      Clock_Init();

      TinyTimerModule_Init(
         &timerModule,
         TinyTimeSource_Cmt0_Init());

      Watchdog_Init();
      Watchdog_EnableAutoKick(&timerModule);

      HeartbeatLed_Init(&timerModule);

      MicroSystemData_Init(&timerModule);

      GeaStack_Init(
         &timerModule,
         MicroSystemData_ExternalDataSource(),
         TinyUart_Uart0_Init(),
         Rx62tGea2Address);

      MicroApplication_Init(
         MicroSystemData_DataSource(),
         GeaStack_GetGea2Interface());
   }
   Interrupts_Enable();

   while(1)
   {
      GeaStack_Run();

      if(!TinyTimerModule_Run(&timerModule))
      {
         Interrupts_WaitForInterrupt();
      }
   }
}
