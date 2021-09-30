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
#include "MicroSystemErds.h"
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

      Watchdog_Init();
      Watchdog_EnableAutoKick(&timerModule);

      HeartbeatLed_Init(&timerModule);

      MicroSystemData_Init(&timerModule);

      TinyStackUsageService_Init(
         &stackUsageService,
         &timerModule,
         MicroSystemData_DataSource(),
         &stackUsageServiceConfiguration);

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
