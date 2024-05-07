/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "IceCabinetFanStartupSpeedVoting.h"
#include "SystemErds.h"
#include "Constants_Time.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

static const IceCabinetFanStartupSpeedVotingConfig_t config = {
   .iceCabinetFanResolvedVoteErd = Erd_IceCabinetFanSpeed_ResolvedVote,
   .iceCabinetFanStartupSpeedVoteErd = Erd_IceCabinetFanSpeed_StartupVote
};

static const IceCabinetFanStartupData_t data = {
   .startupSpeed = FanSpeed_Medium,
   .startupTimeInSeconds = 3
};

TEST_GROUP(IceCabinetFanStartupSpeedVoting)
{
   IceCabinetFanStartupSpeedVoting_t instance;
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);
   }

   void GivenModuleIsInitialized()
   {
      IceCabinetFanStartupSpeedVoting_Init(
         &instance,
         dataModel,
         &data,
         &config);
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks);
   }

   void GivenIceCabinetFanSpeedResolvedVoteIs(FanVotedSpeed_t vote)
   {
      DataModel_Write(dataModel, Erd_IceCabinetFanSpeed_ResolvedVote, &vote);
   }

   void WhenIceCabinetFanSpeedResolvedVoteIs(FanVotedSpeed_t vote)
   {
      GivenIceCabinetFanSpeedResolvedVoteIs(vote);
   }

   void IceCabinetFanSpeedStartupVoteShouldBe(FanVotedSpeed_t expected)
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_IceCabinetFanSpeed_StartupVote, &actual);

      CHECK_EQUAL(expected.speed, actual.speed);
      CHECK_EQUAL(expected.care, actual.care);
   }
};

TEST(IceCabinetFanStartupSpeedVoting, ShouldVoteIceCabinetFanWithStartupSpeedIfResolvedVoteIsNotOff)
{
   GivenIceCabinetFanSpeedResolvedVoteIs({ .speed = FanSpeed_Low, .care = Vote_Care });
   GivenModuleIsInitialized();

   IceCabinetFanSpeedStartupVoteShouldBe({ .speed = data.startupSpeed, .care = Vote_Care });
}

TEST(IceCabinetFanStartupSpeedVoting, ShouldVoteOffAndDontCareIfResolvedVoteIsOff)
{
   GivenIceCabinetFanSpeedResolvedVoteIs({ .speed = FanSpeed_Off, .care = Vote_Care });
   GivenModuleIsInitialized();

   IceCabinetFanSpeedStartupVoteShouldBe({ .speed = FanSpeed_Off, .care = Vote_DontCare });
}

TEST(IceCabinetFanStartupSpeedVoting, ShouldVoteIceCabinetFanWithStartupSpeedIfResolvedVoteIsNotOffForStartupTimeThenVoteOffAndDontCare)
{
   GivenIceCabinetFanSpeedResolvedVoteIs({ .speed = FanSpeed_Low, .care = Vote_Care });
   GivenModuleIsInitialized();

   IceCabinetFanSpeedStartupVoteShouldBe({ .speed = data.startupSpeed, .care = Vote_Care });

   After(data.startupTimeInSeconds * MSEC_PER_SEC - 1);
   IceCabinetFanSpeedStartupVoteShouldBe({ .speed = data.startupSpeed, .care = Vote_Care });

   After(1);
   IceCabinetFanSpeedStartupVoteShouldBe({ .speed = FanSpeed_Off, .care = Vote_DontCare });
}

TEST(IceCabinetFanStartupSpeedVoting, ShouldVoteIceCabinetFanWithStartupSpeedWhenResolvedVoteChangeToANotOffSpeedForStartupTimeThenVoteOffAndDontCare)
{
   GivenIceCabinetFanSpeedResolvedVoteIs({ .speed = FanSpeed_Off, .care = Vote_Care });
   GivenModuleIsInitialized();

   WhenIceCabinetFanSpeedResolvedVoteIs({ .speed = FanSpeed_Low, .care = Vote_Care });
   IceCabinetFanSpeedStartupVoteShouldBe({ .speed = data.startupSpeed, .care = Vote_Care });

   After(data.startupTimeInSeconds * MSEC_PER_SEC - 1);
   IceCabinetFanSpeedStartupVoteShouldBe({ .speed = data.startupSpeed, .care = Vote_Care });

   After(1);
   IceCabinetFanSpeedStartupVoteShouldBe({ .speed = FanSpeed_Off, .care = Vote_DontCare });
}

