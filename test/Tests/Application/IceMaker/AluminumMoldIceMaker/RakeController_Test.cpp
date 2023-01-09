/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "RakeController.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"
#include "SystemErds.h"
}

#include "CppUTest/TestHarness.h"
#include "ReferDataModel_TestDouble.h"

#define Given
#define When
#define And

static const RakeControllerConfig_t config = {
   .rakeControlRequestErd = Erd_AluminumMoldIceMakerRakeControlRequest,
   .rakeMotorVoteErd = Erd_AluminumMoldIceMakerRakeMotor_IceMakerVote,
   .rakeCompletedRevolutionErd = Erd_AluminumMoldIceMakerRakeCompletedRevolution,
   .timerModuleErd = Erd_TimerModule,
   .rakePositionErd = Erd_AluminumMoldIceMakerRakePosition,
   .feelerArmPositionErd = Erd_AluminumMoldIceMakerFeelerArmPosition,
   .rakePositionHasNotBeenHomeErd = Erd_AluminumMoldIceMakerRakeHasNotBeenHome,
   .feelerArmPositionHasBeenBucketFullErd = Erd_AluminumMoldIceMakerFeelerArmHasBeenBucketFull
};

TEST_GROUP(RakeController)
{
   RakeController_t instance;

   ReferDataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleDouble;
   AluminumMoldIceMakerHarvestData_t harvestData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelTestDouble);
      dataModel = dataModelTestDouble.dataModel;
      timerModuleDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelTestDouble);
      harvestData = PersonalityParametricData_Get(dataModel)->aluminumMoldIceMakerData->harvestData;
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleDouble, ticks, 1000);
   }

   void TheModuleIsInitialized()
   {
      RakeController_Init(&instance, dataModel, &config, &harvestData);
   }

   void TheModuleIsInitializedAndRakeControlRequestIsSet()
   {
      Given TheModuleIsInitialized();
      When RakeControlRequestIs(SET);
   }

   void TheModuleIsInitializedAndRakeRevolutionCompletedWithRakePositionHome()
   {
      Given TheModuleIsInitializedAndRakeControlRequestIsSet();
      Given RakePositionIs(RakePosition_Home);
      Given FeelerArmPositionIs(FeelerArmPosition_BucketEmpty);

      When RakePositionIs(RakePosition_NotHome);
      After(harvestData.rakeNotHomeTestTimeInSeconds * MSEC_PER_SEC);

      When FeelerArmPositionIs(FeelerArmPosition_BucketFull);
      After(harvestData.feelerArmTestTimeInSeconds * MSEC_PER_SEC);

      When RakePositionIs(RakePosition_Home);
      RakeMotorVoteShouldBe(MotorState_Off, Vote_Care);
      And RakeCompletedRevolutionShouldBe(SET);
   }

   void TheModuleIsInitializedAndRakeRevolutionCompletedWithFeelerArmHasBeenBucketFull()
   {
      Given TheModuleIsInitializedAndRakeControlRequestIsSet();
      Given RakePositionIs(RakePosition_Home);
      Given FeelerArmPositionIs(FeelerArmPosition_BucketEmpty);

      When RakePositionIs(RakePosition_NotHome);
      After(harvestData.rakeNotHomeTestTimeInSeconds * MSEC_PER_SEC);

      When RakePositionIs(RakePosition_Home);

      When FeelerArmPositionIs(FeelerArmPosition_BucketFull);
      After(harvestData.feelerArmTestTimeInSeconds * MSEC_PER_SEC);
      RakeMotorVoteShouldBe(MotorState_Off, Vote_Care);
      And RakeCompletedRevolutionShouldBe(SET);
   }

   void RakeControlRequestIs(bool state)
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerRakeControlRequest, &state);
   }

   void RakeMotorVoteShouldBe(MotorState_t expectedState, Vote_t expectedCare)
   {
      AluminumMoldIceMakerMotorVotedState_t actualVote;
      DataModel_Read(dataModel, Erd_AluminumMoldIceMakerRakeMotor_IceMakerVote, &actualVote);

      CHECK_EQUAL(expectedState, actualVote.state);
      CHECK_EQUAL(expectedCare, actualVote.care);
   }

   void RakeCompletedRevolutionIs(bool state)
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerRakeCompletedRevolution, &state);
   }

   void RakeCompletedRevolutionShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_AluminumMoldIceMakerRakeCompletedRevolution, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void RakePositionIs(RakePosition_t state)
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerRakePosition, &state);
   }

   void FeelerArmPositionIs(FeelerArmPosition_t state)
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerFeelerArmPosition, &state);
   }

   void RakePositionHasNotBeenHomeShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_AluminumMoldIceMakerRakeHasNotBeenHome, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void FeelerArmPositionHasBeenBucketFullShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_AluminumMoldIceMakerFeelerArmHasBeenBucketFull, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void FeelerArmPositionHasBeenBucketFullIs(bool state)
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerFeelerArmHasBeenBucketFull, &state);
   }

   void RakePositionHasNotBeenHomeIs(bool state)
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerRakeHasNotBeenHome, &state);
   }
};

