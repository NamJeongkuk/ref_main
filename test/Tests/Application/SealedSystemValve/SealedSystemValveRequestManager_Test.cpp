/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "SealedSystemValveRequestManager.h"
#include "SystemErds.h"
#include "Constants_Binary.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "uassert_test.h"
#include "DataModel_TestDouble.h"
#include "ReferDataModel_TestDouble.h"

#define When
#define And
#define Given
#define Then

enum
{
   DontCare = 0,
   StepsToHome = 1850
};

static const SealedSystemValveRequestManagerConfiguration_t config = {
   .positionRequestResolvedVoteErd = Erd_SealedSystemValvePosition_ResolvedVote,
   .stepperMotorPositionRequestErd = Erd_FreshFoodDamperStepperMotorPositionRequest,
   .homingRequestErd = Erd_SealedSystemValveHomingRequest,
   .currentPositionErd = Erd_SealedSystemValveCurrentPosition
};

static const SealedSystemValveData_t parameters = {
   .positionAStep = 40,
   .positionBStep = 30,
   .positionCStep = 50,
   .positionDStep = 10,
   .numberOfStepsToHome = StepsToHome,
   .excitationDelayInMilliseconds = 500,
   .delayBetweenStepEventsInMilliseconds = DontCare
};

TEST_GROUP(SealedSystemValveRequestManager)
{
   SealedSystemValveRequestManager_t instance;

   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
   }

   void GivenTheModuleIsInitialized()
   {
      SealedSystemValveRequestManager_Init(
         &instance,
         dataModel,
         &config,
         &parameters);
   }

   void GivenTheModuleIsInitializedAndHomingIsComplete()
   {
      GivenTheModuleIsInitialized();
      SetStepsToZero();
   }

   void GivenTheModuleIsInitializedAndMovedToPosition(SealedSystemValvePosition_t position)
   {
      GivenTheModuleIsInitializedAndHomingIsComplete();
      WhenTheRequestedPositionIs(position);
      SetStepsToZero();
   }

   void WhenHomingRequestErdIs(bool status)
   {
      DataModel_Write(
         dataModel,
         config.homingRequestErd,
         &status);
   }

   void HomingErdShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(
         dataModel,
         config.homingRequestErd,
         &actual);

      CHECK_EQUAL(expected, actual);
   }

   void MotorRequestedStepsShouldBe(uint16_t expected)
   {
      StepperPositionRequest_t actual;
      DataModel_Read(
         dataModel,
         config.stepperMotorPositionRequestErd,
         &actual);

      CHECK_EQUAL(expected, actual.stepsToMove);
   }

   void DirectionShouldBe(TurningDirection_t expected)
   {
      StepperPositionRequest_t actual;
      DataModel_Read(
         dataModel,
         config.stepperMotorPositionRequestErd,
         &actual);

      CHECK_EQUAL(expected, actual.direction);
   }

   void ResetSubStepShouldBe(bool expected)
   {
      StepperPositionRequest_t actual;
      DataModel_Read(
         dataModel,
         Erd_FreshFoodDamperStepperMotorPositionRequest,
         &actual);

      CHECK_EQUAL(expected, actual.resetSubstep);
   }

   void SetStepsToZero()
   {
      StepperPositionRequest_t position;
      DataModel_Read(
         dataModel,
         config.stepperMotorPositionRequestErd,
         &position);

      position.stepsToMove = 0;
      DataModel_Write(
         dataModel,
         config.stepperMotorPositionRequestErd,
         &position);
   }

   void WhenValveCompletesPositionChange()
   {
      SetStepsToZero();
   }

   void WhenTheRequestedPositionIs(SealedSystemValvePosition_t position)
   {
      SealedSystemValveVotedPosition_t requestedPosition;
      requestedPosition.position = position;
      requestedPosition.care = true;

      DataModel_Write(
         dataModel,
         config.positionRequestResolvedVoteErd,
         &requestedPosition);
   }

   void OneMoreStepToComplete()
   {
      StepperPositionRequest_t position;
      DataModel_Read(
         dataModel,
         config.stepperMotorPositionRequestErd,
         &position);

      position.stepsToMove = 1;
      DataModel_Write(
         dataModel,
         config.stepperMotorPositionRequestErd,
         &position);
   }

   void TheCurrentPositionShouldBe(DamperPosition_t expected)
   {
      DamperPosition_t actual;
      DataModel_Read(
         dataModel,
         config.currentPositionErd,
         &actual);

      CHECK_EQUAL(expected, actual);
   }

   void GivenTheCurrentPositionIs(DamperPosition_t position)
   {
      DataModel_Write(
         dataModel,
         config.currentPositionErd,
         &position);
   }
};

