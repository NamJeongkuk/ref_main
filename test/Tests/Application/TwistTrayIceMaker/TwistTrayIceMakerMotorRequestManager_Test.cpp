/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "TwistTrayIceMakerMotorRequestManager.h"
#include "Constants_Binary.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"

enum
{
   Signal_One = 1
};

enum
{
   PollingFrequencyInMsec = 150
};

static const TwistTrayIceMakerMotorRequestManagerConfig_t requestManagerConfig = {
   .resolvedVoteErd = Erd_TwistTrayIceMakerMotor_ResolvedVote,
   .motorRequestErd = Erd_TwistIceMakerMotorControlRequest,
   .motorEnableErd = Erd_TwistIceMakerMotorDriveEnable,
   .motorActionResultErd = Erd_TwistTrayIceMaker_MotorActionResult,
   .motorDoActionErd = Erd_TwistTrayIceMaker_MotorDoAction
};

TEST_GROUP(TwistTrayIceMakerMotorRequestManager)
{
   TwistTrayIceMakerMotorRequestManager_t instance;

   ReferDataModel_TestDouble_t referDataModelTestDouble;
   I_DataModel_t *dataModel;

   TimerModule_TestDouble_t *timerModuleTestDouble;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&referDataModelTestDouble);
      dataModel = referDataModelTestDouble.dataModel;

      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&referDataModelTestDouble);
   }

   void GivenTheModuleIsInitialized()
   {
      TwistTrayIceMakerMotorRequestManager_Init(
         &instance,
         dataModel,
         &requestManagerConfig);
   }

   void WhenTheMotorResolvedVoteIs(TwistTrayIceMakerMotorAction_t action)
   {
      TwistTrayIceMakerMotorVotedAction_t vote = {
         .action = action,
         .care = Vote_Care
      };

      DataModel_Write(
         dataModel,
         Erd_TwistTrayIceMakerMotor_ResolvedVote,
         &vote);
   }

   void GivenTheMotorResolvedVoteIs(TwistTrayIceMakerMotorAction_t action)
   {
      WhenTheMotorResolvedVoteIs(action);
   }

   void TheMotorControlRequestShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(
         dataModel,
         Erd_TwistIceMakerMotorControlRequest,
         &actual);

      CHECK_EQUAL(expected, actual);
   }

   void WhenTheMotorDriveEnableIs(bool state)
   {
      DataModel_Write(
         dataModel,
         Erd_TwistIceMakerMotorDriveEnable,
         &state);
   }

   void GivenTheMotorDriveEnableIs(bool state)
   {
      WhenTheMotorDriveEnableIs(state);
   }

   void DoActionShouldBe(
      TwistTrayIceMakerMotorAction_t expectedAction,
      Signal_t expectedSignal)
   {
      TwistTrayIceMakerMotorDoAction_t actual;
      DataModel_Read(
         dataModel,
         Erd_TwistTrayIceMaker_MotorDoAction,
         &actual);

      CHECK_EQUAL(expectedAction, actual.action);
      CHECK_EQUAL(expectedSignal, actual.signal);
   }

   void GivenTheModuleIsInitializedAndTheMotorIsRunning()
   {
      GivenTheModuleIsInitialized();
      GivenTheMotorResolvedVoteIs(TwistTrayIceMakerMotorAction_RunCycle);

      WhenTheMotorDriveEnableIs(SET);
      DoActionShouldBe(TwistTrayIceMakerMotorAction_RunCycle, Signal_One);
   }

   void WhenTheMotorActionResultIs(TwistTrayIceMakerMotorActionResult_t actionResult)
   {
      DataModel_Write(
         dataModel,
         Erd_TwistTrayIceMaker_MotorActionResult,
         &actionResult);
   }

   void After(TimerTicks_t ticks, TimeSourceTickCount_t ticksToElapseAtATime = 1000)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks, ticksToElapseAtATime);
   }
};

TEST(TwistTrayIceMakerMotorRequestManager, ShouldInitialize)
{
   GivenTheModuleIsInitialized();
}

TEST(TwistTrayIceMakerMotorRequestManager, ShouldSetMotorControlRequestWhenMotorActionChangesToRunHomingRoutine)
{
   GivenTheMotorResolvedVoteIs(TwistTrayIceMakerMotorAction_Idle);
   GivenTheModuleIsInitialized();
   TheMotorControlRequestShouldBe(CLEAR);

   WhenTheMotorResolvedVoteIs(TwistTrayIceMakerMotorAction_RunHomingRoutine);
   TheMotorControlRequestShouldBe(SET);
}

TEST(TwistTrayIceMakerMotorRequestManager, ShouldSetMotorControlRequestWhenMotorActionChangesToRunCycle)
{
   GivenTheMotorResolvedVoteIs(TwistTrayIceMakerMotorAction_Idle);
   GivenTheModuleIsInitialized();
   TheMotorControlRequestShouldBe(CLEAR);

   WhenTheMotorResolvedVoteIs(TwistTrayIceMakerMotorAction_RunCycle);
   TheMotorControlRequestShouldBe(SET);
}

