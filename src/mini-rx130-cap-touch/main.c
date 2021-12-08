/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <stddef.h>
#include "Interrupts.h"
#include "Clock.h"
#include "Action_RxWatchdog.h"
#include "TimeSource_Cmt0.h"
#include "Timer.h"
#include "Heartbeat.h"
#include "GeaStack.h"
#include "BufferedUart_Channel6.h"
#include "Application.h"
#include "SystemData.h"
#include "Gea2Addresses.h"
#include "InvokeActionOnTimerPeriodic.h"
#include "Action_JumpToBootLoader.h"
#include "Action_Rx2xxSystemReset.h"
#include "Dtc.h"
#include "utils.h"

int main(void)
{
   TimerModule_t timerModule;
   InvokeActionOnTimerPeriodic_t watchdogPetter;
   GeaStack_t geaStack;

   Interrupts_Disable();
   {
      Clock_Init();

      Dtc_Init();

      TimerModule_Init(
         &timerModule,
         TimeSource_Cmt0_Init());

      I_Action_t *watchdogKickAction = Action_RxWatchdog_InitWithConfiguration(Action_RxWatchdogConfiguration_2Seconds);
      InvokeActionOnTimerPeriodic_Init(
         &watchdogPetter,
         watchdogKickAction,
         &timerModule,
         1);

      Heartbeat_Init(&timerModule);

      SystemData_Init(&timerModule);

      GeaStack_Init(
         &geaStack,
         SystemData_DataSource(),
         SystemData_ExternalDataSource(),
         BufferedUart_Channel6_Init(),
         Action_JumpToBootLoader_Init(),
         Action_Rx2xxSystemReset_Init(),
         Rx130Gea2Address);
   }
   Interrupts_Enable();

   Application_Init(
      SystemData_DataSource(),
      SystemData_ExternalDataSource(),
      GeaStack_Gea2PacketEndpoint(&geaStack));

   while(1)
   {
      GeaStack_Run(&geaStack);

      if(!TimerModule_Run(&timerModule))
      {
         Interrupts_WaitForInterrupt();
      }
   }
}
