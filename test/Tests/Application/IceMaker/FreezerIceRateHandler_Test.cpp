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

static const FreezerIceRateHandlerConfig_t freezerIceRateHandlerConfig = {
   .freezerIceRateTriggerSignalErd = Erd_FreezerIceRateTriggerSignal,
   .freezerSetpointFreezerIceRateVoteErd = Erd_FreezerSetpoint_FreezerIceRateVote,
   .freezerEvapFanSpeedFreezerIceRateVoteErd = Erd_FreezerEvapFanSpeed_FreezerIceRateVote,
   .freezerEvapFanSpeedResolvedVoteErd = Erd_FreezerEvapFanSpeed_ResolvedVote,
   .freezerIceRateIsActiveErd = Erd_Freezer_IceRateIsActive
};

static const FreezerIceRateData_t freezerIceRateData = {
   .timeInMinutes = 2,
   .freezerSetpointInDegFx100 = 250,
   .freezerEvaporatorFanSpeed = FanSpeed_High
};

static const FreezerIceRateData_t freezerIceRateDataWithZeroTime = {
   .timeInMinutes = 0,
   .freezerSetpointInDegFx100 = 250,
   .freezerEvaporatorFanSpeed = FanSpeed_High
};

TEST_GROUP(FreezerIceRateHandler)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   FreezerIceRateHandler_t instance;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);
   }

   void GivenTheModuleIsInitialized()
   {
      FreezerIceRateHandler_Init(
         &instance,
         dataModel,
         &freezerIceRateHandlerConfig,
         &freezerIceRateData);
   }

   void GivenTheModuleIsInitializedWithZeroFreezerIceRateTime()
   {
      FreezerIceRateHandler_Init(
         &instance,
         dataModel,
         &freezerIceRateHandlerConfig,
         &freezerIceRateDataWithZeroTime);
   }

   void GivenTheFreezerEvapFanSpeedIceMakerVoteIs(FanSpeed_t speed, bool care)
   {
      FanVotedSpeed_t vote;
      vote.speed = speed;
      vote.care = care;
      DataModel_Write(dataModel, Erd_FreezerEvapFanSpeed_FreezerIceRateVote, &vote);
   }

   void GivenTheIceRateTriggerSignalIsActivated()
   {
      uint8_t signal;
      DataModel_Read(dataModel, Erd_FreezerIceRateTriggerSignal, &signal);
      signal++;
      DataModel_Write(dataModel, Erd_FreezerIceRateTriggerSignal, &signal);
   }

   void WhenTheIceRateTriggerSignalIsActivated()
   {
      GivenTheIceRateTriggerSignalIsActivated();
   }

   void GivenTheFreezerEvapFanSpeedResolvedVoteIs(FanSpeed_t speed, bool care)
   {
      FanVotedSpeed_t votedFanSpeed = {
         .speed = speed,
         .care = care,
      };

      DataModel_Write(dataModel, Erd_FreezerEvapFanSpeed_ResolvedVote, &votedFanSpeed);
   }

   void WhenTheFreezerEvapFanSpeedResolvedVoteIs(FanSpeed_t speed, bool care)
   {
      GivenTheFreezerEvapFanSpeedResolvedVoteIs(speed, care);
   }

   void ThenTheFreezerSetpointIceRateVoteShouldBe(TemperatureDegFx100_t temperature, bool care)
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

   void IceRateActiveErdShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_Freezer_IceRateIsActive, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void GivenIceRateHasAlreadyBeenActiveForIceRateTime()
   {
      WhenTheIceRateTriggerSignalIsActivated();
      After((freezerIceRateData.timeInMinutes * MSEC_PER_MIN) - 1);
      IceRateActiveErdShouldBe(true);

      After(1);
      IceRateActiveErdShouldBe(false);
   }
};

TEST(FreezerIceRateHandler, ShouldVoteForParametricFreezerSetpointWhenIceRateTriggerSignalIsReceived)
{
   GivenTheModuleIsInitialized();

   WhenTheIceRateTriggerSignalIsActivated();
   ThenTheFreezerSetpointIceRateVoteShouldBe(freezerIceRateData.freezerSetpointInDegFx100, Vote_Care);
}

TEST(FreezerIceRateHandler, ShouldVoteForFreezerEvapFanSpeedWithParametricSpeedIfResolvedVoteIsLessThanParametricSpeed)
{
   GivenTheFreezerEvapFanSpeedResolvedVoteIs(FanSpeed_Low, Vote_Care);
   GivenTheModuleIsInitialized();
   GivenTheIceRateTriggerSignalIsActivated();

   ThenTheFreezerEvapFanSpeedIceMakerVoteShouldBe(freezerIceRateData.freezerEvaporatorFanSpeed, Vote_Care);
}

TEST(FreezerIceRateHandler, ShouldVoteForFreezerEvapFanSpeedWithResolvedSpeedIfResolvedVoteIsEqualToParametricSpeed)
{
   GivenTheFreezerEvapFanSpeedResolvedVoteIs(freezerIceRateData.freezerEvaporatorFanSpeed, Vote_Care);
   GivenTheModuleIsInitialized();
   GivenTheIceRateTriggerSignalIsActivated();

   ThenTheFreezerEvapFanSpeedIceMakerVoteShouldBe(freezerIceRateData.freezerEvaporatorFanSpeed, Vote_Care);
}

TEST(FreezerIceRateHandler, ShouldVoteForFreezerEvapFanSpeedWithResolvedSpeedIfResolvedVoteIsGreaterThanParametricSpeed)
{
   GivenTheFreezerEvapFanSpeedResolvedVoteIs(FanSpeed_SuperHigh, Vote_Care);
   GivenTheModuleIsInitialized();
   GivenTheIceRateTriggerSignalIsActivated();

   ThenTheFreezerEvapFanSpeedIceMakerVoteShouldBe(FanSpeed_SuperHigh, Vote_Care);
}

