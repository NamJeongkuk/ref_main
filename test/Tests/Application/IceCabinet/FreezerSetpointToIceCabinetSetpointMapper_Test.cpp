/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "FreezerSetpointToIceCabinetSetpointMapper.h"
#include "PersonalityParametricData.h"
#include "DataModelErdPointerAccess.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

static const FreezerSetpointToIceCabinetSetpointMapperConfig_t config = {
   .freezerResolvedSetpoint = Erd_Freezer_ResolvedSetpointInDegFx100,
   .iceCabinetSetpoint = Erd_IceCabinetSetpoint
};

static const FreezerSetpointToIceCabinetSetpointTableEntry_t entries[] = {
   { -200, 300 },
   { -100, 400 },
   { 0, 500 },
   { 100, 600 },
   { 200, 700 }
};

static const FreezerSetpointToIceCabinetSetpointTable_t mappingTable = {
   .entries = entries,
   .numEntries = NUM_ELEMENTS(entries)
};

TEST_GROUP(FreezerSetpointToIceCabinetSetpoint)
{
   FreezerSetpointToIceCabinetSetpointMapper_t instance;
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
   }

   void GivenFreezerSetpointToIceCabinetSetpointMapperIsInitialized()
   {
      FreezerSetpointToIceCabinetSetpointMapper_Init(
         &instance,
         dataModel,
         &config,
         &mappingTable);
   }

   void GivenFreezerSetpointIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_Freezer_ResolvedSetpointInDegFx100, &temperature);
   }

   void WhenFreezerSetpointIs(TemperatureDegFx100_t temperature)
   {
      GivenFreezerSetpointIs(temperature);
   }

   void IceCabinetSetpointShouldBe(TemperatureDegFx100_t expected)
   {
      TemperatureDegFx100_t actual;
      DataModel_Read(dataModel, Erd_IceCabinetSetpoint, &actual);

      CHECK_EQUAL(expected, actual);
   }
};

TEST(FreezerSetpointToIceCabinetSetpoint, ShouldInitialize)
{
   GivenFreezerSetpointToIceCabinetSetpointMapperIsInitialized();
}

TEST(FreezerSetpointToIceCabinetSetpoint, ShouldWriteIceCabinetSetpointBasedOnFreezerSetpointOnInitWithFreezerSetpointFoundInTable)
{
   GivenFreezerSetpointIs(-200);
   GivenFreezerSetpointToIceCabinetSetpointMapperIsInitialized();

   IceCabinetSetpointShouldBe(300);
}

TEST(FreezerSetpointToIceCabinetSetpoint, ShouldWriteIceCabinetSetpointBasedOnFreezerSetpointOnInitWithFreezerSetpointBetweenTwoFreezerSetpointsInTable)
{
   GivenFreezerSetpointIs(150);
   GivenFreezerSetpointToIceCabinetSetpointMapperIsInitialized();

   IceCabinetSetpointShouldBe(700);
}

TEST(FreezerSetpointToIceCabinetSetpoint, ShouldWriteIceCabinetSetpointBasedOnFreezerSetpointOnInitWithFreezerSetpointGreaterThanFreezerSetpointsInTable)
{
   GivenFreezerSetpointIs(250);
   GivenFreezerSetpointToIceCabinetSetpointMapperIsInitialized();

   IceCabinetSetpointShouldBe(700);
}

TEST(FreezerSetpointToIceCabinetSetpoint, ShouldWriteIceCabinetSetpointBasedOnFreezerSetpointOnInitWithFreezerSetpointLessThanFreezerSetpointsInTable)
{
   GivenFreezerSetpointIs(-250);
   GivenFreezerSetpointToIceCabinetSetpointMapperIsInitialized();

   IceCabinetSetpointShouldBe(300);
}

TEST(FreezerSetpointToIceCabinetSetpoint, ShouldWriteIceCabinetSetpointBasedOnFreezerSetpointWhenFreezerSetpointChanges)
{
   GivenFreezerSetpointIs(-200);
   GivenFreezerSetpointToIceCabinetSetpointMapperIsInitialized();

   IceCabinetSetpointShouldBe(300);

   WhenFreezerSetpointIs(125);
   IceCabinetSetpointShouldBe(700);

   WhenFreezerSetpointIs(0);
   IceCabinetSetpointShouldBe(500);
}
