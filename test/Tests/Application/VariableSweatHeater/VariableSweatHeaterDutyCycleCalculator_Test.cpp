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
   SomeTemperatureInDegFx100 = 7200,
   SomeHumidityPercentx100 = 5000,
   SomeFreshFoodSetpointTemperatureInDegFx100 = 3900,
   SomeFreezerSetpointTemperatureInDegFx100 = 0,
   AnotherTemperatureInDegFx100 = 8000,
   AnotherHumidityPercentx100 = 8500,
   AnotherFreshFoodSetpointTemperatureInDegFx100 = 3700,
   AnotherFreezerSetpointTemperatureInDegFx100 = SomeFreezerSetpointTemperatureInDegFx100,
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

static const VariableSweatHeaterDataType1_t heaterType1 = {
   .temperatureCoefficient = 2100,
   .temperatureSquaredCoefficient = 12,
   .interceptCoefficient = -205000
};

static const VariableSweatHeaterData_t variableSweatHeaterDataType1 = {
   .heaterControlType = 1,
   .variableHeaterFallbackDutyCycleInPercent = 100,
   .heaterType1Data = &heaterType1
};

static const VariableSweatHeaterDataType2_t heaterType2 = {
   .temperatureCoefficient = 2100,
   .humidityCoefficient = 1800,
   .temperatureSquaredCoefficient = 12,
   .humiditySquaredCoefficient = -20,
   .temperatureHumidityCoefficient = -13,
   .interceptCoefficient = -225000
};

static const VariableSweatHeaterData_t variableSweatHeaterDataType2 = {
   .heaterControlType = 2,
   .variableHeaterFallbackDutyCycleInPercent = 100,
   .heaterType2Data = &heaterType2
};

static const VariableSweatHeaterDataType3_t heaterType3 = {
   .temperatureCoefficient = 2100,
   .freshFoodCoefficient = -6900,
   .temperatureSquaredCoefficient = 12,
   .freshFoodSquaredCoefficient = 157,
   .temperatureFreshFoodCoefficient = -38,
   .interceptCoefficient = 2000
};

static const VariableSweatHeaterData_t variableSweatHeaterDataType3 = {
   .heaterControlType = 3,
   .variableHeaterFallbackDutyCycleInPercent = 100,
   .heaterType3Data = &heaterType3
};

static const VariableSweatHeaterDataType4_t heaterType4 = {
   .temperatureCoefficient = 2100,
   .freezerCoefficient = 1000,
   .temperatureSquaredCoefficient = 12,
   .freezerSquaredCoefficient = 15,
   .temperatureFreezerCoefficient = -25,
   .interceptCoefficient = -205000
};

static const VariableSweatHeaterData_t variableSweatHeaterDataType4 = {
   .heaterControlType = 4,
   .variableHeaterFallbackDutyCycleInPercent = 100,
   .heaterType4Data = &heaterType4
};

static const VariableSweatHeaterDataType5_t heaterType5 = {
   .temperatureCoefficient = 2100,
   .humidityCoefficient = 7100,
   .freshFoodCoefficient = -6900,
   .temperatureSquaredCoefficient = 12,
   .humiditySquaredCoefficient = -20,
   .freshFoodSquaredCoefficient = 157,
   .temperatureHumidityCoefficient = -13,
   .temperatureFreshFoodCoefficient = -38,
   .humidityFreshFoodCoefficient = -51,
   .interceptCoefficient = -205000
};

static const VariableSweatHeaterData_t variableSweatHeaterDataType5 = {
   .heaterControlType = 5,
   .variableHeaterFallbackDutyCycleInPercent = 100,
   .heaterType5Data = &heaterType5
};

static const VariableSweatHeaterDataType6_t heaterType6 = {
   .temperatureCoefficient = 2100,
   .humidityCoefficient = 6500,
   .freezerCoefficient = -6900,
   .temperatureSquaredCoefficient = 8,
   .humiditySquaredCoefficient = -20,
   .freezerSquaredCoefficient = -5,
   .temperatureHumidityCoefficient = -11,
   .temperatureFreezerCoefficient = -10,
   .humidityFreezerCoefficient = -42,
   .interceptCoefficient = -460000
};

