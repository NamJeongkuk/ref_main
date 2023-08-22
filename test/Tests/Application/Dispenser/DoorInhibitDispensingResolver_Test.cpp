/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "DoorInhibitDispensingResolver.h"
#include "Constants_Binary.h"
#include "DoorIndex.h"
#include "PersonalityParametricData.h"
#include "SystemErds.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "DataModel_TestDouble.h"
#include "PersonalityParametricDataTestDouble.h"
#include "TddPersonality.h"

enum
{
   LeftSideFreshFoodDoorIsOpenErd,
   RightSideFreshFoodDoorIsOpenErd,
   LeftSideFreezerDoorIsOpenErd,
   RightSideFreezerDoorIsOpenErd,
   ConvertibleCompartmentDoorIsOpenErd,
   DoorInDoorIsOpenErd,
   DispensingInhibitedErd,

   DispensingInhibitedBitmapIndex_SomeInhibitReason = 0
};

static const DataModel_TestDoubleConfigurationEntry_t erds[] = {
   { LeftSideFreshFoodDoorIsOpenErd, sizeof(bool) },
   { RightSideFreshFoodDoorIsOpenErd, sizeof(bool) },
   { LeftSideFreezerDoorIsOpenErd, sizeof(bool) },
   { RightSideFreezerDoorIsOpenErd, sizeof(bool) },
   { ConvertibleCompartmentDoorIsOpenErd, sizeof(bool) },
   { DoorInDoorIsOpenErd, sizeof(bool) },
   { DispensingInhibitedErd, sizeof(DispensingInhibitedBitmap_t) }
};

static DoorInhibitDispensingResolverDoorConfig_t doorsThatInhibitDispense[] = {
   {
      .doorIsOpenErd = LeftSideFreshFoodDoorIsOpenErd,
      .doorIndex = DoorIndex_LeftSideFreshFood,
      .offsetInParametricForDoorInhibitsDispense = OFFSET_OF(DoorInhibitDispenseTable_t, leftHandFreshFood),
   },
   {
      .doorIsOpenErd = RightSideFreshFoodDoorIsOpenErd,
      .doorIndex = DoorIndex_RightSideFreshFood,
      .offsetInParametricForDoorInhibitsDispense = OFFSET_OF(DoorInhibitDispenseTable_t, rightHandFreshFood),
   },
   {
      .doorIsOpenErd = LeftSideFreezerDoorIsOpenErd,
      .doorIndex = DoorIndex_LeftSideFreezer,
      .offsetInParametricForDoorInhibitsDispense = OFFSET_OF(DoorInhibitDispenseTable_t, leftHandFreezer),
   },
};

static const DoorInhibitDispensingResolverConfig_t config = {
   .dispensingInhibitedErd = DispensingInhibitedErd,
   .dispensingInhibitedBitmapIndex = DispensingInhibitedBitmapIndex_SomeInhibitReason,
   .doorsThatInhibitDispense = doorsThatInhibitDispense,
   .numberOfDoors = NUM_ELEMENTS(doorsThatInhibitDispense)
};

