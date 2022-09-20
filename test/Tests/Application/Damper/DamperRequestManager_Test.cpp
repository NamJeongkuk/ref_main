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
      DataModel_Write(
         dataModelDouble.dataModel,
         Erd_FreshFoodDamperHomingRequest,
         clear);
   }

   void StepsAreSetToZero()
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

   void TheRequestedPositionIs(DamperPosition_t position)
   {
      DamperVotedPosition_t damperRequestedPosition;
      damperRequestedPosition.position = position;
      damperRequestedPosition.care = true;

      DataModel_Write(
         dataModelDouble.dataModel,
         Erd_FreshFoodDamperPosition_ResolvedVote,
         &damperRequestedPosition);
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
   StepsAreSetToZero();
   And TheRequestedPositionIs(DamperPosition_Open);

   DamperMotorRequestedStepsShouldBe(StepsToOpen);
   DirectionShouldBe(TurningDirection_Clockwise);
}

TEST(DamperRequestManager, ShouldSetMotorStepsToCloseAndDirectionToCounterClockwiseWhenClosedPositionRequested)
{
   Given TheModuleIsInitialized();
   StepsAreSetToZero();
   When TheRequestedPositionIs(DamperPosition_Closed);

   DamperMotorRequestedStepsShouldBe(StepsToClose);
   DirectionShouldBe(TurningDirection_CounterClockwise);
}

TEST(DamperRequestManager, ShouldHandleConsecutiveRequests)
{
   Given TheModuleIsInitialized();
   StepsAreSetToZero();
   SetHomingFlagFalse();
   And TheRequestedPositionIs(DamperPosition_Open);
   And StepsAreSetToZero();
   And TheRequestedPositionIs(DamperPosition_Closed);

   DamperMotorRequestedStepsShouldBe(StepsToClose);
   DirectionShouldBe(TurningDirection_CounterClockwise);
}

TEST(DamperRequestManager, ShouldHandleLatestRequestThatOccurredWhileMoving)
{
   Given TheModuleIsInitialized();
   StepsAreSetToZero();
   SetHomingFlagFalse();
   And TheRequestedPositionIs(DamperPosition_Open);
   And TheRequestedPositionIs(DamperPosition_Closed);

   DirectionShouldBe(TurningDirection_Clockwise);

   When StepsAreSetToZero();

   DamperMotorRequestedStepsShouldBe(StepsToClose);
   DirectionShouldBe(TurningDirection_CounterClockwise);
}

TEST(DamperRequestManager, ShouldSetMotorStepsToHomeAndDirectionToCounterClockwiseWhenHomingIsRequested)
{
   Given TheModuleIsInitialized();
   StepsAreSetToZero();
   And HomingIsRequested();

   DamperMotorRequestedStepsShouldBe(StepsToHome);
   DirectionShouldBe(TurningDirection_CounterClockwise);
}

TEST(DamperRequestManager, ShouldSetPositionToClosedAfterHoming)
{
   Given TheModuleIsInitialized();
   StepsAreSetToZero();
   And HomingIsRequested();

   TheCurrentPositionShouldBe(DamperPosition_Closed);
   DamperMotorRequestedStepsShouldBe(StepsToHome);
   DirectionShouldBe(TurningDirection_CounterClockwise);
}

TEST(DamperRequestManager, ShouldOpenCloseThenOpenAgain)
{
   Given TheModuleIsInitialized();
   StepsAreSetToZero();
   And TheRequestedPositionIs(DamperPosition_Open);
   And TheRequestedPositionIs(DamperPosition_Closed);
   And TheRequestedPositionIs(DamperPosition_Open);

   DamperMotorRequestedStepsShouldBe(StepsToOpen);
   DirectionShouldBe(TurningDirection_Clockwise);
}
