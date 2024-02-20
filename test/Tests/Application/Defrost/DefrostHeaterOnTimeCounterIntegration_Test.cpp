/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "SideBySideDefrostPlugin.h"
#include "PeriodicNvUpdaterPlugin.h"
#include "SystemErds.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

static const DefrostHeaterOnTimeCounterConfig_t config = {
   .defrostHeaterOnTimeErd = Erd_FreezerDefrostHeaterOnTimeInMinutes,
   .defrostHeaterStateErd = Erd_FreezerDefrostHeater_ResolvedVote,
   .defrostStateErd = Erd_DefrostState
};

TEST_GROUP(DefrostHeaterOnTimeCounterIntegration)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   PeriodicNvUpdaterPlugin_t periodicNvUpdaterPlugin;
   DefrostHeaterOnTimeCounter_t instance;
   const SystemMonitorData_t *systemMonitorData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      systemMonitorData = PersonalityParametricData_Get(dataModel)->systemMonitorData;
   }

   void After(TimerTicks_t ticks, TimeSourceTickCount_t ticksToElapseAtATime = 1000)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks, ticksToElapseAtATime);
   }

   void GivenDefrostHeaterOnTimeCounterIsInitialized()
   {
      DefrostHeaterOnTimeCounter_Init(&instance, dataModel, &config);
   }

   void GivenPeriodicNvUpdaterPluginIsInitialized()
   {
      PeriodicNvUpdaterPlugin_Init(&periodicNvUpdaterPlugin, dataModel);
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

   void WhenTheBoardIsReset(void)
   {
      GivenHeaterOnTimeInMinutesIs(0);
      GivenPeriodicNvUpdaterPluginIsInitialized();
      GivenDefrostHeaterOnTimeCounterIsInitialized();
   }
};

TEST(DefrostHeaterOnTimeCounterIntegration, ShouldNotCountHeaterOnTimeWhileDefrostStateIsNotHeaterOn)
{
   GivenHeaterOnTimeInMinutesIs(0);
   GivenPeriodicNvUpdaterPluginIsInitialized();
   GivenDefrostStateIs(DefrostState_Idle);
   GivenDefrostHeaterOnTimeCounterIsInitialized();

   HeaterOnTimeInMinutesShouldBe(0);

   After(MSEC_PER_MIN);
   HeaterOnTimeInMinutesShouldBe(0);
}

TEST(DefrostHeaterOnTimeCounterIntegration, ShouldCountAtZeroWhenDefrostStateIsHeaterOnAndHeaterIsOn)
{
   GivenDefrostStateIs(DefrostState_HeaterOn);
   GivenDefrostHeaterIs(HeaterState_On);
   GivenPeriodicNvUpdaterPluginIsInitialized();
   GivenDefrostHeaterOnTimeCounterIsInitialized();

   HeaterOnTimeInMinutesShouldBe(0);

   After(MSEC_PER_MIN - 1);
   HeaterOnTimeInMinutesShouldBe(0);

   After(1);
   HeaterOnTimeInMinutesShouldBe(1);
}

TEST(DefrostHeaterOnTimeCounterIntegration, ShouldCountAtZeroWhenDefrostStateIsHeaterOnAndHeaterIsOffThenTurnsOn)
{
   GivenDefrostStateIs(DefrostState_HeaterOn);
   GivenDefrostHeaterIs(HeaterState_Off);
   GivenPeriodicNvUpdaterPluginIsInitialized();
   GivenDefrostHeaterOnTimeCounterIsInitialized();

   HeaterOnTimeInMinutesShouldBe(0);

   After(MSEC_PER_MIN);
   HeaterOnTimeInMinutesShouldBe(0);

   WhenDefrostHeaterIs(HeaterState_On);
   After(MSEC_PER_MIN - 1);
   HeaterOnTimeInMinutesShouldBe(0);

   After(1);
   HeaterOnTimeInMinutesShouldBe(1);
}

TEST(DefrostHeaterOnTimeCounterIntegration, ShouldCountFromZeroAgainWhenTheBoardIsResetWhileDefrostStateIsHeaterOnAndHeaterIsOn)
{
   GivenDefrostStateIs(DefrostState_HeaterOn);
   GivenDefrostHeaterIs(HeaterState_On);
   GivenPeriodicNvUpdaterPluginIsInitialized();
   GivenDefrostHeaterOnTimeCounterIsInitialized();

   HeaterOnTimeInMinutesShouldBe(0);

   After(MSEC_PER_MIN);
   HeaterOnTimeInMinutesShouldBe(1);

   WhenTheBoardIsReset();
   HeaterOnTimeInMinutesShouldBe(0);

   After(MSEC_PER_MIN);
   HeaterOnTimeInMinutesShouldBe(1);
}
