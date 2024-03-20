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
#include "TddPersonality.h"
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

enum
{
   ConvertibleStepsToOpenAsFreshFood = 650,
   ConvertibleStepsToCloseAsFreshFood = 700,
   ConvertibleStepsToOpenAsFreezer = 750,
   ConvertibleStepsToCloseAsFreezer = 800,
   ConvertibleStepsToHome = 1850
};

static const DamperRequestManagerConfiguration_t config = {
   .damperPositionRequestResolvedVoteErd = Erd_FreshFoodDamperPosition_ResolvedVote,
   .damperStepperMotorPositionRequestErd = Erd_DamperStepperMotorPositionRequest,
   .damperHomingRequestErd = Erd_DamperHomingRequest,
   .damperCurrentPositionErd = Erd_DamperCurrentPosition,
   .convertibleCompartmentStateErd = Erd_ConvertibleCompartmentState
};

TEST_GROUP(DamperRequestManager)
{
   DamperRequestManager_t instance;

   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;

   void GivenTheDataModelIsInitializedWithPersonality(PersonalityId_t personalityForTest)
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble, personalityForTest);
      dataModel = dataModelDouble.dataModel;
   }

   void TheModuleIsInitialized()
   {
      DamperRequestManager_Init(
         &instance,
         dataModel,
         &config,
         PersonalityParametricData_Get(dataModel)->damperData);
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
         Erd_DamperHomingRequest,
         &status);
   }

   void HomingErdShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(
         dataModel,
         Erd_DamperHomingRequest,
         &actual);
      CHECK_EQUAL(expected, actual);
   }

   void DamperMotorRequestedStepsShouldBe(uint16_t expected)
   {
      StepperPositionRequest_t actual;
      DataModel_Read(
         dataModel,
         Erd_DamperStepperMotorPositionRequest,
         &actual);

      CHECK_EQUAL(expected, actual.stepsToMove);
   }

   void DirectionShouldBe(TurningDirection_t expected)
   {
      StepperPositionRequest_t actual;
      DataModel_Read(
         dataModel,
         Erd_DamperStepperMotorPositionRequest,
         &actual);

      CHECK_EQUAL(expected, actual.direction);
   }

   void ResetSubStepShouldBe(bool expected)
   {
      StepperPositionRequest_t actual;
      DataModel_Read(
         dataModel,
         Erd_DamperStepperMotorPositionRequest,
         &actual);

      CHECK_EQUAL(expected, actual.resetSubstep);
   }

   void SetHomingFlagFalse()
   {
      DataModel_Write(
         dataModel,
         Erd_DamperHomingRequest,
         clear);
   }

   void StepsToMoveShouldBe(uint16_t expectedStepsToMove)
   {
      StepperPositionRequest_t position;
      DataModel_Read(
         dataModel,
         Erd_DamperStepperMotorPositionRequest,
         &position);

      CHECK_EQUAL(expectedStepsToMove, position.stepsToMove);
   }

   void StepsAreSetToZero()
   {
      StepperPositionRequest_t position;
      DataModel_Read(
         dataModel,
         Erd_DamperStepperMotorPositionRequest,
         &position);

      position.stepsToMove = 0;
      DataModel_Write(
         dataModel,
         Erd_DamperStepperMotorPositionRequest,
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
         Erd_DamperStepperMotorPositionRequest,
         &position);

      position.stepsToMove = 1;
      DataModel_Write(
         dataModel,
         Erd_DamperStepperMotorPositionRequest,
         &position);
   }

   void TheCurrentPositionShouldBe(DamperPosition_t expected)
   {
      DamperPosition_t actual;
      DataModel_Read(
         dataModel,
         Erd_DamperCurrentPosition,
         &actual);

      CHECK_EQUAL(expected, actual);
   }

   void WriteTheCurrentPositionIs(DamperPosition_t position)
   {
      DataModel_Write(
         dataModel,
         Erd_DamperCurrentPosition,
         &position);
   }

   void GivenConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_t state)
   {
      DataModel_Write(
         dataModel,
         Erd_ConvertibleCompartmentState,
         &state);
   }

   void WhenConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_t state)
   {
      GivenConvertibleCompartmentStateIs(state);
   }
};

TEST(DamperRequestManager, ShouldInitializeTheModule)
{
   GivenTheDataModelIsInitializedWithPersonality(TddPersonality_SwqaSideBySide);
   TheModuleIsInitialized();
}

TEST(DamperRequestManager, ShouldRequestHomingStepsAndDirectionToCounterClockwiseAndResetSubstepOnInitialization)
{
   GivenTheDataModelIsInitializedWithPersonality(TddPersonality_SwqaSideBySide);
   Given TheModuleIsInitialized();

   DamperMotorRequestedStepsShouldBe(StepsToHome);
   DirectionShouldBe(TurningDirection_Clockwise);
   ResetSubStepShouldBe(true);
}

TEST(DamperRequestManager, ShouldSetMotorStepsToOpenAndDirectionToCounterClockwiseAndResetSubstepToFalseWhenOpenPositionRequested)
{
   GivenTheDataModelIsInitializedWithPersonality(TddPersonality_SwqaSideBySide);
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
   GivenTheDataModelIsInitializedWithPersonality(TddPersonality_SwqaSideBySide);
   GivenTheModuleIsInitializedAndHomingIsComplete();
   When TheRequestedPositionIs(DamperPosition_Closed);

   DamperMotorRequestedStepsShouldBe(StepsToClose);
   DirectionShouldBe(TurningDirection_Clockwise);
   ResetSubStepShouldBe(false);
}

