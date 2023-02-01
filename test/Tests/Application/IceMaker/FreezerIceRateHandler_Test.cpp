/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "FreezerIceRateHandler.h"
#include "SystemErds.h"
#include "Constants_Time.h"
#include "Vote.h"
#include "PersonalityParametricData.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

static const FreezerIceRateHandlerConfig_t iceRateHandlerConfig = {
   .freezerIceRateTriggerSignal = Erd_FreezerIceRateTriggerSignal,
   .freezerSetpointUserVote = Erd_FreezerSetpoint_UserVote,
   .freezerSetpointFreezerIceRateVote = Erd_FreezerSetpoint_FreezerIceRateVote,
   .freezerEvapFanSpeedFreezerIceRateVote = Erd_FreezerEvapFanSpeed_FreezerIceRateVote,
   .freezerEvapFanSpeedResolvedVote = Erd_FreezerEvapFanSpeed_ResolvedVote,
};

TEST_GROUP(FreezerIceRateHandler)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   FreezerIceRateHandler_t instance;
   const FreezerIceRateData_t *freezerIceRateData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      freezerIceRateData = PersonalityParametricData_Get(dataModel)->iceMakerData->freezerIceRateData;
   }

   void GivenTheModuleIsInitialized()
   {
      FreezerIceRateHandler_Init(
         &instance,
         dataModel,
         &iceRateHandlerConfig,
         freezerIceRateData);
   }

   void GivenTheFreezerSetpointUserVoteIs(TemperatureDegFx100_t temperature, bool care)
   {
      SetpointVotedTemperature_t setpointVotedTemperature = {
         .temperatureInDegFx100 = temperature,
         .care = care,
      };

      DataModel_Write(dataModel, Erd_FreezerSetpoint_UserVote, &setpointVotedTemperature);
   }

   void GivenTheIceRateTriggerSignalIsActivated()
   {
      uint8_t signal;
      DataModel_Read(dataModel, Erd_FreezerIceRateTriggerSignal, &signal);
      signal++;
      DataModel_Write(dataModel, Erd_FreezerIceRateTriggerSignal, &signal);
   }

   void GivenTheFreezerEvapFanSpeedResolvedVoteIs(FanSpeed_t speed, bool care)
   {
      FanVotedSpeed_t votedFanSpeed = {
         .speed = speed,
         .care = care,
      };

      DataModel_Write(dataModel, Erd_FreezerEvapFanSpeed_ResolvedVote, &votedFanSpeed);
   }

   void ThenTheFreezerSetpointIceMakerVoteShouldBe(TemperatureDegFx100_t temperature, bool care)
   {
      SetpointVotedTemperature_t actual;
      DataModel_Read(dataModel, Erd_FreezerSetpoint_FreezerIceRateVote, &actual);

      CHECK_EQUAL(temperature, actual.temperatureInDegFx100);
      CHECK_EQUAL(care, actual.care);
   }

   void ThenTheFreezerEvapFanSpeedIceMakerVoteShouldBe(FanSpeed_t speed, bool care)
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_FreezerEvapFanSpeed_FreezerIceRateVote, &actual);

      CHECK_EQUAL(speed, actual.speed);
      CHECK_EQUAL(care, actual.care);
   }

   void After(TimerTicks_t ticks, TimeSourceTickCount_t ticksToElapseAtATime = 1000)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks, ticksToElapseAtATime);
   }
};

TEST(FreezerIceRateHandler, FreezerSetpointIceMakerVoteShouldBeSetToUserVoteIfUserVoteIsLessThanParametricSetpoint)
{
   GivenTheModuleIsInitialized();
   GivenTheIceRateTriggerSignalIsActivated();

   GivenTheFreezerSetpointUserVoteIs(200, Vote_Care);

   ThenTheFreezerSetpointIceMakerVoteShouldBe(200, Vote_Care);
}

TEST(FreezerIceRateHandler, FreezerSetpointIceMakerVoteShouldBeSetToParametricSetpointIfUserVoteIsGreaterThanParametricSetpoint)
{
   GivenTheModuleIsInitialized();
   GivenTheIceRateTriggerSignalIsActivated();

   GivenTheFreezerSetpointUserVoteIs(300, Vote_Care);

   ThenTheFreezerSetpointIceMakerVoteShouldBe(250, Vote_Care);
}

TEST(FreezerIceRateHandler, FreezerEvapFanSpeedIceMakerVoteShouldBeFanSpeedMediumIfResolvedVoteIsLessThanFanSpeedMedium)
{
   GivenTheModuleIsInitialized();
   GivenTheIceRateTriggerSignalIsActivated();

   GivenTheFreezerEvapFanSpeedResolvedVoteIs(FanSpeed_Low, Vote_Care);

   ThenTheFreezerEvapFanSpeedIceMakerVoteShouldBe(FanSpeed_Medium, Vote_Care);
}

TEST(FreezerIceRateHandler, ShouldUnsubscribeToErdsAfterTimerExpired)
{
   GivenTheModuleIsInitialized();
   GivenTheIceRateTriggerSignalIsActivated();

   GivenTheFreezerSetpointUserVoteIs(300, Vote_Care);
   GivenTheFreezerEvapFanSpeedResolvedVoteIs(FanSpeed_Low, Vote_Care);

   ThenTheFreezerSetpointIceMakerVoteShouldBe(250, Vote_Care);
   ThenTheFreezerEvapFanSpeedIceMakerVoteShouldBe(FanSpeed_Medium, Vote_Care);

   After(2 * MSEC_PER_MIN);

   GivenTheFreezerSetpointUserVoteIs(999, Vote_Care);
   GivenTheFreezerEvapFanSpeedResolvedVoteIs(FanSpeed_High, Vote_Care);

   ThenTheFreezerSetpointIceMakerVoteShouldBe(250, Vote_DontCare);
   ThenTheFreezerEvapFanSpeedIceMakerVoteShouldBe(FanSpeed_Medium, Vote_DontCare);
}
