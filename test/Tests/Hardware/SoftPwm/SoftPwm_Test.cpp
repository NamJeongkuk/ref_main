/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "SoftPwm.h"
#include "PwmDutyCycle.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"
#include "DataModelErdPointerAccess.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "DataModel_TestDouble.h"
#include "TimerModule_TestDouble.h"
#include "uassert_test.h"

#define Given
#define When
#define And
#define Then

enum
{
   ZeroPwmDutyCycle = 0,
   SomePositivePwmDutyCycle = 12345,
   AnotherPositivePwmDutyCycle = 20000,
   SomeFrequencyX100 = 10,
   SomeNumberOfCycles = 10
};

enum
{
   SomePeriodInMs = SomeFrequencyX100 * MSEC_PER_SEC,
   SomeOnCycleInMs = ((SomePositivePwmDutyCycle * SomeFrequencyX100 * MSEC_PER_SEC) / PwmDutyCycle_Max),
   AnotherOnCycleInMs = ((AnotherPositivePwmDutyCycle * SomeFrequencyX100 * MSEC_PER_SEC) / PwmDutyCycle_Max)
};

enum
{
   Erd_SomePwmDutyCycle,
   Erd_BspGpio,
   Erd_TimerModule
};

static const DataModel_TestDoubleConfigurationEntry_t erds[] = {
   { Erd_SomePwmDutyCycle, sizeof(PwmDutyCycle_t) },
   { Erd_BspGpio, sizeof(bool) },
   { Erd_TimerModule, sizeof(TimerModule_t *) }
};

static const SoftPwmConfiguration_t config = {
   .pwmDutyCycleErd = Erd_SomePwmDutyCycle,
   .gpioErd = Erd_BspGpio,
   .timerModuleErd = Erd_TimerModule,
   .frequencyX100 = SomeFrequencyX100
};

TEST_GROUP(SoftPwm)
{
   DataModel_TestDouble_t dataModelTestDouble;
   TimerModule_TestDouble_t timerModuleTestDouble;
   I_DataModel_t *dataModel;
   TimerModule_t timerModule;
   SoftPwm_t instance;

   void setup()
   {
      DataModel_TestDouble_Init(&dataModelTestDouble, erds, NUM_ELEMENTS(erds));
      dataModel = dataModelTestDouble.dataModel;

      TimerModule_TestDouble_Init(&timerModuleTestDouble);
      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &timerModuleTestDouble.timerModule);
   }

   void ModuleIsInitialized()
   {
      SoftPwm_Init(&instance, dataModel, &config);
   }

   void PwmDutyCycleIs(PwmDutyCycle_t pwmDutyCycle)
   {
      DataModel_Write(dataModel, Erd_SomePwmDutyCycle, &pwmDutyCycle);
   }

   void GpioShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_BspGpio, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void After(TimerTicks_t ticks, TimeSourceTickCount_t ticksToElapseAtATime = 1000)
   {
      TimerModule_TestDouble_ElapseTime(&timerModuleTestDouble, ticks, ticksToElapseAtATime);
   }

   void GpioJustStartedACycleAndIsOn()
   {
      Given ModuleIsInitialized();

      When PwmDutyCycleIs(SomePositivePwmDutyCycle);
      GpioShouldBe(ON);
   }

   void GpioJustTurnedOffAfterBeingOn()
   {
      Given ModuleIsInitialized();

      When PwmDutyCycleIs(SomePositivePwmDutyCycle);
      GpioShouldBe(ON);

      After(SomeOnCycleInMs - 1);
      GpioShouldBe(ON);

      After(1);
      GpioShouldBe(OFF);
   }

   void GpioCompletedOneCycleOfOnThenOffAndIsOnAgain()
   {
      Given GpioJustTurnedOffAfterBeingOn();

      After((SomeFrequencyX100 * MSEC_PER_SEC - SomeOnCycleInMs) - 1);
      GpioShouldBe(OFF);

      After(1);
      GpioShouldBe(ON);
   }

   void GpioCyclesOnThenOffThenOnAgain()
   {
      After(SomeOnCycleInMs - 1);
      GpioShouldBe(ON);

      After(1);
      GpioShouldBe(OFF);

      After((SomeFrequencyX100 * MSEC_PER_SEC - SomeOnCycleInMs) - 1);
      GpioShouldBe(OFF);

      After(1);
      GpioShouldBe(ON);
   }

   void GpioIsOffWhenPwmDutyCycleIsOff()
   {
      Given ModuleIsInitialized();

      When PwmDutyCycleIs(SomePositivePwmDutyCycle);
      GpioShouldBe(ON);

      When PwmDutyCycleIs(ZeroPwmDutyCycle);
      GpioShouldBe(OFF);
   }
};

