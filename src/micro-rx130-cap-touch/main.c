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
#include "SystemErds.h"
#include "Gea2Addresses.h"
#include "TinyStackUsageService.h"
#include "utils.h"

static TinyTimerModule_t timerModule;
static TinyStackUsageService_t stackUsageService;

extern char _stackStart;
extern char _stackEnd;
static const TinyStackUsageServiceConfiguration_t stackUsageServiceConfiguration = {
   .start = (uintptr_t)&_stackStart,
   .end = (uintptr_t)&_stackEnd,
   .percentFreeErd = Erd_StackUsagePercentFree
};

int main(void)
{
   Interrupts_Disable();
   {
      Clock_Init();

      TinyStackUsageService_InitializeStack(
         &stackUsageServiceConfiguration);

      TinyTimerModule_Init(
         &timerModule,
         TinyTimeSource_Cmt0_Init());

      Watchdog_Init(&timerModule);

      Heartbeat_Init(&timerModule);

      SystemData_Init(&timerModule);

      TinyStackUsageService_Init(
         &stackUsageService,
         &timerModule,
         SystemData_DataSource(),
         &stackUsageServiceConfiguration);

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
      GeaStack_Run();

      if(!TinyTimerModule_Run(&timerModule))
      {
         Interrupts_WaitForInterrupt();
      }
   }
}
