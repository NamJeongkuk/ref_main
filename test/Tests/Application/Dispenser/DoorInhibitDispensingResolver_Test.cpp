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
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "DataModel_TestDouble.h"
#include "PersonalityParametricDataTestDouble.h"
#include "TddPersonality.h"

enum
{
   LeftHandFreshFoodDoorIsOpenErd,
   RightHandFreshFoodDoorIsOpenErd,
   LeftHandFreezerDoorIsOpenErd,
   RightHandFreezerDoorIsOpenErd,
   ConvertibleCompartmentDoorIsOpenErd,
   DoorInDoorIsOpenErd,
   DispensingInhibitedErd,

   DispensingInhibitedBitmapIndex_SomeInhibitReason = 0
};

static const DataModel_TestDoubleConfigurationEntry_t erds[] = {
   { LeftHandFreshFoodDoorIsOpenErd, sizeof(bool) },
   { RightHandFreshFoodDoorIsOpenErd, sizeof(bool) },
   { LeftHandFreezerDoorIsOpenErd, sizeof(bool) },
   { RightHandFreezerDoorIsOpenErd, sizeof(bool) },
   { ConvertibleCompartmentDoorIsOpenErd, sizeof(bool) },
   { DoorInDoorIsOpenErd, sizeof(bool) },
   { DispensingInhibitedErd, sizeof(DispensingInhibitedBitmap_t) }
};

