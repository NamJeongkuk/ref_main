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
   DispensingInhibitedByDoorErd
};

static const DataModel_TestDoubleConfigurationEntry_t erds[] = {
   { LeftHandFreshFoodDoorIsOpenErd, sizeof(bool) },
   { RightHandFreshFoodDoorIsOpenErd, sizeof(bool) },
   { LeftHandFreezerDoorIsOpenErd, sizeof(bool) },
   { RightHandFreezerDoorIsOpenErd, sizeof(bool) },
   { ConvertibleCompartmentDoorIsOpenErd, sizeof(bool) },
   { DoorInDoorIsOpenErd, sizeof(bool) },
   { DispensingInhibitedByDoorErd, sizeof(bool) }
};

static const DoorInhibitDispensingResolverConfig_t config = {
   .leftHandFreshFoodDoorIsOpenErd = LeftHandFreshFoodDoorIsOpenErd,
   .rightHandFreshFoodDoorIsOpenErd = RightHandFreshFoodDoorIsOpenErd,
   .leftHandFreezerDoorIsOpenErd = LeftHandFreezerDoorIsOpenErd,
   .rightHandFreezerDoorIsOpenErd = RightHandFreezerDoorIsOpenErd,
   .convertibleCompartmentDoorIsOpenErd = ConvertibleCompartmentDoorIsOpenErd,
   .doorInDoorIsOpenErd = DoorInDoorIsOpenErd,
   .dispensingInhibitedByDoorErd = DispensingInhibitedByDoorErd
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

   void TheDispensingInhibitedByDoorShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, DispensingInhibitedByDoorErd, &actual);

      CHECK_EQUAL(expected, actual);
   }
};

TEST(DoorInhibitDispensingResolverWithAllDoorsInhibited, ShouldInitialize)
{
   GivenTheModuleIsInitialized();
}

TEST(DoorInhibitDispensingResolverWithAllDoorsInhibited, ShouldInhibitDispensingWhenLeftHandFreshFoodDoorOpensOnInit)
{
   GivenTheLeftHandFreshFoodDoorOpens();
   TheDispensingInhibitedByDoorShouldBe(CLEAR);

   WhenTheModuleIsInitialized();
   TheDispensingInhibitedByDoorShouldBe(SET);
}

TEST(DoorInhibitDispensingResolverWithAllDoorsInhibited, ShouldInhibitDispensingWhenRightHandFreshFoodDoorOpensOnInit)
{
   GivenTheRightHandFreshFoodDoorOpens();
   TheDispensingInhibitedByDoorShouldBe(CLEAR);

   WhenTheModuleIsInitialized();
   TheDispensingInhibitedByDoorShouldBe(SET);
}

TEST(DoorInhibitDispensingResolverWithAllDoorsInhibited, ShouldInhibitDispensingWhenLeftHandFreezerDoorOpensOnInit)
{
   GivenTheLeftHandFreezerDoorOpens();
   TheDispensingInhibitedByDoorShouldBe(CLEAR);

   WhenTheModuleIsInitialized();
   TheDispensingInhibitedByDoorShouldBe(SET);
}

TEST(DoorInhibitDispensingResolverWithAllDoorsInhibited, ShouldInhibitDispensingWhenRightHandFreezerDoorOpensOnInit)
{
   GivenTheRightHandFreezerDoorOpens();
   TheDispensingInhibitedByDoorShouldBe(CLEAR);

   WhenTheModuleIsInitialized();
   TheDispensingInhibitedByDoorShouldBe(SET);
}

TEST(DoorInhibitDispensingResolverWithAllDoorsInhibited, ShouldInhibitDispensingWhenConvertibleCompartmentDoorOpensOnInit)
{
   GivenTheConvertibleCompartmentDoorOpens();
   TheDispensingInhibitedByDoorShouldBe(CLEAR);

   WhenTheModuleIsInitialized();
   TheDispensingInhibitedByDoorShouldBe(SET);
}

TEST(DoorInhibitDispensingResolverWithAllDoorsInhibited, ShouldInhibitDispensingWhenDoorInDoorOpensOnInit)
{
   GivenTheDoorInDoorOpens();
   TheDispensingInhibitedByDoorShouldBe(CLEAR);

   WhenTheModuleIsInitialized();
   TheDispensingInhibitedByDoorShouldBe(SET);
}

