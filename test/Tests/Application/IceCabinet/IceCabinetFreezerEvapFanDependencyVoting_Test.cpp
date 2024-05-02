/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "IceCabinetFreezerEvapFanDependencyVoting.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

const IceCabinetFreezerEvapFanDependencyVotingConfiguration_t config = {
   .freezerEvapFanSpeedResolvedVoteErd = Erd_FreezerEvapFanSpeed_ResolvedVote,
   .iceCabinetDependencyOnFreezerFanVoteErd = Erd_IceCabinetFanSpeed_FreezerFanDependencyVote
};

TEST_GROUP(IceCabinetFreezerEvapFanDependencyVoting)
{
   IceCabinetFreezerEvapFanDependencyVoting_t instance;
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
      IceCabinetFreezerEvapFanDependencyVoting_Init(
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

   void IceCabinetFanVoteShouldBe(FanVotedSpeed_t expected)
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_IceCabinetFanSpeed_FreezerFanDependencyVote, &actual);

      CHECK_EQUAL(expected.speed, actual.speed);
      CHECK_EQUAL(expected.care, actual.care);
   }
};

TEST(IceCabinetFreezerEvapFanDependencyVoting, ShouldVoteOffForIceCabinetFanWhenFreezerEvapResolvedFanIsOffOnInit)
{
   GivenFreezerEvapFanResolvedVoteIs({ FanSpeed_Off, Vote_Care });
   GivenModuleIsInitialized();

   IceCabinetFanVoteShouldBe({ FanSpeed_Off, Vote_Care });
}

TEST(IceCabinetFreezerEvapFanDependencyVoting, ShouldVoteDontCareForIceCabinetFanWhenFreezerEvapResolvedFanIsNotOffOnInit)
{
   GivenFreezerEvapFanResolvedVoteIs({ FanSpeed_High, Vote_Care });
   GivenModuleIsInitialized();

   IceCabinetFanVoteShouldBe({ FanSpeed_Off, Vote_DontCare });
}

TEST(IceCabinetFreezerEvapFanDependencyVoting, ShouldVoteOffForIceCabinetFanWhenFreezerEvapResolvedFanChangesToOff)
{
   GivenFreezerEvapFanResolvedVoteIs({ FanSpeed_High, Vote_Care });
   GivenModuleIsInitialized();
   IceCabinetFanVoteShouldBe({ FanSpeed_Off, Vote_DontCare });

   WhenFreezerEvapFanResolvedVoteIs({ FanSpeed_Off, Vote_Care });
   IceCabinetFanVoteShouldBe({ FanSpeed_Off, Vote_Care });
}

TEST(IceCabinetFreezerEvapFanDependencyVoting, ShouldVoteDontCareForIceCabinetFanWhenFreezerEvapResolvedFanChangesToNotOff)
{
   GivenFreezerEvapFanResolvedVoteIs({ FanSpeed_Off, Vote_Care });
   GivenModuleIsInitialized();
   IceCabinetFanVoteShouldBe({ FanSpeed_Off, Vote_Care });

   WhenFreezerEvapFanResolvedVoteIs({ FanSpeed_High, Vote_Care });
   IceCabinetFanVoteShouldBe({ FanSpeed_Off, Vote_DontCare });
}

TEST(IceCabinetFreezerEvapFanDependencyVoting, ShouldVoteDontCareForIceCabinetFanWhenFreezerEvapResolvedFanIsDontCareOnInit)
{
   GivenFreezerEvapFanResolvedVoteIs({ FanSpeed_Off, Vote_DontCare });
   GivenModuleIsInitialized();

   IceCabinetFanVoteShouldBe({ FanSpeed_Off, Vote_DontCare });
}

TEST(IceCabinetFreezerEvapFanDependencyVoting, ShouldVoteDontCareForIceCabinetFanWhenFreezerEvapResolvedFanChangesToDontCare)
{
   GivenFreezerEvapFanResolvedVoteIs({ FanSpeed_Off, Vote_Care });
   GivenModuleIsInitialized();
   IceCabinetFanVoteShouldBe({ FanSpeed_Off, Vote_Care });

   WhenFreezerEvapFanResolvedVoteIs({ FanSpeed_Off, Vote_DontCare });
   IceCabinetFanVoteShouldBe({ FanSpeed_Off, Vote_DontCare });
}
