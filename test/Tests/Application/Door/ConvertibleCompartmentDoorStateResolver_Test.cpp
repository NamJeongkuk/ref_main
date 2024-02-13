/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "ConvertibleCompartmentDoorStateResolver.h"
#include "SystemErds.h"
#include "Constants_Binary.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

enum
{
   Closes = 0,
   Closed = Closes,
   Opens = 1,
   Open = Opens
};

static ConvertibleCompartmentDoorResolverConfiguration_t config = {
   .convertibleCompartmentDoorIsOpenErd = Erd_ConvertibleCompartmentDoorStatus,
   .convertibleCompartmentStateErd = Erd_ConvertibleCompartmentState,
   .convertibleCompartmentAsFreshFoodDoorIsOpenErd = Erd_ConvertibleCompartmentAsFreshFoodDoorStatus,
   .convertibleCompartmentAsFreezerDoorIsOpenErd = Erd_ConvertibleCompartmentAsFreezerDoorStatus
};

TEST_GROUP(ConvertibleCompartmentDoorStateResolver)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   ConvertibleCompartmentDoorStateResolver_t instance;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
   }

   void GivenModuleIsInitialized()
   {
      ConvertibleCompartmentDoorStateResolver_Init(&instance, dataModel, &config);
   }

   void GivenConvertibleCompartmentDoorIs(bool state)
   {
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentDoorStatus, &state);
   }

   void GivenConvertibleCompartmentDoorIsActingAsA(ConvertibleCompartmentStateType_t state)
   {
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentState, &state);
   }

   void GivenConvertibleCompartmentActingAsAFreshFoodDoorIs(bool state)
   {
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentAsFreshFoodDoorStatus, &state);
   }

   void GivenConvertibleCompartmentActingAsAFreezerDoorIs(bool state)
   {
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentAsFreezerDoorStatus, &state);
   }

   void ConvertibleCompartmentAsFreshFoodDoorShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_ConvertibleCompartmentAsFreshFoodDoorStatus, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void ConvertibleCompartmentAsFreezerDoorShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_ConvertibleCompartmentAsFreezerDoorStatus, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void WhenConvertibleCompartmentIsActingAsA(ConvertibleCompartmentStateType_t state)
   {
      GivenConvertibleCompartmentDoorIsActingAsA(state);
   }

   void WhenConvertibleCompartmentDoor(bool state)
   {
      GivenConvertibleCompartmentDoorIs(state);
   }

   void GivenConvertibleCompartmentIsActingAsAFreshFoodAndTheDoorIsOpen()
   {
      GivenConvertibleCompartmentDoorIsActingAsA(ConvertibleCompartmentStateType_FreshFood);
      GivenConvertibleCompartmentDoorIs(Open);
      GivenModuleIsInitialized();

      ConvertibleCompartmentAsFreshFoodDoorShouldBe(Open);
   }

   void GivenConvertibleCompartmentIsActingAsAFreezerAndTheDoorIsOpen()
   {
      GivenConvertibleCompartmentDoorIsActingAsA(ConvertibleCompartmentStateType_Freezer);
      GivenConvertibleCompartmentDoorIs(Open);
      GivenModuleIsInitialized();

      ConvertibleCompartmentAsFreezerDoorShouldBe(Open);
   }

   void GivenConvertibleCompartmentIsActingAsAFreshFoodAndTheDoorIsClosed()
   {
      GivenConvertibleCompartmentDoorIsActingAsA(ConvertibleCompartmentStateType_FreshFood);
      GivenConvertibleCompartmentDoorIs(Closed);
      GivenModuleIsInitialized();

      ConvertibleCompartmentAsFreshFoodDoorShouldBe(Closed);
   }

   void GivenConvertibleCompartmentIsActingAsAFreezerAndTheDoorIsClosed()
   {
      GivenConvertibleCompartmentDoorIsActingAsA(ConvertibleCompartmentStateType_Freezer);
      GivenConvertibleCompartmentDoorIs(Closed);
      GivenModuleIsInitialized();

      ConvertibleCompartmentAsFreezerDoorShouldBe(Closed);
   }
};

TEST(ConvertibleCompartmentDoorStateResolver, ShouldSetConvertibleCompartmentAsFreshFoodDoorToOpenAndAsFreezerDoorToClosedWhenTheDoorIsOpenAndItIsActingAsAFreshFood)
{
   GivenConvertibleCompartmentDoorIsActingAsA(ConvertibleCompartmentStateType_FreshFood);
   GivenConvertibleCompartmentDoorIs(Open);
   GivenModuleIsInitialized();

   ConvertibleCompartmentAsFreshFoodDoorShouldBe(Open);
   ConvertibleCompartmentAsFreezerDoorShouldBe(Closed);
}

TEST(ConvertibleCompartmentDoorStateResolver, ShouldSetConvertibleCompartmentAsFreezerDoorToOpenAndAsFreshFoodDoorToClosedWhenTheDoorIsOpenAndItIsActingAsAFreezer)
{
   GivenConvertibleCompartmentDoorIsActingAsA(ConvertibleCompartmentStateType_Freezer);
   GivenConvertibleCompartmentDoorIs(Open);
   GivenModuleIsInitialized();

   ConvertibleCompartmentAsFreezerDoorShouldBe(Open);
   ConvertibleCompartmentAsFreshFoodDoorShouldBe(Closed);
}