TEST(DoorInhibitDispensingResolverWithAllDoorsInhibited, ShouldInhibitDispensingWhenLeftHandFreshFoodDoorOpens)
{
   GivenTheModuleIsInitialized();
   TheDispensingInhibitedByDoorShouldBe(CLEAR);

   WhenTheLeftHandFreshFoodDoorOpens();
   TheDispensingInhibitedByDoorShouldBe(SET);
}

TEST(DoorInhibitDispensingResolverWithAllDoorsInhibited, ShouldAllowDispensingWhenLeftHandFreshFoodDoorCloses)
{
   GivenTheModuleIsInitialized();
   GivenTheLeftHandFreshFoodDoorOpens();
   TheDispensingInhibitedByDoorShouldBe(SET);

   WhenTheLeftHandFreshFoodDoorCloses();
   TheDispensingInhibitedByDoorShouldBe(CLEAR);
}

TEST(DoorInhibitDispensingResolverWithAllDoorsInhibited, ShouldInhibitDispensingWhenRightHandFreshFoodDoorOpens)
{
   GivenTheModuleIsInitialized();
   TheDispensingInhibitedByDoorShouldBe(CLEAR);

   WhenTheRightHandFreshFoodDoorOpens();
   TheDispensingInhibitedByDoorShouldBe(SET);
}

TEST(DoorInhibitDispensingResolverWithAllDoorsInhibited, ShouldAllowDispensingWhenRightHandFreshFoodDoorCloses)
{
   GivenTheModuleIsInitialized();
   GivenTheRightHandFreshFoodDoorOpens();
   TheDispensingInhibitedByDoorShouldBe(SET);

   WhenTheRightHandFreshFoodDoorCloses();
   TheDispensingInhibitedByDoorShouldBe(CLEAR);
}

TEST(DoorInhibitDispensingResolverWithAllDoorsInhibited, ShouldInhibitDispensingWhenLeftHandFreezerDoorOpens)
{
   GivenTheModuleIsInitialized();
   TheDispensingInhibitedByDoorShouldBe(CLEAR);

   WhenTheLeftHandFreezerDoorOpens();
   TheDispensingInhibitedByDoorShouldBe(SET);
}

TEST(DoorInhibitDispensingResolverWithAllDoorsInhibited, ShouldAllowDispensingWhenLeftHandFreezerDoorCloses)
{
   GivenTheModuleIsInitialized();
   GivenTheLeftHandFreezerDoorOpens();
   TheDispensingInhibitedByDoorShouldBe(SET);

   WhenTheLeftHandFreezerDoorCloses();
   TheDispensingInhibitedByDoorShouldBe(CLEAR);
}

TEST(DoorInhibitDispensingResolverWithAllDoorsInhibited, ShouldInhibitDispensingWhenRightHandFreezerDoorOpens)
{
   GivenTheModuleIsInitialized();
   TheDispensingInhibitedByDoorShouldBe(CLEAR);

   WhenTheRightHandFreezerDoorOpens();
   TheDispensingInhibitedByDoorShouldBe(SET);
}

TEST(DoorInhibitDispensingResolverWithAllDoorsInhibited, ShouldAllowDispensingWhenRightHandFreezerDoorCloses)
{
   GivenTheModuleIsInitialized();
   GivenTheRightHandFreezerDoorOpens();
   TheDispensingInhibitedByDoorShouldBe(SET);

   WhenTheRightHandFreezerDoorCloses();
   TheDispensingInhibitedByDoorShouldBe(CLEAR);
}

TEST(DoorInhibitDispensingResolverWithAllDoorsInhibited, ShouldInhibitDispensingWhenConvertibleCompartmentDoorOpens)
{
   GivenTheModuleIsInitialized();
   TheDispensingInhibitedByDoorShouldBe(CLEAR);

   WhenTheConvertibleCompartmentDoorOpens();
   TheDispensingInhibitedByDoorShouldBe(SET);
}

TEST(DoorInhibitDispensingResolverWithAllDoorsInhibited, ShouldInhibitDispensingWhenConvertibleCompartmentDoorCloses)
{
   GivenTheModuleIsInitialized();
   GivenTheConvertibleCompartmentDoorOpens();
   TheDispensingInhibitedByDoorShouldBe(SET);

   WhenTheConvertibleCompartmentDoorCloses();
   TheDispensingInhibitedByDoorShouldBe(CLEAR);
}

