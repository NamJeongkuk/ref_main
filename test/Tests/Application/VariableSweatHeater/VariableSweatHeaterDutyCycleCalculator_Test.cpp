/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "VariableSweatHeaterDutyCycleCalculator.h"
#include "PersonalityParametricData.h"
#include "SystemErds.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

enum
{
   NewTemperatureInDegFx100 = 7200,
   NewHumidityPercentx100 = 5000,
   NewFreshFoodSetpointTemperatureInDegFx100 = 3900,
   NewFreezerSetpointTemperatureInDegFx100 = 0
};

static const VariableSweatHeaterDutyCycleCalculatorConfig_t variableSweatHeaterDutyCycleCalculatorConfiguration = {
   .ambientFilteredTemperatureResolvedInDegFx100Erd = Erd_Ambient_FilteredTemperatureResolvedInDegFx100,
   .ambientFilteredHumidityResolvedPercentx100Erd = Erd_Ambient_FilteredHumidityResolvedPercentx100,
   .freshFoodSetpointResolvedVoteErd = Erd_FreshFoodSetpoint_ResolvedVote,
   .freezerSetpointResolvedVoteErd = Erd_FreezerSetpoint_ResolvedVote,
   .recessHeaterVariableAntiSweatVoteErd = Erd_RecessHeater_VariableAntiSweatVote
};

TEST_GROUP(VariableSweatHeaterDutyCycleCalculator)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   VariableSweatHeaterDutyCycleCalculator_t instance;
   VariableSweatHeaterData_t variableSweatHeaterData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      variableSweatHeaterData = PersonalityParametricData_Get(dataModel)->recessHeaterData->variableSweatHeaterData;
   }

   void GivenTheModuleIsInitialized()
   {
      VariableSweatHeaterDutyCycleCalculator_Init(
         &instance,
         dataModel,
         &variableSweatHeaterData,
         &variableSweatHeaterDutyCycleCalculatorConfiguration);
   }

   void WhenAmbientTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(
         dataModel,
         Erd_Ambient_FilteredTemperatureResolvedInDegFx100,
         &temperature);
   }

   void GivenAmbientTemperatureIs(TemperatureDegFx100_t temperature)
   {
      WhenAmbientTemperatureIs(temperature);
   }

   void WhenAmbientHumidityIs(RelativeHumidityPercentx100_t humidity)
   {
      DataModel_Write(
         dataModel,
         Erd_Ambient_FilteredHumidityResolvedPercentx100,
         &humidity);
   }

   void GivenAmbientHumidityIs(RelativeHumidityPercentx100_t humidity)
   {
      WhenAmbientHumidityIs(humidity);
   }

   void WhenFreshFoodSetpointTemperatureIs(TemperatureDegFx100_t temperature)
   {
      SetpointVotedTemperature_t setpointTemperature = {
         .temperatureInDegFx100 = temperature,
         .care = Vote_Care
      };

      DataModel_Write(
         dataModel,
         Erd_FreshFoodSetpoint_ResolvedVote,
         &setpointTemperature);
   }

   void GivenFreshFoodSetpointTemperatureIs(TemperatureDegFx100_t temperature)
   {
      WhenFreshFoodSetpointTemperatureIs(temperature);
   }

   void WhenFreezerSetpointTemperatureIs(TemperatureDegFx100_t temperature)
   {
      SetpointVotedTemperature_t setpointTemperature = {
         .temperatureInDegFx100 = temperature,
         .care = Vote_Care
      };

      DataModel_Write(
         dataModel,
         Erd_FreezerSetpoint_ResolvedVote,
         &setpointTemperature);
   }

   void GivenFreezerSetpointTemperatureIs(TemperatureDegFx100_t temperature)
   {
      WhenFreezerSetpointTemperatureIs(temperature);
   }

   void TheDutyCycleInPercentShouldBe(PercentageDutyCycle_t expectedPercentageDutyCycle)
   {
      PercentageDutyCycleVote_t actual;
      DataModel_Read(
         dataModel,
         Erd_RecessHeater_VariableAntiSweatVote,
         &actual);

      CHECK_EQUAL(expectedPercentageDutyCycle, actual.percentageDutyCycle);
   }
};

TEST(VariableSweatHeaterDutyCycleCalculator, ShouldCalculateTheDutyCycleAndCorrectTheValueWithClampOnInitialization)
{
   GivenAmbientTemperatureIs(0);
   GivenAmbientHumidityIs(0);
   GivenFreshFoodSetpointTemperatureIs(0);
   GivenFreezerSetpointTemperatureIs(0);
   GivenTheModuleIsInitialized();

   TheDutyCycleInPercentShouldBe(0);
}

TEST(VariableSweatHeaterDutyCycleCalculator, ShouldCalculateTheDutyCycleWhenAmbientTemperatureChanges)
{
   GivenTheModuleIsInitialized();

   WhenAmbientTemperatureIs(NewTemperatureInDegFx100);
   TheDutyCycleInPercentShouldBe(8);
}

TEST(VariableSweatHeaterDutyCycleCalculator, ShouldCalculateTheDutyCycleAndCorrectTheValueWithClampWhenAmbientHumidityChanges)
{
   GivenTheModuleIsInitialized();

   WhenAmbientHumidityIs(NewHumidityPercentx100);
   TheDutyCycleInPercentShouldBe(100);
}

TEST(VariableSweatHeaterDutyCycleCalculator, ShouldCalculateTheDutyCycleAndCorrectTheValueWithClampWhenFreshFoodSetpointTemperatureChanges)
{
   GivenTheModuleIsInitialized();

   WhenFreshFoodSetpointTemperatureIs(NewFreshFoodSetpointTemperatureInDegFx100);
   TheDutyCycleInPercentShouldBe(0);
}

TEST(VariableSweatHeaterDutyCycleCalculator, ShouldCalculateTheDutyCycleAndCorrectTheValueWithClampWhenFreezerSetpointTemperatureChanges)
{
   GivenTheModuleIsInitialized();
   GivenAmbientTemperatureIs(NewTemperatureInDegFx100);
   GivenAmbientHumidityIs(NewHumidityPercentx100);

   WhenFreezerSetpointTemperatureIs(NewFreezerSetpointTemperatureInDegFx100);
   TheDutyCycleInPercentShouldBe(100);
}

TEST(VariableSweatHeaterDutyCycleCalculator, ShouldRecalculateTheDutyCycleAndCorrectTheValueWithClampWheneverTemperatureOrHumidityChanges)
{
   GivenTheModuleIsInitialized();
   GivenAmbientTemperatureIs(NewTemperatureInDegFx100);
   TheDutyCycleInPercentShouldBe(8);

   WhenAmbientHumidityIs(NewHumidityPercentx100);
   TheDutyCycleInPercentShouldBe(100);

   WhenFreshFoodSetpointTemperatureIs(NewFreshFoodSetpointTemperatureInDegFx100);
   TheDutyCycleInPercentShouldBe(30);

   WhenFreezerSetpointTemperatureIs(NewFreezerSetpointTemperatureInDegFx100);
   TheDutyCycleInPercentShouldBe(30);
}