TEST(FreezerIceRateHandler, ShouldUpdateFreezerEvapFanSpeedVoteWhenResolvedVoteChanges)
{
   GivenTheFreezerEvapFanSpeedResolvedVoteIs(FanSpeed_SuperHigh, Vote_Care);
   GivenTheModuleIsInitialized();
   GivenTheIceRateTriggerSignalIsActivated();

   ThenTheFreezerEvapFanSpeedIceMakerVoteShouldBe(FanSpeed_SuperHigh, Vote_Care);

   WhenTheFreezerEvapFanSpeedResolvedVoteIs(FanSpeed_Low, Vote_Care);
   ThenTheFreezerEvapFanSpeedIceMakerVoteShouldBe(freezerIceRateData.freezerEvaporatorFanSpeed, Vote_Care);
}

TEST(FreezerIceRateHandler, ShouldSetIceRateActiveToTrueWhenIceRateSignalTriggerActivated)
{
   GivenTheModuleIsInitialized();
   IceRateActiveErdShouldBe(false);

   WhenTheIceRateTriggerSignalIsActivated();
   IceRateActiveErdShouldBe(true);
}

TEST(FreezerIceRateHandler, ShouldSetIceRateActiveToFalseAfterIceRateTimerExpires)
{
   GivenTheModuleIsInitialized();
   IceRateActiveErdShouldBe(false);

   WhenTheIceRateTriggerSignalIsActivated();
   After((freezerIceRateData.timeInMinutes * MSEC_PER_MIN) - 1);
   IceRateActiveErdShouldBe(true);

   After(1);
   IceRateActiveErdShouldBe(false);
}

TEST(FreezerIceRateHandler, ShouldSetIceRateActiveToTrueAfterIceRateSignalTriggeredAgain)
{
   GivenTheModuleIsInitialized();
   GivenIceRateHasAlreadyBeenActiveForIceRateTime();

   WhenTheIceRateTriggerSignalIsActivated();
   After((freezerIceRateData.timeInMinutes * MSEC_PER_MIN) - 1);
   IceRateActiveErdShouldBe(true);

   After(1);
   IceRateActiveErdShouldBe(false);
}

TEST(FreezerIceRateHandler, ShouldVoteDontCareForFreezerSetpointAndFreezerEvapFanSpeedAfterIceRateTimerExpires)
{
   GivenTheFreezerEvapFanSpeedResolvedVoteIs(FanSpeed_SuperHigh, Vote_Care);
   GivenTheModuleIsInitialized();
   GivenTheIceRateTriggerSignalIsActivated();

   ThenTheFreezerEvapFanSpeedIceMakerVoteShouldBe(FanSpeed_SuperHigh, Vote_Care);
   ThenTheFreezerSetpointIceRateVoteShouldBe(freezerIceRateData.freezerSetpointInDegFx100, Vote_Care);

   After((freezerIceRateData.timeInMinutes * MSEC_PER_MIN) - 1);
   ThenTheFreezerEvapFanSpeedIceMakerVoteShouldBe(FanSpeed_SuperHigh, Vote_Care);
   ThenTheFreezerSetpointIceRateVoteShouldBe(freezerIceRateData.freezerSetpointInDegFx100, Vote_Care);

   After(1);
   ThenTheFreezerEvapFanSpeedIceMakerVoteShouldBe(FanSpeed_SuperHigh, Vote_DontCare);
   ThenTheFreezerSetpointIceRateVoteShouldBe(freezerIceRateData.freezerSetpointInDegFx100, Vote_DontCare);
}

TEST(FreezerIceRateHandler, ShouldUnsubscribeToFreezerEvapFanSpeedResolvedErdAfterTimerExpires)
{
   GivenTheModuleIsInitialized();
   GivenTheIceRateTriggerSignalIsActivated();

   WhenTheFreezerEvapFanSpeedResolvedVoteIs(FanSpeed_Low, Vote_Care);

   ThenTheFreezerSetpointIceRateVoteShouldBe(freezerIceRateData.freezerSetpointInDegFx100, Vote_Care);
   ThenTheFreezerEvapFanSpeedIceMakerVoteShouldBe(freezerIceRateData.freezerEvaporatorFanSpeed, Vote_Care);

   After(freezerIceRateData.timeInMinutes * MSEC_PER_MIN);

   WhenTheFreezerEvapFanSpeedResolvedVoteIs(FanSpeed_SuperHigh, Vote_Care);

   ThenTheFreezerSetpointIceRateVoteShouldBe(freezerIceRateData.freezerSetpointInDegFx100, Vote_DontCare);
   ThenTheFreezerEvapFanSpeedIceMakerVoteShouldBe(freezerIceRateData.freezerEvaporatorFanSpeed, Vote_DontCare);
}

TEST(FreezerIceRateHandler, ShouldNotReactToFreezerIceRateTriggerSignalIfTimeIsZero)
{
   GivenTheFreezerEvapFanSpeedResolvedVoteIs(FanSpeed_Low, Vote_Care);
   GivenTheFreezerEvapFanSpeedIceMakerVoteIs(FanSpeed_Off, Vote_DontCare);
   GivenTheModuleIsInitializedWithZeroFreezerIceRateTime();
   GivenTheIceRateTriggerSignalIsActivated();

   ThenTheFreezerEvapFanSpeedIceMakerVoteShouldBe(FanSpeed_Off, Vote_DontCare);
}