static const VariableSweatHeaterData_t variableSweatHeaterDataType6 = {
   .heaterControlType = 6,
   .variableHeaterFallbackDutyCycleInPercent = 100,
   .heaterType6Data = &heaterType6
};

static const VariableSweatHeaterDataType7_t heaterType7 = {
   .temperatureCoefficient = 2100,
   .humidityCoefficient = 7100,
   .freshFoodCoefficient = -6900,
   .freezerCoefficient = -1200,
   .temperatureSquaredCoefficient = 12,
   .humiditySquaredCoefficient = -20,
   .freshFoodSquaredCoefficient = 157,
   .freezerSquaredCoefficient = 121,
   .temperatureHumidityCoefficient = -13,
   .temperatureFreshFoodCoefficient = -38,
   .temperatureFreezerCoefficient = -40,
   .humidityFreshFoodCoefficient = -37,
   .humidityFreezerCoefficient = -42,
   .freshFoodFreezerCoefficient = -38,
   .interceptCoefficient = -205000
};

static const VariableSweatHeaterData_t variableSweatHeaterDataType7 = {
   .heaterControlType = 7,
   .variableHeaterFallbackDutyCycleInPercent = 100,
   .heaterType7Data = &heaterType7
};

TEST_GROUP(VariableSweatHeaterDutyCycleCalculator)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   VariableSweatHeaterDutyCycleCalculator_t instance;
   const VariableSweatHeaterData_t *variableSweatHeaterData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      variableSweatHeaterData = PersonalityParametricData_Get(dataModel)->recessHeaterData->variableSweatHeaterData;
   }

   void GivenTheModuleIsInitializedWithHeaterType(HeaterControlType_t type)
   {
      switch(type)
      {
         case HeaterControlType_1:
            VariableSweatHeaterDutyCycleCalculator_Init(
               &instance,
               dataModel,
               &variableSweatHeaterDataType1,
               &variableSweatHeaterDutyCycleCalculatorConfiguration);
            break;

         case HeaterControlType_2:
            VariableSweatHeaterDutyCycleCalculator_Init(
               &instance,
               dataModel,
               &variableSweatHeaterDataType2,
               &variableSweatHeaterDutyCycleCalculatorConfiguration);
            break;

         case HeaterControlType_3:
            VariableSweatHeaterDutyCycleCalculator_Init(
               &instance,
               dataModel,
               &variableSweatHeaterDataType3,
               &variableSweatHeaterDutyCycleCalculatorConfiguration);
            break;

         case HeaterControlType_4:
            VariableSweatHeaterDutyCycleCalculator_Init(
               &instance,
               dataModel,
               &variableSweatHeaterDataType4,
               &variableSweatHeaterDutyCycleCalculatorConfiguration);
            break;

         case HeaterControlType_5:
            VariableSweatHeaterDutyCycleCalculator_Init(
               &instance,
               dataModel,
               &variableSweatHeaterDataType5,
               &variableSweatHeaterDutyCycleCalculatorConfiguration);
            break;

         case HeaterControlType_6:
            VariableSweatHeaterDutyCycleCalculator_Init(
               &instance,
               dataModel,
               &variableSweatHeaterDataType6,
               &variableSweatHeaterDutyCycleCalculatorConfiguration);
            break;

         case HeaterControlType_7:
            VariableSweatHeaterDutyCycleCalculator_Init(
               &instance,
               dataModel,
               variableSweatHeaterData,
               &variableSweatHeaterDutyCycleCalculatorConfiguration);
            break;
      }
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

   void WhenAmbientThermistorIsValidChangesTo(bool state)
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

   void GivenTheAmbientThermistorIsValidIs(bool state)
   {
      WhenAmbientThermistorIsValidChangesTo(state);
   }

   void GivenTheAmbientHumiditySensorIsValidIs(bool state)
   {
      WhenAmbientHumiditySensorIsValidChangesTo(state);
   }

   void GivenBothAmbientTemperatureAndAmbientHumidityAreValid()
   {
      GivenTheAmbientThermistorIsValidIs(Valid);
      GivenTheAmbientHumiditySensorIsValidIs(Valid);
   }

   void GivenModuleIsInitializedWithBothAmbientThermistorAndAmbientHumidityAreValidAndHeaterIsType(HeaterControlType_t type)
   {
      GivenBothAmbientTemperatureAndAmbientHumidityAreValid();
      GivenTheModuleIsInitializedWithHeaterType(type);
   }
};