TEST_GROUP(DoorInhibitDispensingResolverWithAllDoorsInhibited)
{
   DoorInhibitDispensingResolver_t instance;
   DataModel_TestDouble_t dataModelTestDouble;

   I_DataModel_t *dataModel;
   PersonalityParametricData_t *parametricData;
   const DoorInhibitDispenseTable_t *doorInhibitDispenseTable;

   void setup()
   {
      DataModel_TestDouble_Init(&dataModelTestDouble, erds, NUM_ELEMENTS(erds));
      dataModel = dataModelTestDouble.dataModel;

      parametricData = (PersonalityParametricData_t *)GivenThatTheApplicationParametricDataHasBeenLoadedIntoAPointer(TddPersonality_DevelopmentSingleEvaporator);
      doorInhibitDispenseTable = parametricData->dispenserData->doorInhibitWaterDispenseTable;
   }

   void GivenTheModuleIsInitialized()
   {
      DoorInhibitDispensingResolver_Init(
         &instance,
         dataModel,
         &config,
         doorInhibitDispenseTable);
   }

   void WhenTheModuleIsInitialized()
   {
      GivenTheModuleIsInitialized();
   }

   void WhenTheLeftSideFreshFoodDoorOpens()
   {
      DataModel_Write(dataModel, LeftSideFreshFoodDoorIsOpenErd, set);
   }

   void GivenTheLeftSideFreshFoodDoorOpens()
   {
      WhenTheLeftSideFreshFoodDoorOpens();
   }

   void WhenTheLeftSideFreshFoodDoorCloses()
   {
      DataModel_Write(dataModel, LeftSideFreshFoodDoorIsOpenErd, clear);
   }

   void WhenTheRightSideFreshFoodDoorOpens()
   {
      DataModel_Write(dataModel, RightSideFreshFoodDoorIsOpenErd, set);
   }

   void GivenTheRightSideFreshFoodDoorOpens()
   {
      WhenTheRightSideFreshFoodDoorOpens();
   }

   void WhenTheRightSideFreshFoodDoorCloses()
   {
      DataModel_Write(dataModel, RightSideFreshFoodDoorIsOpenErd, clear);
   }

   void WhenTheLeftSideFreezerDoorOpens()
   {
      DataModel_Write(dataModel, LeftSideFreezerDoorIsOpenErd, set);
   }

   void GivenTheLeftSideFreezerDoorOpens()
   {
      WhenTheLeftSideFreezerDoorOpens();
   }

   void WhenTheLeftSideFreezerDoorCloses()
   {
      DataModel_Write(dataModel, LeftSideFreezerDoorIsOpenErd, clear);
   }

   void WhenTheRightSideFreezerDoorOpens()
   {
      DataModel_Write(dataModel, RightSideFreezerDoorIsOpenErd, set);
   }

   void GivenTheRightSideFreezerDoorOpens()
   {
      WhenTheRightSideFreezerDoorOpens();
   }

   void WhenTheRightSideFreezerDoorCloses()
   {
      DataModel_Write(dataModel, RightSideFreezerDoorIsOpenErd, clear);
   }

   void WhenTheConvertibleCompartmentDoorOpens()
   {
      DataModel_Write(dataModel, ConvertibleCompartmentDoorIsOpenErd, set);
   }

   void GivenTheConvertibleCompartmentDoorOpens()
   {
      WhenTheConvertibleCompartmentDoorOpens();
   }

   void WhenTheConvertibleCompartmentDoorCloses()
   {
      DataModel_Write(dataModel, ConvertibleCompartmentDoorIsOpenErd, clear);
   }

   void WhenTheDoorInDoorOpens()
   {
      DataModel_Write(dataModel, DoorInDoorIsOpenErd, set);
   }

   void GivenTheDoorInDoorOpens()
   {
      WhenTheDoorInDoorOpens();
   }

   void WhenTheDoorInDoorCloses()
   {
      DataModel_Write(dataModel, DoorInDoorIsOpenErd, clear);
   }

   void TheDispensingInhibitedByDoorBitmapIndexShouldBe(bool expected)
   {
      DispensingInhibitedBitmap_t actual;
      DataModel_Read(dataModel, DispensingInhibitedErd, &actual);

      CHECK_EQUAL(expected, BIT_STATE(actual, DispensingInhibitedBitmapIndex_SomeInhibitReason));
   }
};

TEST(DoorInhibitDispensingResolverWithAllDoorsInhibited, ShouldInitialize)
{
   GivenTheModuleIsInitialized();
}

TEST(DoorInhibitDispensingResolverWithAllDoorsInhibited, ShouldInhibitDispensingWhenLeftSideFreshFoodDoorOpensOnInit)
{
   GivenTheLeftSideFreshFoodDoorOpens();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(CLEAR);

   WhenTheModuleIsInitialized();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(SET);
}

TEST(DoorInhibitDispensingResolverWithAllDoorsInhibited, ShouldInhibitDispensingWhenRightSideFreshFoodDoorOpensOnInit)
{
   GivenTheRightSideFreshFoodDoorOpens();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(CLEAR);

   WhenTheModuleIsInitialized();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(SET);
}