TEST(RakeController, ShouldInitialize)
{
   TheModuleIsInitialized();
}

TEST(RakeController, ShouldVoteToTurnOnRakeMotorWhenRakeControlRequestIsSet)
{
   Given TheModuleIsInitialized();
   RakeMotorVoteShouldBe(MotorState_Off, Vote_DontCare);

   When RakeControlRequestIs(SET);
   RakeMotorVoteShouldBe(MotorState_On, Vote_Care);
}

TEST(RakeController, ShouldVoteToTurnOffRakeMotorWhenIceMakerRakeControlRequestIsClear)
{
   Given TheModuleIsInitialized();
   Given RakeControlRequestIs(SET);
   RakeMotorVoteShouldBe(MotorState_On, Vote_Care);

   When RakeControlRequestIs(CLEAR);
   RakeMotorVoteShouldBe(MotorState_Off, Vote_Care);
}

TEST(RakeController, ShouldClearRakeCompletedRevolutionWhenIceMakerRakeControlRequestIsClear)
{
   Given TheModuleIsInitialized();
   Given RakeControlRequestIs(SET);
   Given RakeCompletedRevolutionIs(SET);

   When RakeControlRequestIs(CLEAR);
   RakeCompletedRevolutionShouldBe(CLEAR);
}

TEST(RakeController, ShouldClearRakePositionHasNotBeenHomeWhenRakeControlRequestIsClear)
{
   Given TheModuleIsInitialized();
   Given RakePositionHasNotBeenHomeIs(SET);
   Given RakeControlRequestIs(SET);

   When RakeControlRequestIs(CLEAR);
   RakePositionHasNotBeenHomeShouldBe(CLEAR);
}

TEST(RakeController, ShouldClearFeelerArmPositionHasBeenBucketFullWhenRakeControlRequestIsClear)
{
   Given TheModuleIsInitialized();
   Given FeelerArmPositionHasBeenBucketFullIs(SET);
   Given RakeControlRequestIs(SET);

   When RakeControlRequestIs(CLEAR);
   FeelerArmPositionHasBeenBucketFullShouldBe(CLEAR);
}

TEST(RakeController, ShouldSetRakePositionHasNotBeenHomeWhenRakePositionHasNotBeenHomeForRakeNotHomeTestTimeWhileRakeControlRequestIsSet)
{
   Given TheModuleIsInitializedAndRakeControlRequestIsSet();
   Given RakePositionIs(RakePosition_Home);

   When RakePositionIs(RakePosition_NotHome);
   After(harvestData.rakeNotHomeTestTimeInSeconds * MSEC_PER_SEC - 1);
   RakePositionHasNotBeenHomeShouldBe(CLEAR);

   After(1);
   RakePositionHasNotBeenHomeShouldBe(SET);
}

TEST(RakeController, ShouldNotSetRakeHasNotBeenHomeAfterRakeNotHomeTestTimeWhenRakePositionChangedToHomeBeforeTheTimeout)
{
   Given TheModuleIsInitializedAndRakeControlRequestIsSet();
   Given RakePositionIs(RakePosition_Home);

   When RakePositionIs(RakePosition_NotHome);
   After(harvestData.rakeNotHomeTestTimeInSeconds * MSEC_PER_SEC - 1);
   RakePositionHasNotBeenHomeShouldBe(CLEAR);

   When RakePositionIs(RakePosition_Home);
   After(1);
   RakePositionHasNotBeenHomeShouldBe(CLEAR);
}

