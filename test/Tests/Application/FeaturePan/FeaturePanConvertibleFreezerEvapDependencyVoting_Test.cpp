/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "FeaturePanConvertibleFreezerEvapFanDependencyVoting.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

const FeaturePanConvertibleFreezerEvapFanDependencyVotingConfiguration_t config = {
   .freezerEvapFanResolvedVoteErd = Erd_FreezerEvapFanSpeed_ResolvedVote,
   .convertibleCompartmentDamperVoteErd = Erd_ConvertibleCompartmentDamperPosition_FreezerEvapFanDependencyVote
};

TEST_GROUP(FeaturePanConvertibleFreezerEvapFanDependencyVoting)
{
   FeaturePanConvertibleFreezerEvapFanDependencyVoting_t instance;
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
      FeaturePanConvertibleFreezerEvapFanDependencyVoting_Init(
         &instance,
         dataModel,
         &config);
   }

   void GivenFreezerEvapFanResolvedVoteIs(FanVotedSpeed_t fanSpeed)
   {
      DataModel_Write(dataModel, Erd_FreezerEvapFanSpeed_ResolvedVote, &fanSpeed);
   }

   void WhenFreezerEvapFanResolvedVoteIs(FanVotedSpeed_t fanSpeed)
   {
      GivenFreezerEvapFanResolvedVoteIs(fanSpeed);
   }

   void ConvertibleCompartmentDamperPositionShouldBe(DamperVotedPosition_t expected)
   {
      DamperVotedPosition_t actual;
      DataModel_Read(dataModel, Erd_ConvertibleCompartmentDamperPosition_FreezerEvapFanDependencyVote, &actual);

      CHECK_EQUAL(expected.position, actual.position);
      CHECK_EQUAL(expected.care, actual.care);
   }
};

TEST(FeaturePanConvertibleFreezerEvapFanDependencyVoting, ShouldVoteClosedForConvertibleCompartmentDamperWhenFreezerEvapResolvedFanIsOffOnInit)
{
   GivenFreezerEvapFanResolvedVoteIs({ FanSpeed_Off, Vote_Care });
   GivenModuleIsInitialized();

   ConvertibleCompartmentDamperPositionShouldBe({ DamperPosition_Closed, Vote_Care });
}

TEST(FeaturePanConvertibleFreezerEvapFanDependencyVoting, ShouldVoteDontCareForConvertibleCompartmentDamperWhenFreezerEvapResolvedFanIsNotOffOnInit)
{
   GivenFreezerEvapFanResolvedVoteIs({ FanSpeed_High, Vote_Care });
   GivenModuleIsInitialized();

   ConvertibleCompartmentDamperPositionShouldBe({ DamperPosition_Closed, Vote_DontCare });
}

TEST(FeaturePanConvertibleFreezerEvapFanDependencyVoting, ShouldVoteClosedForConvertibleCompartmentDamperWhenFreezerEvapResolvedFanChangesToOff)
{
   GivenFreezerEvapFanResolvedVoteIs({ FanSpeed_High, Vote_Care });
   GivenModuleIsInitialized();
   ConvertibleCompartmentDamperPositionShouldBe({ DamperPosition_Closed, Vote_DontCare });

   WhenFreezerEvapFanResolvedVoteIs({ FanSpeed_Off, Vote_Care });
   ConvertibleCompartmentDamperPositionShouldBe({ DamperPosition_Closed, Vote_Care });
}

TEST(FeaturePanConvertibleFreezerEvapFanDependencyVoting, ShouldVoteDontCareForConvertibleCompartmentDamperWhenFreezerEvapResolvedFanChangesToNotOff)
{
   GivenFreezerEvapFanResolvedVoteIs({ FanSpeed_Off, Vote_Care });
   GivenModuleIsInitialized();
   ConvertibleCompartmentDamperPositionShouldBe({ DamperPosition_Closed, Vote_Care });

   WhenFreezerEvapFanResolvedVoteIs({ FanSpeed_High, Vote_Care });
   ConvertibleCompartmentDamperPositionShouldBe({ DamperPosition_Closed, Vote_DontCare });
}

TEST(FeaturePanConvertibleFreezerEvapFanDependencyVoting, ShouldVoteDontCareForConvertibleCompartmentDamperWhenFreezerEvapResolvedFanIsDontCareOnInit)
{
   GivenFreezerEvapFanResolvedVoteIs({ FanSpeed_Off, Vote_DontCare });
   GivenModuleIsInitialized();

   ConvertibleCompartmentDamperPositionShouldBe({ DamperPosition_Closed, Vote_DontCare });
}

TEST(FeaturePanConvertibleFreezerEvapFanDependencyVoting, ShouldVoteDontCareForConvertibleCompartmentDamperWhenFreezerEvapResolvedFanChangesToDontCare)
{
   GivenFreezerEvapFanResolvedVoteIs({ FanSpeed_Off, Vote_Care });
   GivenModuleIsInitialized();
   ConvertibleCompartmentDamperPositionShouldBe({ DamperPosition_Closed, Vote_Care });

   WhenFreezerEvapFanResolvedVoteIs({ FanSpeed_Off, Vote_DontCare });
   ConvertibleCompartmentDamperPositionShouldBe({ DamperPosition_Closed, Vote_DontCare });
}