TEST(DoorInhibitDispensingResolverWithAllDoorsInhibited, ShouldInhibitDispensingWhenDoorInDoorOpens)
{
   GivenTheModuleIsInitialized();
   TheDispensingInhibitedByDoorShouldBe(CLEAR);

   WhenTheDoorInDoorOpens();
   TheDispensingInhibitedByDoorShouldBe(SET);
}

TEST(DoorInhibitDispensingResolverWithAllDoorsInhibited, ShouldAllowDispensingWhenDoorInDoorCloses)
{
   GivenTheModuleIsInitialized();
   GivenTheDoorInDoorOpens();
   TheDispensingInhibitedByDoorShouldBe(SET);

   WhenTheDoorInDoorCloses();
   TheDispensingInhibitedByDoorShouldBe(CLEAR);
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
   TheDispensingInhibitedByDoorShouldBe(SET);

   WhenTheLeftHandFreshFoodDoorCloses();
   TheDispensingInhibitedByDoorShouldBe(SET);

   WhenTheRightHandFreshFoodDoorCloses();
   TheDispensingInhibitedByDoorShouldBe(SET);

   WhenTheLeftHandFreezerDoorCloses();
   TheDispensingInhibitedByDoorShouldBe(SET);

   WhenTheRightHandFreezerDoorCloses();
   TheDispensingInhibitedByDoorShouldBe(SET);

   WhenTheConvertibleCompartmentDoorCloses();
   TheDispensingInhibitedByDoorShouldBe(SET);

   WhenTheDoorInDoorCloses();
   TheDispensingInhibitedByDoorShouldBe(CLEAR);
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

   void TheDispensingInhibitedByDoorShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, DispensingInhibitedByDoorErd, &actual);

      CHECK_EQUAL(expected, actual);
   }
};

TEST(DoorInhibitDispensingResolverWithAllDoorsNotInhibited, ShouldNotInhibitDispensingWhenLeftHandFreshFoodDoorOpens)
{
   GivenTheModuleIsInitialized();
   TheDispensingInhibitedByDoorShouldBe(CLEAR);

   WhenTheLeftHandFreshFoodDoorOpens();
   TheDispensingInhibitedByDoorShouldBe(CLEAR);
}

TEST(DoorInhibitDispensingResolverWithAllDoorsNotInhibited, ShouldNotInhibitDispensingWhenRightHandFreshFoodDoorOpens)
{
   GivenTheModuleIsInitialized();
   TheDispensingInhibitedByDoorShouldBe(CLEAR);

   WhenTheRightHandFreshFoodDoorOpens();
   TheDispensingInhibitedByDoorShouldBe(CLEAR);
}

TEST(DoorInhibitDispensingResolverWithAllDoorsNotInhibited, ShouldNotInhibitDispensingWhenLeftHandFreezerDoorOpens)
{
   GivenTheModuleIsInitialized();
   TheDispensingInhibitedByDoorShouldBe(CLEAR);

   WhenTheLeftHandFreezerDoorOpens();
   TheDispensingInhibitedByDoorShouldBe(CLEAR);
}

TEST(DoorInhibitDispensingResolverWithAllDoorsNotInhibited, ShouldNotInhibitDispensingWhenRightHandFreezerDoorOpens)
{
   GivenTheModuleIsInitialized();
   TheDispensingInhibitedByDoorShouldBe(CLEAR);

   WhenTheRightHandFreezerDoorOpens();
   TheDispensingInhibitedByDoorShouldBe(CLEAR);
}

TEST(DoorInhibitDispensingResolverWithAllDoorsNotInhibited, ShouldNotInhibitDispensingWhenConvertibleCompartmentDoorOpens)
{
   GivenTheModuleIsInitialized();
   TheDispensingInhibitedByDoorShouldBe(CLEAR);

   WhenTheConvertibleCompartmentDoorOpens();
   TheDispensingInhibitedByDoorShouldBe(CLEAR);
}

TEST(DoorInhibitDispensingResolverWithAllDoorsNotInhibited, ShouldNotInhibitDispensingWhenDoorInDoorOpens)
{
   GivenTheModuleIsInitialized();
   TheDispensingInhibitedByDoorShouldBe(CLEAR);

   WhenTheDoorInDoorOpens();
   TheDispensingInhibitedByDoorShouldBe(CLEAR);
}
