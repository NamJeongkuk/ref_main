/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "CalculatedGridLines.h"
#include "FreshFoodNoFreezeLimitTrippedFaultDetector.h"
#include "SystemErds.h"
}

#include "CppUTest/TestHarness.h"
#include "ReferDataModel_TestDouble.h"

enum
{
   FreshFoodNoFreezeLimitGridValue = 500,
   FreshFoodLowHysteresisValue = 1000
};

static const FreshFoodNoFreezeLimitTrippedFaultDetectorConfig_t config = {
   .calculateGridLineErd = Erd_FreshFoodAndFreezerGrid_CalculatedGridLines,
   .freshFoodFilteredTemperatureErd = Erd_FreshFood_FilteredTemperatureResolvedInDegFx100,
   .freshFoodNoFreezeLimitTrippedFaultErd = Erd_FreshFoodNoFreezeLimitTrippedFault
};

TEST_GROUP(FreshFoodNoFreezeLimitTrippedFaultDetector)
{
   FreshFoodNoFreezeLimitTrippedFaultDetector_t instance;
   ReferDataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelTestDouble);
      dataModel = dataModelTestDouble.dataModel;
   }

   void GivenTheModuleIsInitialized()
   {
      FreshFoodNoFreezeLimitTrippedFaultDetector_Init(
         &instance,
         dataModel,
         &config);
   }

   void WhenTheModuleIsInitialized()
   {
      GivenTheModuleIsInitialized();
   }

   void GivenFreshFoodNoFreezeLimitIs(TemperatureDegFx100_t temperature)
   {
      TwoDimensionalCalculatedGridLines_t calculatedGridLines;
      DataModel_Read(dataModel, Erd_FreshFoodAndFreezerGrid_CalculatedGridLines, &calculatedGridLines);
      calculatedGridLines.firstDimensionGridLines.gridLinesDegFx100[GridLine_Nfl] = temperature;
      DataModel_Write(dataModel, Erd_FreshFoodAndFreezerGrid_CalculatedGridLines, &calculatedGridLines);
   }

   void GivenFreshFoodLowHysteresisIs(TemperatureDegFx100_t temperature)
   {
      TwoDimensionalCalculatedGridLines_t calculatedGridLines;
      DataModel_Read(dataModel, Erd_FreshFoodAndFreezerGrid_CalculatedGridLines, &calculatedGridLines);
      calculatedGridLines.firstDimensionGridLines.gridLinesDegFx100[GridLine_FreshFoodLowHyst] = temperature;
      DataModel_Write(dataModel, Erd_FreshFoodAndFreezerGrid_CalculatedGridLines, &calculatedGridLines);
   }

   void GivenFreshFoodFilteredTemperatureInDegFx100Is(TemperatureDegFx100_t temperatureDegFx100)
   {
      DataModel_Write(dataModel, config.freshFoodFilteredTemperatureErd, &temperatureDegFx100);
   }

   void WhenFreshFoodFilteredTemperatureInDegFx100Is(TemperatureDegFx100_t temperatureDegFx100)
   {
      GivenFreshFoodFilteredTemperatureInDegFx100Is(temperatureDegFx100);
   }

   void FreshFoodNoFreezeLimitTrippedFaultShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_FreshFoodNoFreezeLimitTrippedFault, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void GivenFreshFoodNoFreezeLimitTrippedFaultIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreshFoodNoFreezeLimitTrippedFault, &state);
   }
};

TEST(FreshFoodNoFreezeLimitTrippedFaultDetector, ShouldSetFreshFoodNoFreezeLimitTrippedFaultWhenFreshFoodTemperatureIsBelowTheNoFreezeLimitOnInit)
{
   GivenFreshFoodFilteredTemperatureInDegFx100Is(FreshFoodNoFreezeLimitGridValue - 1);
   GivenFreshFoodNoFreezeLimitIs(FreshFoodNoFreezeLimitGridValue);

   WhenTheModuleIsInitialized();
   FreshFoodNoFreezeLimitTrippedFaultShouldBe(true);
}

TEST(FreshFoodNoFreezeLimitTrippedFaultDetector, ShouldNotSetFreshFoodNoFreezeLimitTrippedFaultWhenFreshFoodTemperatureIsEqualToTheNoFreezeLimitOnInit)
{
   GivenFreshFoodFilteredTemperatureInDegFx100Is(FreshFoodNoFreezeLimitGridValue);
   GivenFreshFoodNoFreezeLimitIs(FreshFoodNoFreezeLimitGridValue);
   FreshFoodNoFreezeLimitTrippedFaultShouldBe(false);

   WhenTheModuleIsInitialized();
   FreshFoodNoFreezeLimitTrippedFaultShouldBe(false);
}

