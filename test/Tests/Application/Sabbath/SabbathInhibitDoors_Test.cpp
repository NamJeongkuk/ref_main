/*!
 * @file
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "SabbathInhibitDoors.h"
#include "Constants_Binary.h"
}

#include "CppUTest/TestHarness.h"
#include "ReferDataModel_TestDouble.h"

enum
{
   Closed = false,
   Opened = true
};

static const SabbathDoorResolvedPair_t doorResolvedPairs[] = {
   { .doorStatusErd = Erd_LeftSideFreshFoodDoorIsOpen, .doorStatusResolvedErd = Erd_LeftSideFreshFoodDoorIsOpenResolved },
   { .doorStatusErd = Erd_DoorInDoorIsOpen, .doorStatusResolvedErd = Erd_DoorInDoorIsOpenResolved },
   { .doorStatusErd = Erd_LeftSideFreezerDoorStatus, .doorStatusResolvedErd = Erd_LeftSideFreezerDoorStatusResolved },
   { .doorStatusErd = Erd_RightSideFreshFoodDoorStatus, .doorStatusResolvedErd = Erd_RightSideFreshFoodDoorStatusResolved },
};

static const SabbathInhibitDoorsConfiguration_t config = {
   .doorResolvedPairErdList = doorResolvedPairs,
   .numberOfPairs = NUM_ELEMENTS(doorResolvedPairs),
   .sabbathModeErd = Erd_SabbathModeEnable,
   .enhancedSabbathModeErd = Erd_EnhancedSabbathModeEnable,
   .sabbathGpioErd = Erd_Gpio_SABBATH
};

static bool testPassed;

TEST_GROUP(SabbathInhibitDoors)
{
   ReferDataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;
   SabbathInhibitDoors_t instance;
   EventSubscription_t subscription;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelTestDouble);
      dataModel = dataModelTestDouble.dataModel;
      testPassed = false;
   }

   void GivenTheSabbathInhibitDoorsIsInitialized()
   {
      SabbathInhibitDoors_Init(
         &instance,
         dataModel,
         &config);
   }

   void GivenSabbathModeIs(bool state)
   {
      DataModel_Write(
         dataModel,
         Erd_SabbathModeEnable,
         &state);
   }

   void GivenEnhancedSabbathModeIs(bool state)
   {
      DataModel_Write(
         dataModel,
         Erd_EnhancedSabbathModeEnable,
         &state);
   }

   void WhenEnhancedSabbathModeBecomes(bool state)
   {
      GivenEnhancedSabbathModeIs(state);
   }

   void WhenSabbathModeBecomes(bool state)
   {
      GivenSabbathModeIs(state);
   }

   void GivenAllDoorsAre(bool state)
   {
      for(uint8_t index; index < NUM_ELEMENTS(doorResolvedPairs); index++)
      {
         DataModel_Write(
            dataModel,
            doorResolvedPairs[index].doorStatusErd,
            &state);
      }
   }

   void WhenAllDoorsBecome(bool state)
   {
      GivenAllDoorsAre(state);
   }

   void AllResolvedDoorStatesShouldBe(bool expectedState)
   {
      bool actualState;
      for(uint8_t index; index < NUM_ELEMENTS(doorResolvedPairs); index++)
      {
         DataModel_Read(
            dataModel,
            doorResolvedPairs[index].doorStatusResolvedErd,
            &actualState);

         CHECK_EQUAL(expectedState, actualState);
      }
   }

   void GivenTheModuleIsInitalizedAndNormalSabbathAndEnhancedSabbathAreDisabled()
   {
      GivenEnhancedSabbathModeIs(DISABLED);
      GivenSabbathModeIs(DISABLED);
      GivenTheSabbathInhibitDoorsIsInitialized();
   }

   void GivenTheModuleIsInitalizedAndNormalSabbathAndEnhancedSabbathAreEnabled()
   {
      GivenEnhancedSabbathModeIs(ENABLED);
      GivenSabbathModeIs(ENABLED);
      GivenTheSabbathInhibitDoorsIsInitialized();
   }

   void GivenASpecificDoorIs(Erd_t specificDoor, bool state)
   {
      if(specificDoor == Erd_LeftSideFreshFoodDoorIsOpen)
      {
         DataModel_Write(
            dataModel,
            Erd_LeftSideFreshFoodDoorIsOpen,
            &state);
      }
      else if(specificDoor == Erd_DoorInDoorIsOpen)
      {
         DataModel_Write(
            dataModel,
            Erd_DoorInDoorIsOpen,
            &state);
      }
      else if(specificDoor == Erd_LeftSideFreezerDoorStatus)
      {
         DataModel_Write(
            dataModel,
            Erd_LeftSideFreezerDoorStatus,
            &state);
      }
      else
      {
         DataModel_Write(
            dataModel,
            Erd_RightSideFreshFoodDoorStatus,
            &state);
      }
   }

   void AllResolvedDoorStatesShouldBeTheRealDoorState()
   {
      for(uint8_t index = 0; index < NUM_ELEMENTS(doorResolvedPairs); index++)
      {
         bool resolvedDoorState;
         DataModel_Read(
            dataModel,
            doorResolvedPairs[index].doorStatusResolvedErd,
            &resolvedDoorState);

         bool realDoorState;
         DataModel_Read(
            dataModel,
            doorResolvedPairs[index].doorStatusErd,
            &realDoorState);

         CHECK_EQUAL(resolvedDoorState, realDoorState);
      }
   }

   void GivenSabbathGpioIs(bool state)
   {
      DataModel_Write(dataModel, config.sabbathGpioErd, &state);
   }

   void SabbathGpioShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, config.sabbathGpioErd, &actual);
      CHECK_EQUAL(expected, actual);
   }

   static void DataModelChangedSabbathGpioShouldBeSetWhenDoorIsResolvedToClosed(void *context, const void *_args)
   {
      REINTERPRET(dataModel, context, I_DataModel_t *);
      REINTERPRET(args, _args, const DataModelOnDataChangeArgs_t *);

      if(args->erd == doorResolvedPairs[0].doorStatusResolvedErd)
      {
         bool gpioState;
         DataModel_Read(
            dataModel,
            config.sabbathGpioErd,
            &gpioState);

         testPassed = gpioState ? true : false;
      }
   }

   void SabbathGpioShouldBeSetWhenResolvedDoorsAreClosedUponEnteringSabbath(void)
   {
      EventSubscription_Init(
         &subscription,
         dataModel,
         DataModelChangedSabbathGpioShouldBeSetWhenDoorIsResolvedToClosed);
      Event_Subscribe(dataModel->OnDataChange, &subscription);

      WhenSabbathModeBecomes(ENABLED);
      CHECK_TRUE(testPassed);
   }

   static void DataModelChangedResolvedDoorsShouldBeClosedWhenSabbathGpioIsSet(void *context, const void *_args)
   {
      REINTERPRET(dataModel, context, I_DataModel_t *);
      REINTERPRET(args, _args, const DataModelOnDataChangeArgs_t *);

      if(args->erd == config.sabbathGpioErd)
      {
         bool doorState;
         testPassed = true;

         for(uint8_t index = 0; index < config.numberOfPairs; index++)
         {
            DataModel_Read(
               dataModel,
               doorResolvedPairs[index].doorStatusResolvedErd,
               &doorState);

            if(doorState == Opened)
            {
               testPassed = false;
               return;
            }
         }
      }
   }

   void ResolvedDoorsShouldRemainClosedWhenSabbathGpioIsSetUponDisablingSabbath(void)
   {
      EventSubscription_Init(
         &subscription,
         dataModel,
         DataModelChangedResolvedDoorsShouldBeClosedWhenSabbathGpioIsSet);
      Event_Subscribe(dataModel->OnDataChange, &subscription);

      WhenSabbathModeBecomes(DISABLED);
      WhenEnhancedSabbathModeBecomes(DISABLED);
      CHECK_TRUE(testPassed);
   }
};

TEST(SabbathInhibitDoors, ShouldReportResolvedDoorsClosedIfInSabbathModeOnInitialization)
{
   GivenSabbathModeIs(ENABLED);
   GivenAllDoorsAre(Opened);
   GivenTheSabbathInhibitDoorsIsInitialized();
   AllResolvedDoorStatesShouldBe(Closed);
}

TEST(SabbathInhibitDoors, ShouldReportResolvedDoorsClosedIfInEnhancedSabbathModeOnInitialization)
{
   GivenEnhancedSabbathModeIs(ENABLED);
   GivenAllDoorsAre(Opened);
   GivenTheSabbathInhibitDoorsIsInitialized();
   AllResolvedDoorStatesShouldBe(Closed);
}

TEST(SabbathInhibitDoors, ShouldReportResolvedDoorsAsRealDoorStateWhileSabbathAndEnhancedSabbathIsDisabled)
{
   GivenSabbathModeIs(DISABLED);
   GivenEnhancedSabbathModeIs(DISABLED);
   GivenTheSabbathInhibitDoorsIsInitialized();

   WhenAllDoorsBecome(Opened);
   AllResolvedDoorStatesShouldBeTheRealDoorState();
}

TEST(SabbathInhibitDoors, ShouldReportResolvedDoorsClosedIfInSabbathMode)
{
   GivenTheSabbathInhibitDoorsIsInitialized();
   GivenAllDoorsAre(Opened);
   AllResolvedDoorStatesShouldBeTheRealDoorState();

   WhenSabbathModeBecomes(ENABLED);
   AllResolvedDoorStatesShouldBe(Closed);
}

TEST(SabbathInhibitDoors, ShouldReportResolvedDoorsClosedIfInEnhancedSabbath)
{
   GivenTheSabbathInhibitDoorsIsInitialized();
   GivenAllDoorsAre(Opened);
   AllResolvedDoorStatesShouldBeTheRealDoorState();

   WhenEnhancedSabbathModeBecomes(ENABLED);
   AllResolvedDoorStatesShouldBe(Closed);
}

TEST(SabbathInhibitDoors, ShouldReportResolvedDoorsAsRealDoorStateWhenEnhancedSabbathIsEnabledThenDisabledWhileEnhancedSabbathDisabled)
{
   GivenSabbathModeIs(ENABLED);
   GivenEnhancedSabbathModeIs(DISABLED);
   GivenTheSabbathInhibitDoorsIsInitialized();
   GivenAllDoorsAre(Opened);
   AllResolvedDoorStatesShouldBe(Closed);

   WhenSabbathModeBecomes(DISABLED);
   AllResolvedDoorStatesShouldBeTheRealDoorState();

   WhenAllDoorsBecome(Closed);
   AllResolvedDoorStatesShouldBeTheRealDoorState();
}

TEST(SabbathInhibitDoors, ShouldReportResolvedDoorsAsRealDoorStateWhenSabbathAndEnhancedSabbathDisabledAndLeftSideFreshFoodDoorStateChanges)
{
   GivenTheModuleIsInitalizedAndNormalSabbathAndEnhancedSabbathAreDisabled();
   GivenASpecificDoorIs(Erd_LeftSideFreshFoodDoorIsOpen, Opened);
   AllResolvedDoorStatesShouldBeTheRealDoorState();
}

TEST(SabbathInhibitDoors, ShouldReportResolvedDoorsAsRealDoorStateWhenSabbathAndEnhancedSabbathDisabledAndDoorInDoorChanges)
{
   GivenTheModuleIsInitalizedAndNormalSabbathAndEnhancedSabbathAreDisabled();
   GivenASpecificDoorIs(Erd_DoorInDoorIsOpen, Opened);
   AllResolvedDoorStatesShouldBeTheRealDoorState();
}

TEST(SabbathInhibitDoors, ShouldReportResolvedDoorsAsRealDoorStateWhenSabbathAndEnhancedSabbathDisabledAndLeftSideFreezerDoorChanges)
{
   GivenTheModuleIsInitalizedAndNormalSabbathAndEnhancedSabbathAreDisabled();
   GivenASpecificDoorIs(Erd_LeftSideFreezerDoorStatus, Opened);
   AllResolvedDoorStatesShouldBeTheRealDoorState();
}

TEST(SabbathInhibitDoors, ShouldReportResolvedDoorsAsRealDoorStateWhenSabbathAndEnhancedSabbathDisabledAndRightSideFreshFoodDoorChanges)
{
   GivenTheModuleIsInitalizedAndNormalSabbathAndEnhancedSabbathAreDisabled();
   GivenASpecificDoorIs(Erd_RightSideFreshFoodDoorStatus, Opened);
   AllResolvedDoorStatesShouldBeTheRealDoorState();
}

TEST(SabbathInhibitDoors, ShouldClearSabbathGpioWhenEitherSabbathOrEnhancedSabbathAreEntered)
{
   GivenSabbathGpioIs(CLEAR);
   GivenTheModuleIsInitalizedAndNormalSabbathAndEnhancedSabbathAreDisabled();
   SabbathGpioShouldBe(SET);

   WhenSabbathModeBecomes(ENABLED);
   SabbathGpioShouldBe(CLEAR);

   WhenSabbathModeBecomes(DISABLED);
   SabbathGpioShouldBe(SET);

   WhenEnhancedSabbathModeBecomes(ENABLED);
   SabbathGpioShouldBe(CLEAR);
}

TEST(SabbathInhibitDoors, ShouldSetSabbathGpioWhenBothSabbathAndEnhancedSabbathAreCleared)
{
   GivenSabbathGpioIs(SET);
   GivenTheModuleIsInitalizedAndNormalSabbathAndEnhancedSabbathAreEnabled();
   SabbathGpioShouldBe(CLEAR);

   WhenSabbathModeBecomes(DISABLED);
   SabbathGpioShouldBe(CLEAR);

   WhenEnhancedSabbathModeBecomes(DISABLED);
   SabbathGpioShouldBe(SET);
}

TEST(SabbathInhibitDoors, ShouldVoteTheDoorStatesClosedBeforeClearingGpio)
{
   GivenSabbathGpioIs(SET);
   GivenAllDoorsAre(Opened);
   GivenTheModuleIsInitalizedAndNormalSabbathAndEnhancedSabbathAreDisabled();

   SabbathGpioShouldBeSetWhenResolvedDoorsAreClosedUponEnteringSabbath();
}

TEST(SabbathInhibitDoors, ShouldSetGpioBeforeVotingForTheDoorStates)
{
   GivenAllDoorsAre(Opened);
   GivenTheModuleIsInitalizedAndNormalSabbathAndEnhancedSabbathAreEnabled();

   ResolvedDoorsShouldRemainClosedWhenSabbathGpioIsSetUponDisablingSabbath();
}
