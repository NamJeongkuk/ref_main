/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "CompressorStartup.h"
#include "Constants_Time.h"
#include "SystemErds.h"
#include "TddPersonality.h"
#include "Vote.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

enum
{
   OneEvaporator = 1,
   TwoEvaporators = 2,
   ThreeEvaporators = 3
};

static CompressorStartupConfiguration_t configuration = {
   .compressorResolvedVotedSpeedErd = Erd_CompressorSpeed_ResolvedVote,
   .condenserFanSpeedStartupVoteErd = Erd_CondenserFanSpeed_CompressorStartUpVote,
   .freezerEvaporatorFanSpeedStartupVoteErd = Erd_FreezerEvapFanSpeed_CompressorStartUpVote,
   .freshFoodEvaporatorFanSpeedStartupVoteErd = Erd_FreshFoodEvapFanSpeed_CompressorStartUpVote,
   .compressorSpeedStartupVoteErd = Erd_CompressorSpeed_StartupVote,
   .coolingModeErd = Erd_CoolingMode
};

TEST_GROUP(CompressorStartup)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   CompressorStartup_t instance;
   CompressorData_t parametric;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      parametric = {
         .compressorIsSingleSpeed = false,
         .compressorTimes = { .startupOnTimeInSeconds = 5 },
         .compressorSpeeds = { .startupSpeedFrequencyInHz = 30 }
      };
   }

   void GivenCompressorStartupIsInitializedWith(uint8_t numberOfEvaporators)
   {
      CompressorStartup_Init(&instance, dataModel, &parametric, numberOfEvaporators, &configuration);
   }

   void After(TimerTicks_t ticks, TimeSourceTickCount_t ticksToElapseAtATime = 1000)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks, ticksToElapseAtATime);
   }

   void GivenCompressorResolvedSpeedIs(CompressorSpeed_t speed)
   {
      CompressorVotedSpeed_t vote = { .speed = speed, .care = Vote_Care };

      DataModel_Write(dataModel, configuration.compressorResolvedVotedSpeedErd, &vote.speed);
   }

   void WhenCompressorResolvedSpeedIs(CompressorSpeed_t speed)
   {
      GivenCompressorResolvedSpeedIs(speed);
   }

   void GivenTheCompressorIsSingleSpeed(void)
   {
      parametric.compressorIsSingleSpeed = true;
   }

   void GivenCoolingModeIs(CoolingMode_t coolingMode)
   {
      DataModel_Write(dataModel, Erd_CoolingMode, &coolingMode);
   }

   void CondenserFanSpeedVoteShouldBe(FanSpeed_t expected)
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_CondenserFanSpeed_CompressorStartUpVote, &actual);

      CHECK_EQUAL(expected, actual.speed);
      CHECK_TRUE(actual.care);
   }

   void CondenserFanSpeedVoteShouldBeDontCare()
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_CondenserFanSpeed_CompressorStartUpVote, &actual);

      CHECK_FALSE(actual.care);
   }

   void FreshFoodEvaporatorFanSpeedVoteShouldBe(FanSpeed_t expected)
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_FreshFoodEvapFanSpeed_CompressorStartUpVote, &actual);

      CHECK_EQUAL(expected, actual.speed);
      CHECK_TRUE(actual.care);
   }

   void FreshFoodEvaporatorFanSpeedVoteShouldBeDontCare()
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_FreshFoodEvapFanSpeed_CompressorStartUpVote, &actual);

      CHECK_FALSE(actual.care);
   }

   void FreezerEvaporatorFanSpeedVoteShouldBe(FanSpeed_t expected)
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_FreezerEvapFanSpeed_CompressorStartUpVote, &actual);

      CHECK_EQUAL(expected, actual.speed);
      CHECK_TRUE(actual.care);
   }

   void FreezerEvaporatorFanSpeedVoteShouldBeDontCare()
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_FreezerEvapFanSpeed_CompressorStartUpVote, &actual);

      CHECK_FALSE(actual.care);
   }

   void CompressorSpeedStartupVoteShouldBe(CompressorSpeed_t expected)
   {
      CompressorVotedSpeed_t actual;
      DataModel_Read(dataModel, configuration.compressorSpeedStartupVoteErd, &actual);

      CHECK_EQUAL(expected, actual.speed);
      CHECK_TRUE(actual.care);
   }

   void CompressorSpeedStartupVoteShouldBeDontCare()
   {
      CompressorVotedSpeed_t actual;
      DataModel_Read(dataModel, configuration.compressorSpeedStartupVoteErd, &actual);

      CHECK_FALSE(actual.care);
   }
};

