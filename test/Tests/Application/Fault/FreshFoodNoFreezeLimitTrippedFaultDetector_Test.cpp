/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "FreshFoodNoFreezeLimitTrippedFaultDetector.h"
#include "SystemErds.h"
}

#include "CppUTest/TestHarness.h"
#include "ReferDataModel_TestDouble.h"

enum
{
   FreshFoodNoFreezeLimitGridValue = 500
};

static const FreshFoodNoFreezeLimitTrippedFaultDetectorConfig_t config = {
   .calculateGridLineErd = Erd_Grid_CalculatedGridLines,
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
      CalculatedGridLines_t calculatedGridLines;
      DataModel_Read(dataModel, Erd_Grid_CalculatedGridLines, &calculatedGridLines);
      calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_Nfl] = temperature;
      DataModel_Write(dataModel, Erd_Grid_CalculatedGridLines, &calculatedGridLines);
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

TEST(FreshFoodNoFreezeLimitTrippedFaultDetector, ShouldSetFreshFoodNoFreezeLimitTrippedFaultWhenFreshFoodTemperatureIsEqualToTheNoFreezeLimitOnInit)
{
   GivenFreshFoodFilteredTemperatureInDegFx100Is(FreshFoodNoFreezeLimitGridValue);
   GivenFreshFoodNoFreezeLimitIs(FreshFoodNoFreezeLimitGridValue);

   WhenTheModuleIsInitialized();
   FreshFoodNoFreezeLimitTrippedFaultShouldBe(true);
}

TEST(FreshFoodNoFreezeLimitTrippedFaultDetector, ShouldClearFreshFoodNoFreezeLimitTrippedFaultWhenFreshFoodTemperatureIsGreaterThanNoFreezeLimitOnInit)
{
   GivenFreshFoodFilteredTemperatureInDegFx100Is(FreshFoodNoFreezeLimitGridValue + 1);
   GivenFreshFoodNoFreezeLimitIs(FreshFoodNoFreezeLimitGridValue);
   GivenFreshFoodNoFreezeLimitTrippedFaultIs(true);

   WhenTheModuleIsInitialized();
   FreshFoodNoFreezeLimitTrippedFaultShouldBe(false);
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

TEST(FreshFoodNoFreezeLimitTrippedFaultDetector, ShouldSetFreshFoodNoFreezeLimitTrippedFaultWhenFreshFoodTemperatureChangesToEqualToTheNoFreezeLimit)
{
   GivenFreshFoodFilteredTemperatureInDegFx100Is(FreshFoodNoFreezeLimitGridValue + 1);
   GivenFreshFoodNoFreezeLimitIs(FreshFoodNoFreezeLimitGridValue);
   GivenTheModuleIsInitialized();
   FreshFoodNoFreezeLimitTrippedFaultShouldBe(false);

   WhenFreshFoodFilteredTemperatureInDegFx100Is(FreshFoodNoFreezeLimitGridValue);
   FreshFoodNoFreezeLimitTrippedFaultShouldBe(true);
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