TEST(DoorInhibitDispensingResolverWithAllDoorsInhibited, ShouldInhibitDispensingWhenLeftSideFreezerDoorOpensOnInit)
{
   GivenTheLeftSideFreezerDoorOpens();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(CLEAR);

   WhenTheModuleIsInitialized();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(SET);
}

TEST(DoorInhibitDispensingResolverWithAllDoorsInhibited, ShouldInhibitDispensingWhenLeftSideFreshFoodDoorOpens)
{
   GivenTheModuleIsInitialized();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(CLEAR);

   WhenTheLeftSideFreshFoodDoorOpens();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(SET);
}

TEST(DoorInhibitDispensingResolverWithAllDoorsInhibited, ShouldAllowDispensingWhenLeftSideFreshFoodDoorCloses)
{
   GivenTheModuleIsInitialized();
   GivenTheLeftSideFreshFoodDoorOpens();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(SET);

   WhenTheLeftSideFreshFoodDoorCloses();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(CLEAR);
}

TEST(DoorInhibitDispensingResolverWithAllDoorsInhibited, ShouldInhibitDispensingWhenRightSideFreshFoodDoorOpens)
{
   GivenTheModuleIsInitialized();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(CLEAR);

   WhenTheRightSideFreshFoodDoorOpens();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(SET);
}

TEST(DoorInhibitDispensingResolverWithAllDoorsInhibited, ShouldAllowDispensingWhenRightSideFreshFoodDoorCloses)
{
   GivenTheModuleIsInitialized();
   GivenTheRightSideFreshFoodDoorOpens();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(SET);

   WhenTheRightSideFreshFoodDoorCloses();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(CLEAR);
}

TEST(DoorInhibitDispensingResolverWithAllDoorsInhibited, ShouldInhibitDispensingWhenLeftSideFreezerDoorOpens)
{
   GivenTheModuleIsInitialized();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(CLEAR);

   WhenTheLeftSideFreezerDoorOpens();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(SET);
}

TEST(DoorInhibitDispensingResolverWithAllDoorsInhibited, ShouldAllowDispensingWhenLeftSideFreezerDoorCloses)
{
   GivenTheModuleIsInitialized();
   GivenTheLeftSideFreezerDoorOpens();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(SET);

   WhenTheLeftSideFreezerDoorCloses();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(CLEAR);
}

TEST(DoorInhibitDispensingResolverWithAllDoorsInhibited, ShouldInhibitDispensingUntilAllTheDoorsAreClosed)
{
   GivenTheModuleIsInitialized();
   GivenTheLeftSideFreshFoodDoorOpens();
   GivenTheRightSideFreshFoodDoorOpens();
   GivenTheLeftSideFreezerDoorOpens();
   GivenTheRightSideFreezerDoorOpens();
   GivenTheConvertibleCompartmentDoorOpens();
   GivenTheDoorInDoorOpens();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(SET);

   WhenTheLeftSideFreshFoodDoorCloses();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(SET);

   WhenTheRightSideFreshFoodDoorCloses();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(SET);

   WhenTheLeftSideFreezerDoorCloses();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(CLEAR);
}

