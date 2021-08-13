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
#include "NanoApplication.h"
#include "NanoSystemData.h"
#include "Gea2Addresses.h"

static TinyTimerModule_t timerModule;
static NanoApplication_t application;
static NanoSystemData_t systemData;

int main(void)
{
   RamParityCheck_Disable();

   Interrupts_Disable();
   {
      Clock_Init();

      TinyTimerModule_Init(
         &timerModule,
         TinyTimeSource_Tau0Channel3_Init());

      Watchdog_Init(&timerModule);

      Heartbeat_Init(&timerModule);

      NanoSystemData_Init(&systemData);

      GeaStack_Init(
         &timerModule,
         NanoSystemData_ExternalDataSource(&systemData),
         TinyUart_Uart1_Init(),
         Rl78Gea2Address);

      NanoApplication_Init(
         &application,
         NanoSystemData_DataSource(&systemData));
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
