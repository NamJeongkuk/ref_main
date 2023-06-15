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
#include "HeaterControlType.h"

enum
{
   NewTemperatureInDegFx100 = 7200,
   NewHumidityPercentx100 = 5000,
   NewFreshFoodSetpointTemperatureInDegFx100 = 3900,
   NewFreezerSetpointTemperatureInDegFx100 = 0,
   Invalid = false,
   Valid = true,
};

static const VariableSweatHeaterDutyCycleCalculatorConfig_t variableSweatHeaterDutyCycleCalculatorConfiguration = {
   .ambientFilteredTemperatureResolvedInDegFx100Erd = Erd_Ambient_FilteredTemperatureResolvedInDegFx100,
   .ambientFilteredHumidityResolvedPercentx100Erd = Erd_Ambient_FilteredHumidityResolvedPercentx100,
   .freshFoodSetpointResolvedVoteErd = Erd_FreshFoodSetpoint_ResolvedVote,
   .freezerSetpointResolvedVoteErd = Erd_FreezerSetpoint_ResolvedVote,
   .recessHeaterVariableAntiSweatVoteErd = Erd_RecessHeater_VariableAntiSweatVote,
   .ambientTemperatureIsValidResolvedErd = Erd_AmbientTemperature_IsValidResolved,
   .ambientHumidityIsValidResolvedErd = Erd_AmbientHumidity_IsValidResolved
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

   void WhenHeaterControlTypeIs(uint8_t heaterControlType)
   {
      variableSweatHeaterData.heaterControlType = heaterControlType;
      VariableSweatHeaterDutyCycleCalculator_Init(
         &instance,
         dataModel,
         &variableSweatHeaterData,
         &variableSweatHeaterDutyCycleCalculatorConfiguration);
   }

   void GivenHeaterControlTypeIs(HeaterControlType_t heaterControlType)
   {
      WhenHeaterControlTypeIs(heaterControlType);
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

   void WhenAmbientTemperatureSensorIsValidChangesTo(bool state)
   {
      DataModel_Write(
         dataModel,
         Erd_AmbientTemperature_IsValidResolved,
         &state);
   }

   void WhenAmbientHumiditySensorIsValidChangesTo(bool state)
   {
      DataModel_Write(
         dataModel,
         Erd_AmbientHumidity_IsValidResolved,
         &state);
   }

   void GivenTheAmbientTemperatureSensorIsValidIs(bool state)
   {
      WhenAmbientTemperatureSensorIsValidChangesTo(state);
   }

   void GivenTheAmbientHumiditySensorIsValidIs(bool state)
   {
      WhenAmbientHumiditySensorIsValidChangesTo(state);
   }

   void GivenBothAmbientTemperatureAndAmbientHumidityAreValid()
   {
      GivenTheAmbientTemperatureSensorIsValidIs(Valid);
      GivenTheAmbientHumiditySensorIsValidIs(Valid);
   }
};

TEST(VariableSweatHeaterDutyCycleCalculator, ShouldCalculateTheDutyCycleAndCorrectTheValueWithClampOnInitialization)
{
   GivenAmbientTemperatureIs(0);
   GivenAmbientHumidityIs(0);
   GivenFreshFoodSetpointTemperatureIs(0);
   GivenFreezerSetpointTemperatureIs(0);
   GivenBothAmbientTemperatureAndAmbientHumidityAreValid();
   GivenTheModuleIsInitialized();

   TheDutyCycleInPercentShouldBe(0);
}

TEST(VariableSweatHeaterDutyCycleCalculator, ShouldCalculateTheDutyCycleWhenAmbientTemperatureChanges)
{
   GivenBothAmbientTemperatureAndAmbientHumidityAreValid();
   GivenTheModuleIsInitialized();

   WhenAmbientTemperatureIs(NewTemperatureInDegFx100);
   TheDutyCycleInPercentShouldBe(8);
}

TEST(VariableSweatHeaterDutyCycleCalculator, ShouldCalculateTheDutyCycleAndCorrectTheValueWithClampWhenAmbientHumidityChanges)
{
   GivenBothAmbientTemperatureAndAmbientHumidityAreValid();
   GivenTheModuleIsInitialized();

   WhenAmbientHumidityIs(NewHumidityPercentx100);
   TheDutyCycleInPercentShouldBe(100);
}

TEST(VariableSweatHeaterDutyCycleCalculator, ShouldCalculateTheDutyCycleAndCorrectTheValueWithClampWhenFreshFoodSetpointTemperatureChanges)
{
   GivenBothAmbientTemperatureAndAmbientHumidityAreValid();
   GivenTheModuleIsInitialized();

   WhenFreshFoodSetpointTemperatureIs(NewFreshFoodSetpointTemperatureInDegFx100);
   TheDutyCycleInPercentShouldBe(0);
}

TEST(VariableSweatHeaterDutyCycleCalculator, ShouldCalculateTheDutyCycleAndCorrectTheValueWithClampWhenFreezerSetpointTemperatureChanges)
{
   GivenBothAmbientTemperatureAndAmbientHumidityAreValid();
   GivenTheModuleIsInitialized();
   GivenAmbientTemperatureIs(NewTemperatureInDegFx100);
   GivenAmbientHumidityIs(NewHumidityPercentx100);

   WhenFreezerSetpointTemperatureIs(NewFreezerSetpointTemperatureInDegFx100);
   TheDutyCycleInPercentShouldBe(100);
}

TEST(VariableSweatHeaterDutyCycleCalculator, ShouldRecalculateTheDutyCycleAndCorrectTheValueWithClampWheneverTemperatureOrHumidityChanges)
{
   GivenBothAmbientTemperatureAndAmbientHumidityAreValid();
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

TEST(VariableSweatHeaterDutyCycleCalculator, ShouldUseVariableAntiSweatHeaterDutyCycleInPercentWhenAmbientTemperatureBecomesNotValid)
{
   GivenBothAmbientTemperatureAndAmbientHumidityAreValid();
   GivenTheModuleIsInitialized();

   WhenAmbientTemperatureSensorIsValidChangesTo(Invalid);
   TheDutyCycleInPercentShouldBe(variableSweatHeaterData.variableHeaterFallbackDutyCycleInPercent);
}

TEST(VariableSweatHeaterDutyCycleCalculator, ShouldUseVariableAntiSweatHeaterDutyCycleInPercentWhenAmbientHumidityBecomesNotValid)
{
   GivenBothAmbientTemperatureAndAmbientHumidityAreValid();
   GivenTheModuleIsInitialized();

   WhenAmbientHumiditySensorIsValidChangesTo(Invalid);
   TheDutyCycleInPercentShouldBe(variableSweatHeaterData.variableHeaterFallbackDutyCycleInPercent);
}

TEST(VariableSweatHeaterDutyCycleCalculator, ShouldUseVariableAntiSweatHeaterDutyCycleInPercentWhenBothAmbientTemperatureAndAmbientHumidityBecomeNotValid)
{
   GivenBothAmbientTemperatureAndAmbientHumidityAreValid();
   GivenTheModuleIsInitialized();

   WhenAmbientTemperatureSensorIsValidChangesTo(Invalid);
   TheDutyCycleInPercentShouldBe(variableSweatHeaterData.variableHeaterFallbackDutyCycleInPercent);

   WhenAmbientHumiditySensorIsValidChangesTo(Invalid);
   TheDutyCycleInPercentShouldBe(variableSweatHeaterData.variableHeaterFallbackDutyCycleInPercent);
}

TEST(VariableSweatHeaterDutyCycleCalculator, ShouldSwitchFromVariableAntiSweatHeaterDutyCycleInPercentToCalculatedDutyCycleOnceBothSensorsBecomeValid)
{
   GivenBothAmbientTemperatureAndAmbientHumidityAreValid();
   GivenTheModuleIsInitialized();
   GivenAmbientTemperatureIs(NewTemperatureInDegFx100);
   TheDutyCycleInPercentShouldBe(8);

   WhenAmbientTemperatureSensorIsValidChangesTo(Invalid);
   TheDutyCycleInPercentShouldBe(variableSweatHeaterData.variableHeaterFallbackDutyCycleInPercent);

   WhenAmbientHumiditySensorIsValidChangesTo(Invalid);
   TheDutyCycleInPercentShouldBe(variableSweatHeaterData.variableHeaterFallbackDutyCycleInPercent);

   WhenAmbientTemperatureSensorIsValidChangesTo(Valid);
   TheDutyCycleInPercentShouldBe(variableSweatHeaterData.variableHeaterFallbackDutyCycleInPercent);

   WhenAmbientHumiditySensorIsValidChangesTo(Valid);
   TheDutyCycleInPercentShouldBe(8);
}

TEST(VariableSweatHeaterDutyCycleCalculator, ShouldCalculateDutyCycleWhenHeaterControlTypeDoesNotDependOnAmbientHumidityAndAmbientTemperatureIsValid)
{
   GivenBothAmbientTemperatureAndAmbientHumidityAreValid();
   GivenTheModuleIsInitialized();
   GivenHeaterControlTypeIs(HeaterControlType_1);
   GivenAmbientTemperatureIs(NewTemperatureInDegFx100);
   TheDutyCycleInPercentShouldBe(8);

   WhenAmbientHumiditySensorIsValidChangesTo(Invalid);
   TheDutyCycleInPercentShouldBe(8);

   WhenAmbientTemperatureSensorIsValidChangesTo(Invalid);
   TheDutyCycleInPercentShouldBe(variableSweatHeaterData.variableHeaterFallbackDutyCycleInPercent);
}

TEST(VariableSweatHeaterDutyCycleCalculator, ShouldCalculateDutyCycleWhenHeaterControlTypeSwitchesBetweenTypeDependentOnAmbientHumidityAndTypeNot)
{
   GivenBothAmbientTemperatureAndAmbientHumidityAreValid();
   GivenTheModuleIsInitialized();
   GivenHeaterControlTypeIs(HeaterControlType_7);
   GivenAmbientTemperatureIs(NewTemperatureInDegFx100);
   TheDutyCycleInPercentShouldBe(8);

   WhenAmbientHumiditySensorIsValidChangesTo(Invalid);
   TheDutyCycleInPercentShouldBe(variableSweatHeaterData.variableHeaterFallbackDutyCycleInPercent);

   WhenHeaterControlTypeIs(HeaterControlType_1);
   TheDutyCycleInPercentShouldBe(8);
}