TEST(CompressorStartup, ShouldVoteForCompressorWhenResolvedCompressorVoteChangesToOn)
{
   GivenCompressorResolvedSpeedIs(CompressorSpeed_Off);
   GivenCompressorStartupIsInitializedWith(OneEvaporator);

   WhenCompressorResolvedSpeedIs(CompressorSpeed_High);
   CompressorSpeedStartupVoteShouldBe(parametric.compressorSpeeds.startupSpeedFrequencyInHz);
}

TEST(CompressorStartup, ShouldVoteDontCareForCondenserFanSpeedVoteWhenTheModuleIsInitialized)
{
   GivenCompressorStartupIsInitializedWith(OneEvaporator);
   CondenserFanSpeedVoteShouldBeDontCare();
}

TEST(CompressorStartup, ShouldVoteDontCareIfItIsACompressorSpeedChange)
{
   GivenCompressorResolvedSpeedIs(CompressorSpeed_High);
   GivenCompressorStartupIsInitializedWith(OneEvaporator);

   WhenCompressorResolvedSpeedIs(CompressorSpeed_Low);
   CompressorSpeedStartupVoteShouldBeDontCare();
   CondenserFanSpeedVoteShouldBeDontCare();
   FreezerEvaporatorFanSpeedVoteShouldBeDontCare();
   FreshFoodEvaporatorFanSpeedVoteShouldBeDontCare();
}

TEST(CompressorStartup, ShouldVoteDontCareForCompressorIfItIsSingleSpeed)
{
   GivenTheCompressorIsSingleSpeed();
   GivenCompressorResolvedSpeedIs(CompressorSpeed_Off);
   GivenCompressorStartupIsInitializedWith(OneEvaporator);

   WhenCompressorResolvedSpeedIs(CompressorSpeed_Low);
   CompressorSpeedStartupVoteShouldBeDontCare();
}

TEST(CompressorStartup, ShouldVoteForCondenserFanLowWhenResolvedCompressorVoteChangesToOn)
{
   GivenCompressorResolvedSpeedIs(CompressorSpeed_Off);
   GivenCompressorStartupIsInitializedWith(OneEvaporator);

   WhenCompressorResolvedSpeedIs(CompressorSpeed_High);
   CondenserFanSpeedVoteShouldBe(FanSpeed_Low);
}

TEST(CompressorStartup, ShouldVoteLowForFreezerEvapAndFreshFoodEvapFanWhenResolvedCompressorVoteChangesToOn)
{
   GivenCompressorResolvedSpeedIs(CompressorSpeed_Off);
   GivenCompressorStartupIsInitializedWith(OneEvaporator);

   WhenCompressorResolvedSpeedIs(CompressorSpeed_High);
   FreezerEvaporatorFanSpeedVoteShouldBe(FanSpeed_Low);
   FreshFoodEvaporatorFanSpeedVoteShouldBe(FanSpeed_Low);
}

