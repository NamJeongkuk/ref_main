/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "IceRateHandler.h"
#include "SystemErds.h"
#include "Constants_Time.h"
#include "Vote.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

enum
{

};

static const IceRateHandlerConfig_t iceRateHandlerConfig = {
   .iceRateTriggerSignal = Erd_IceRateTriggerSignal,
   .freezerSetpointUserVote = Erd_FreezerSetpoint_UserVote,
   .freezerSetpointIceRateVote = Erd_FreezerSetpoint_IceRateVote,
   .freezerEvapFanSpeedIceRateVote = Erd_FreezerEvapFanSpeed_IceRateVote,
   .freezerEvapFanSpeedResolvedVote = Erd_FreezerEvapFanSpeed_ResolvedVote,
};

TEST_GROUP(IceRateHandler)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   IceRateHandler_t instance;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);
   }

   void GivenTheModuleIsInitialized()
   {
      IceRateHandler_Init(
         &instance,
         dataModel,
         &iceRateHandlerConfig);
   }

   void GivenTheFreezerSetpointUserVoteIs(TemperatureDegFx100_t temperature, bool care)
   {
      SetpointVotedTemperature_t setpointVotedTemperature = {
         .temperature = temperature,
         .care = care,
      };

      DataModel_Write(dataModel, Erd_FreezerSetpoint_UserVote, &setpointVotedTemperature);
   }

   void GivenTheIceRateTriggerSignalIsActivated()
   {
      uint8_t signal;
      DataModel_Read(dataModel, Erd_IceRateTriggerSignal, &signal);
      signal++;
      DataModel_Write(dataModel, Erd_IceRateTriggerSignal, &signal);
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
      DataModel_Read(dataModel, Erd_FreezerSetpoint_IceRateVote, &actual);

      CHECK_EQUAL(temperature, actual.temperature);
      CHECK_EQUAL(care, actual.care);
   }

   void ThenTheFreezerEvapFanSpeedIceMakerVoteShouldBe(FanSpeed_t speed, bool care)
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_FreezerEvapFanSpeed_IceRateVote, &actual);

      CHECK_EQUAL(speed, actual.speed);
      CHECK_EQUAL(care, actual.care);
   }

   void After(TimerTicks_t ticks, TimeSourceTickCount_t ticksToElapseAtATime = 1000)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks, ticksToElapseAtATime);
   }
};

TEST(IceRateHandler, FreezerSetpointIceMakerVoteShouldBeSetToUserVoteIfUserVoteIsLessThanParametricSetpoint)
{
   GivenTheModuleIsInitialized();
   GivenTheIceRateTriggerSignalIsActivated();

   GivenTheFreezerSetpointUserVoteIs(200, Vote_Care);

   ThenTheFreezerSetpointIceMakerVoteShouldBe(200, Vote_Care);
}

TEST(IceRateHandler, FreezerSetpointIceMakerVoteShouldBeSetToParametricSetpointIfUserVoteIsGreaterThanParametricSetpoint)
{
   GivenTheModuleIsInitialized();
   GivenTheIceRateTriggerSignalIsActivated();

   GivenTheFreezerSetpointUserVoteIs(300, Vote_Care);

   ThenTheFreezerSetpointIceMakerVoteShouldBe(250, Vote_Care);
}

TEST(IceRateHandler, FreezerEvapFanSpeedIceMakerVoteShouldBeFanSpeedMediumIfResolvedVoteIsLessThanFanSpeedMedium)
{
   GivenTheModuleIsInitialized();
   GivenTheIceRateTriggerSignalIsActivated();

   GivenTheFreezerEvapFanSpeedResolvedVoteIs(FanSpeed_Low, Vote_Care);

   ThenTheFreezerEvapFanSpeedIceMakerVoteShouldBe(FanSpeed_Medium, Vote_Care);
}

TEST(IceRateHandler, ShouldUnsubscribeToErdsAfterTimerExpired)
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