TEST(DamperRequestManager, ShouldHandleConsecutiveRequests)
{
   GivenTheDataModelIsInitializedWithPersonality(TddPersonality_SwqaSideBySide);
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
   GivenTheDataModelIsInitializedWithPersonality(TddPersonality_SwqaSideBySide);
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
   GivenTheDataModelIsInitializedWithPersonality(TddPersonality_SwqaSideBySide);
   GivenTheModuleIsInitializedAndHomingIsComplete();
   And HomingErdIsSetTo(true);

   DamperMotorRequestedStepsShouldBe(StepsToHome);
   DirectionShouldBe(TurningDirection_Clockwise);

   When TheDamperCompletesPositionChange();
   HomingErdShouldBe(false);
}

TEST(DamperRequestManager, ShouldSetPositionToClosedAfterHoming)
{
   GivenTheDataModelIsInitializedWithPersonality(TddPersonality_SwqaSideBySide);
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
   GivenTheDataModelIsInitializedWithPersonality(TddPersonality_SwqaSideBySide);
   GivenTheModuleIsInitializedAndHomingIsComplete();
   And TheRequestedPositionIs(DamperPosition_Open);
   And TheRequestedPositionIs(DamperPosition_Closed);
   And TheRequestedPositionIs(DamperPosition_Open);

   DamperMotorRequestedStepsShouldBe(StepsToOpen);
   DirectionShouldBe(TurningDirection_CounterClockwise);
}

TEST(DamperRequestManager, ShouldChangeDamperPositionAfterDamperCompletesMovement)
{
   GivenTheDataModelIsInitializedWithPersonality(TddPersonality_SwqaSideBySide);
   GivenTheModuleIsInitializedAndHomingIsComplete();

   When TheRequestedPositionIs(DamperPosition_Open);
   TheCurrentPositionShouldBe(DamperPosition_Closed);

   When TheDamperCompletesPositionChange();
   TheCurrentPositionShouldBe(DamperPosition_Open);
}

TEST(DamperRequestManager, ShouldUseCorrectStepsForConvertibleCompartmentAsFreshFood)
{
   GivenTheDataModelIsInitializedWithPersonality(TddPersonality_DevelopmentDualEvapFourDoor);
   GivenConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_FreshFood);
   GivenTheModuleIsInitializedAndHomingIsComplete();

   When TheRequestedPositionIs(DamperPosition_Open);
   StepsToMoveShouldBe(ConvertibleStepsToOpenAsFreshFood);
   When TheDamperCompletesPositionChange();
   TheCurrentPositionShouldBe(DamperPosition_Open);

   When TheRequestedPositionIs(DamperPosition_Closed);
   StepsToMoveShouldBe(ConvertibleStepsToCloseAsFreshFood);
   When TheDamperCompletesPositionChange();
   TheCurrentPositionShouldBe(DamperPosition_Closed);
}

TEST(DamperRequestManager, ShouldUseCorrectStepsForConvertibleCompartmentAsFreezer)
{
   GivenTheDataModelIsInitializedWithPersonality(TddPersonality_DevelopmentDualEvapFourDoor);
   GivenConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_Freezer);
   GivenTheModuleIsInitializedAndHomingIsComplete();

   When TheRequestedPositionIs(DamperPosition_Open);
   StepsToMoveShouldBe(ConvertibleStepsToOpenAsFreezer);
   When TheDamperCompletesPositionChange();
   TheCurrentPositionShouldBe(DamperPosition_Open);

   When TheRequestedPositionIs(DamperPosition_Closed);
   StepsToMoveShouldBe(ConvertibleStepsToCloseAsFreezer);
   When TheDamperCompletesPositionChange();
   TheCurrentPositionShouldBe(DamperPosition_Closed);
}

TEST(DamperRequestManager, ShouldHomeAndReopenAfterConvertibleCompartmentStateChangesWhileDamperIsOpen)
{
   GivenTheDataModelIsInitializedWithPersonality(TddPersonality_DevelopmentDualEvapFourDoor);
   GivenConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_Freezer);
   GivenTheModuleIsInitializedAndHomingIsComplete();

   When TheRequestedPositionIs(DamperPosition_Open);
   When TheDamperCompletesPositionChange();

   WhenConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_FreshFood);
   StepsToMoveShouldBe(ConvertibleStepsToHome);

   When TheDamperCompletesPositionChange();
   TheCurrentPositionShouldBe(DamperPosition_Closed);
   StepsToMoveShouldBe(ConvertibleStepsToOpenAsFreshFood);
   When TheDamperCompletesPositionChange();
   TheCurrentPositionShouldBe(DamperPosition_Open);

   WhenConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_Freezer);
   StepsToMoveShouldBe(ConvertibleStepsToHome);

   When TheDamperCompletesPositionChange();
   TheCurrentPositionShouldBe(DamperPosition_Closed);
   StepsToMoveShouldBe(ConvertibleStepsToOpenAsFreezer);
   When TheDamperCompletesPositionChange();
   TheCurrentPositionShouldBe(DamperPosition_Open);
}

TEST(DamperRequestManager, ShouldNotHomeWhenConvertibleCompartmentStateChangesWhileDamperIsClosed)
{
   GivenTheDataModelIsInitializedWithPersonality(TddPersonality_DevelopmentDualEvapFourDoor);
   GivenConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_Freezer);
   GivenTheModuleIsInitializedAndHomingIsComplete();

   WhenConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_FreshFood);
   StepsToMoveShouldBe(0);
   TheCurrentPositionShouldBe(DamperPosition_Closed);

   WhenConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_Freezer);
   StepsToMoveShouldBe(0);
   TheCurrentPositionShouldBe(DamperPosition_Closed);
}