TEST(VariableSweatHeaterDutyCycleCalculator, ShouldCalculateTheDutyCycleAndCorrectTheValueWithClampOnInitialization)
{
   GivenAmbientTemperatureIs(0);
   GivenAmbientHumidityIs(0);
   GivenFreshFoodSetpointTemperatureIs(0);
   GivenFreezerSetpointTemperatureIs(0);
   GivenModuleIsInitializedWithBothAmbientThermistorAndAmbientHumidityAreValidAndHeaterIsType(HeaterControlType_7);

   TheDutyCycleInPercentShouldBe(0);
}

TEST(VariableSweatHeaterDutyCycleCalculator, ShouldCalculateTheDutyCycleWhenAmbientTemperatureChanges)
{
   GivenModuleIsInitializedWithBothAmbientThermistorAndAmbientHumidityAreValidAndHeaterIsType(HeaterControlType_7);

   WhenAmbientTemperatureIs(SomeTemperatureInDegFx100);
   TheDutyCycleInPercentShouldBe(8);
}

TEST(VariableSweatHeaterDutyCycleCalculator, ShouldCalculateTheDutyCycleAndCorrectTheValueWithClampWhenAmbientHumidityChanges)
{
   GivenModuleIsInitializedWithBothAmbientThermistorAndAmbientHumidityAreValidAndHeaterIsType(HeaterControlType_7);

   WhenAmbientHumidityIs(SomeHumidityPercentx100);
   TheDutyCycleInPercentShouldBe(100);
}

TEST(VariableSweatHeaterDutyCycleCalculator, ShouldCalculateTheDutyCycleAndCorrectTheValueWithClampWhenFreshFoodSetpointTemperatureChanges)
{
   GivenModuleIsInitializedWithBothAmbientThermistorAndAmbientHumidityAreValidAndHeaterIsType(HeaterControlType_7);

   WhenFreshFoodSetpointTemperatureIs(SomeFreshFoodSetpointTemperatureInDegFx100);
   TheDutyCycleInPercentShouldBe(0);
}

TEST(VariableSweatHeaterDutyCycleCalculator, ShouldCalculateTheDutyCycleAndCorrectTheValueWithClampWhenFreezerSetpointTemperatureChanges)
{
   GivenAmbientTemperatureIs(SomeTemperatureInDegFx100);
   GivenAmbientHumidityIs(SomeHumidityPercentx100);
   GivenModuleIsInitializedWithBothAmbientThermistorAndAmbientHumidityAreValidAndHeaterIsType(HeaterControlType_7);

   WhenFreezerSetpointTemperatureIs(SomeFreezerSetpointTemperatureInDegFx100);
   TheDutyCycleInPercentShouldBe(100);
}

TEST(VariableSweatHeaterDutyCycleCalculator, ShouldRecalculateTheDutyCycleAndCorrectTheValueWithClampWheneverTemperatureOrHumidityChanges)
{
   GivenAmbientTemperatureIs(SomeTemperatureInDegFx100);
   GivenModuleIsInitializedWithBothAmbientThermistorAndAmbientHumidityAreValidAndHeaterIsType(HeaterControlType_7);
   TheDutyCycleInPercentShouldBe(8);

   WhenAmbientHumidityIs(SomeHumidityPercentx100);
   TheDutyCycleInPercentShouldBe(100);

   WhenFreshFoodSetpointTemperatureIs(SomeFreshFoodSetpointTemperatureInDegFx100);
   TheDutyCycleInPercentShouldBe(30);

   WhenFreezerSetpointTemperatureIs(SomeFreezerSetpointTemperatureInDegFx100);
   TheDutyCycleInPercentShouldBe(30);
}

TEST(VariableSweatHeaterDutyCycleCalculator, ShouldUseVariableAntiSweatHeaterDutyCycleInPercentWhenAmbientTemperatureBecomesNotValid)
{
   GivenModuleIsInitializedWithBothAmbientThermistorAndAmbientHumidityAreValidAndHeaterIsType(HeaterControlType_7);

   WhenAmbientThermistorIsValidChangesTo(Invalid);
   TheDutyCycleInPercentShouldBe(variableSweatHeaterData->variableHeaterFallbackDutyCycleInPercent);
}

