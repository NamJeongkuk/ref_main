/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "CrossAmbientOffsetCalculator.h"
#include "PersonalityParametricData.h"
#include "SystemErds.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

#define Given
#define When
#define And

enum
{
   TemperatureLessThanMinAmbientOffsetInDegFx100 = 5000,
   TemperatureExactlyOnMinAmbientOffsetInDegFx100 = 6000,
   TemperatureBetweenMinandMaxAmbientOffsetInDegFx100 = 7000,
   AnotherTemperatureBetweenMinandMaxAmbientOffsetInDegFx100 = 8000,
   TemperatureExactlyOnMaxAmbientOffsetInDegFx100 = 10000,
   TemperatureGreaterThanMaxAmbientOffsetInDegFx100 = 11000
};

static const CrossAmbientOffsetCalculatorConfig_t config = {
   .ambientWindowAveragedTemperatureInDegFx100Erd = Erd_Ambient_WindowAveragedTemperatureInDegFx100,
   .crossAmbientOffsetInDegFx100Erd = Erd_FreshFood_CrossAmbientOffsetInDegFx100,
};

TEST_GROUP(CrossAmbientOffsetCalculator)
{
   I_DataModel_t *dataModel;
   const CrossAmbientOffsetData_t *crossAmbientOffsetData;

   CrossAmbientOffsetCalculator_t instance;
   ReferDataModel_TestDouble_t dataModelDouble;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;

      crossAmbientOffsetData =
         PersonalityParametricData_Get(dataModel)->freshFoodThermalOffsetData->crossAmbientOffsetData;
   }

   void TheModuleIsInitialized()
   {
      CrossAmbientOffsetCalculator_Init(
         &instance,
         dataModel,
         crossAmbientOffsetData,
         &config);
   }

   void CrossAmbientOffsetInDegFx100ShouldBe(TemperatureDegFx100_t expected)
   {
      TemperatureDegFx100_t actual;
      DataModel_Read(
         dataModel,
         Erd_FreshFood_CrossAmbientOffsetInDegFx100,
         &actual);

      CHECK_EQUAL(expected, actual);
   }

   void AverageTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(
         dataModel,
         Erd_Ambient_WindowAveragedTemperatureInDegFx100,
         &temperature);
   }
};

TEST(CrossAmbientOffsetCalculator, ShouldCalculateCrossAmbientOffsetOnErdChange)
{
   Given TheModuleIsInitialized();
   When AverageTemperatureIs(TemperatureBetweenMinandMaxAmbientOffsetInDegFx100);
   CrossAmbientOffsetInDegFx100ShouldBe(
      (crossAmbientOffsetData->slopeCoefficient * (TemperatureBetweenMinandMaxAmbientOffsetInDegFx100 - 9000)) / 1000);

   When AverageTemperatureIs(AnotherTemperatureBetweenMinandMaxAmbientOffsetInDegFx100);
   CrossAmbientOffsetInDegFx100ShouldBe(
      (crossAmbientOffsetData->slopeCoefficient * (AnotherTemperatureBetweenMinandMaxAmbientOffsetInDegFx100 - 9000)) / 1000);
}

TEST(CrossAmbientOffsetCalculator, ShouldDefaultToMinValueWhenCrossAmbientOffsetIsLessThanMin)
{
   Given TheModuleIsInitialized();
   Given AverageTemperatureIs(TemperatureLessThanMinAmbientOffsetInDegFx100);
   CrossAmbientOffsetInDegFx100ShouldBe(crossAmbientOffsetData->minimumAmbientOffsetInDegFx100);
}

TEST(CrossAmbientOffsetCalculator, ShouldDefaultToMaxValueWhenCrossAmbientOffsetIsGreaterThanMax)
{
   Given TheModuleIsInitialized();
   Given AverageTemperatureIs(TemperatureGreaterThanMaxAmbientOffsetInDegFx100);
   CrossAmbientOffsetInDegFx100ShouldBe(crossAmbientOffsetData->maximumAmbientOffsetInDegFx100);
}

TEST(CrossAmbientOffsetCalculator, ShouldDefaultToMinValueWhenCrossAmbientOffsetIsExactlyMin)
{
   Given TheModuleIsInitialized();
   Given AverageTemperatureIs(TemperatureExactlyOnMinAmbientOffsetInDegFx100);
   CrossAmbientOffsetInDegFx100ShouldBe(crossAmbientOffsetData->minimumAmbientOffsetInDegFx100);
}

TEST(CrossAmbientOffsetCalculator, ShouldDefaultToMaxValueWhenCrossAmbientOffsetIsExactlyMax)
{
   Given TheModuleIsInitialized();
   Given AverageTemperatureIs(TemperatureExactlyOnMaxAmbientOffsetInDegFx100);
   CrossAmbientOffsetInDegFx100ShouldBe(crossAmbientOffsetData->maximumAmbientOffsetInDegFx100);
}