TEST(SoftPwm, ShouldHaveGpioOffOnInit)
{
   Given ModuleIsInitialized();
   GpioShouldBe(OFF);
}

TEST(SoftPwm, ShouldTurnGpioOnWhenDutyCycleIsGreaterThanZero)
{
   Given ModuleIsInitialized();

   When PwmDutyCycleIs(SomePositivePwmDutyCycle);
   GpioShouldBe(ON);
}

TEST(SoftPwm, ShouldTurnGpioOffAfterOnTimeIsComplete)
{
   Given GpioJustStartedACycleAndIsOn();

   After(SomeOnCycleInMs - 1);
   GpioShouldBe(ON);

   After(1);
   GpioShouldBe(OFF);
}

TEST(SoftPwm, ShouldTurnGpioOnAgainAfterOffTimeIsComplete)
{
   Given GpioJustTurnedOffAfterBeingOn();

   After((SomePeriodInMs - SomeOnCycleInMs) - 1);
   GpioShouldBe(OFF);

   After(1);
   GpioShouldBe(ON);
}

TEST(SoftPwm, ShouldCycleOnAndOffMultipleTimes)
{
   Given GpioCompletedOneCycleOfOnThenOffAndIsOnAgain();

   for(uint8_t i; i < SomeNumberOfCycles; i++)
   {
      GpioCyclesOnThenOffThenOnAgain();
   }
}

TEST(SoftPwm, ShouldNotTurnOnGpioWhenDutyCycleIsZero)
{
   Given ModuleIsInitialized();

   When PwmDutyCycleIs(ZeroPwmDutyCycle);
   GpioShouldBe(OFF);
}

TEST(SoftPwm, ShouldStartCycleOverWhenDutyCycleChangesInMiddleOfOnTime)
{
   Given GpioJustStartedACycleAndIsOn();

   After(SomeOnCycleInMs - 1);
   PwmDutyCycleIs(AnotherPositivePwmDutyCycle);
   GpioShouldBe(ON);

   After(AnotherOnCycleInMs - 1);
   GpioShouldBe(ON);

   After(1);
   GpioShouldBe(OFF);
}

TEST(SoftPwm, ShouldStartCycleOverWhenDutyCycleChangesInMiddleOfOffTime)
{
   Given GpioJustTurnedOffAfterBeingOn();

   After((SomePeriodInMs - SomeOnCycleInMs) - 1);
   GpioShouldBe(OFF);

   PwmDutyCycleIs(AnotherPositivePwmDutyCycle);
   GpioShouldBe(ON);

   After(AnotherOnCycleInMs - 1);
   GpioShouldBe(ON);

   After(1);
   GpioShouldBe(OFF);
}

TEST(SoftPwm, ShouldTurnGpioOffWhenDutyCycleChangesToZero)
{
   Given ModuleIsInitialized();

   When PwmDutyCycleIs(SomePositivePwmDutyCycle);
   GpioShouldBe(ON);

   When PwmDutyCycleIs(ZeroPwmDutyCycle);
   GpioShouldBe(OFF);
}

TEST(SoftPwm, ShouldNotTurnGpioOnAfterOnTimeWhenDutyCycleChangesToZero)
{
   Given GpioIsOffWhenPwmDutyCycleIsOff();

   After((SomePeriodInMs - SomeOnCycleInMs) - 1);
   GpioShouldBe(OFF);

   After(1);
   GpioShouldBe(OFF);
}

TEST(SoftPwm, ShouldTurnGpioOnWhenDutyCycleChangesToNonZeroValue)
{
   Given GpioIsOffWhenPwmDutyCycleIsOff();

   When PwmDutyCycleIs(SomePositivePwmDutyCycle);
   GpioShouldBe(ON);
}

TEST(SoftPwm, ShouldTurnGpioOnAndKeepItOnWhenDutyCycleIsMax)
{
   Given ModuleIsInitialized();

   When PwmDutyCycleIs(PwmDutyCycle_Max);
   GpioShouldBe(ON);

   After(SomeOnCycleInMs - 1);
   GpioShouldBe(ON);

   After(1);
   GpioShouldBe(ON);
}

TEST(SoftPwm, ShouldTurnGpioOFAndKeepItOFWhenDutyCycleIsZero)
{
   Given ModuleIsInitialized();

   When PwmDutyCycleIs(PwmDutyCycle_Min);
   GpioShouldBe(OFF);

   After(SomeOnCycleInMs - 1);
   GpioShouldBe(OFF);

   After(1);
   GpioShouldBe(OFF);
}