TEST(CompressorStartup, ShouldVoteDontCareForCompressorWhenStartupTimeExpires)
{
   GivenCompressorResolvedSpeedIs(CompressorSpeed_Off);
   GivenCompressorStartupIsInitializedWith(OneEvaporator);

   WhenCompressorResolvedSpeedIs(CompressorSpeed_High);

   After(parametric.compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC - 1);
   CompressorSpeedStartupVoteShouldBe(parametric.compressorSpeeds.startupSpeedFrequencyInHz);

   After(1);
   CompressorSpeedStartupVoteShouldBeDontCare();
}

TEST(CompressorStartup, ShouldPerformStartupMultipleTimes)
{
   GivenCompressorResolvedSpeedIs(CompressorSpeed_High);
   GivenCompressorStartupIsInitializedWith(OneEvaporator);

   WhenCompressorResolvedSpeedIs(CompressorSpeed_Off);
   WhenCompressorResolvedSpeedIs(CompressorSpeed_High);
   After(parametric.compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC);
   CompressorSpeedStartupVoteShouldBeDontCare();

   WhenCompressorResolvedSpeedIs(CompressorSpeed_Off);
   WhenCompressorResolvedSpeedIs(CompressorSpeed_High);
   CompressorSpeedStartupVoteShouldBe(parametric.compressorSpeeds.startupSpeedFrequencyInHz);
}

TEST(CompressorStartup, ShouldVoteDontCareForCondenserFanWhenStartupTimeExpires)
{
   GivenCompressorResolvedSpeedIs(CompressorSpeed_Off);
   GivenCompressorStartupIsInitializedWith(OneEvaporator);

   WhenCompressorResolvedSpeedIs(CompressorSpeed_High);

   After(parametric.compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC - 1);
   CondenserFanSpeedVoteShouldBe(FanSpeed_Low);

   After(1);
   CondenserFanSpeedVoteShouldBeDontCare();
}

TEST(CompressorStartup, ShouldVoteDontCareForFreezerAndFreshFoodEvapFanWhenStartupTimeExpires)
{
   GivenCompressorResolvedSpeedIs(CompressorSpeed_Off);
   GivenCompressorStartupIsInitializedWith(OneEvaporator);

   WhenCompressorResolvedSpeedIs(CompressorSpeed_High);

   After(parametric.compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC - 1);
   FreezerEvaporatorFanSpeedVoteShouldBe(FanSpeed_Low);
   FreshFoodEvaporatorFanSpeedVoteShouldBe(FanSpeed_Low);

   After(1);
   FreezerEvaporatorFanSpeedVoteShouldBeDontCare();
   FreshFoodEvaporatorFanSpeedVoteShouldBeDontCare();
}

TEST(CompressorStartup, ShouldVoteDontCareForFreshFoodEvapFanWhenStartupTimeExpires)
{
   GivenCompressorResolvedSpeedIs(CompressorSpeed_Off);
   GivenCoolingModeIs(CoolingMode_FreshFood);
   GivenCompressorStartupIsInitializedWith(TwoEvaporators);

   WhenCompressorResolvedSpeedIs(CompressorSpeed_High);

   After(parametric.compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC - 1);
   FreshFoodEvaporatorFanSpeedVoteShouldBe(FanSpeed_Low);

   After(1);
   FreshFoodEvaporatorFanSpeedVoteShouldBeDontCare();
}

TEST(CompressorStartup, ShouldVoteForFreshFoodEvapFanLowWhenResolvedCompressorVoteChangesToOnAndCoolingModeIsFreshFood)
{
   GivenCompressorResolvedSpeedIs(CompressorSpeed_Off);
   GivenCoolingModeIs(CoolingMode_FreshFood);
   GivenCompressorStartupIsInitializedWith(ThreeEvaporators);

   WhenCompressorResolvedSpeedIs(CompressorSpeed_High);
   FreshFoodEvaporatorFanSpeedVoteShouldBe(FanSpeed_Low);
}

