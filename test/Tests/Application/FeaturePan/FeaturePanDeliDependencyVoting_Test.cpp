/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "FeaturePanDeliDependencyVoting.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

const FeaturePanDeliDependencyVotingConfiguration_t config = {
   .evapFanResolvedVoteErd = Erd_FreshFoodEvapFanSpeed_ResolvedVote,
   .fanVoteErd = Erd_DeliFanSpeed_FreshFoodFanDependencyVote,
   .damperVoteErd = Erd_DeliDamperPosition_FreshFoodFanDependencyVote
};

TEST_GROUP(FeaturePanDeliDependencyVoting)
{
   FeaturePanDeliDependencyVoting_t instance;
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
      FeaturePanDeliDependencyVoting_Init(
         &instance,
         dataModel,
         &config);
   }

   void GivenEvapFanResolvedVoteIs(FanVotedSpeed_t fanSpeed)
   {
      DataModel_Write(dataModel, Erd_FreshFoodEvapFanSpeed_ResolvedVote, &fanSpeed);
   }

   void WhenEvapFanResolvedVoteIs(FanVotedSpeed_t fanSpeed)
   {
      GivenEvapFanResolvedVoteIs(fanSpeed);
   }

   void FanVoteShouldBe(FanVotedSpeed_t expected)
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_DeliFanSpeed_FreshFoodFanDependencyVote, &actual);

      CHECK_EQUAL(expected.speed, actual.speed);
      CHECK_EQUAL(expected.care, actual.care);
   }

   void DamperPositionShouldBe(DamperVotedPosition_t expected)
   {
      DamperVotedPosition_t actual;
      DataModel_Read(dataModel, Erd_DeliDamperPosition_FreshFoodFanDependencyVote, &actual);

      CHECK_EQUAL(expected.position, actual.position);
      CHECK_EQUAL(expected.care, actual.care);
   }
};

TEST(FeaturePanDeliDependencyVoting, ShouldVoteOffForFanWhenEvapResolvedFanIsOffOnInit)
{
   GivenEvapFanResolvedVoteIs({ FanSpeed_Off, Vote_Care });
   GivenModuleIsInitialized();

   FanVoteShouldBe({ FanSpeed_Off, Vote_Care });
}

TEST(FeaturePanDeliDependencyVoting, ShouldVoteDontCareForFanWhenEvapResolvedFanIsNotOffOnInit)
{
   GivenEvapFanResolvedVoteIs({ FanSpeed_High, Vote_Care });
   GivenModuleIsInitialized();

   FanVoteShouldBe({ FanSpeed_Off, Vote_DontCare });
}

TEST(FeaturePanDeliDependencyVoting, ShouldVoteClosedForDamperWhenEvapResolvedFanIsOffOnInit)
{
   GivenEvapFanResolvedVoteIs({ FanSpeed_Off, Vote_Care });
   GivenModuleIsInitialized();

   DamperPositionShouldBe({ DamperPosition_Closed, Vote_Care });
}

TEST(FeaturePanDeliDependencyVoting, ShouldVoteDontCareForDamperWhenEvapResolvedFanIsNotOffOnInit)
{
   GivenEvapFanResolvedVoteIs({ FanSpeed_High, Vote_Care });
   GivenModuleIsInitialized();

   DamperPositionShouldBe({ DamperPosition_Closed, Vote_DontCare });
}

TEST(FeaturePanDeliDependencyVoting, ShouldVoteOffForFanWhenEvapResolvedFanChangesToOff)
{
   GivenEvapFanResolvedVoteIs({ FanSpeed_High, Vote_Care });
   GivenModuleIsInitialized();
   FanVoteShouldBe({ FanSpeed_Off, Vote_DontCare });

   WhenEvapFanResolvedVoteIs({ FanSpeed_Off, Vote_Care });
   FanVoteShouldBe({ FanSpeed_Off, Vote_Care });
}

TEST(FeaturePanDeliDependencyVoting, ShouldVoteDontCareForFanWhenEvapResolvedFanChangesToNotOff)
{
   GivenEvapFanResolvedVoteIs({ FanSpeed_Off, Vote_Care });
   GivenModuleIsInitialized();
   FanVoteShouldBe({ FanSpeed_Off, Vote_Care });

   WhenEvapFanResolvedVoteIs({ FanSpeed_High, Vote_Care });
   FanVoteShouldBe({ FanSpeed_Off, Vote_DontCare });
}

TEST(FeaturePanDeliDependencyVoting, ShouldVoteClosedForDamperWhenEvapResolvedFanChangesToOff)
{
   GivenEvapFanResolvedVoteIs({ FanSpeed_High, Vote_Care });
   GivenModuleIsInitialized();
   DamperPositionShouldBe({ DamperPosition_Closed, Vote_DontCare });

   WhenEvapFanResolvedVoteIs({ FanSpeed_Off, Vote_Care });
   DamperPositionShouldBe({ DamperPosition_Closed, Vote_Care });
}

TEST(FeaturePanDeliDependencyVoting, ShouldVoteDontCareForDamperWhenEvapResolvedFanChangesToNotOff)
{
   GivenEvapFanResolvedVoteIs({ FanSpeed_Off, Vote_Care });
   GivenModuleIsInitialized();
   DamperPositionShouldBe({ DamperPosition_Closed, Vote_Care });

   WhenEvapFanResolvedVoteIs({ FanSpeed_High, Vote_Care });
   DamperPositionShouldBe({ DamperPosition_Closed, Vote_DontCare });
}

TEST(FeaturePanDeliDependencyVoting, ShouldVoteDontCareForFanAndDamperWhenEvapResolvedFanIsDontCareOnInit)
{
   GivenEvapFanResolvedVoteIs({ FanSpeed_Off, Vote_DontCare });
   GivenModuleIsInitialized();

   FanVoteShouldBe({ FanSpeed_Off, Vote_DontCare });
   DamperPositionShouldBe({ DamperPosition_Closed, Vote_DontCare });
}

TEST(FeaturePanDeliDependencyVoting, ShouldVoteDontCareForFanAndDamperWhenEvapResolvedFanChangesToDontCare)
{
   GivenEvapFanResolvedVoteIs({ FanSpeed_Off, Vote_Care });
   GivenModuleIsInitialized();
   FanVoteShouldBe({ FanSpeed_Off, Vote_Care });
   DamperPositionShouldBe({ DamperPosition_Closed, Vote_Care });

   WhenEvapFanResolvedVoteIs({ FanSpeed_Off, Vote_DontCare });
   FanVoteShouldBe({ FanSpeed_Off, Vote_DontCare });
   DamperPositionShouldBe({ DamperPosition_Closed, Vote_DontCare });
}
