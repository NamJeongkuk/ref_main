/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "Application.h"
#include "SystemErds.h"
#include "Constants_Binary.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "uassert_test.h"
#include "ReferDataModel_TestDouble.h"
#include "TddPersonality.h"
#include "PersonalityTestSetup.h"

TEST_GROUP(IceFormationIntegration)
{
   Application_t instance;
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   ResetReason_t resetReason;

   void setup(PersonalityId_t personality)
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble, personality);
      dataModel = dataModelDouble.dataModel;
   }

   void GivenApplicationHasBeenInitialized()
   {
      Application_Init(&instance, dataModel, resetReason);
   }

   void WhenIceFormationIs(bool state)
   {
      DataModel_Write(dataModel, Erd_IceCabinet_IceFormationIsActive, &state);
   }

   void IceFormationOffsetShouldBe(TemperatureDegFx100_t expected)
   {
      TemperatureDegFx100_t actual;
      DataModel_Read(dataModel, Erd_IceCabinet_IceFormationOffsetInDegFx100, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void IceFormationOffsetShouldBeSetToValueFromParametricData()
   {
      IceFormationOffsetShouldBe(
         PersonalityParametricData_Get(dataModel)->iceCabinetThermalOffsetData->iceFormationOffsetInDegFx100);
   }
};

#define PERSONALITIES(ENTRY) \
   ENTRY(TddPersonality_DevelopmentDualEvapFourDoor)

TEST(IceFormationIntegration, ShouldWriteIceFormationOffsetWhenIceFormationIsActive)
{
   GivenApplicationHasBeenInitialized();

   WhenIceFormationIs(true);
   IceFormationOffsetShouldBeSetToValueFromParametricData();
}

TEST(IceFormationIntegration, ShouldClearIceFormationOffsetWhenIceFormationIsNotActive)
{
   GivenApplicationHasBeenInitialized();

   WhenIceFormationIs(true);
   WhenIceFormationIs(false);
   IceFormationOffsetShouldBe(0);
}

#undef PERSONALITIES
#define PERSONALITIES(ENTRY)                                       \
   ENTRY(TddPersonality_DevelopmentSingleEvaporator)               \
   ENTRY(TddPersonality_DevelopmentSingleDoorSingleEvapAllFreezer) \
   ENTRY(TddPersonality_DevelopmentSingleDoorSingleEvapAllFreshFood)

TEST(IceFormationIntegration, ShouldNotSetIceFormationOffsetRegardlessOfIceFormationState)
{
   GivenApplicationHasBeenInitialized();

   IceFormationOffsetShouldBe(0);
   WhenIceFormationIs(true);
   IceFormationOffsetShouldBe(0);
   WhenIceFormationIs(false);
   IceFormationOffsetShouldBe(0);
}
