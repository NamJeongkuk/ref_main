/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "FeaturePanAsDeliPanEvapFanVoting.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

enum
{
   FeaturePanDeliCoolingSpeedParametric = FanSpeed_Low
};

const FeaturePanAsDeliPanEvapFanVotingConfig_t config = {
   .featurePanCoolingModeErd = Erd_FeaturePanCoolingMode,
   .fanEvapVoteErd = Erd_FreshFoodEvapFanSpeed_DeliPanSpeedVote,
   .fanEvapGridVoteErd = Erd_FreshFoodEvapFanSpeed_GridVote
};

const FeaturePanData_t featurePanData = {
   .freshFoodFanDeliCoolingSpeed = FeaturePanDeliCoolingSpeedParametric
};

TEST_GROUP(FeaturePanAsDeliPanEvapFanVoting)
{
   FeaturePanAsDeliPanEvapFanVoting_t instance;
   ReferDataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelTestDouble);
      dataModel = dataModelTestDouble.dataModel;

      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelTestDouble);
   }

   void GivenModuleIsInitialized()
   {
      FeaturePanAsDeliPanEvapFanVoting_Init(
         &instance,
         dataModel,
         &config,
         &featurePanData);
   }

   void GivenFeaturePanCoolingModeIs(FeaturePanCoolingMode_t featurePanCoolingMode)
   {
      DataModel_Write(dataModel, Erd_FeaturePanCoolingMode, &featurePanCoolingMode);
   }

   void WhenFeaturePanCoolingModeIs(FeaturePanCoolingMode_t featurePanCoolingMode)
   {
      GivenFeaturePanCoolingModeIs(featurePanCoolingMode);
   }

   void GivenFreshFoodEvapFanGridVoteIs(FanVotedSpeed_t fanVotedSpeed)
   {
      DataModel_Write(dataModel, Erd_FreshFoodEvapFanSpeed_GridVote, &fanVotedSpeed);
   }

   void WhenFreshFoodEvapFanGridVoteIs(FanVotedSpeed_t fanVotedSpeed)
   {
      GivenFreshFoodEvapFanGridVoteIs(fanVotedSpeed);
   }

   void FreshFoodEvapFanSpeedShouldBe(FanVotedSpeed_t expected)
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_FreshFoodEvapFanSpeed_DeliPanSpeedVote, &actual);

      CHECK_EQUAL(expected.speed, actual.speed);
      CHECK_EQUAL(expected.care, actual.care);
   }
};

TEST(FeaturePanAsDeliPanEvapFanVoting, ShouldVoteFanSpeedToDeliCoolingSpeedWhenDeliCoolingSpeedIsGreaterThanGridVoteAndFeaturePanCoolingModeIsActiveCoolingOnInit)
{
   GivenFeaturePanCoolingModeIs(FeaturePanCoolingMode_ActiveCooling);
   GivenFreshFoodEvapFanGridVoteIs({ FeaturePanDeliCoolingSpeedParametric - 1, Vote_Care });
   GivenModuleIsInitialized();

   FreshFoodEvapFanSpeedShouldBe({ FeaturePanDeliCoolingSpeedParametric, Vote_Care });
}

TEST(FeaturePanAsDeliPanEvapFanVoting, ShouldVoteCareAndSetFanSpeedToGridVoteSpeedWhenGridVoteSpeedIsGreaterThanDeliCoolingSpeedAndFeaturePanCoolingModeIsActiveCoolingOnInit)
{
   GivenFeaturePanCoolingModeIs(FeaturePanCoolingMode_ActiveCooling);
   GivenFreshFoodEvapFanGridVoteIs({ FeaturePanDeliCoolingSpeedParametric + 1, Vote_Care });
   GivenModuleIsInitialized();

   FreshFoodEvapFanSpeedShouldBe({ FeaturePanDeliCoolingSpeedParametric + 1, Vote_Care });
}