TEST_GROUP(DoorInhibitDispensingResolverWithAllDoorsNotInhibited)
{
   DoorInhibitDispensingResolver_t instance;
   DataModel_TestDouble_t dataModelTestDouble;

   I_DataModel_t *dataModel;
   PersonalityParametricData_t *parametricData;
   const DoorInhibitDispenseTable_t *doorInhibitDispenseTable;

   void setup()
   {
      DataModel_TestDouble_Init(&dataModelTestDouble, erds, NUM_ELEMENTS(erds));
      dataModel = dataModelTestDouble.dataModel;

      parametricData = (PersonalityParametricData_t *)GivenThatTheApplicationParametricDataHasBeenLoadedIntoAPointer(TddPersonality_DevelopmentSingleEvaporator);
      doorInhibitDispenseTable = parametricData->dispenserData->doorInhibitIceDispenseTable;
   }

   void GivenTheModuleIsInitialized()
   {
      DoorInhibitDispensingResolver_Init(
         &instance,
         dataModel,
         &config,
         doorInhibitDispenseTable);
   }

   void WhenTheLeftSideFreshFoodDoorOpens()
   {
      DataModel_Write(dataModel, LeftSideFreshFoodDoorIsOpenErd, set);
   }

   void GivenTheLeftSideFreshFoodDoorOpens()
   {
      WhenTheLeftSideFreshFoodDoorOpens();
   }

   void WhenTheLeftSideFreshFoodDoorCloses()
   {
      DataModel_Write(dataModel, LeftSideFreshFoodDoorIsOpenErd, clear);
   }

   void WhenTheRightSideFreshFoodDoorOpens()
   {
      DataModel_Write(dataModel, RightSideFreshFoodDoorIsOpenErd, set);
   }

   void GivenTheRightSideFreshFoodDoorOpens()
   {
      WhenTheRightSideFreshFoodDoorOpens();
   }

   void WhenTheRightSideFreshFoodDoorCloses()
   {
      DataModel_Write(dataModel, RightSideFreshFoodDoorIsOpenErd, clear);
   }

   void WhenTheLeftSideFreezerDoorOpens()
   {
      DataModel_Write(dataModel, LeftSideFreezerDoorIsOpenErd, set);
   }

   void GivenTheLeftSideFreezerDoorOpens()
   {
      WhenTheLeftSideFreezerDoorOpens();
   }

   void WhenTheLeftSideFreezerDoorCloses()
   {
      DataModel_Write(dataModel, LeftSideFreezerDoorIsOpenErd, clear);
   }

   void WhenTheRightSideFreezerDoorOpens()
   {
      DataModel_Write(dataModel, RightSideFreezerDoorIsOpenErd, set);
   }

   void GivenTheRightSideFreezerDoorOpens()
   {
      WhenTheRightSideFreezerDoorOpens();
   }

   void WhenTheRightSideFreezerDoorCloses()
   {
      DataModel_Write(dataModel, RightSideFreezerDoorIsOpenErd, clear);
   }

   void WhenTheConvertibleCompartmentDoorOpens()
   {
      DataModel_Write(dataModel, ConvertibleCompartmentDoorIsOpenErd, set);
   }

   void GivenTheConvertibleCompartmentDoorOpens()
   {
      WhenTheConvertibleCompartmentDoorOpens();
   }

   void WhenTheConvertibleCompartmentDoorCloses()
   {
      DataModel_Write(dataModel, ConvertibleCompartmentDoorIsOpenErd, clear);
   }

   void WhenTheDoorInDoorOpens()
   {
      DataModel_Write(dataModel, DoorInDoorIsOpenErd, set);
   }

   void GivenTheDoorInDoorOpens()
   {
      WhenTheDoorInDoorOpens();
   }

   void WhenTheDoorInDoorCloses()
   {
      DataModel_Write(dataModel, DoorInDoorIsOpenErd, clear);
   }

   void TheDispensingInhibitedByDoorBitmapIndexShouldBe(bool expected)
   {
      DispensingInhibitedBitmap_t actual;
      DataModel_Read(dataModel, DispensingInhibitedErd, &actual);

      CHECK_EQUAL(expected, BIT_STATE(actual, DispensingInhibitedBitmapIndex_SomeInhibitReason));
   }
};

TEST(DoorInhibitDispensingResolverWithAllDoorsNotInhibited, ShouldNotInhibitDispensingWhenLeftSideFreshFoodDoorOpens)
{
   GivenTheModuleIsInitialized();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(CLEAR);

   WhenTheLeftSideFreshFoodDoorOpens();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(CLEAR);
}

TEST(DoorInhibitDispensingResolverWithAllDoorsNotInhibited, ShouldNotInhibitDispensingWhenRightSideFreshFoodDoorOpens)
{
   GivenTheModuleIsInitialized();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(CLEAR);

   WhenTheRightSideFreshFoodDoorOpens();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(CLEAR);
}

TEST(DoorInhibitDispensingResolverWithAllDoorsNotInhibited, ShouldNotInhibitDispensingWhenLeftSideFreezerDoorOpens)
{
   GivenTheModuleIsInitialized();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(CLEAR);

   WhenTheLeftSideFreezerDoorOpens();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(CLEAR);
}
