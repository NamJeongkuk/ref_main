/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "Signal.h"
#include "TimeSource_SystemTickWithAcceleration.h"
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

static const TimeSource_SystemTickWithAcceleration_Config_t config = {
   .accelerationEnabledErd = Erd_TimeAcceleration_Enable,
   .ticksRemainingInAccelerationErd = Erd_TimeAcceleration_Ticks,
   .completeSignalErd = Erd_TimeAcceleration_CompleteSignal,
   .maxTicksPerRtc = MaxTicksToSkipAtOnce,
};

TEST_GROUP(TimeSource_SystemTickWithAcceleration)
{
   DataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;

   Interrupt_TestDouble_t sysTickTestDouble;

   TimeSource_SystemTickWithAcceleration_t timeSource_SystemTickWithAcceleration;

   void setup()
   {
      DataModel_TestDouble_Init(&dataModelTestDouble, erds, NUM_ELEMENTS(erds));
      dataModel = &dataModelTestDouble.interface;

      Interrupt_TestDouble_Init(&sysTickTestDouble);
   }

   void TheModuleIsInitialized()
   {
      TimeSource_SystemTickWithAcceleration_Init(
         &timeSource_SystemTickWithAcceleration,
         &config,
         &sysTickTestDouble.interface);
      TimeSource_SystemTickWithAcceleration_SetDataModel(
         &timeSource_SystemTickWithAcceleration,
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
         TimeSource_GetTicks(&timeSource_SystemTickWithAcceleration.interface);
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

   void TheSystemTickWithAccelerationIsRun(void)
   {
      TimeSource_SystemTickWithAcceleration_Run(
         &timeSource_SystemTickWithAcceleration);
   }
};

TEST(TimeSource_SystemTickWithAcceleration, ShouldCountTicks)
{
   Given TheModuleIsInitialized();
   TickCountShouldBe(0);

   When TheSysTickInterruptFires();
   TickCountShouldBe(1);

   When TheSysTickInterruptFires();
   TickCountShouldBe(2);
}

TEST(TimeSource_SystemTickWithAcceleration, RunShouldDoNothingWhenAccelerationIsDisabled)
{
   Given TheModuleIsInitialized();
   Given AccelerationIs(Disabled);

   When TheSystemTickWithAccelerationIsRun();
   TickCountShouldBe(0);

   When TheSysTickInterruptFires();
   TickCountShouldBe(1);
}

TEST(TimeSource_SystemTickWithAcceleration, AccelerationShouldBeDisabledIfEnabledWithZeroTicksRemaining)
{
   Given TheModuleIsInitialized();
   Given AccelerationIs(Enabled);
   Given TicksRemainingIs(0);

   When TheSystemTickWithAccelerationIsRun();
   AccelerationShouldBe(Disabled);
}

TEST(TimeSource_SystemTickWithAcceleration, AccelerationShouldDoNothingIfDisabledWithTicksRemaining)
{
   Given TheModuleIsInitialized();
   Given AccelerationIs(Disabled);
   Given TicksRemainingIs(SomeNumberOfTicks);

   When TheSystemTickWithAccelerationIsRun();
   TicksRemainingIs(SomeNumberOfTicks);
   TickCountShouldBe(0);
}

TEST(TimeSource_SystemTickWithAcceleration, ShouldCompleteAccelerationInOneRunIfEnabledAndTicksRemainingIsLessThanMax)
{
   Given TheModuleIsInitialized();
   Given AccelerationIs(Enabled);
   Given TicksRemainingIs(MaxTicksToSkipAtOnce - 1);

   When TheSystemTickWithAccelerationIsRun();
   TicksRemainingShouldBe(0);
   AccelerationShouldBe(Disabled);
   TickCountShouldBe(MaxTicksToSkipAtOnce - 1);
}

TEST(TimeSource_SystemTickWithAcceleration, ShouldCompleteAccelerationInOneRunIfEnabledAndTicksRemainingIsEqualToMax)
{
   Given TheModuleIsInitialized();
   Given AccelerationIs(Enabled);
   Given TicksRemainingIs(MaxTicksToSkipAtOnce);

   When TheSystemTickWithAccelerationIsRun();
   TicksRemainingShouldBe(0);
   AccelerationShouldBe(Disabled);
   TickCountShouldBe(MaxTicksToSkipAtOnce);
}

TEST(TimeSource_SystemTickWithAcceleration, ShouldCompleteAccelerationInMultipleRunsIfEnabledAndTicksRemainingIsGreaterThanMax)
{
   Given TheModuleIsInitialized();
   Given AccelerationIs(Enabled);
   Given TicksRemainingIs(MaxTicksToSkipAtOnce + SomeNumberOfTicks);

   When TheSystemTickWithAccelerationIsRun();
   TicksRemainingShouldBe(SomeNumberOfTicks);
   TickCountShouldBe(MaxTicksToSkipAtOnce);

   When TheSystemTickWithAccelerationIsRun();
   TicksRemainingShouldBe(0);
   AccelerationShouldBe(Disabled);
   TickCountShouldBe(MaxTicksToSkipAtOnce + SomeNumberOfTicks);
}

TEST(TimeSource_SystemTickWithAcceleration, TicksBeforeEnableShouldBeRetained)
{
   Given TheModuleIsInitialized();

   When TheSysTickInterruptFires();
   TickCountShouldBe(1);

   Given AccelerationIs(Enabled);
   Given TicksRemainingIs(MaxTicksToSkipAtOnce);

   When TheSystemTickWithAccelerationIsRun();
   TicksRemainingShouldBe(0);
   AccelerationShouldBe(Disabled);
   TickCountShouldBe(MaxTicksToSkipAtOnce + 1);
}

TEST(TimeSource_SystemTickWithAcceleration, TicksBetweenRunsShouldAddToTickCount)
{
   Given TheModuleIsInitialized();
   Given AccelerationIs(Enabled);
   Given TicksRemainingIs(MaxTicksToSkipAtOnce + SomeNumberOfTicks);

   When TheSystemTickWithAccelerationIsRun();
   TicksRemainingShouldBe(SomeNumberOfTicks);
   TickCountShouldBe(MaxTicksToSkipAtOnce);

   When TheSysTickInterruptFires();
   TickCountShouldBe(MaxTicksToSkipAtOnce + 1);

   When TheSystemTickWithAccelerationIsRun();
   TicksRemainingShouldBe(0);
   AccelerationShouldBe(Disabled);
   TickCountShouldBe(MaxTicksToSkipAtOnce + SomeNumberOfTicks + 1);
}

TEST(TimeSource_SystemTickWithAcceleration, ShouldSignalWhenAccelerationIsComplete)
{
   Given TheModuleIsInitialized();
   Given AccelerationIs(Enabled);
   Given TicksRemainingIs(MaxTicksToSkipAtOnce);
   Given CompleteSignalIs(0);

   When TheSystemTickWithAccelerationIsRun();
   CompleteSignalShouldBe(1);
}