TEST(ConvertibleCompartmentDoorStateResolver, ShouldSetConvertibleCompartmentAsFreshFoodDoorToClosedAndAsFreezerDoorToClosedWhenTheDoorIsClosedAndItIsActingAsAFreshFood)
{
   GivenConvertibleCompartmentDoorIsActingAsA(ConvertibleCompartmentStateType_FreshFood);
   GivenConvertibleCompartmentDoorIs(Closed);
   GivenModuleIsInitialized();

   ConvertibleCompartmentAsFreshFoodDoorShouldBe(Closed);
   ConvertibleCompartmentAsFreezerDoorShouldBe(Closed);
}

TEST(ConvertibleCompartmentDoorStateResolver, ShouldSetConvertibleCompartmentAsFreezerDoorToClosedAndAsFreezerDoorToClosedWhenTheDoorIsClosedAndItIsActingAsAFreezer)
{
   GivenConvertibleCompartmentDoorIsActingAsA(ConvertibleCompartmentStateType_Freezer);
   GivenConvertibleCompartmentDoorIs(Closed);
   GivenModuleIsInitialized();

   ConvertibleCompartmentAsFreshFoodDoorShouldBe(Closed);
   ConvertibleCompartmentAsFreezerDoorShouldBe(Closed);
}

TEST(ConvertibleCompartmentDoorStateResolver, ShouldSetConvertibleCompartmentAsFreshFoodToClosedWhenDoorCloses)
{
   GivenConvertibleCompartmentIsActingAsAFreshFoodAndTheDoorIsOpen();

   WhenConvertibleCompartmentDoor(Closes);
   ConvertibleCompartmentAsFreshFoodDoorShouldBe(Closed);
}

TEST(ConvertibleCompartmentDoorStateResolver, ShouldSetConvertibleCompartmentAsFreezerToClosedWhenDoorCloses)
{
   GivenConvertibleCompartmentIsActingAsAFreezerAndTheDoorIsOpen();

   WhenConvertibleCompartmentDoor(Closes);
   ConvertibleCompartmentAsFreezerDoorShouldBe(Closed);
}

TEST(ConvertibleCompartmentDoorStateResolver, ShouldSetConvertibleCompartmentAsFreshFoodToOpenWhenDoorOpens)
{
   GivenConvertibleCompartmentIsActingAsAFreshFoodAndTheDoorIsClosed();

   WhenConvertibleCompartmentDoor(Opens);
   ConvertibleCompartmentAsFreshFoodDoorShouldBe(Open);
}

TEST(ConvertibleCompartmentDoorStateResolver, ShouldSetConvertibleCompartmentAsFreezerToOpenWhenDoorOpens)
{
   GivenConvertibleCompartmentIsActingAsAFreezerAndTheDoorIsClosed();

   WhenConvertibleCompartmentDoor(Opens);
   ConvertibleCompartmentAsFreezerDoorShouldBe(Open);
}

TEST(ConvertibleCompartmentDoorStateResolver, ShouldSetConvertibleCompartmentActingAsFreezerDoorToOpenAndFreshFoodDoorToClosedWhenConvertibleCompartmentSwitchesFromActingLikeAFreshFoodToAFreezer)
{
   GivenConvertibleCompartmentIsActingAsAFreshFoodAndTheDoorIsOpen();

   WhenConvertibleCompartmentIsActingAsA(ConvertibleCompartmentStateType_Freezer);
   ConvertibleCompartmentAsFreshFoodDoorShouldBe(Closed);
   ConvertibleCompartmentAsFreezerDoorShouldBe(Open);
}

TEST(ConvertibleCompartmentDoorStateResolver, ShouldSetConvertibleCompartmentActingAsFreshFoodDoorToOpenAndFreezerDoorToClosedWhenConvertibleCompartmentSwitchesFromActingLikeAFreezerToAFreshFood)
{
   GivenConvertibleCompartmentIsActingAsAFreezerAndTheDoorIsOpen();

   WhenConvertibleCompartmentIsActingAsA(ConvertibleCompartmentStateType_FreshFood);
   ConvertibleCompartmentAsFreshFoodDoorShouldBe(Open);
   ConvertibleCompartmentAsFreezerDoorShouldBe(Closed);
}

TEST(ConvertibleCompartmentDoorStateResolver, ShouldSetConvertibleCompartmentActingAsFreshFoodDoorToClosedAndFreezerDoorToClosedWhenConvertibleCompartmentSwitchesFromActingLikeAFreshFoodToAFreezer)
{
   GivenConvertibleCompartmentActingAsAFreezerDoorIs(Open);
   GivenConvertibleCompartmentIsActingAsAFreshFoodAndTheDoorIsClosed();

   WhenConvertibleCompartmentIsActingAsA(ConvertibleCompartmentStateType_Freezer);
   ConvertibleCompartmentAsFreshFoodDoorShouldBe(Closed);
   ConvertibleCompartmentAsFreezerDoorShouldBe(Closed);
}

TEST(ConvertibleCompartmentDoorStateResolver, ShouldSetConvertibleCompartmentActingAsFreshFoodDoorToClosedAndFreezerDoorToClosedWhenConvertibleCompartmentSwitchesFromActingLikeAFreezerToAFreshFood)
{
   GivenConvertibleCompartmentActingAsAFreshFoodDoorIs(Open);
   GivenConvertibleCompartmentIsActingAsAFreezerAndTheDoorIsClosed();

   WhenConvertibleCompartmentIsActingAsA(ConvertibleCompartmentStateType_FreshFood);
   ConvertibleCompartmentAsFreshFoodDoorShouldBe(Closed);
   ConvertibleCompartmentAsFreezerDoorShouldBe(Closed);
}