TEST(RakeController, ShouldNotSetRakePositionHasNotBeenHomeAfterRakeNotHomeTestTimeWhileRakeControlRequestIsClear)
{
   Given TheModuleIsInitializedAndRakeControlRequestIsSet();
   Given RakePositionIs(RakePosition_Home);

   When RakePositionIs(RakePosition_NotHome);
   After(harvestData.rakeNotHomeTestTimeInSeconds * MSEC_PER_SEC - 1);
   RakePositionHasNotBeenHomeShouldBe(CLEAR);

   When RakeControlRequestIs(CLEAR);
   After(1);
   RakePositionHasNotBeenHomeShouldBe(CLEAR);
}

TEST(RakeController, ShouldNotSetRakePositionHasNotBeenHomeAfterRakeNotHomeTimeWhileRakeControlRequestIsClear)
{
   Given TheModuleIsInitializedAndRakeControlRequestIsSet();
   Given RakePositionIs(RakePosition_Home);

   When RakeControlRequestIs(CLEAR);
   And RakePositionIs(RakePosition_NotHome);

   After(harvestData.rakeNotHomeTestTimeInSeconds * MSEC_PER_SEC);
   RakePositionHasNotBeenHomeShouldBe(CLEAR);
}

TEST(RakeController, ShouldSetFeelerArmPositionHasBeenBucketFullWhenFeelerArmPositionHasBeenBucketFullForFeelerArmTestTimeWhileRakeControlRequestIsSet)
{
   Given TheModuleIsInitializedAndRakeControlRequestIsSet();
   Given FeelerArmPositionIs(FeelerArmPosition_BucketEmpty);

   When FeelerArmPositionIs(FeelerArmPosition_BucketFull);
   After(harvestData.feelerArmTestTimeInSeconds * MSEC_PER_SEC - 1);
   FeelerArmPositionHasBeenBucketFullShouldBe(CLEAR);

   After(1);
   FeelerArmPositionHasBeenBucketFullShouldBe(SET);
}

TEST(RakeController, ShouldNotSetFeelerArmPositionHasBeenBucketFullAfterFeelerArmTestTimeWhenFeelerArmPositionChangedToBucketEmptyBeforeTheTimeout)
{
   Given TheModuleIsInitializedAndRakeControlRequestIsSet();
   Given FeelerArmPositionIs(FeelerArmPosition_BucketEmpty);

   When FeelerArmPositionIs(FeelerArmPosition_BucketFull);
   After(harvestData.feelerArmTestTimeInSeconds * MSEC_PER_SEC - 1);
   FeelerArmPositionHasBeenBucketFullShouldBe(CLEAR);

   When FeelerArmPositionIs(FeelerArmPosition_BucketEmpty);
   After(1);
   FeelerArmPositionHasBeenBucketFullShouldBe(CLEAR);
}

TEST(RakeController, ShouldNotSetFeelerArmPositionHasBeenBucketFullAfterFeelerArmTestTimeWhileRakeControlRequestIsClear)
{
   Given TheModuleIsInitializedAndRakeControlRequestIsSet();
   Given FeelerArmPositionIs(FeelerArmPosition_BucketEmpty);

   When FeelerArmPositionIs(FeelerArmPosition_BucketFull);
   After(harvestData.feelerArmTestTimeInSeconds * MSEC_PER_SEC - 1);
   FeelerArmPositionHasBeenBucketFullShouldBe(CLEAR);

   When RakeControlRequestIs(CLEAR);
   After(1);
   FeelerArmPositionHasBeenBucketFullShouldBe(CLEAR);
}

TEST(RakeController, ShouldNotSetFeelerArmHasBeenFullAfterItWasSet)
{
   Given TheModuleIsInitializedAndRakeControlRequestIsSet();
   Given FeelerArmPositionIs(FeelerArmPosition_BucketEmpty);

   When FeelerArmPositionIs(FeelerArmPosition_BucketFull);
   After(harvestData.feelerArmTestTimeInSeconds * MSEC_PER_SEC);
   FeelerArmPositionHasBeenBucketFullShouldBe(SET);

   When FeelerArmPositionHasBeenBucketFullIs(CLEAR);
   And FeelerArmPositionIs(FeelerArmPosition_BucketEmpty);
   And FeelerArmPositionIs(FeelerArmPosition_BucketFull);

   After(harvestData.feelerArmTestTimeInSeconds * MSEC_PER_SEC);
   FeelerArmPositionHasBeenBucketFullShouldBe(CLEAR);
}