TEST(TwistTrayIceMakerMotorRequestManager, ShouldNotRequestMotorControlIfVoteChangesToIdle)
{
   GivenTheModuleIsInitialized();

   WhenTheMotorResolvedVoteIs(TwistTrayIceMakerMotorAction_RunCycle);
   TheMotorControlRequestShouldBe(SET);
   WhenTheMotorActionResultIs(TwistTrayIceMakerMotorActionResult_Harvested);

   WhenTheMotorResolvedVoteIs(TwistTrayIceMakerMotorAction_Idle);
   TheMotorControlRequestShouldBe(CLEAR);
}

TEST(TwistTrayIceMakerMotorRequestManager, ShouldUpdateDoActionToTheMotorActionWhenMotorIsEnabled)
{
   GivenTheMotorResolvedVoteIs(TwistTrayIceMakerMotorAction_RunCycle);
   GivenTheModuleIsInitialized();

   WhenTheMotorDriveEnableIs(SET);
   DoActionShouldBe(TwistTrayIceMakerMotorAction_RunCycle, Signal_One);
}

TEST(TwistTrayIceMakerMotorRequestManager, ShouldIncrementDoActionSignalMultipleTimesWhenMotorDriveIsEnabled)
{
   GivenTheMotorResolvedVoteIs(TwistTrayIceMakerMotorAction_RunCycle);
   GivenTheModuleIsInitialized();

   WhenTheMotorDriveEnableIs(SET);
   DoActionShouldBe(TwistTrayIceMakerMotorAction_RunCycle, Signal_One);

   WhenTheMotorDriveEnableIs(CLEAR);
   WhenTheMotorDriveEnableIs(SET);

   DoActionShouldBe(TwistTrayIceMakerMotorAction_RunCycle, Signal_One + 1);

   WhenTheMotorDriveEnableIs(CLEAR);
   WhenTheMotorDriveEnableIs(SET);

   DoActionShouldBe(TwistTrayIceMakerMotorAction_RunCycle, Signal_One + 2);
}

TEST(TwistTrayIceMakerMotorRequestManager, ShouldClearMotorControlRequestWhenMotorActionResultIsMotorError)
{
   GivenTheModuleIsInitializedAndTheMotorIsRunning();
   TheMotorControlRequestShouldBe(SET);

   WhenTheMotorActionResultIs(TwistTrayIceMakerMotorActionResult_MotorError);
   TheMotorControlRequestShouldBe(CLEAR);
}

TEST(TwistTrayIceMakerMotorRequestManager, ShouldClearMotorControlRequestWhenMotorActionResultIsHarvested)
{
   GivenTheModuleIsInitializedAndTheMotorIsRunning();
   TheMotorControlRequestShouldBe(SET);

   WhenTheMotorActionResultIs(TwistTrayIceMakerMotorActionResult_Harvested);
   TheMotorControlRequestShouldBe(CLEAR);
}

TEST(TwistTrayIceMakerMotorRequestManager, ShouldClearMotorControlRequestWhenMotorActionResultIsBucketWasFull)
{
   GivenTheModuleIsInitializedAndTheMotorIsRunning();
   TheMotorControlRequestShouldBe(SET);

   WhenTheMotorActionResultIs(TwistTrayIceMakerMotorActionResult_BucketWasFull);
   TheMotorControlRequestShouldBe(CLEAR);
}

TEST(TwistTrayIceMakerMotorRequestManager, ShouldClearMotorControlRequestWhenMotorActionResultIsHomed)
{
   GivenTheModuleIsInitializedAndTheMotorIsRunning();
   TheMotorControlRequestShouldBe(SET);

   WhenTheMotorActionResultIs(TwistTrayIceMakerMotorActionResult_Homed);
   TheMotorControlRequestShouldBe(CLEAR);
}

TEST(TwistTrayIceMakerMotorRequestManager, ShouldNotClearMotorControlRequestWhenMotorActionResultIsHarvesting)
{
   GivenTheModuleIsInitializedAndTheMotorIsRunning();
   TheMotorControlRequestShouldBe(SET);

   WhenTheMotorActionResultIs(TwistTrayIceMakerMotorActionResult_Harvesting);
   TheMotorControlRequestShouldBe(SET);
}

TEST(TwistTrayIceMakerMotorRequestManager, ShouldNotClearMotorRequestWhenMotorActionResultIsHoming)
{
   GivenTheModuleIsInitializedAndTheMotorIsRunning();
   TheMotorControlRequestShouldBe(SET);

   WhenTheMotorActionResultIs(TwistTrayIceMakerMotorActionResult_Homing);
   TheMotorControlRequestShouldBe(SET);
}
