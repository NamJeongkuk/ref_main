/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "DefrostHeaterOnTimeCounter.h"
#include "SystemErds.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

enum
{
   SomeMinutes = 10
};

static const DefrostHeaterOnTimeCounterConfig_t config = {
   .defrostHeaterOnTimeErd = Erd_FreezerDefrostHeaterOnTimeInMinutes,
   .defrostHeaterStateErd = Erd_FreezerDefrostHeater_ResolvedVote,
   .defrostStateErd = Erd_DefrostState
};

TEST_GROUP(DefrostHeaterOnTimeCounter)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   DefrostHeaterOnTimeCounter_t instance;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);
   }

   void After(TimerTicks_t ticks, TimeSourceTickCount_t ticksToElapseAtATime = 1000)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks, ticksToElapseAtATime);
   }

   void GivenDefrostHeaterOnTimeCounterIsInitialized()
   {
      DefrostHeaterOnTimeCounter_Init(&instance, dataModel, &config);
   }

   void GivenDefrostStateIs(DefrostState_t state)
   {
      DataModel_Write(dataModel, Erd_DefrostState, &state);
   }

   void GivenDefrostHeaterIs(HeaterState_t heaterState)
   {
      HeaterVotedState_t resolvedVote = { .state = heaterState, .care = true };
      DataModel_Write(dataModel, Erd_FreezerDefrostHeater_ResolvedVote, &resolvedVote);
   }

   void WhenDefrostHeaterIs(HeaterState_t heaterState)
   {
      GivenDefrostHeaterIs(heaterState);
   }

   void GivenHeaterOnTimeInMinutesIs(uint8_t minutes)
   {
      DataModel_Write(dataModel, Erd_FreezerDefrostHeaterOnTimeInMinutes, &minutes);
   }

   void HeaterOnTimeInMinutesShouldBe(uint8_t expected)
   {
      uint8_t actual;
      DataModel_Read(dataModel, Erd_FreezerDefrostHeaterOnTimeInMinutes, &actual);

      CHECK_EQUAL(expected, actual);
   }
};

TEST(DefrostHeaterOnTimeCounter, ShouldResetHeaterOnTimeAndNotCountOnInitWhenDefrostStateIsNotHeaterOnAndHeaterIsOff)
{
   GivenHeaterOnTimeInMinutesIs(SomeMinutes);
   GivenDefrostStateIs(DefrostState_Idle);
   GivenDefrostHeaterIs(HeaterState_Off);
   GivenDefrostHeaterOnTimeCounterIsInitialized();

   HeaterOnTimeInMinutesShouldBe(0);

   After(MSEC_PER_MIN);
   HeaterOnTimeInMinutesShouldBe(0);
}

TEST(DefrostHeaterOnTimeCounter, ShouldCountHeaterOnTimeOnInitWhenDefrostStateIsHeaterOnAndHeaterIsOn)
{
   GivenDefrostStateIs(DefrostState_HeaterOn);
   GivenDefrostHeaterIs(HeaterState_On);
   GivenDefrostHeaterOnTimeCounterIsInitialized();

   After(MSEC_PER_MIN - 1);
   HeaterOnTimeInMinutesShouldBe(0);

   After(1);
   HeaterOnTimeInMinutesShouldBe(1);

   After(MSEC_PER_MIN);
   HeaterOnTimeInMinutesShouldBe(2);
}

TEST(DefrostHeaterOnTimeCounter, ShouldNotCountHeaterOnTimeOnInitWhenHeaterIsOnAndDefrostStateIsNotHeaterOn)
{
   GivenDefrostStateIs(DefrostState_Idle);
   GivenDefrostHeaterIs(HeaterState_On);
   GivenDefrostHeaterOnTimeCounterIsInitialized();

   After(MSEC_PER_MIN);
   HeaterOnTimeInMinutesShouldBe(0);
}

TEST(DefrostHeaterOnTimeCounter, ShouldNotResetHeaterOnTimeOnInitAndNotCountWhenHeaterIsOffAndDefrostStateIsHeaterOn)
{
   GivenHeaterOnTimeInMinutesIs(SomeMinutes);
   GivenDefrostStateIs(DefrostState_HeaterOn);
   GivenDefrostHeaterIs(HeaterState_Off);
   GivenDefrostHeaterOnTimeCounterIsInitialized();

   After(MSEC_PER_MIN);
   HeaterOnTimeInMinutesShouldBe(SomeMinutes);
}

TEST(DefrostHeaterOnTimeCounter, ShouldCountHeaterOnTimeWhenDefrostStateIsHeaterOnAndHeaterChangesToOn)
{
   GivenDefrostStateIs(DefrostState_HeaterOn);
   GivenDefrostHeaterIs(HeaterState_Off);
   GivenDefrostHeaterOnTimeCounterIsInitialized();

   WhenDefrostHeaterIs(HeaterState_On);
   After(MSEC_PER_MIN - 1);
   HeaterOnTimeInMinutesShouldBe(0);

   After(1);
   HeaterOnTimeInMinutesShouldBe(1);
}

TEST(DefrostHeaterOnTimeCounter, ShouldResetHeaterOnTimeWhenDefrostStateIsHeaterOnAndHeaterChangesToOff)
{
   GivenDefrostStateIs(DefrostState_HeaterOn);
   GivenDefrostHeaterIs(HeaterState_On);
   GivenDefrostHeaterOnTimeCounterIsInitialized();

   After(MSEC_PER_MIN);
   HeaterOnTimeInMinutesShouldBe(1);

   WhenDefrostHeaterIs(HeaterState_Off);
   HeaterOnTimeInMinutesShouldBe(0);

   After(MSEC_PER_MIN);
   HeaterOnTimeInMinutesShouldBe(0);
}

TEST(DefrostHeaterOnTimeCounter, ShouldNotCountHeaterOnTimeWhenDefrostStateIsNotHeaterOnAndHeaterChangesToOn)
{
   GivenDefrostStateIs(DefrostState_Idle);
   GivenDefrostHeaterIs(HeaterState_Off);
   GivenDefrostHeaterOnTimeCounterIsInitialized();

   WhenDefrostHeaterIs(HeaterState_On);
   After(MSEC_PER_MIN);
   HeaterOnTimeInMinutesShouldBe(0);
}

TEST(DefrostHeaterOnTimeCounter, ShouldNotCountHeaterOnTimeWhenDefrostStateIsNotHeaterOnAndHeaterChangesToOff)
{
   GivenDefrostStateIs(DefrostState_Idle);
   GivenDefrostHeaterIs(HeaterState_On);
   GivenDefrostHeaterOnTimeCounterIsInitialized();

   WhenDefrostHeaterIs(HeaterState_Off);
   After(MSEC_PER_MIN);
   HeaterOnTimeInMinutesShouldBe(0);
}
