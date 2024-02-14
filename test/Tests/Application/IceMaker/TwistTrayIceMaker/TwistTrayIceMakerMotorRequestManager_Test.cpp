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

static const TwistTrayIceMakerMotorRequestManagerConfig_t motorRequestManagerConfig = {
   .resolvedVoteErd = Erd_IceMaker0_TwistMotor_ResolvedVote,
   .motorRequestErd = Erd_TwistIceMakerMotorControlRequest,
   .motorEnableErd = Erd_TwistIceMakerMotorDriveEnable,
   .motorActionResultErd = Erd_IceMaker0_MotorActionResult,
   .motorRequestedStateErd = Erd_IceMaker0_MotorRequestedState
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
         &motorRequestManagerConfig);
   }

   void WhenTheMotorResolvedVoteIs(IceMakerMotorState_t state)
   {
      IceMakerMotorVotedState_t vote = {
         .state = state,
         .care = Vote_Care
      };

      DataModel_Write(
         dataModel,
         Erd_IceMaker0_TwistMotor_ResolvedVote,
         &vote);
   }

   void GivenTheMotorResolvedVoteIs(IceMakerMotorState_t action)
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

   void MotorRequestedStateShouldBe(
      IceMakerMotorState_t expectedState,
      Signal_t expectedSignal)
   {
      IceMakerMotorRequestedState_t actual;
      DataModel_Read(
         dataModel,
         Erd_IceMaker0_MotorRequestedState,
         &actual);

      CHECK_EQUAL(expectedState, actual.state);
      CHECK_EQUAL(expectedSignal, actual.signal);
   }

   void GivenTheModuleIsInitializedAndTheMotorIsRunning()
   {
      GivenTheModuleIsInitialized();
      GivenTheMotorResolvedVoteIs(IceMakerMotorState_Run);

      WhenTheMotorDriveEnableIs(SET);
      MotorRequestedStateShouldBe(IceMakerMotorState_Run, Signal_One);
   }

   void WhenTheMotorActionResultIs(IceMakerMotorActionResult_t actionResult)
   {
      DataModel_Write(
         dataModel,
         Erd_IceMaker0_MotorActionResult,
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
   GivenTheMotorResolvedVoteIs(IceMakerMotorState_Off);
   GivenTheModuleIsInitialized();
   TheMotorControlRequestShouldBe(CLEAR);

   WhenTheMotorResolvedVoteIs(IceMakerMotorState_Homing);
   TheMotorControlRequestShouldBe(SET);
}

TEST(TwistTrayIceMakerMotorRequestManager, ShouldSetMotorControlRequestWhenMotorActionChangesToRunCycle)
{
   GivenTheMotorResolvedVoteIs(IceMakerMotorState_Off);
   GivenTheModuleIsInitialized();
   TheMotorControlRequestShouldBe(CLEAR);

   WhenTheMotorResolvedVoteIs(IceMakerMotorState_Run);
   TheMotorControlRequestShouldBe(SET);
}

TEST(TwistTrayIceMakerMotorRequestManager, ShouldPassThroughIdleCommandIfVoteChangesToIdleButNotRequestMotorControlEnable)
{
   GivenTheModuleIsInitialized();

   WhenTheMotorResolvedVoteIs(IceMakerMotorState_Run);
   TheMotorControlRequestShouldBe(SET);
   WhenTheMotorDriveEnableIs(SET);
   WhenTheMotorActionResultIs(IceMakerMotorActionResult_Harvested);
   TheMotorControlRequestShouldBe(CLEAR);

   WhenTheMotorResolvedVoteIs(IceMakerMotorState_Off);
   MotorRequestedStateShouldBe(IceMakerMotorState_Off, Signal_One + 1);
   TheMotorControlRequestShouldBe(CLEAR);
}

TEST(TwistTrayIceMakerMotorRequestManager, ShouldUpdateMotorRequestedStateToTheMotorResolvedVoteWhenMotorDriveIsEnabled)
{
   GivenTheMotorResolvedVoteIs(IceMakerMotorState_Run);
   GivenTheModuleIsInitialized();

   WhenTheMotorDriveEnableIs(SET);
   MotorRequestedStateShouldBe(IceMakerMotorState_Run, Signal_One);
}

TEST(TwistTrayIceMakerMotorRequestManager, ShouldIncrementMotorRequestedStateSignalMultipleTimesWhenMotorDriveIsEnabled)
{
   GivenTheMotorResolvedVoteIs(IceMakerMotorState_Run);
   GivenTheModuleIsInitialized();

   WhenTheMotorDriveEnableIs(SET);
   MotorRequestedStateShouldBe(IceMakerMotorState_Run, Signal_One);

   WhenTheMotorDriveEnableIs(CLEAR);
   WhenTheMotorDriveEnableIs(SET);

   MotorRequestedStateShouldBe(IceMakerMotorState_Run, Signal_One + 1);

   WhenTheMotorDriveEnableIs(CLEAR);
   WhenTheMotorDriveEnableIs(SET);

   MotorRequestedStateShouldBe(IceMakerMotorState_Run, Signal_One + 2);
}

TEST(TwistTrayIceMakerMotorRequestManager, ShouldClearMotorControlRequestWhenMotorActionResultIsMotorError)
{
   GivenTheModuleIsInitializedAndTheMotorIsRunning();
   TheMotorControlRequestShouldBe(SET);

   WhenTheMotorActionResultIs(IceMakerMotorActionResult_MotorError);
   TheMotorControlRequestShouldBe(CLEAR);
}

TEST(TwistTrayIceMakerMotorRequestManager, ShouldClearMotorControlRequestWhenMotorActionResultIsHarvested)
{
   GivenTheModuleIsInitializedAndTheMotorIsRunning();
   TheMotorControlRequestShouldBe(SET);

   WhenTheMotorActionResultIs(IceMakerMotorActionResult_Harvested);
   TheMotorControlRequestShouldBe(CLEAR);
}

TEST(TwistTrayIceMakerMotorRequestManager, ShouldClearMotorControlRequestWhenMotorActionResultIsBucketWasFull)
{
   GivenTheModuleIsInitializedAndTheMotorIsRunning();
   TheMotorControlRequestShouldBe(SET);

   WhenTheMotorActionResultIs(IceMakerMotorActionResult_BucketWasFull);
   TheMotorControlRequestShouldBe(CLEAR);
}

TEST(TwistTrayIceMakerMotorRequestManager, ShouldClearMotorControlRequestWhenMotorActionResultIsHomed)
{
   GivenTheModuleIsInitializedAndTheMotorIsRunning();
   TheMotorControlRequestShouldBe(SET);

   WhenTheMotorActionResultIs(IceMakerMotorActionResult_Homed);
   TheMotorControlRequestShouldBe(CLEAR);
}

TEST(TwistTrayIceMakerMotorRequestManager, ShouldNotClearMotorControlRequestWhenMotorActionResultIsHarvesting)
{
   GivenTheModuleIsInitializedAndTheMotorIsRunning();
   TheMotorControlRequestShouldBe(SET);

   WhenTheMotorActionResultIs(IceMakerMotorActionResult_Harvesting);
   TheMotorControlRequestShouldBe(SET);
}

TEST(TwistTrayIceMakerMotorRequestManager, ShouldNotClearMotorRequestWhenMotorActionResultIsHoming)
{
   GivenTheModuleIsInitializedAndTheMotorIsRunning();
   TheMotorControlRequestShouldBe(SET);

   WhenTheMotorActionResultIs(IceMakerMotorActionResult_Homing);
   TheMotorControlRequestShouldBe(SET);
}