TEST(SealedSystemValveRequestManager, ShouldInitializeTheModule)
{
   GivenTheModuleIsInitialized();
}

TEST(SealedSystemValveRequestManager, ShouldSetInitialPositionToHome)
{
   GivenTheCurrentPositionIs(SealedSystemValvePosition_A);
   GivenTheModuleIsInitialized();

   TheCurrentPositionShouldBe(SealedSystemValvePosition_Home);
}

TEST(SealedSystemValveRequestManager, ShouldRequestHomingStepsAndDirectionToCounterClockwiseAndSubstepResetOnInitialization)
{
   GivenTheModuleIsInitialized();

   MotorRequestedStepsShouldBe(StepsToHome);
   DirectionShouldBe(TurningDirection_CounterClockwise);
   ResetSubStepShouldBe(true);
}

TEST(SealedSystemValveRequestManager, ShouldSetMotorStepsToPositionAAndDirectionToClockwiseWhenRequestedPositionIsAFromHome)
{
   GivenTheModuleIsInitializedAndHomingIsComplete();

   WhenTheRequestedPositionIs(SealedSystemValvePosition_A);
   MotorRequestedStepsShouldBe(parameters.positionAStep);
   DirectionShouldBe(TurningDirection_Clockwise);
   ResetSubStepShouldBe(false);
}

TEST(SealedSystemValveRequestManager, ShouldSetMotorStepsToPositionBAndDirectionToClockwiseWhenRequestedPositionIsBFromHome)
{
   GivenTheModuleIsInitializedAndHomingIsComplete();

   WhenTheRequestedPositionIs(SealedSystemValvePosition_B);
   MotorRequestedStepsShouldBe(parameters.positionBStep);
   DirectionShouldBe(TurningDirection_Clockwise);
   ResetSubStepShouldBe(false);
}

TEST(SealedSystemValveRequestManager, ShouldSetMotorStepsToPositionCAndDirectionToClockwiseWhenRequestedPositionIsCFromHome)
{
   GivenTheModuleIsInitializedAndHomingIsComplete();

   WhenTheRequestedPositionIs(SealedSystemValvePosition_C);
   MotorRequestedStepsShouldBe(parameters.positionCStep);
   DirectionShouldBe(TurningDirection_Clockwise);
   ResetSubStepShouldBe(false);
}

TEST(SealedSystemValveRequestManager, ShouldSetMotorStepsToPositionDAndDirectionToClockwiseWhenRequestedPositionIsDFromHome)
{
   GivenTheModuleIsInitializedAndHomingIsComplete();

   WhenTheRequestedPositionIs(SealedSystemValvePosition_D);
   MotorRequestedStepsShouldBe(parameters.positionDStep);
   DirectionShouldBe(TurningDirection_Clockwise);
   ResetSubStepShouldBe(false);
}

TEST(SealedSystemValveRequestManager, ShouldMoveBetweenPositionsClockWise)
{
   GivenTheModuleIsInitializedAndMovedToPosition(SealedSystemValvePosition_D);

   WhenTheRequestedPositionIs(SealedSystemValvePosition_A);
   MotorRequestedStepsShouldBe(parameters.positionAStep - parameters.positionDStep);
   DirectionShouldBe(TurningDirection_Clockwise);
}

TEST(SealedSystemValveRequestManager, ShouldMoveBetweenPositionsCounterClockwise)
{
   GivenTheModuleIsInitializedAndMovedToPosition(SealedSystemValvePosition_B);

   WhenTheRequestedPositionIs(SealedSystemValvePosition_C);
   MotorRequestedStepsShouldBe(parameters.positionCStep - parameters.positionBStep);
   DirectionShouldBe(TurningDirection_Clockwise);
}

