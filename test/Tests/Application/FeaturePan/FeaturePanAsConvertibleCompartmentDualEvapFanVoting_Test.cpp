/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "FeaturePanAsConvertibleCompartmentDualEvapFanVoting.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

const FeaturePanAsConvertibleCompartmentDualEvapFanVotingConfig_t config = {
   .featurePanModeErd = Erd_FeaturePanMode,
   .evapFanVote = Erd_FreezerEvapFanSpeed_FeaturePanVote
};

TEST_GROUP(FeaturePanAsConvertibleCompartmentDualEvapFanVoting)
{
   FeaturePanAsConvertibleCompartmentDualEvapFanVoting_t instance;
   ReferDataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelTestDouble);
      dataModel = dataModelTestDouble.dataModel;

      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelTestDouble);
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks);
   }

   void GivenModuleIsInitialized()
   {
      FeaturePanAsConvertibleCompartmentDualEvapFanVoting_Init(
         &instance,
         dataModel,
         &config);
   }

   void GivenFreezerEvapFanVoteIs(FanSpeed_t votedSpeed, Vote_t care)
   {
      FanVotedSpeed_t fanVotedSpeed = {
         .speed = votedSpeed,
         .care = care
      };

      DataModel_Write(dataModel, Erd_FreezerEvapFanSpeed_FeaturePanVote, &fanVotedSpeed);
   }

   void FreezerEvapFanVoteShouldBe(FanSpeed_t expectedSpeed, Vote_t expectedCare)
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_FreezerEvapFanSpeed_FeaturePanVote, &actual);

      CHECK_EQUAL(expectedSpeed, actual.speed);
      CHECK_EQUAL(expectedCare, actual.care);
   }

   void GivenFeaturePanModeIs(FeaturePanMode_t featurePanMode)
   {
      DataModel_Write(dataModel, Erd_FeaturePanMode, &featurePanMode);
   }

   void WhenFeaturePanModeBecomes(FeaturePanMode_t featurePanMode)
   {
      GivenFeaturePanModeIs(featurePanMode);
   }

   void FeaturePanModeShouldBe(FeaturePanMode_t expected)
   {
      FeaturePanMode_t actual;
      DataModel_Read(dataModel, Erd_FeaturePanMode, &actual);

      CHECK_EQUAL(expected, actual);
   }
};

TEST(FeaturePanAsConvertibleCompartmentDualEvapFanVoting, ShouldVoteFreezerEvapFanHighWhenFeaturePanModeIsActiveCoolingOnInit)
{
   GivenFreezerEvapFanVoteIs(FanSpeed_Off, Vote_DontCare);
   GivenFeaturePanModeIs(FeaturePanMode_ActiveCooling);
   GivenModuleIsInitialized();

   FreezerEvapFanVoteShouldBe(FanSpeed_High, Vote_Care);
}

TEST(FeaturePanAsConvertibleCompartmentDualEvapFanVoting, ShouldNotVoteFreezerEvapFanHighWhenFeaturePanModeIsNeutralOnInit)
{
   GivenFreezerEvapFanVoteIs(FanSpeed_Off, Vote_DontCare);
   GivenFeaturePanModeIs(FeaturePanMode_Neutral);
   GivenModuleIsInitialized();

   FreezerEvapFanVoteShouldBe(FanSpeed_High, Vote_DontCare);
}

TEST(FeaturePanAsConvertibleCompartmentDualEvapFanVoting, ShouldVoteFreezerEvapFanHighWhenFeaturePanModeChangesToActiveCooling)
{
   GivenFreezerEvapFanVoteIs(FanSpeed_Off, Vote_DontCare);
   GivenFeaturePanModeIs(FeaturePanMode_Neutral);
   GivenModuleIsInitialized();

   WhenFeaturePanModeBecomes(FeaturePanMode_ActiveCooling);
   FreezerEvapFanVoteShouldBe(FanSpeed_High, Vote_Care);
}

TEST(FeaturePanAsConvertibleCompartmentDualEvapFanVoting, ShouldNotVoteFreezerEvapFanHighWhenFeaturePanModeChangesToNeutral)
{
   GivenFreezerEvapFanVoteIs(FanSpeed_Off, Vote_DontCare);
   GivenFeaturePanModeIs(FeaturePanMode_ActiveCooling);
   GivenModuleIsInitialized();

   WhenFeaturePanModeBecomes(FeaturePanMode_Neutral);
   FreezerEvapFanVoteShouldBe(FanSpeed_High, Vote_DontCare);
}