TEST(CompressorStartup, ShouldVoteForFreezerEvapFanOffWhenResolvedCompressorVoteChangesToOnAndCoolingModeIsFreshFood)
{
   GivenCompressorResolvedSpeedIs(CompressorSpeed_Off);
   GivenCoolingModeIs(CoolingMode_FreshFood);
   GivenCompressorStartupIsInitializedWith(ThreeEvaporators);

   WhenCompressorResolvedSpeedIs(CompressorSpeed_High);
   FreezerEvaporatorFanSpeedVoteShouldBe(FanSpeed_Off);
}

TEST(CompressorStartup, ShouldVoteForFreshFoodEvapFanOffWhenResolvedCompressorVoteChangesToOnAndCoolingModeIsFreezer)
{
   GivenCompressorResolvedSpeedIs(CompressorSpeed_Off);
   GivenCoolingModeIs(CoolingMode_Freezer);
   GivenCompressorStartupIsInitializedWith(ThreeEvaporators);

   WhenCompressorResolvedSpeedIs(CompressorSpeed_High);
   FreshFoodEvaporatorFanSpeedVoteShouldBe(FanSpeed_Off);
}

TEST(CompressorStartup, ShouldVoteForFreezerEvapFanLowWhenResolvedCompressorVoteChangesToOnAndCoolingModeIsFreezer)
{
   GivenCompressorResolvedSpeedIs(CompressorSpeed_Off);
   GivenCoolingModeIs(CoolingMode_Freezer);
   GivenCompressorStartupIsInitializedWith(ThreeEvaporators);

   WhenCompressorResolvedSpeedIs(CompressorSpeed_High);
   FreezerEvaporatorFanSpeedVoteShouldBe(FanSpeed_Low);
}

TEST(CompressorStartup, ShouldVoteForFreshFoodEvapFanOffWhenResolvedCompressorVoteChangesToOnAndCoolingModeIsConvertibleCompartment)
{
   GivenCompressorResolvedSpeedIs(CompressorSpeed_Off);
   GivenCoolingModeIs(CoolingMode_ConvertibleCompartment);
   GivenCompressorStartupIsInitializedWith(ThreeEvaporators);

   WhenCompressorResolvedSpeedIs(CompressorSpeed_High);
   FreshFoodEvaporatorFanSpeedVoteShouldBe(FanSpeed_Off);
}

TEST(CompressorStartup, ShouldVoteForFreezerEvapFanLowWhenResolvedCompressorVoteChangesToOnAndCoolingModeIsConvertibleCompartment)
{
   GivenCompressorResolvedSpeedIs(CompressorSpeed_Off);
   GivenCoolingModeIs(CoolingMode_ConvertibleCompartment);
   GivenCompressorStartupIsInitializedWith(ThreeEvaporators);

   WhenCompressorResolvedSpeedIs(CompressorSpeed_High);
   FreezerEvaporatorFanSpeedVoteShouldBe(FanSpeed_Low);
}

TEST(CompressorStartup, ShouldVoteForFreshFoodEvapFanOffWhenResolvedCompressorVoteChangesToOnAndCoolingModeIsUnknown)
{
   GivenCompressorResolvedSpeedIs(CompressorSpeed_Off);
   GivenCoolingModeIs(CoolingMode_Off);
   GivenCompressorStartupIsInitializedWith(ThreeEvaporators);

   WhenCompressorResolvedSpeedIs(CompressorSpeed_High);
   FreshFoodEvaporatorFanSpeedVoteShouldBe(FanSpeed_Off);
}

TEST(CompressorStartup, ShouldVoteForFreezerEvapFanLowWhenResolvedCompressorVoteChangesToOnAndCoolingModeIsUnknown)
{
   GivenCompressorResolvedSpeedIs(CompressorSpeed_Off);
   GivenCoolingModeIs(CoolingMode_Off);
   GivenCompressorStartupIsInitializedWith(ThreeEvaporators);

   WhenCompressorResolvedSpeedIs(CompressorSpeed_High);
   FreezerEvaporatorFanSpeedVoteShouldBe(FanSpeed_Low);
}
