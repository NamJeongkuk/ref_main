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
   .sabbathModeErd = Erd_SabbathMode,
   .enhancedSabbathModeErd = Erd_EnhancedSabbathModeStatus
};

TEST_GROUP(SabbathInhibitDoors)
{
   ReferDataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;
   SabbathInhibitDoors_t instance;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelTestDouble);
      dataModel = dataModelTestDouble.dataModel;
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
         Erd_SabbathMode,
         &state);
   }

   void GivenEnhancedSabbathModeIs(bool state)
   {
      DataModel_Write(
         dataModel,
         Erd_EnhancedSabbathModeStatus,
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
      }
      CHECK_EQUAL(expectedState, actualState);
   }

   void GivenTheModuleIsInitalizedAndNormalSabbathAndEnhancedSabbathAreDisabled()
   {
      GivenEnhancedSabbathModeIs(DISABLED);
      GivenSabbathModeIs(DISABLED);
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
