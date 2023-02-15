/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "Signal.h"
#include "TimeSource_SysTickWithAcceleration.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "uassert_test.h"
#include "DataModel_TestDouble.h"
#include "Interrupt_TestDouble.h"
#include "SystemErds.h"

#define Given
#define When

enum
{
   MaxTicksToSkipAtOnce = 100,
   SomeNumberOfTicks = 23,
   Enabled = true,
   Disabled = false,
};

static const DataModel_TestDoubleConfigurationEntry_t erds[] = {
   { Erd_TimeAcceleration_Enable, sizeof(bool) },
   { Erd_TimeAcceleration_Ticks, sizeof(uint32_t) },
   { Erd_TimeAcceleration_CompleteSignal, sizeof(Signal_t) },
};

static const TimeSource_SysTickWithAcceleration_Config_t config = {
   .accelerationEnabledErd = Erd_TimeAcceleration_Enable,
   .ticksRemainingInAccelerationErd = Erd_TimeAcceleration_Ticks,
   .completeSignalErd = Erd_TimeAcceleration_CompleteSignal,
   .maxTicksPerRtc = MaxTicksToSkipAtOnce,
};

TEST_GROUP(TimeSource_SysTickWithAcceleration)
{
   DataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;

   Interrupt_TestDouble_t sysTickTestDouble;

   TimeSource_SysTickWithAcceleration_t timeSource_SysTickWithAcceleration;

   void setup()
   {
      DataModel_TestDouble_Init(&dataModelTestDouble, erds, NUM_ELEMENTS(erds));
      dataModel = &dataModelTestDouble.interface;

      Interrupt_TestDouble_Init(&sysTickTestDouble);
   }

   void TheModuleIsInitialized()
   {
      TimeSource_SysTickWithAcceleration_Init(
         &timeSource_SysTickWithAcceleration,
         &config,
         &sysTickTestDouble.interface);
      TimeSource_SysTickWithAcceleration_SetDataModel(
         &timeSource_SysTickWithAcceleration,
         dataModel);
   }

   void AccelerationIs(bool value)
   {
      DataModel_Write(dataModel, Erd_TimeAcceleration_Enable, &value);
   }

   void AccelerationShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_TimeAcceleration_Enable, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void TicksRemainingIs(uint32_t value)
   {
      DataModel_Write(dataModel, Erd_TimeAcceleration_Ticks, &value);
   }

   void TicksRemainingShouldBe(uint32_t expected)
   {
      uint32_t actual;
      DataModel_Read(dataModel, Erd_TimeAcceleration_Ticks, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void TickCountShouldBe(TimeSourceTickCount_t expected)
   {
      TimeSourceTickCount_t actual =
         TimeSource_GetTicks(&timeSource_SysTickWithAcceleration.interface);
      CHECK_EQUAL(expected, actual);
   }

   void CompleteSignalIs(Signal_t signal)
   {
      DataModel_Write(dataModel, Erd_TimeAcceleration_CompleteSignal, &signal);
   }

   void CompleteSignalShouldBe(Signal_t expected)
   {
      Signal_t actual;
      DataModel_Read(dataModel, Erd_TimeAcceleration_CompleteSignal, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void TheSysTickInterruptFires(void)
   {
      Interrupt_TestDouble_TriggerInterrupt(&sysTickTestDouble);
   }

   void TheSysTickWithAccelerationIsRun(void)
   {
      TimeSource_SysTickWithAcceleration_Run(
         &timeSource_SysTickWithAcceleration);
   }
};

TEST(TimeSource_SysTickWithAcceleration, ShouldCountTicks)
{
   Given TheModuleIsInitialized();
   TickCountShouldBe(0);

   When TheSysTickInterruptFires();
   TickCountShouldBe(1);

   When TheSysTickInterruptFires();
   TickCountShouldBe(2);
}

TEST(TimeSource_SysTickWithAcceleration, RunShouldDoNothingWhenAccelerationIsDisabled)
{
   Given TheModuleIsInitialized();
   Given AccelerationIs(Disabled);

   When TheSysTickWithAccelerationIsRun();
   TickCountShouldBe(0);

   When TheSysTickInterruptFires();
   TickCountShouldBe(1);
}

TEST(TimeSource_SysTickWithAcceleration, AccelerationShouldBeDisabledIfEnabledWithZeroTicksRemaining)
{
   Given TheModuleIsInitialized();
   Given AccelerationIs(Enabled);
   Given TicksRemainingIs(0);

   When TheSysTickWithAccelerationIsRun();
   AccelerationShouldBe(Disabled);
}

TEST(TimeSource_SysTickWithAcceleration, AccelerationShouldDoNothingIfDisabledWithTicksRemaining)
{
   Given TheModuleIsInitialized();
   Given AccelerationIs(Disabled);
   Given TicksRemainingIs(SomeNumberOfTicks);

   When TheSysTickWithAccelerationIsRun();
   TicksRemainingIs(SomeNumberOfTicks);
   TickCountShouldBe(0);
}

TEST(TimeSource_SysTickWithAcceleration, ShouldCompleteAccelerationInOneRunIfEnabledAndTicksRemainingIsLessThanMax)
{
   Given TheModuleIsInitialized();
   Given AccelerationIs(Enabled);
   Given TicksRemainingIs(MaxTicksToSkipAtOnce - 1);

   When TheSysTickWithAccelerationIsRun();
   TicksRemainingShouldBe(0);
   AccelerationShouldBe(Disabled);
   TickCountShouldBe(MaxTicksToSkipAtOnce - 1);
}

TEST(TimeSource_SysTickWithAcceleration, ShouldCompleteAccelerationInOneRunIfEnabledAndTicksRemainingIsEqualToMax)
{
   Given TheModuleIsInitialized();
   Given AccelerationIs(Enabled);
   Given TicksRemainingIs(MaxTicksToSkipAtOnce);

   When TheSysTickWithAccelerationIsRun();
   TicksRemainingShouldBe(0);
   AccelerationShouldBe(Disabled);
   TickCountShouldBe(MaxTicksToSkipAtOnce);
}

TEST(TimeSource_SysTickWithAcceleration, ShouldCompleteAccelerationInMultipleRunsIfEnabledAndTicksRemainingIsGreaterThanMax)
{
   Given TheModuleIsInitialized();
   Given AccelerationIs(Enabled);
   Given TicksRemainingIs(MaxTicksToSkipAtOnce + SomeNumberOfTicks);

   When TheSysTickWithAccelerationIsRun();
   TicksRemainingShouldBe(SomeNumberOfTicks);
   TickCountShouldBe(MaxTicksToSkipAtOnce);

   When TheSysTickWithAccelerationIsRun();
   TicksRemainingShouldBe(0);
   AccelerationShouldBe(Disabled);
   TickCountShouldBe(MaxTicksToSkipAtOnce + SomeNumberOfTicks);
}

TEST(TimeSource_SysTickWithAcceleration, TicksBeforeEnableShouldBeRetained)
{
   Given TheModuleIsInitialized();

   When TheSysTickInterruptFires();
   TickCountShouldBe(1);

   Given AccelerationIs(Enabled);
   Given TicksRemainingIs(MaxTicksToSkipAtOnce);

   When TheSysTickWithAccelerationIsRun();
   TicksRemainingShouldBe(0);
   AccelerationShouldBe(Disabled);
   TickCountShouldBe(MaxTicksToSkipAtOnce + 1);
}

TEST(TimeSource_SysTickWithAcceleration, TicksBetweenRunsShouldAddToTickCount)
{
   Given TheModuleIsInitialized();
   Given AccelerationIs(Enabled);
   Given TicksRemainingIs(MaxTicksToSkipAtOnce + SomeNumberOfTicks);

   When TheSysTickWithAccelerationIsRun();
   TicksRemainingShouldBe(SomeNumberOfTicks);
   TickCountShouldBe(MaxTicksToSkipAtOnce);

   When TheSysTickInterruptFires();
   TickCountShouldBe(MaxTicksToSkipAtOnce + 1);

   When TheSysTickWithAccelerationIsRun();
   TicksRemainingShouldBe(0);
   AccelerationShouldBe(Disabled);
   TickCountShouldBe(MaxTicksToSkipAtOnce + SomeNumberOfTicks + 1);
}

TEST(TimeSource_SysTickWithAcceleration, ShouldSignalWhenAccelerationIsComplete)
{
   Given TheModuleIsInitialized();
   Given AccelerationIs(Enabled);
   Given TicksRemainingIs(MaxTicksToSkipAtOnce);
   Given CompleteSignalIs(0);

   When TheSysTickWithAccelerationIsRun();
   CompleteSignalShouldBe(1);
}