TEST(FeaturePanAsDeliPanEvapFanVoting, ShouldVoteDontCareForFanSpeedWhenFeaturePanCoolingModeIsNotActiveCoolingOnInit)
{
   GivenFeaturePanCoolingModeIs(FeaturePanCoolingMode_Neutral);
   GivenModuleIsInitialized();

   FreshFoodEvapFanSpeedShouldBe({ FanSpeed_Off, Vote_DontCare });
}

TEST(FeaturePanAsDeliPanEvapFanVoting, ShouldVoteDontCareForFanSpeedWhenGridVoteIsDontCareOnInit)
{
   GivenFreshFoodEvapFanGridVoteIs({ FeaturePanDeliCoolingSpeedParametric - 1, Vote_DontCare });
   GivenModuleIsInitialized();

   FreshFoodEvapFanSpeedShouldBe({ FanSpeed_Off, Vote_DontCare });
}

TEST(FeaturePanAsDeliPanEvapFanVoting, ShouldVoteDontCareAndFanSpeedOffWhenFeaturePanCoolingModeChangesToNotActiveCooling)
{
   GivenFeaturePanCoolingModeIs(FeaturePanCoolingMode_ActiveCooling);
   GivenFreshFoodEvapFanGridVoteIs({ FeaturePanDeliCoolingSpeedParametric - 1, Vote_Care });
   GivenModuleIsInitialized();
   FreshFoodEvapFanSpeedShouldBe({ FeaturePanDeliCoolingSpeedParametric, Vote_Care });

   WhenFeaturePanCoolingModeIs(FeaturePanCoolingMode_Neutral);
   FreshFoodEvapFanSpeedShouldBe({ FanSpeed_Off, Vote_DontCare });
}

TEST(FeaturePanAsDeliPanEvapFanVoting, ShouldVoteForDeliCoolingSpeedWhenGridVoteSpeedChangesToLessThanDeliCoolingSpeed)
{
   GivenFeaturePanCoolingModeIs(FeaturePanCoolingMode_ActiveCooling);
   GivenFreshFoodEvapFanGridVoteIs({ FeaturePanDeliCoolingSpeedParametric + 1, Vote_Care });
   GivenModuleIsInitialized();
   FreshFoodEvapFanSpeedShouldBe({ FeaturePanDeliCoolingSpeedParametric + 1, Vote_Care });

   WhenFreshFoodEvapFanGridVoteIs({ FeaturePanDeliCoolingSpeedParametric - 1, Vote_Care });
   FreshFoodEvapFanSpeedShouldBe({ FeaturePanDeliCoolingSpeedParametric, Vote_Care });
}

TEST(FeaturePanAsDeliPanEvapFanVoting, ShouldVoteFanSpeedOffWhenGridVoteChangesToOff)
{
   GivenFeaturePanCoolingModeIs(FeaturePanCoolingMode_ActiveCooling);
   GivenFreshFoodEvapFanGridVoteIs({ FeaturePanDeliCoolingSpeedParametric + 1, Vote_Care });
   GivenModuleIsInitialized();
   FreshFoodEvapFanSpeedShouldBe({ FeaturePanDeliCoolingSpeedParametric + 1, Vote_Care });

   WhenFreshFoodEvapFanGridVoteIs({ FanSpeed_Off, Vote_Care });
   FreshFoodEvapFanSpeedShouldBe({ FanSpeed_Off, Vote_Care });
}

TEST(FeaturePanAsDeliPanEvapFanVoting, ShouldVoteDontCareWhenGridVoteChangesToDontCare)
{
   GivenFeaturePanCoolingModeIs(FeaturePanCoolingMode_ActiveCooling);
   GivenFreshFoodEvapFanGridVoteIs({ FeaturePanDeliCoolingSpeedParametric + 1, Vote_Care });
   GivenModuleIsInitialized();
   FreshFoodEvapFanSpeedShouldBe({ FeaturePanDeliCoolingSpeedParametric + 1, Vote_Care });

   WhenFreshFoodEvapFanGridVoteIs({ FeaturePanDeliCoolingSpeedParametric + 1, Vote_DontCare });
   FreshFoodEvapFanSpeedShouldBe({ FanSpeed_Off, Vote_DontCare });
}
