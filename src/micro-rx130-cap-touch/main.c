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
#include "Heartbeat.h"
#include "GeaStack.h"
#include "TinyUart_Uart6.h"
#include "Application.h"
#include "SystemData.h"
#include "Gea2Addresses.h"
#include "utils.h"

int main(void)
{
   TinyTimerModule_t timerModule;

   Interrupts_Disable();
   {
      Clock_Init();

      TinyTimerModule_Init(
         &timerModule,
         TinyTimeSource_Cmt0_Init());

      Watchdog_Init(&timerModule);

      Heartbeat_Init(&timerModule);

      SystemData_Init(&timerModule);

      GeaStack_Init(
         &timerModule,
         SystemData_ExternalDataSource(),
         TinyUart_Uart6_Init(),
         Rx130Gea2Address);
   }
   Interrupts_Enable();

   Application_Init(
      SystemData_DataSource(),
      SystemData_ExternalDataSource(),
      GeaStack_Gea2Interface());

   while(1)
   {
      TinyTimerModule_Run(&timerModule);
      GeaStack_Run();
   }
}