static const DoorInhibitDispensingResolverConfig_t config = {
   .leftHandFreshFoodDoorIsOpenErd = LeftHandFreshFoodDoorIsOpenErd,
   .rightHandFreshFoodDoorIsOpenErd = RightHandFreshFoodDoorIsOpenErd,
   .leftHandFreezerDoorIsOpenErd = LeftHandFreezerDoorIsOpenErd,
   .rightHandFreezerDoorIsOpenErd = RightHandFreezerDoorIsOpenErd,
   .convertibleCompartmentDoorIsOpenErd = ConvertibleCompartmentDoorIsOpenErd,
   .doorInDoorIsOpenErd = DoorInDoorIsOpenErd,
   .dispensingInhibitedErd = DispensingInhibitedErd,
   .dispensingInhibitedBitmapIndex = DispensingInhibitedBitmapIndex_SomeInhibitReason
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

   void WhenTheLeftHandFreshFoodDoorOpens()
   {
      DataModel_Write(dataModel, LeftHandFreshFoodDoorIsOpenErd, set);
   }

   void GivenTheLeftHandFreshFoodDoorOpens()
   {
      WhenTheLeftHandFreshFoodDoorOpens();
   }

   void WhenTheLeftHandFreshFoodDoorCloses()
   {
      DataModel_Write(dataModel, LeftHandFreshFoodDoorIsOpenErd, clear);
   }

   void WhenTheRightHandFreshFoodDoorOpens()
   {
      DataModel_Write(dataModel, RightHandFreshFoodDoorIsOpenErd, set);
   }

   void GivenTheRightHandFreshFoodDoorOpens()
   {
      WhenTheRightHandFreshFoodDoorOpens();
   }

   void WhenTheRightHandFreshFoodDoorCloses()
   {
      DataModel_Write(dataModel, RightHandFreshFoodDoorIsOpenErd, clear);
   }

   void WhenTheLeftHandFreezerDoorOpens()
   {
      DataModel_Write(dataModel, LeftHandFreezerDoorIsOpenErd, set);
   }

   void GivenTheLeftHandFreezerDoorOpens()
   {
      WhenTheLeftHandFreezerDoorOpens();
   }

   void WhenTheLeftHandFreezerDoorCloses()
   {
      DataModel_Write(dataModel, LeftHandFreezerDoorIsOpenErd, clear);
   }

   void WhenTheRightHandFreezerDoorOpens()
   {
      DataModel_Write(dataModel, RightHandFreezerDoorIsOpenErd, set);
   }

   void GivenTheRightHandFreezerDoorOpens()
   {
      WhenTheRightHandFreezerDoorOpens();
   }

   void WhenTheRightHandFreezerDoorCloses()
   {
      DataModel_Write(dataModel, RightHandFreezerDoorIsOpenErd, clear);
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

TEST(DoorInhibitDispensingResolverWithAllDoorsInhibited, ShouldInhibitDispensingWhenLeftHandFreshFoodDoorOpensOnInit)
{
   GivenTheLeftHandFreshFoodDoorOpens();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(CLEAR);

   WhenTheModuleIsInitialized();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(SET);
}

TEST(DoorInhibitDispensingResolverWithAllDoorsInhibited, ShouldInhibitDispensingWhenRightHandFreshFoodDoorOpensOnInit)
{
   GivenTheRightHandFreshFoodDoorOpens();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(CLEAR);

   WhenTheModuleIsInitialized();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(SET);
}

TEST(DoorInhibitDispensingResolverWithAllDoorsInhibited, ShouldInhibitDispensingWhenLeftHandFreezerDoorOpensOnInit)
{
   GivenTheLeftHandFreezerDoorOpens();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(CLEAR);

   WhenTheModuleIsInitialized();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(SET);
}

TEST(DoorInhibitDispensingResolverWithAllDoorsInhibited, ShouldInhibitDispensingWhenRightHandFreezerDoorOpensOnInit)
{
   GivenTheRightHandFreezerDoorOpens();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(CLEAR);

   WhenTheModuleIsInitialized();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(SET);
}

TEST(DoorInhibitDispensingResolverWithAllDoorsInhibited, ShouldInhibitDispensingWhenConvertibleCompartmentDoorOpensOnInit)
{
   GivenTheConvertibleCompartmentDoorOpens();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(CLEAR);

   WhenTheModuleIsInitialized();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(SET);
}

TEST(DoorInhibitDispensingResolverWithAllDoorsInhibited, ShouldInhibitDispensingWhenDoorInDoorOpensOnInit)
{
   GivenTheDoorInDoorOpens();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(CLEAR);

   WhenTheModuleIsInitialized();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(SET);
}

TEST(DoorInhibitDispensingResolverWithAllDoorsInhibited, ShouldInhibitDispensingWhenLeftHandFreshFoodDoorOpens)
{
   GivenTheModuleIsInitialized();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(CLEAR);

   WhenTheLeftHandFreshFoodDoorOpens();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(SET);
}

TEST(DoorInhibitDispensingResolverWithAllDoorsInhibited, ShouldAllowDispensingWhenLeftHandFreshFoodDoorCloses)
{
   GivenTheModuleIsInitialized();
   GivenTheLeftHandFreshFoodDoorOpens();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(SET);

   WhenTheLeftHandFreshFoodDoorCloses();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(CLEAR);
}

TEST(DoorInhibitDispensingResolverWithAllDoorsInhibited, ShouldInhibitDispensingWhenRightHandFreshFoodDoorOpens)
{
   GivenTheModuleIsInitialized();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(CLEAR);

   WhenTheRightHandFreshFoodDoorOpens();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(SET);
}

TEST(DoorInhibitDispensingResolverWithAllDoorsInhibited, ShouldAllowDispensingWhenRightHandFreshFoodDoorCloses)
{
   GivenTheModuleIsInitialized();
   GivenTheRightHandFreshFoodDoorOpens();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(SET);

   WhenTheRightHandFreshFoodDoorCloses();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(CLEAR);
}

TEST(DoorInhibitDispensingResolverWithAllDoorsInhibited, ShouldInhibitDispensingWhenLeftHandFreezerDoorOpens)
{
   GivenTheModuleIsInitialized();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(CLEAR);

   WhenTheLeftHandFreezerDoorOpens();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(SET);
}

TEST(DoorInhibitDispensingResolverWithAllDoorsInhibited, ShouldAllowDispensingWhenLeftHandFreezerDoorCloses)
{
   GivenTheModuleIsInitialized();
   GivenTheLeftHandFreezerDoorOpens();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(SET);

   WhenTheLeftHandFreezerDoorCloses();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(CLEAR);
}

TEST(DoorInhibitDispensingResolverWithAllDoorsInhibited, ShouldInhibitDispensingWhenRightHandFreezerDoorOpens)
{
   GivenTheModuleIsInitialized();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(CLEAR);

   WhenTheRightHandFreezerDoorOpens();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(SET);
}

TEST(DoorInhibitDispensingResolverWithAllDoorsInhibited, ShouldAllowDispensingWhenRightHandFreezerDoorCloses)
{
   GivenTheModuleIsInitialized();
   GivenTheRightHandFreezerDoorOpens();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(SET);

   WhenTheRightHandFreezerDoorCloses();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(CLEAR);
}

TEST(DoorInhibitDispensingResolverWithAllDoorsInhibited, ShouldInhibitDispensingWhenConvertibleCompartmentDoorOpens)
{
   GivenTheModuleIsInitialized();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(CLEAR);

   WhenTheConvertibleCompartmentDoorOpens();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(SET);
}

TEST(DoorInhibitDispensingResolverWithAllDoorsInhibited, ShouldInhibitDispensingWhenConvertibleCompartmentDoorCloses)
{
   GivenTheModuleIsInitialized();
   GivenTheConvertibleCompartmentDoorOpens();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(SET);

   WhenTheConvertibleCompartmentDoorCloses();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(CLEAR);
}

TEST(DoorInhibitDispensingResolverWithAllDoorsInhibited, ShouldInhibitDispensingWhenDoorInDoorOpens)
{
   GivenTheModuleIsInitialized();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(CLEAR);

   WhenTheDoorInDoorOpens();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(SET);
}

TEST(DoorInhibitDispensingResolverWithAllDoorsInhibited, ShouldAllowDispensingWhenDoorInDoorCloses)
{
   GivenTheModuleIsInitialized();
   GivenTheDoorInDoorOpens();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(SET);

   WhenTheDoorInDoorCloses();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(CLEAR);
}

TEST(DoorInhibitDispensingResolverWithAllDoorsInhibited, ShouldInhibitDispensingUntilAllTheDoorsAreClosed)
{
   GivenTheModuleIsInitialized();
   GivenTheLeftHandFreshFoodDoorOpens();
   GivenTheRightHandFreshFoodDoorOpens();
   GivenTheLeftHandFreezerDoorOpens();
   GivenTheRightHandFreezerDoorOpens();
   GivenTheConvertibleCompartmentDoorOpens();
   GivenTheDoorInDoorOpens();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(SET);

   WhenTheLeftHandFreshFoodDoorCloses();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(SET);

   WhenTheRightHandFreshFoodDoorCloses();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(SET);

   WhenTheLeftHandFreezerDoorCloses();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(SET);

   WhenTheRightHandFreezerDoorCloses();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(SET);

   WhenTheConvertibleCompartmentDoorCloses();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(SET);

   WhenTheDoorInDoorCloses();
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

   void WhenTheLeftHandFreshFoodDoorOpens()
   {
      DataModel_Write(dataModel, LeftHandFreshFoodDoorIsOpenErd, set);
   }

   void GivenTheLeftHandFreshFoodDoorOpens()
   {
      WhenTheLeftHandFreshFoodDoorOpens();
   }

   void WhenTheLeftHandFreshFoodDoorCloses()
   {
      DataModel_Write(dataModel, LeftHandFreshFoodDoorIsOpenErd, clear);
   }

   void WhenTheRightHandFreshFoodDoorOpens()
   {
      DataModel_Write(dataModel, RightHandFreshFoodDoorIsOpenErd, set);
   }

   void GivenTheRightHandFreshFoodDoorOpens()
   {
      WhenTheRightHandFreshFoodDoorOpens();
   }

   void WhenTheRightHandFreshFoodDoorCloses()
   {
      DataModel_Write(dataModel, RightHandFreshFoodDoorIsOpenErd, clear);
   }

   void WhenTheLeftHandFreezerDoorOpens()
   {
      DataModel_Write(dataModel, LeftHandFreezerDoorIsOpenErd, set);
   }

   void GivenTheLeftHandFreezerDoorOpens()
   {
      WhenTheLeftHandFreezerDoorOpens();
   }

   void WhenTheLeftHandFreezerDoorCloses()
   {
      DataModel_Write(dataModel, LeftHandFreezerDoorIsOpenErd, clear);
   }

   void WhenTheRightHandFreezerDoorOpens()
   {
      DataModel_Write(dataModel, RightHandFreezerDoorIsOpenErd, set);
   }

   void GivenTheRightHandFreezerDoorOpens()
   {
      WhenTheRightHandFreezerDoorOpens();
   }

   void WhenTheRightHandFreezerDoorCloses()
   {
      DataModel_Write(dataModel, RightHandFreezerDoorIsOpenErd, clear);
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

TEST(DoorInhibitDispensingResolverWithAllDoorsNotInhibited, ShouldNotInhibitDispensingWhenLeftHandFreshFoodDoorOpens)
{
   GivenTheModuleIsInitialized();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(CLEAR);

   WhenTheLeftHandFreshFoodDoorOpens();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(CLEAR);
}

TEST(DoorInhibitDispensingResolverWithAllDoorsNotInhibited, ShouldNotInhibitDispensingWhenRightHandFreshFoodDoorOpens)
{
   GivenTheModuleIsInitialized();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(CLEAR);

   WhenTheRightHandFreshFoodDoorOpens();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(CLEAR);
}

TEST(DoorInhibitDispensingResolverWithAllDoorsNotInhibited, ShouldNotInhibitDispensingWhenLeftHandFreezerDoorOpens)
{
   GivenTheModuleIsInitialized();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(CLEAR);

   WhenTheLeftHandFreezerDoorOpens();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(CLEAR);
}

TEST(DoorInhibitDispensingResolverWithAllDoorsNotInhibited, ShouldNotInhibitDispensingWhenRightHandFreezerDoorOpens)
{
   GivenTheModuleIsInitialized();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(CLEAR);

   WhenTheRightHandFreezerDoorOpens();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(CLEAR);
}

TEST(DoorInhibitDispensingResolverWithAllDoorsNotInhibited, ShouldNotInhibitDispensingWhenConvertibleCompartmentDoorOpens)
{
   GivenTheModuleIsInitialized();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(CLEAR);

   WhenTheConvertibleCompartmentDoorOpens();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(CLEAR);
}

TEST(DoorInhibitDispensingResolverWithAllDoorsNotInhibited, ShouldNotInhibitDispensingWhenDoorInDoorOpens)
{
   GivenTheModuleIsInitialized();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(CLEAR);

   WhenTheDoorInDoorOpens();
   TheDispensingInhibitedByDoorBitmapIndexShouldBe(CLEAR);
}
