/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "UlTestsPlugin.h"
#include "UlStartupTest.h"
#include "UlRamTest.h"
#include "UlRomTest.h"
#include "UlCpuTest.h"
#include "Reset.h"
#include "utils.h"

enum
{
   State_RamTest,
   State_CpuTest,
   State_RomTest,
   State_Watchdog,
   State_Max
};

static struct
{
   TinyTimerModule_t *timerModule;
   TinyTimer_t testTimer;
   TinyTimer_t resourceWatchdogTimer;
   TinyTimerTicks_t testPeriod;
   TinyTimerTicks_t resourceWatchdogTimeout;
   uint8_t state;
} instance;

static void RunTest(void *_instance, TinyTimerModule_t *timerModule);

static void ResourceWatchdogTimedOut(void *context, TinyTimerModule_t *timerModule)
{
   IGNORE(timerModule);
   Reset();
}

static void KickResourceWatchdog(void)
{
   TinyTimerModule_Start(
      instance.timerModule,
      &instance.resourceWatchdogTimer,
      instance.resourceWatchdogTimeout,
      ResourceWatchdogTimedOut,
      NULL);
}

static void ArmTestTimer(void)
{
   TinyTimerModule_Start(
      instance.timerModule,
      &instance.testTimer,
      instance.testPeriod,
      RunTest,
      NULL);
}

static void RunTest(void *_instance, TinyTimerModule_t *timerModule)
{
   IGNORE(_instance);
   IGNORE(timerModule);

   switch(instance.state)
   {
      case State_RamTest:
         UlRamTest_Run();
         break;

      case State_CpuTest:
         UlCpuTest_Run();
         break;

      case State_RomTest:
         UlRomTest_Run();
         break;

      case State_Watchdog:
         KickResourceWatchdog();
         break;

      default:
         Reset();
         break;
   }

   instance.state = (instance.state + 1) % State_Max;

   ArmTestTimer();
}

void UlTestsPlugin_Init(
   TinyTimerModule_t *timerModule,
   const ImageHeader_t *applicationHeader,
   bool romCheckErrorEnabled,
   TinyTimerTicks_t testPeriod,
   TinyTimerTicks_t resourceWatchdogTimeout,
   uint16_t bytesToCrcPerRomCheck)
{
   instance.timerModule = timerModule;
   instance.testPeriod = testPeriod;
   instance.resourceWatchdogTimeout = resourceWatchdogTimeout;

   UlRamTest_Init();

   UlCpuTest_Init();

   UlRomTest_Init(
      applicationHeader,
      bytesToCrcPerRomCheck,
      romCheckErrorEnabled);

   instance.state = 0;

   UlStartupTest_Run();

   KickResourceWatchdog();
   ArmTestTimer();
}