TEST(IceCabinetFanStartupSpeedVoting, ShouldNotVoteDontCareWhenResolvedVoteChangesToOffBeforeStartupTimeCompletes)
{
   GivenIceCabinetFanSpeedResolvedVoteIs({ .speed = FanSpeed_Low, .care = Vote_Care });
   GivenModuleIsInitialized();

   IceCabinetFanSpeedStartupVoteShouldBe({ .speed = data.startupSpeed, .care = Vote_Care });

   After(data.startupTimeInSeconds * MSEC_PER_SEC - 1);
   IceCabinetFanSpeedStartupVoteShouldBe({ .speed = data.startupSpeed, .care = Vote_Care });

   WhenIceCabinetFanSpeedResolvedVoteIs({ .speed = FanSpeed_Off, .care = Vote_Care });
   IceCabinetFanSpeedStartupVoteShouldBe({ .speed = data.startupSpeed, .care = Vote_Care });

   After(1);
   IceCabinetFanSpeedStartupVoteShouldBe({ .speed = FanSpeed_Off, .care = Vote_DontCare });
}

TEST(IceCabinetFanStartupSpeedVoting, ShouldRestartStartupTimerWhenResolvedVoteChangesBackToNonOffSpeed)
{
   GivenIceCabinetFanSpeedResolvedVoteIs({ .speed = FanSpeed_Low, .care = Vote_Care });
   GivenModuleIsInitialized();

   IceCabinetFanSpeedStartupVoteShouldBe({ .speed = data.startupSpeed, .care = Vote_Care });

   After(data.startupTimeInSeconds * MSEC_PER_SEC - 1);
   IceCabinetFanSpeedStartupVoteShouldBe({ .speed = data.startupSpeed, .care = Vote_Care });

   WhenIceCabinetFanSpeedResolvedVoteIs({ .speed = FanSpeed_Off, .care = Vote_Care });
   IceCabinetFanSpeedStartupVoteShouldBe({ .speed = data.startupSpeed, .care = Vote_Care });

   WhenIceCabinetFanSpeedResolvedVoteIs({ .speed = FanSpeed_Low, .care = Vote_Care });
   After(data.startupTimeInSeconds * MSEC_PER_SEC - 1);
   IceCabinetFanSpeedStartupVoteShouldBe({ .speed = data.startupSpeed, .care = Vote_Care });

   After(1);
   IceCabinetFanSpeedStartupVoteShouldBe({ .speed = FanSpeed_Off, .care = Vote_DontCare });
}

TEST(IceCabinetFanStartupSpeedVoting, ShouldNotRestartStartupTimerWhenResolvedSpeedChangesToADifferentNonOffSpeed)
{
   GivenIceCabinetFanSpeedResolvedVoteIs({ .speed = FanSpeed_Low, .care = Vote_Care });
   GivenModuleIsInitialized();

   IceCabinetFanSpeedStartupVoteShouldBe({ .speed = data.startupSpeed, .care = Vote_Care });
   WhenIceCabinetFanSpeedResolvedVoteIs({ .speed = data.startupSpeed, .care = Vote_Care }); // assume it wins

   After(data.startupTimeInSeconds * MSEC_PER_SEC - 1);
   IceCabinetFanSpeedStartupVoteShouldBe({ .speed = data.startupSpeed, .care = Vote_Care });

   After(1);
   IceCabinetFanSpeedStartupVoteShouldBe({ .speed = FanSpeed_Off, .care = Vote_DontCare });
}
