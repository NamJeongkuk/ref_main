/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "Application.h"
#include "DataModelErdPointerAccess.h"
#include "PersonalityParametricData.h"
#include "Constants_Time.h"
#include "SystemErds.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"
#include "TddPersonality.h"

TEST_GROUP(IceCabinetFanStartupSpeedVotingIntegration)
{
   Application_t instance;
   ResetReason_t resetReason;
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   const IceCabinetFanStartupData_t *iceCabinetFanStartupData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble, TddPersonality_SwqaMultidoor);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      iceCabinetFanStartupData = PersonalityParametricData_Get(dataModel)->iceCabinetData->iceCabinetFanStartupData;
   }

   void GivenApplicationHasBeenInitialized()
   {
      Application_Init(
         &instance,
         dataModel,
         resetReason);
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks);
   }

   void WhenIceCabinetFanSpeedCoolingSystemOffVoteIs(FanVotedSpeed_t vote)
   {
      DataModel_Write(dataModel, Erd_IceCabinetFanSpeed_CoolingSystemOffVote, &vote);
   }

   void WhenIceCabinetFanSpeedLoadOffDoorOpenVoteIs(FanVotedSpeed_t vote)
   {
      DataModel_Write(dataModel, Erd_IceCabinetFanSpeed_LoadOffDoorOpenVote, &vote);
   }

   void IceCabinetFanSpeedResolvedVoteShouldBe(FanVotedSpeed_t expected)
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_IceCabinetFanSpeed_ResolvedVote, &actual);

      CHECK_EQUAL(expected.speed, actual.speed);
      CHECK_EQUAL(expected.care, actual.care);
   }

   void IceCabinetFanSpeedStartupVoteShouldBe(FanVotedSpeed_t expected)
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_IceCabinetFanSpeed_StartupVote, &actual);

      CHECK_EQUAL(expected.speed, actual.speed);
      CHECK_EQUAL(expected.care, actual.care);
   }

   void IceCabinetFanSpeedWinningVoteErdIs(WinningVoteErd_t expected)
   {
      WinningVoteErd_t actual;
      DataModel_Read(dataModel, Erd_IceCabinetFanSpeed_WinningVoteErd, &actual);

      CHECK_EQUAL(expected, actual);
   }
};

TEST(IceCabinetFanStartupSpeedVotingIntegration, ShouldVoteWithStartupSpeedAndCareForStartupVoteWhenResolvedVoteIsNonOffSpeed)
{
   GivenApplicationHasBeenInitialized();
   IceCabinetFanSpeedResolvedVoteShouldBe({ .speed = FanSpeed_Off, .care = Vote_DontCare });

   WhenIceCabinetFanSpeedCoolingSystemOffVoteIs({ .speed = FanSpeed_Low, .care = Vote_Care });
   IceCabinetFanSpeedResolvedVoteShouldBe({ .speed = FanSpeed_Low, .care = Vote_Care });
   IceCabinetFanSpeedWinningVoteErdIs(Erd_IceCabinetFanSpeed_CoolingSystemOffVote);
   IceCabinetFanSpeedStartupVoteShouldBe({ .speed = iceCabinetFanStartupData->startupSpeed, .care = Vote_Care });
}

TEST(IceCabinetFanStartupSpeedVotingIntegration, ShouldNotVoteDontCareForStartupVoteWhenResolvedVoteChangesToOff)
{
   GivenApplicationHasBeenInitialized();
   IceCabinetFanSpeedResolvedVoteShouldBe({ .speed = FanSpeed_Off, .care = Vote_DontCare });

   WhenIceCabinetFanSpeedCoolingSystemOffVoteIs({ .speed = FanSpeed_Low, .care = Vote_Care });
   IceCabinetFanSpeedResolvedVoteShouldBe({ .speed = FanSpeed_Low, .care = Vote_Care });
   IceCabinetFanSpeedWinningVoteErdIs(Erd_IceCabinetFanSpeed_CoolingSystemOffVote);
   IceCabinetFanSpeedStartupVoteShouldBe({ .speed = iceCabinetFanStartupData->startupSpeed, .care = Vote_Care });

   WhenIceCabinetFanSpeedCoolingSystemOffVoteIs({ .speed = FanSpeed_Off, .care = Vote_Care });
   IceCabinetFanSpeedResolvedVoteShouldBe({ .speed = FanSpeed_Off, .care = Vote_Care });
   IceCabinetFanSpeedWinningVoteErdIs(Erd_IceCabinetFanSpeed_CoolingSystemOffVote);
   IceCabinetFanSpeedStartupVoteShouldBe({ .speed = iceCabinetFanStartupData->startupSpeed, .care = Vote_Care });
}