TEST(FreshFoodNoFreezeLimitTrippedFaultDetector, ShouldClearFreshFoodNoFreezeLimitTrippedFaultWhenFreshFoodTemperatureIsAboveLowHysteresisOnInit)
{
   GivenFreshFoodFilteredTemperatureInDegFx100Is(FreshFoodLowHysteresisValue + 1);
   GivenFreshFoodNoFreezeLimitIs(FreshFoodNoFreezeLimitGridValue);
   GivenFreshFoodLowHysteresisIs(FreshFoodLowHysteresisValue);
   GivenFreshFoodNoFreezeLimitTrippedFaultIs(true);

   WhenTheModuleIsInitialized();
   FreshFoodNoFreezeLimitTrippedFaultShouldBe(false);
}

TEST(FreshFoodNoFreezeLimitTrippedFaultDetector, ShouldNotClearFreshFoodNoFreezeLimitTrippedFaultWhenFreshFoodTemperatureIsBetweenNoFreezeLimitAndLowHysteresisOnInit)
{
   GivenFreshFoodFilteredTemperatureInDegFx100Is(FreshFoodNoFreezeLimitGridValue + 1);
   GivenFreshFoodNoFreezeLimitIs(FreshFoodNoFreezeLimitGridValue);
   GivenFreshFoodLowHysteresisIs(FreshFoodLowHysteresisValue);
   GivenFreshFoodNoFreezeLimitTrippedFaultIs(true);

   WhenTheModuleIsInitialized();
   FreshFoodNoFreezeLimitTrippedFaultShouldBe(true);
}

TEST(FreshFoodNoFreezeLimitTrippedFaultDetector, ShouldSetFreshFoodNoFreezeLimitTrippedFaultWhenFreshFoodTemperatureChangesToBelowTheNoFreezeLimit)
{
   GivenFreshFoodFilteredTemperatureInDegFx100Is(FreshFoodNoFreezeLimitGridValue + 1);
   GivenFreshFoodNoFreezeLimitIs(FreshFoodNoFreezeLimitGridValue);
   GivenTheModuleIsInitialized();
   FreshFoodNoFreezeLimitTrippedFaultShouldBe(false);

   WhenFreshFoodFilteredTemperatureInDegFx100Is(FreshFoodNoFreezeLimitGridValue - 20);
   FreshFoodNoFreezeLimitTrippedFaultShouldBe(true);
}

TEST(FreshFoodNoFreezeLimitTrippedFaultDetector, ShouldNotSetFreshFoodNoFreezeLimitTrippedFaultWhenFreshFoodTemperatureChangesToEqualToTheNoFreezeLimit)
{
   GivenFreshFoodFilteredTemperatureInDegFx100Is(FreshFoodNoFreezeLimitGridValue + 1);
   GivenFreshFoodNoFreezeLimitIs(FreshFoodNoFreezeLimitGridValue);
   GivenTheModuleIsInitialized();
   FreshFoodNoFreezeLimitTrippedFaultShouldBe(false);

   WhenFreshFoodFilteredTemperatureInDegFx100Is(FreshFoodNoFreezeLimitGridValue);
   FreshFoodNoFreezeLimitTrippedFaultShouldBe(false);
}

TEST(FreshFoodNoFreezeLimitTrippedFaultDetector, ShouldClearFreshFoodNoFreezeLimitTrippedFaultWhenFreshFoodTemperatureChangesToGreaterThanTheNoFreezeLimit)
{
   GivenFreshFoodFilteredTemperatureInDegFx100Is(FreshFoodNoFreezeLimitGridValue - 20);
   GivenFreshFoodNoFreezeLimitIs(FreshFoodNoFreezeLimitGridValue);
   GivenTheModuleIsInitialized();
   FreshFoodNoFreezeLimitTrippedFaultShouldBe(true);

   WhenFreshFoodFilteredTemperatureInDegFx100Is(FreshFoodNoFreezeLimitGridValue + 10);
   FreshFoodNoFreezeLimitTrippedFaultShouldBe(false);
}

TEST(FreshFoodNoFreezeLimitTrippedFaultDetector, ShouldNotClearFreshFoodNoFreezeLimitTrippedFaultWhenFreshFoodTemperatureChangesToBetweenNoFreezeLimitAndLowHysteresisValue)
{
   GivenFreshFoodFilteredTemperatureInDegFx100Is(FreshFoodNoFreezeLimitGridValue - 20);
   GivenFreshFoodNoFreezeLimitIs(FreshFoodNoFreezeLimitGridValue);
   GivenFreshFoodLowHysteresisIs(FreshFoodLowHysteresisValue);
   GivenTheModuleIsInitialized();
   FreshFoodNoFreezeLimitTrippedFaultShouldBe(true);

   WhenFreshFoodFilteredTemperatureInDegFx100Is(FreshFoodNoFreezeLimitGridValue + 1);
   FreshFoodNoFreezeLimitTrippedFaultShouldBe(true);
}