TEST(SealedSystemValveRequestManager, ShouldQueueHomePositionRequestIfAlreadyMoving)
{
   GivenTheModuleIsInitializedAndHomingIsComplete();

   WhenTheRequestedPositionIs(SealedSystemValvePosition_C);
   WhenTheRequestedPositionIs(SealedSystemValvePosition_Home);

   DirectionShouldBe(TurningDirection_Clockwise);
   MotorRequestedStepsShouldBe(parameters.positionCStep);

   WhenValveCompletesPositionChange();
   MotorRequestedStepsShouldBe(StepsToHome);
   DirectionShouldBe(TurningDirection_CounterClockwise);
}

TEST(SealedSystemValveRequestManager, ShouldQueueHomingRequestErdIfAlreadyMoving)
{
   GivenTheModuleIsInitializedAndHomingIsComplete();

   WhenTheRequestedPositionIs(SealedSystemValvePosition_C);
   WhenHomingRequestErdIs(SET);

   DirectionShouldBe(TurningDirection_Clockwise);
   MotorRequestedStepsShouldBe(parameters.positionCStep);

   WhenValveCompletesPositionChange();
   MotorRequestedStepsShouldBe(StepsToHome);
   DirectionShouldBe(TurningDirection_CounterClockwise);

   WhenValveCompletesPositionChange();
   HomingErdShouldBe(CLEAR);
}

TEST(SealedSystemValveRequestManager, ShouldQueuePositionRequestIfAlreadyMoving)
{
   GivenTheModuleIsInitializedAndHomingIsComplete();

   WhenTheRequestedPositionIs(SealedSystemValvePosition_C);
   WhenTheRequestedPositionIs(SealedSystemValvePosition_D);

   DirectionShouldBe(TurningDirection_Clockwise);
   MotorRequestedStepsShouldBe(parameters.positionCStep);

   WhenValveCompletesPositionChange();
   MotorRequestedStepsShouldBe(parameters.positionCStep - parameters.positionDStep);
   DirectionShouldBe(TurningDirection_CounterClockwise);
}

TEST(SealedSystemValveRequestManager, ShouldSetMotorStepsToHomeAndDirectionToCounterClockwiseWhenHomingIsRequested)
{
   GivenTheModuleIsInitializedAndHomingIsComplete();

   WhenHomingRequestErdIs(SET);
   MotorRequestedStepsShouldBe(StepsToHome);
   DirectionShouldBe(TurningDirection_CounterClockwise);

   HomingErdShouldBe(SET);
   WhenValveCompletesPositionChange();
   HomingErdShouldBe(CLEAR);
}

TEST(SealedSystemValveRequestManager, ShouldSetPositionToHomeAfterHoming)
{
   GivenTheModuleIsInitializedAndMovedToPosition(SealedSystemValvePosition_C);

   WhenHomingRequestErdIs(SET);
   MotorRequestedStepsShouldBe(StepsToHome);
   DirectionShouldBe(TurningDirection_CounterClockwise);

   WhenValveCompletesPositionChange();
   TheCurrentPositionShouldBe(SealedSystemValvePosition_Home);
   HomingErdShouldBe(CLEAR);
}

TEST(SealedSystemValveRequestManager, ShouldMoveBetweenMultiplePositions)
{
   GivenTheModuleIsInitializedAndHomingIsComplete();

   WhenTheRequestedPositionIs(SealedSystemValvePosition_C);
   DirectionShouldBe(TurningDirection_Clockwise);
   MotorRequestedStepsShouldBe(parameters.positionCStep);

   WhenValveCompletesPositionChange();
   WhenTheRequestedPositionIs(SealedSystemValvePosition_D);
   DirectionShouldBe(TurningDirection_CounterClockwise);
   MotorRequestedStepsShouldBe(parameters.positionCStep - parameters.positionDStep);

   WhenValveCompletesPositionChange();
   WhenTheRequestedPositionIs(SealedSystemValvePosition_A);
   DirectionShouldBe(TurningDirection_Clockwise);
   MotorRequestedStepsShouldBe(parameters.positionAStep - parameters.positionDStep);
}

TEST(SealedSystemValveRequestManager, ShouldChangeCurrentPositionErdAfterMovementIsCompleted)
{
   GivenTheModuleIsInitializedAndHomingIsComplete();

   WhenTheRequestedPositionIs(SealedSystemValvePosition_C);
   TheCurrentPositionShouldBe(SealedSystemValvePosition_Home);

   WhenValveCompletesPositionChange();
   TheCurrentPositionShouldBe(SealedSystemValvePosition_C);
}