TEST(VariableSweatHeaterDutyCycleCalculator, ShouldUseVariableAntiSweatHeaterDutyCycleInPercentWhenAmbientHumidityBecomesNotValid)
{
   GivenModuleIsInitializedWithBothAmbientThermistorAndAmbientHumidityAreValidAndHeaterIsType(HeaterControlType_7);

   WhenAmbientHumiditySensorIsValidChangesTo(Invalid);
   TheDutyCycleInPercentShouldBe(variableSweatHeaterData->variableHeaterFallbackDutyCycleInPercent);
}

TEST(VariableSweatHeaterDutyCycleCalculator, ShouldSwitchFromCalculatedDutyCycleToFallbackDutyCycleWhenAmbientThermistorIsInvalidAndStayFallbackWhenAmbientHumiditySensorIsInvalidForHeaterControlTypeThatIsDependentOnAmbientHumidity)
{
   GivenModuleIsInitializedWithBothAmbientThermistorAndAmbientHumidityAreValidAndHeaterIsType(HeaterControlType_7);

   WhenAmbientThermistorIsValidChangesTo(Invalid);
   TheDutyCycleInPercentShouldBe(variableSweatHeaterData->variableHeaterFallbackDutyCycleInPercent);

   WhenAmbientHumiditySensorIsValidChangesTo(Invalid);
   TheDutyCycleInPercentShouldBe(variableSweatHeaterData->variableHeaterFallbackDutyCycleInPercent);
}

TEST(VariableSweatHeaterDutyCycleCalculator, ShouldSwitchFromCalculatedDutyCycleToFallbackDutyCycleWhenAmbientHumiditySensorIsInvalidAndStayFallbackWhenAmbientThermistorIsInvalidForHeaterControlTypeThatIsDependentOnAmbientHumidity)
{
   GivenAmbientTemperatureIs(SomeTemperatureInDegFx100);
   GivenModuleIsInitializedWithBothAmbientThermistorAndAmbientHumidityAreValidAndHeaterIsType(HeaterControlType_7);
   TheDutyCycleInPercentShouldBe(8);

   WhenAmbientHumiditySensorIsValidChangesTo(Invalid);
   TheDutyCycleInPercentShouldBe(variableSweatHeaterData->variableHeaterFallbackDutyCycleInPercent);

   WhenAmbientThermistorIsValidChangesTo(Invalid);
   TheDutyCycleInPercentShouldBe(variableSweatHeaterData->variableHeaterFallbackDutyCycleInPercent);
}

TEST(VariableSweatHeaterDutyCycleCalculator, ShouldSwitchFromVariableAntiSweatHeaterDutyCycleInPercentToCalculatedDutyCycleOnceBothSensorsBecomeValid)
{
   GivenAmbientTemperatureIs(SomeTemperatureInDegFx100);
   GivenModuleIsInitializedWithBothAmbientThermistorAndAmbientHumidityAreValidAndHeaterIsType(HeaterControlType_7);
   TheDutyCycleInPercentShouldBe(8);

   WhenAmbientThermistorIsValidChangesTo(Invalid);
   TheDutyCycleInPercentShouldBe(variableSweatHeaterData->variableHeaterFallbackDutyCycleInPercent);

   WhenAmbientHumiditySensorIsValidChangesTo(Invalid);
   TheDutyCycleInPercentShouldBe(variableSweatHeaterData->variableHeaterFallbackDutyCycleInPercent);

   WhenAmbientThermistorIsValidChangesTo(Valid);
   TheDutyCycleInPercentShouldBe(variableSweatHeaterData->variableHeaterFallbackDutyCycleInPercent);

   WhenAmbientHumiditySensorIsValidChangesTo(Valid);
   TheDutyCycleInPercentShouldBe(8);
}

