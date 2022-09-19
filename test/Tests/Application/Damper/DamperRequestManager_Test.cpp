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

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
   }

   void TheModuleIsInitialized()
   {
      DamperRequestManager_Init(&instance, dataModelDouble.dataModel, &config);
   }

   void HomingIsRequested()
   {
      DataModel_Write(
         dataModelDouble.dataModel,
         Erd_FreshFoodDamperHomingRequest,
         set);
   }

   void DamperMotorRequestedStepsShouldBe(uint16_t expected)
   {
      StepperPositionRequest_t actual;

      DataModel_Read(
         dataModelDouble.dataModel,
         Erd_FreshFoodDamperStepperMotorPositionRequest,
         &actual);
      CHECK_EQUAL(expected, actual.stepsToMove);
   }

   void DirectionShouldBe(TurningDirection_t expected)
   {
      StepperPositionRequest_t actual;

      DataModel_Read(
         dataModelDouble.dataModel,
         Erd_FreshFoodDamperStepperMotorPositionRequest,
         &actual);

      CHECK_EQUAL(expected, actual.direction);
   }

   void SetHomingFlagFalse()
   {
      bool HomingFlagState = false;

      DataModel_Write(
         dataModelDouble.dataModel,
         Erd_FreshFoodDamperHomingRequest,
         &HomingFlagState);
   }

   void SetStepsToZero()
   {
      StepperPositionRequest_t StepsCompleted;
      StepsCompleted.stepsToMove = 0;

      DataModel_Write(
         dataModelDouble.dataModel,
         Erd_FreshFoodDamperStepperMotorPositionRequest,
         &StepsCompleted);
   }

   void TheRequestedPositionIs(DamperPosition_t position)
   {
      DamperVotedPosition_t DamperRequestedPosition;

      DamperRequestedPosition.position = position;
      DataModel_Write(
         dataModelDouble.dataModel,
         Erd_FreshFoodDamperPosition_ResolvedVote,
         &DamperRequestedPosition);
   }

   void AllStepsAreComplete()
   {
      StepperPositionRequest_t position;

      DataModel_Read(
         dataModelDouble.dataModel,
         Erd_FreshFoodDamperStepperMotorPositionRequest,
         &position);
      position.stepsToMove = 0;
      DataModel_Write(
         dataModelDouble.dataModel,
         Erd_FreshFoodDamperStepperMotorPositionRequest,
         &position);
   }

   void OneMoreStepToComplete()
   {
      StepperPositionRequest_t position;

      DataModel_Read(
         dataModelDouble.dataModel,
         Erd_FreshFoodDamperStepperMotorPositionRequest,
         &position);

      position.stepsToMove = 1;

      DataModel_Write(
         dataModelDouble.dataModel,
         Erd_FreshFoodDamperStepperMotorPositionRequest,
         &position);
   }

   void TheCurrentPositionShouldBe(DamperPosition_t expected)
   {
      DamperPosition_t actual;

      DataModel_Read(
         dataModelDouble.dataModel,
         Erd_FreshFoodDamperCurrentPosition,
         &actual);

      CHECK_EQUAL(expected, actual);
   }

   void WriteTheCurrentPositionIs(DamperPosition_t position)
   {
      DataModel_Write(
         dataModelDouble.dataModel,
         Erd_FreshFoodDamperCurrentPosition,
         &position);
   }
};

TEST(DamperRequestManager, ShouldInitializeTheModule)
{
   TheModuleIsInitialized();
}

TEST(DamperRequestManager, ShouldSetMotorStepsToOpenAndDirectionToClockwiseWhenOpenPositionRequested)
{
   Given TheRequestedPositionIs(DamperPosition_Closed);
   And TheModuleIsInitialized();
   SetStepsToZero();
   And TheRequestedPositionIs(DamperPosition_Open);

   DamperMotorRequestedStepsShouldBe(StepsToOpen);
   DirectionShouldBe(Clockwise);
}

TEST(DamperRequestManager, ShouldSetMotorStepsToCloseAndDirectionToCounterClockwiseWhenClosedPositionRequested)
{
   And TheModuleIsInitialized();
   SetStepsToZero();
   When TheRequestedPositionIs(DamperPosition_Closed);

   DamperMotorRequestedStepsShouldBe(StepsToClose);
   DirectionShouldBe(CounterClockwise);
}

TEST(DamperRequestManager, ShouldHandleConsecutiveRequests)
{
   Given TheModuleIsInitialized();
   SetStepsToZero();
   SetHomingFlagFalse();
   And TheRequestedPositionIs(DamperPosition_Open);
   And AllStepsAreComplete();
   And TheRequestedPositionIs(DamperPosition_Closed);

   DamperMotorRequestedStepsShouldBe(StepsToClose);
   DirectionShouldBe(CounterClockwise);
}

TEST(DamperRequestManager, ShouldHandleLatestRequestThatOccuredWhileMoving)
{
   Given TheModuleIsInitialized();
   SetStepsToZero();
   SetHomingFlagFalse();
   And TheRequestedPositionIs(DamperPosition_Open);
   And TheRequestedPositionIs(DamperPosition_Closed);

   DirectionShouldBe(Clockwise);

   When AllStepsAreComplete();

   DamperMotorRequestedStepsShouldBe(StepsToClose);
   DirectionShouldBe(CounterClockwise);
}

TEST(DamperRequestManager, ShouldSetMotorStepsToHomeAndDirectionToCounterClockwiseWhenHomingIsRequested)
{
   When TheModuleIsInitialized();
   SetStepsToZero();
   And HomingIsRequested();

   DamperMotorRequestedStepsShouldBe(StepsToHome);
   DirectionShouldBe(CounterClockwise);
}

TEST(DamperRequestManager, ShouldSetPositionToClosedAfterHoming)
{
   Then TheModuleIsInitialized();
   SetStepsToZero();
   And HomingIsRequested();

   TheCurrentPositionShouldBe(DamperPosition_Closed);
   DamperMotorRequestedStepsShouldBe(StepsToHome);
   DirectionShouldBe(CounterClockwise);
}

TEST(DamperRequestManager, ShouldOpenCloseThenOpenAgain)
{
   Given TheModuleIsInitialized();
   SetStepsToZero();
   And TheRequestedPositionIs(DamperPosition_Open);
   And TheRequestedPositionIs(DamperPosition_Closed);
   And TheRequestedPositionIs(DamperPosition_Open);

   DamperMotorRequestedStepsShouldBe(StepsToOpen);
   DirectionShouldBe(Clockwise);
}
