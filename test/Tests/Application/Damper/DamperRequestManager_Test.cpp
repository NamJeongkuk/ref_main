/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "DamperRequestManager.h"
#include "SystemErds.h"
#include "Constants_Binary.h"
#include "PersonalityParametricData.h"
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
   StepsToOpen = 650,
   StepsToClose = 700,
   StepsToHome = 1850
};

static const DamperRequestManagerConfiguration_t config = {
   .damperPositionRequestResolvedVoteErd = Erd_FreshFoodDamperPosition_ResolvedVote,
   .damperStepperMotorPositionRequestErd = Erd_FreshFoodDamperStepperMotorPositionRequest,
   .damperHomingRequestErd = Erd_FreshFoodDamperHomingRequest,
   .damperCurrentPositionErd = Erd_FreshFoodDamperCurrentPosition
};

TEST_GROUP(DamperRequestManager)
{
   DamperRequestManager_t instance;

   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
   }

   void TheModuleIsInitialized()
   {
      DamperRequestManager_Init(
         &instance,
         dataModel,
         &config,
         PersonalityParametricData_Get(dataModel)->freshFoodDamperData);
   }

   void GivenTheModuleIsInitializedAndHomingIsComplete()
   {
      TheModuleIsInitialized();
      StepsAreSetToZero();
   }

   void HomingErdIsSetTo(bool status)
   {
      DataModel_Write(
         dataModel,
         Erd_FreshFoodDamperHomingRequest,
         &status);
   }

   void HomingErdShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(
         dataModel,
         Erd_FreshFoodDamperHomingRequest,
         &actual);
      CHECK_EQUAL(expected, actual);
   }

   void DamperMotorRequestedStepsShouldBe(uint16_t expected)
   {
      StepperPositionRequest_t actual;
      DataModel_Read(
         dataModel,
         Erd_FreshFoodDamperStepperMotorPositionRequest,
         &actual);

      CHECK_EQUAL(expected, actual.stepsToMove);
   }

   void DirectionShouldBe(TurningDirection_t expected)
   {
      StepperPositionRequest_t actual;
      DataModel_Read(
         dataModel,
         Erd_FreshFoodDamperStepperMotorPositionRequest,
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

   void SetHomingFlagFalse()
   {
      DataModel_Write(
         dataModel,
         Erd_FreshFoodDamperHomingRequest,
         clear);
   }

   void StepsAreSetToZero()
   {
      StepperPositionRequest_t position;
      DataModel_Read(
         dataModel,
         Erd_FreshFoodDamperStepperMotorPositionRequest,
         &position);

      position.stepsToMove = 0;
      DataModel_Write(
         dataModel,
         Erd_FreshFoodDamperStepperMotorPositionRequest,
         &position);
   }

   void TheDamperCompletesPositionChange()
   {
      StepsAreSetToZero();
   }

   void TheRequestedPositionIs(DamperPosition_t position)
   {
      DamperVotedPosition_t damperRequestedPosition;
      damperRequestedPosition.position = position;
      damperRequestedPosition.care = true;

      DataModel_Write(
         dataModel,
         Erd_FreshFoodDamperPosition_ResolvedVote,
         &damperRequestedPosition);
   }

   void OneMoreStepToComplete()
   {
      StepperPositionRequest_t position;
      DataModel_Read(
         dataModel,
         Erd_FreshFoodDamperStepperMotorPositionRequest,
         &position);

      position.stepsToMove = 1;
      DataModel_Write(
         dataModel,
         Erd_FreshFoodDamperStepperMotorPositionRequest,
         &position);
   }

   void TheCurrentPositionShouldBe(DamperPosition_t expected)
   {
      DamperPosition_t actual;
      DataModel_Read(
         dataModel,
         Erd_FreshFoodDamperCurrentPosition,
         &actual);

      CHECK_EQUAL(expected, actual);
   }

   void WriteTheCurrentPositionIs(DamperPosition_t position)
   {
      DataModel_Write(
         dataModel,
         Erd_FreshFoodDamperCurrentPosition,
         &position);
   }
};

TEST(DamperRequestManager, ShouldInitializeTheModule)
{
   TheModuleIsInitialized();
}

TEST(DamperRequestManager, ShouldRequestHomingStepsAndDirectionToCounterClockwiseAndResetSubstepOnInitialization)
{
   Given TheModuleIsInitialized();

   DamperMotorRequestedStepsShouldBe(StepsToHome);
   DirectionShouldBe(TurningDirection_Clockwise);
   ResetSubStepShouldBe(true);
}

TEST(DamperRequestManager, ShouldSetMotorStepsToOpenAndDirectionToCounterClockwiseAndResetSubstepToFalseWhenOpenPositionRequested)
{
   Given TheRequestedPositionIs(DamperPosition_Closed);
   And TheModuleIsInitialized();
   And TheDamperCompletesPositionChange();
   And TheRequestedPositionIs(DamperPosition_Open);

   DamperMotorRequestedStepsShouldBe(StepsToOpen);
   DirectionShouldBe(TurningDirection_CounterClockwise);
   ResetSubStepShouldBe(false);
}

TEST(DamperRequestManager, ShouldSetMotorStepsToCloseAndDirectionToClockwiseWhenClosedPositionRequested)
{
   GivenTheModuleIsInitializedAndHomingIsComplete();
   When TheRequestedPositionIs(DamperPosition_Closed);

   DamperMotorRequestedStepsShouldBe(StepsToClose);
   DirectionShouldBe(TurningDirection_Clockwise);
   ResetSubStepShouldBe(false);
}

TEST(DamperRequestManager, ShouldHandleConsecutiveRequests)
{
   GivenTheModuleIsInitializedAndHomingIsComplete();
   SetHomingFlagFalse();
   And TheRequestedPositionIs(DamperPosition_Open);
   And TheDamperCompletesPositionChange();
   And TheRequestedPositionIs(DamperPosition_Closed);

   DamperMotorRequestedStepsShouldBe(StepsToClose);
   DirectionShouldBe(TurningDirection_Clockwise);
}

TEST(DamperRequestManager, ShouldHandleLatestRequestThatOccurredWhileMoving)
{
   GivenTheModuleIsInitializedAndHomingIsComplete();
   SetHomingFlagFalse();
   And TheRequestedPositionIs(DamperPosition_Open);
   And TheRequestedPositionIs(DamperPosition_Closed);

   DirectionShouldBe(TurningDirection_CounterClockwise);

   When StepsAreSetToZero();

   DamperMotorRequestedStepsShouldBe(StepsToClose);
   DirectionShouldBe(TurningDirection_Clockwise);
}

TEST(DamperRequestManager, ShouldSetMotorStepsToHomeAndDirectionToCounterClockwiseWhenHomingIsRequested)
{
   GivenTheModuleIsInitializedAndHomingIsComplete();
   And HomingErdIsSetTo(true);

   DamperMotorRequestedStepsShouldBe(StepsToHome);
   DirectionShouldBe(TurningDirection_Clockwise);

   When TheDamperCompletesPositionChange();
   HomingErdShouldBe(false);
}

TEST(DamperRequestManager, ShouldSetPositionToClosedAfterHoming)
{
   GivenTheModuleIsInitializedAndHomingIsComplete();
   And HomingErdIsSetTo(true);

   DamperMotorRequestedStepsShouldBe(StepsToHome);
   DirectionShouldBe(TurningDirection_Clockwise);

   When TheDamperCompletesPositionChange();
   TheCurrentPositionShouldBe(DamperPosition_Closed);
   HomingErdShouldBe(false);
}

TEST(DamperRequestManager, ShouldOpenCloseThenOpenAgain)
{
   GivenTheModuleIsInitializedAndHomingIsComplete();
   And TheRequestedPositionIs(DamperPosition_Open);
   And TheRequestedPositionIs(DamperPosition_Closed);
   And TheRequestedPositionIs(DamperPosition_Open);

   DamperMotorRequestedStepsShouldBe(StepsToOpen);
   DirectionShouldBe(TurningDirection_CounterClockwise);
}

TEST(DamperRequestManager, ShouldChangeDamperStateAfterDamperCompletesMovement)
{
   GivenTheModuleIsInitializedAndHomingIsComplete();

   When TheRequestedPositionIs(DamperPosition_Open);
   TheCurrentPositionShouldBe(DamperPosition_Closed);

   When TheDamperCompletesPositionChange();
   TheCurrentPositionShouldBe(DamperPosition_Open);
}