TEST(VariableSweatHeaterDutyCycleCalculator, ShouldCalculateDutyCycleForType1Heater)
{
   GivenAmbientTemperatureIs(AnotherTemperatureInDegFx100);
   GivenAmbientHumidityIs(AnotherHumidityPercentx100);
   GivenFreshFoodSetpointTemperatureIs(AnotherFreshFoodSetpointTemperatureInDegFx100);
   GivenFreezerSetpointTemperatureIs(AnotherFreezerSetpointTemperatureInDegFx100);
   GivenModuleIsInitializedWithBothAmbientThermistorAndAmbientHumidityAreValidAndHeaterIsType(HeaterControlType_1);

   TheDutyCycleInPercentShouldBe(39);
}

TEST(VariableSweatHeaterDutyCycleCalculator, ShouldCalculateDutyCycleForType2Heater)
{
   GivenAmbientTemperatureIs(AnotherTemperatureInDegFx100);
   GivenAmbientHumidityIs(AnotherHumidityPercentx100);
   GivenFreshFoodSetpointTemperatureIs(AnotherFreshFoodSetpointTemperatureInDegFx100);
   GivenFreezerSetpointTemperatureIs(AnotherFreezerSetpointTemperatureInDegFx100);
   GivenModuleIsInitializedWithBothAmbientThermistorAndAmbientHumidityAreValidAndHeaterIsType(HeaterControlType_2);

   TheDutyCycleInPercentShouldBe(84);
}

TEST(VariableSweatHeaterDutyCycleCalculator, ShouldCalculateDutyCycleForType3Heater)
{
   GivenAmbientTemperatureIs(AnotherTemperatureInDegFx100);
   GivenAmbientHumidityIs(AnotherHumidityPercentx100);
   GivenFreshFoodSetpointTemperatureIs(AnotherFreshFoodSetpointTemperatureInDegFx100);
   GivenFreezerSetpointTemperatureIs(AnotherFreezerSetpointTemperatureInDegFx100);
   GivenModuleIsInitializedWithBothAmbientThermistorAndAmbientHumidityAreValidAndHeaterIsType(HeaterControlType_3);

   TheDutyCycleInPercentShouldBe(93);
}

TEST(VariableSweatHeaterDutyCycleCalculator, ShouldCalculateDutyCycleForType4Heater)
{
   GivenAmbientTemperatureIs(AnotherTemperatureInDegFx100);
   GivenAmbientHumidityIs(AnotherHumidityPercentx100);
   GivenFreshFoodSetpointTemperatureIs(AnotherFreshFoodSetpointTemperatureInDegFx100);
   GivenFreezerSetpointTemperatureIs(AnotherFreezerSetpointTemperatureInDegFx100);
   GivenModuleIsInitializedWithBothAmbientThermistorAndAmbientHumidityAreValidAndHeaterIsType(HeaterControlType_4);

   TheDutyCycleInPercentShouldBe(39);
}

TEST(VariableSweatHeaterDutyCycleCalculator, ShouldCalculateDutyCycleForType5Heater)
{
   GivenAmbientTemperatureIs(AnotherTemperatureInDegFx100);
   GivenAmbientHumidityIs(AnotherHumidityPercentx100);
   GivenFreshFoodSetpointTemperatureIs(AnotherFreshFoodSetpointTemperatureInDegFx100);
   GivenFreezerSetpointTemperatureIs(AnotherFreezerSetpointTemperatureInDegFx100);
   GivenModuleIsInitializedWithBothAmbientThermistorAndAmbientHumidityAreValidAndHeaterIsType(HeaterControlType_5);

   TheDutyCycleInPercentShouldBe(97);
}

TEST(VariableSweatHeaterDutyCycleCalculator, ShouldCalculateDutyCycleForType6Heater)
{
   GivenAmbientTemperatureIs(AnotherTemperatureInDegFx100);
   GivenAmbientHumidityIs(AnotherHumidityPercentx100);
   GivenFreshFoodSetpointTemperatureIs(AnotherFreshFoodSetpointTemperatureInDegFx100);
   GivenFreezerSetpointTemperatureIs(AnotherFreezerSetpointTemperatureInDegFx100);
   GivenModuleIsInitializedWithBothAmbientThermistorAndAmbientHumidityAreValidAndHeaterIsType(HeaterControlType_6);

   TheDutyCycleInPercentShouldBe(92);
}
