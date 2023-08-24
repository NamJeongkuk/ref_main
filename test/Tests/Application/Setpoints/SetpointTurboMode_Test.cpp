/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "Constants_Binary.h"
#include "Constants_Time.h"
#include "SetpointTurboMode.h"
#include "SystemErds.h"
}

#include "CppUTest/TestHarness.h"
#include "ReferDataModel_TestDouble.h"

static const SetpointTurboModeConfig_t setpointTurboModeConfig = {
   .turboModeSetpointVoteErd = Erd_FreezerSetpoint_TurboFreezeVote,
   .turboModeOnOffStatusErd = Erd_TurboFreezeOnOffStatus,
   .turboModeOnTimeInMinutesErd = Erd_TurboFreezeOnTimeInMinutes,
   .timerModuleErd = Erd_TimerModule,
};

TEST_GROUP(SetpointTurboMode)
{
   I_DataModel_t *dataModel;
   SetpointTurboMode_t instance;
   ReferDataModel_TestDouble_t dataModelDouble;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   const TurboModeData_t *turboModeSetpointData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      turboModeSetpointData = PersonalityParametricData_Get(dataModel)->turboModeSetpointData->freezerTurboFreezeSetpointData;
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks);
   }

   void GivenTheModuleIsInitialized()
   {
      SetpointTurboMode_Init(
         &instance,
         dataModel,
         &setpointTurboModeConfig,
         turboModeSetpointData);
   }

   void WhenTurboModeStatusIs(bool state)
   {
      DataModel_Write(dataModel, Erd_TurboFreezeOnOffStatus, &state);
   }

   void TurboVoteShouldBe(TemperatureDegFx100_t expectedTemperature, Vote_t expectedVote)
   {
      SetpointVotedTemperature_t actual;
      DataModel_Read(dataModel, Erd_FreezerSetpoint_TurboFreezeVote, &actual);

      CHECK_EQUAL(expectedTemperature, actual.temperatureInDegFx100);
      CHECK_EQUAL(expectedVote, actual.care);
   }

   void TurboModeOnTimeInMinutesShouldBe(uint16_t expected)
   {
      uint16_t actual;
      DataModel_Read(dataModel, Erd_TurboFreezeOnTimeInMinutes, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void TurboModeStatusShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_TurboFreezeOnOffStatus, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void GivenTurboModeOnTimeInMinutesIs(uint16_t minutes)
   {
      DataModel_Write(dataModel, Erd_TurboFreezeOnTimeInMinutes, &minutes);
   }
};

TEST(SetpointTurboMode, ShouldStartTimerForTurboModeWhenTurboModeIsRequestedAndExitWhenTurboModeTimerExpires)
{
   GivenTheModuleIsInitialized();
   TurboModeOnTimeInMinutesShouldBe(0);

   WhenTurboModeStatusIs(ON);
   TurboVoteShouldBe(turboModeSetpointData->turboModeSetpointInDegFx100, Vote_Care);

   After((turboModeSetpointData->turboModeOnTimeInMinutes * MSEC_PER_MIN) - 1);
   TurboModeOnTimeInMinutesShouldBe(turboModeSetpointData->turboModeOnTimeInMinutes - 1);
   TurboVoteShouldBe(turboModeSetpointData->turboModeSetpointInDegFx100, Vote_Care);

   After(1);
   TurboVoteShouldBe(0, Vote_DontCare);
   TurboModeOnTimeInMinutesShouldBe(0);
}

TEST(SetpointTurboMode, ShouldStartTimerForTurboModeWhenTurboModeIsRequestedAndExitWhenTurboModeIsRequestedOff)
{
   GivenTheModuleIsInitialized();

   WhenTurboModeStatusIs(ON);
   TurboVoteShouldBe(turboModeSetpointData->turboModeSetpointInDegFx100, Vote_Care);

   After((turboModeSetpointData->turboModeOnTimeInMinutes * MSEC_PER_MIN) - 1);

   WhenTurboModeStatusIs(OFF);
   TurboVoteShouldBe(0, Vote_DontCare);
   TurboModeOnTimeInMinutesShouldBe(0);
}

TEST(SetpointTurboMode, ShouldIncrementTurboModeTimerEveryMinuteWhileInTurboMode)
{
   GivenTheModuleIsInitialized();
   TurboModeOnTimeInMinutesShouldBe(0);

   WhenTurboModeStatusIs(ON);
   After(MSEC_PER_MIN - 1);
   TurboModeOnTimeInMinutesShouldBe(0);

   After(1);
   TurboModeOnTimeInMinutesShouldBe(1);

   After(MSEC_PER_MIN - 1);
   TurboModeOnTimeInMinutesShouldBe(1);

   After(1);
   TurboModeOnTimeInMinutesShouldBe(2);
}

TEST(SetpointTurboMode, ShouldResetTurboModeOnTimeInMinutesWhenTurboModeIsStoppedAndThenStartedAgain)
{
   GivenTheModuleIsInitialized();

   WhenTurboModeStatusIs(ON);
   After(MSEC_PER_MIN - 1);
   TurboModeOnTimeInMinutesShouldBe(0);

   After(1);
   TurboModeOnTimeInMinutesShouldBe(1);

   After(MSEC_PER_MIN - 1);
   TurboModeOnTimeInMinutesShouldBe(1);

   WhenTurboModeStatusIs(OFF);
   TurboModeOnTimeInMinutesShouldBe(0);

   After(MSEC_PER_MIN);
   TurboModeOnTimeInMinutesShouldBe(0);

   WhenTurboModeStatusIs(ON);
   After(MSEC_PER_MIN - 1);
   TurboModeOnTimeInMinutesShouldBe(0);

   After(1);
   TurboModeOnTimeInMinutesShouldBe(1);
}

TEST(SetpointTurboMode, ShouldVoteForDefaultParametricTemperatureWhenEnteringTurboMode)
{
   GivenTheModuleIsInitialized();

   WhenTurboModeStatusIs(ON);
   TurboVoteShouldBe(turboModeSetpointData->turboModeSetpointInDegFx100, Vote_Care);
}

TEST(SetpointTurboMode, ShouldStartTurboModeOnInitializationIfPreviousTurboModeDidNotComplete)
{
   GivenTurboModeOnTimeInMinutesIs(turboModeSetpointData->turboModeOnTimeInMinutes - 2);
   GivenTheModuleIsInitialized();

   TurboVoteShouldBe(turboModeSetpointData->turboModeSetpointInDegFx100, Vote_Care);
   TurboModeStatusShouldBe(SET);

   After(MSEC_PER_MIN - 1);
   TurboModeOnTimeInMinutesShouldBe(turboModeSetpointData->turboModeOnTimeInMinutes - 2);

   After(1);
   TurboModeOnTimeInMinutesShouldBe(turboModeSetpointData->turboModeOnTimeInMinutes - 1);

   After(MSEC_PER_MIN);
   TurboModeOnTimeInMinutesShouldBe(0);
   TurboVoteShouldBe(0, Vote_DontCare);
}
