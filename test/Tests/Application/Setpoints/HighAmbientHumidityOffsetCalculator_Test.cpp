/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "HighAmbientHumidityOffsetCalculator.h"
#include "PersonalityParametricData.h"
#include "SystemErds.h"
#include "Constants_Binary.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

enum
{
   LessThanFreshFoodUserSetpointTrigger = 19,
   GreaterThanFreshFoodUserSetpointTrigger = 21,
};

enum
{
   LessThanFreshFoodRelativeHumidityTrigger = 2000,
   GreaterThanFreshFoodRelativeHumidityTrigger = 4000,
};

static const HighAmbientCalculatorConfig_t freshFoodConfig = {
   .setpointStatusErd = Erd_FreshFoodSetpointStatus,
   .highAmbientOffsetErd = Erd_FreshFood_HighAmbientOffsetInDegFx100,
   .ambientFilteredHumidityResolvedPercentx100Erd = Erd_Ambient_FilteredHumidityResolvedPercentx100,
   .ambientHumiditySensorIsValidResolvedErd = Erd_AmbientHumidity_IsValidResolved
};

TEST_GROUP(HighAmbientOffsetCalculator)
{
   I_DataModel_t *dataModel;
   const HighAmbientOffsetData_t *freshFoodHighAmbientOffsetData;

   HighAmbientCalculator_t instance;
   ReferDataModel_TestDouble_t dataModelDouble;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;

      freshFoodHighAmbientOffsetData =
         PersonalityParametricData_Get(dataModel)->setpointData->adjustedSetpointData->freshFoodAdjustedSetpointData->highAmbientOffsetData;
   }

   void GivenTheFreshFoodModuleIsInitialized()
   {
      HighAmbientHumidityOffsetCalculator_Init(
         &instance,
         dataModel,
         freshFoodHighAmbientOffsetData,
         &freshFoodConfig);
   }

   void FreshFoodHighAmbientOffsetShouldBe(TemperatureDegFx100_t expected)
   {
      TemperatureDegFx100_t actual;
      DataModel_Read(
         dataModel,
         Erd_FreshFood_HighAmbientOffsetInDegFx100,
         &actual);

      CHECK_EQUAL(expected, actual);
   }

   void WhenTheFreshFoodSetpointStatusBecomes(Setpoint_t setpoint)
   {
      DataModel_Write(
         dataModel,
         Erd_FreshFoodSetpointStatus,
         &setpoint);
   }

   void WhenTheAmbientFilteredHumidityBecomes(RelativeHumidityPercentx100_t relativeHumidity)
   {
      DataModel_Write(
         dataModel,
         Erd_Ambient_FilteredHumidityResolvedPercentx100,
         &relativeHumidity);
   }

   void WhenTheAmbientHumiditySensorIsValid()
   {
      DataModel_Write(
         dataModel,
         Erd_AmbientHumidity_IsValidResolved,
         set);
   }

   void WhenTheAmbientHumiditySensorIsInvalid()
   {
      DataModel_Write(
         dataModel,
         Erd_AmbientHumidity_IsValidResolved,
         clear);
   }

   void GivenTheHighAmbientOffsetIsSet()
   {
      DataModel_Write(
         dataModel,
         Erd_FreshFood_HighAmbientOffsetInDegFx100,
         &freshFoodHighAmbientOffsetData->offsetForHighAmbientHumidityInDegFx100);
   }
};

TEST(HighAmbientOffsetCalculator, ShouldSetHighAmbientOffsetWhenSetpointStatusBecomesLessThanUserSetpointTriggerAndAmbientFilteredHumidityBecomesGreaterThanRelativeHumidityTrigger)
{
   GivenTheFreshFoodModuleIsInitialized();

   WhenTheAmbientHumiditySensorIsValid();
   WhenTheFreshFoodSetpointStatusBecomes(LessThanFreshFoodUserSetpointTrigger);
   WhenTheAmbientFilteredHumidityBecomes(GreaterThanFreshFoodRelativeHumidityTrigger);
   FreshFoodHighAmbientOffsetShouldBe(freshFoodHighAmbientOffsetData->offsetForHighAmbientHumidityInDegFx100);
}

TEST(HighAmbientOffsetCalculator, ShouldBeZeroWhenSetPointStatusBecomesGreaterThanUserSetpoint)
{
   GivenTheHighAmbientOffsetIsSet();
   GivenTheFreshFoodModuleIsInitialized();

   WhenTheAmbientHumiditySensorIsValid();
   WhenTheFreshFoodSetpointStatusBecomes(GreaterThanFreshFoodUserSetpointTrigger);
   WhenTheAmbientFilteredHumidityBecomes(GreaterThanFreshFoodRelativeHumidityTrigger);
   FreshFoodHighAmbientOffsetShouldBe(0);
}

TEST(HighAmbientOffsetCalculator, ShouldBeZeroWhenAmbientFilteredHumidityBecomesLessThanRelativeHumidityTrigger)
{
   GivenTheHighAmbientOffsetIsSet();
   GivenTheFreshFoodModuleIsInitialized();

   WhenTheAmbientHumiditySensorIsValid();
   WhenTheFreshFoodSetpointStatusBecomes(LessThanFreshFoodUserSetpointTrigger);
   WhenTheAmbientFilteredHumidityBecomes(LessThanFreshFoodRelativeHumidityTrigger);
   FreshFoodHighAmbientOffsetShouldBe(0);
}

TEST(HighAmbientOffsetCalculator, ShouldBeZeroWhenAmbientHumiditySensorIsInvalid)
{
   GivenTheHighAmbientOffsetIsSet();
   GivenTheFreshFoodModuleIsInitialized();

   WhenTheAmbientHumiditySensorIsInvalid();
   WhenTheFreshFoodSetpointStatusBecomes(LessThanFreshFoodUserSetpointTrigger);
   WhenTheAmbientFilteredHumidityBecomes(GreaterThanFreshFoodRelativeHumidityTrigger);
   FreshFoodHighAmbientOffsetShouldBe(0);
}