TEST(RakeController, ShouldVoteToTurnOffRakeMotorAndSetRakeCompletedRevolutionWhenRakePositionIsHomeAfterRakePositionHasNotBeenHomeAndFeelerArmPositionHasBeenBucketFullAreSet)
{
   Given TheModuleIsInitializedAndRakeControlRequestIsSet();
   Given RakePositionIs(RakePosition_Home);
   Given FeelerArmPositionIs(FeelerArmPosition_BucketEmpty);

   When RakePositionIs(RakePosition_NotHome);
   After(harvestData.rakeNotHomeTestTimeInSeconds * MSEC_PER_SEC);
   RakePositionHasNotBeenHomeShouldBe(SET);

   When FeelerArmPositionIs(FeelerArmPosition_BucketFull);
   After(harvestData.feelerArmTestTimeInSeconds * MSEC_PER_SEC);
   FeelerArmPositionHasBeenBucketFullShouldBe(SET);

   When RakePositionIs(RakePosition_Home);
   And RakeMotorVoteShouldBe(MotorState_Off, Vote_Care);
   RakeCompletedRevolutionShouldBe(SET);
}

TEST(RakeController, ShouldNotSetRakeCompletedRevolutionWhenRakePositionIsNotHomeAfterRakePositionHasNotBeenHomeAndFeelerArmPositionHasBeenBucketFullAreSet)
{
   Given TheModuleIsInitializedAndRakeControlRequestIsSet();
   Given RakePositionIs(RakePosition_Home);
   Given FeelerArmPositionIs(FeelerArmPosition_BucketEmpty);

   When RakePositionIs(RakePosition_NotHome);
   After(harvestData.rakeNotHomeTestTimeInSeconds * MSEC_PER_SEC);
   RakePositionHasNotBeenHomeShouldBe(SET);

   When FeelerArmPositionIs(FeelerArmPosition_BucketFull);
   After(harvestData.feelerArmTestTimeInSeconds * MSEC_PER_SEC);
   FeelerArmPositionHasBeenBucketFullShouldBe(SET);

   RakeCompletedRevolutionShouldBe(CLEAR);
}

TEST(RakeController, ShouldVoteToTurnOffRakeMotorAndSetRakeCompletedRevolutionWhenFeelerArmPositionHasBeenBucketFullIsSetWhileRakePositionIsHomeAndRakePositionHasNotBeenHomeIsSet)
{
   Given TheModuleIsInitializedAndRakeControlRequestIsSet();
   Given RakePositionIs(RakePosition_Home);
   Given FeelerArmPositionIs(FeelerArmPosition_BucketEmpty);

   When RakePositionIs(RakePosition_NotHome);
   After(harvestData.rakeNotHomeTestTimeInSeconds * MSEC_PER_SEC);

   When RakePositionIs(RakePosition_Home);

   When FeelerArmPositionIs(FeelerArmPosition_BucketFull);
   After(harvestData.feelerArmTestTimeInSeconds * MSEC_PER_SEC);
   RakeMotorVoteShouldBe(MotorState_Off, Vote_Care);
   And RakeCompletedRevolutionShouldBe(SET);
}

TEST(RakeController, ShouldNotSetRakePositionHasNotBeenHomeAfterRakeRevolutionCompletedIsSetWithRakeHomePosition)
{
   Given TheModuleIsInitializedAndRakeRevolutionCompletedWithRakePositionHome();

   When RakePositionHasNotBeenHomeIs(CLEAR);
   And RakePositionIs(RakePosition_Home);
   And RakePositionIs(RakePosition_NotHome);

   After(harvestData.rakeNotHomeTestTimeInSeconds * MSEC_PER_SEC);
   RakePositionHasNotBeenHomeShouldBe(CLEAR);
}

TEST(RakeController, ShouldNotSetRakePositionHasNotBeenHomeAfterRakeRevolutionCompletedIsSetWithFeelerArmHasBeenBucketFull)
{
   Given TheModuleIsInitializedAndRakeRevolutionCompletedWithFeelerArmHasBeenBucketFull();

   When RakePositionHasNotBeenHomeIs(CLEAR);
   And RakePositionIs(RakePosition_Home);
   And RakePositionIs(RakePosition_NotHome);

   After(harvestData.rakeNotHomeTestTimeInSeconds * MSEC_PER_SEC);
   RakePositionHasNotBeenHomeShouldBe(CLEAR);
}