TEST(IceCabinetFanStartupSpeedVotingIntegration, ShouldVoteWithStartupSpeedAndCareForStartupVoteForStartupTimeWhenResolvedVoteChangesFromOffToNonOffSpeed)
{
   GivenApplicationHasBeenInitialized();
   IceCabinetFanSpeedResolvedVoteShouldBe({ .speed = FanSpeed_Off, .care = Vote_DontCare });

   WhenIceCabinetFanSpeedCoolingSystemOffVoteIs({ .speed = FanSpeed_Low, .care = Vote_Care });
   IceCabinetFanSpeedResolvedVoteShouldBe({ .speed = FanSpeed_Low, .care = Vote_Care });
   IceCabinetFanSpeedWinningVoteErdIs(Erd_IceCabinetFanSpeed_CoolingSystemOffVote);
   IceCabinetFanSpeedStartupVoteShouldBe({ .speed = iceCabinetFanStartupData->startupSpeed, .care = Vote_Care });

   After(iceCabinetFanStartupData->startupTimeInSeconds * MSEC_PER_SEC - 1);
   IceCabinetFanSpeedStartupVoteShouldBe({ .speed = iceCabinetFanStartupData->startupSpeed, .care = Vote_Care });

   After(1);
   IceCabinetFanSpeedStartupVoteShouldBe({ .speed = FanSpeed_Off, .care = Vote_DontCare });
}

TEST(IceCabinetFanStartupSpeedVotingIntegration, ShouldNotRestartStartupTimerIfResolvedVoteChangesToADifferentNonOffSpeed)
{
   GivenApplicationHasBeenInitialized();
   IceCabinetFanSpeedResolvedVoteShouldBe({ .speed = FanSpeed_Off, .care = Vote_DontCare });

   WhenIceCabinetFanSpeedCoolingSystemOffVoteIs({ .speed = FanSpeed_Low, .care = Vote_Care });
   IceCabinetFanSpeedResolvedVoteShouldBe({ .speed = FanSpeed_Low, .care = Vote_Care });
   IceCabinetFanSpeedWinningVoteErdIs(Erd_IceCabinetFanSpeed_CoolingSystemOffVote);
   IceCabinetFanSpeedStartupVoteShouldBe({ .speed = iceCabinetFanStartupData->startupSpeed, .care = Vote_Care });

   After(iceCabinetFanStartupData->startupTimeInSeconds * MSEC_PER_SEC - 1);
   IceCabinetFanSpeedStartupVoteShouldBe({ .speed = iceCabinetFanStartupData->startupSpeed, .care = Vote_Care });

   WhenIceCabinetFanSpeedCoolingSystemOffVoteIs({ .speed = FanSpeed_Medium, .care = Vote_Care });
   IceCabinetFanSpeedResolvedVoteShouldBe({ .speed = FanSpeed_Medium, .care = Vote_Care });
   IceCabinetFanSpeedWinningVoteErdIs(Erd_IceCabinetFanSpeed_CoolingSystemOffVote);
   IceCabinetFanSpeedStartupVoteShouldBe({ .speed = iceCabinetFanStartupData->startupSpeed, .care = Vote_Care });

   After(1);
   IceCabinetFanSpeedStartupVoteShouldBe({ .speed = FanSpeed_Off, .care = Vote_DontCare });
}

TEST(IceCabinetFanStartupSpeedVotingIntegration, ShouldFallToNextLowestPriorityWhenIceCabinetFanSpeedStartupVoteIsWinningThenNoLongerCares)
{
   GivenApplicationHasBeenInitialized();
   IceCabinetFanSpeedResolvedVoteShouldBe({ .speed = FanSpeed_Off, .care = Vote_DontCare });

   WhenIceCabinetFanSpeedLoadOffDoorOpenVoteIs({ .speed = FanSpeed_High, .care = Vote_Care });

   IceCabinetFanSpeedResolvedVoteShouldBe({ .speed = iceCabinetFanStartupData->startupSpeed, .care = Vote_Care }); // Startup vote is higher priority than load off door open
   IceCabinetFanSpeedWinningVoteErdIs(Erd_IceCabinetFanSpeed_StartupVote);
   IceCabinetFanSpeedStartupVoteShouldBe({ .speed = iceCabinetFanStartupData->startupSpeed, .care = Vote_Care });

   After(iceCabinetFanStartupData->startupTimeInSeconds * MSEC_PER_SEC - 1);
   IceCabinetFanSpeedStartupVoteShouldBe({ .speed = iceCabinetFanStartupData->startupSpeed, .care = Vote_Care });
   IceCabinetFanSpeedWinningVoteErdIs(Erd_IceCabinetFanSpeed_StartupVote);

   After(1);
   IceCabinetFanSpeedStartupVoteShouldBe({ .speed = FanSpeed_Off, .care = Vote_DontCare });
   IceCabinetFanSpeedResolvedVoteShouldBe({ .speed = FanSpeed_High, .care = Vote_Care }); // Load off door open vote wins
   IceCabinetFanSpeedWinningVoteErdIs(Erd_IceCabinetFanSpeed_LoadOffDoorOpenVote);
}
