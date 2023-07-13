/*!
 * @file
 * @brief Integration test for Ambient Temperature
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "Application.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"
#include "SystemErds.h"
}

#include "CppUTest/TestHarness.h"
#include "ReferDataModel_TestDouble.h"
#include "CppUTestExt/MockSupport.h"

enum
{
   SomeTemperatureInDegFx100 = 3300,
   SomeOtherTemperatureInDegFx100 = 3500,
   Valid = true,
   Invalid = false,
   TenPercentInPercentx100 = 1000,
   AmbientTemperatureAveragingSampleFrequencyInMinutes = 5,
   AmbientTemperatureAverageSampleCounts = 24,
   AmbientTemperatureAveragingSampleFrequencyForTwoHoursInMinutes = 120
};

TEST_GROUP(AmbientTemperatureIntegration)
{
   Application_t instance;
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   ResetReason_t resetReason;
   TimerModule_TestDouble_t *timerModuleTestDouble;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);
   }

   void GivenTheApplicationHasBeenInitialized()
   {
      Application_Init(
         &instance,
         dataModel,
         resetReason);
   }

   void GivenTheAmbientFilteredInternalTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(
         dataModel,
         Erd_Ambient_FilteredInternalTemperatureInDegFx100,
         &temperature);
   }

   void WhenTheAmbientFilteredInternalTemperatureBecomes(TemperatureDegFx100_t temperature)
   {
      GivenTheAmbientFilteredInternalTemperatureIs(temperature);
   }

   void GivenTheAmbientThermistorIs(bool state)
   {
      DataModel_Write(
         dataModel,
         Erd_Ambient_ThermistorIsValid,
         &state);
   }

   void GivenTheAmbientHumiditySensorIs(bool state)
   {
      DataModel_Write(
         dataModel,
         Erd_Ambient_HumiditySensorIsValid,
         &state);
   }

   void TheResolvedFilteredInternalTemperatureShouldBe(TemperatureDegFx100_t expectedState)
   {
      TemperatureDegFx100_t actualState;
      DataModel_Read(
         dataModel,
         Erd_Ambient_FilteredInternalTemperatureResolvedInDegFx100,
         &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void TheResolvedAmbientThermistorShouldBe(bool expectedState)
   {
      bool actualState;
      DataModel_Read(
         dataModel,
         Erd_AmbientThermistor_IsValidResolved,
         &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void TheResolvedAmbientHumiditySensorShouldBe(bool expectedState)
   {
      bool actualState;
      DataModel_Read(
         dataModel,
         Erd_AmbientHumiditySensor_IsValidResolved,
         &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void WhenTheAmbientFilteredInternalTemperatureOverrideBecomes(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(
         dataModel,
         Erd_Ambient_FilteredInternalTemperatureOverrideRequest,
         set);

      DataModel_Write(
         dataModel,
         Erd_Ambient_FilteredInternalTemperatureOverrideValueInDegFx100,
         &temperature);
   }

   void WhenTheAmbientThermistorOverrideBecomes(bool state)
   {
      DataModel_Write(
         dataModel,
         Erd_AmbientThermistor_IsValidOverrideRequest,
         set);

      DataModel_Write(
         dataModel,
         Erd_AmbientThermistor_IsValidOverrideValue,
         &state);
   }

   void WhenTheAmbientHumiditySensorOverrideBecomes(bool state)
   {
      DataModel_Write(
         dataModel,
         Erd_AmbientHumiditySensor_IsValidOverrideRequest,
         set);

      DataModel_Write(
         dataModel,
         Erd_AmbientHumiditySensor_IsValidOverrideValue,
         &state);
   }

   void WhenAllOverridesAreCleared(void)
   {
      DataModel_Write(
         dataModel,
         Erd_Ambient_FilteredInternalTemperatureOverrideRequest,
         clear);

      DataModel_Write(
         dataModel,
         Erd_AmbientThermistor_IsValidOverrideRequest,
         clear);

      DataModel_Write(
         dataModel,
         Erd_AmbientHumiditySensor_IsValidOverrideRequest,
         clear);
   }

   void TheAmbientWindowAverageTemperatureFilterShouldBe(TemperatureDegFx100_t expectedTemperature)
   {
      TemperatureDegFx100_t actualTemperature;
      DataModel_Read(
         dataModel,
         Erd_Ambient_WindowAveragedTemperatureInDegFx100,
         &actualTemperature);

      CHECK_EQUAL(expectedTemperature, actualTemperature);
   }

   void GivenTheAmbientInternalSensorDataInPercentx100Is(RelativeHumidityPercentx100_t percentx100)
   {
      DataModel_Write(
         dataModel,
         Erd_AmbientHumidity_FilteredRelativeHumidityPercentx100,
         &percentx100);
   }

   void TheResolvedFilteredHumidityShouldBe(RelativeHumidityPercentx100_t expected)
   {
      RelativeHumidityPercentx100_t actual;
      DataModel_Read(
         dataModel,
         Erd_Ambient_FilteredHumidityResolvedPercentx100,
         &actual);

      CHECK_EQUAL(expected, actual);
   }

   void AmbientTemperaturePluginReadyShouldBeSet()
   {
      bool state;
      DataModel_Read(
         dataModel,
         Erd_AmbientTemperaturePluginReady,
         &state);

      CHECK_TRUE(state);
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks);
   }
};

TEST(AmbientTemperatureIntegration, ShouldInitialize)
{
   GivenTheApplicationHasBeenInitialized();
}

TEST(AmbientTemperatureIntegration, ShouldAllowOverridesWhenSetAndReleaseControlWhenCleared)
{
   GivenTheApplicationHasBeenInitialized();
   GivenTheAmbientFilteredInternalTemperatureIs(SomeTemperatureInDegFx100);
   GivenTheAmbientThermistorIs(Valid);
   GivenTheAmbientHumiditySensorIs(Valid);

   TheResolvedFilteredInternalTemperatureShouldBe(SomeTemperatureInDegFx100);
   TheResolvedAmbientThermistorShouldBe(Valid);
   TheResolvedAmbientHumiditySensorShouldBe(Valid);

   WhenTheAmbientFilteredInternalTemperatureOverrideBecomes(SomeOtherTemperatureInDegFx100);
   WhenTheAmbientThermistorOverrideBecomes(Invalid);
   WhenTheAmbientHumiditySensorOverrideBecomes(Invalid);

   TheResolvedFilteredInternalTemperatureShouldBe(SomeOtherTemperatureInDegFx100);
   TheResolvedAmbientThermistorShouldBe(Invalid);
   TheResolvedAmbientHumiditySensorShouldBe(Invalid);

   WhenAllOverridesAreCleared();

   TheResolvedFilteredInternalTemperatureShouldBe(SomeTemperatureInDegFx100);
   TheResolvedAmbientThermistorShouldBe(Valid);
   TheResolvedAmbientHumiditySensorShouldBe(Valid);
}

TEST(AmbientTemperatureIntegration, ShouldInitializeResolvedFilteredTemperatureBeforeWindowAveraging)
{
   GivenTheAmbientThermistorIs(Valid);
   GivenTheAmbientFilteredInternalTemperatureIs(SomeTemperatureInDegFx100);
   GivenTheApplicationHasBeenInitialized();

   TheResolvedFilteredInternalTemperatureShouldBe(SomeTemperatureInDegFx100);
   TheAmbientWindowAverageTemperatureFilterShouldBe(SomeTemperatureInDegFx100);
}

TEST(AmbientTemperatureIntegration, ShouldCorrectlyCalculateAmbientTmemperatureWindowAveraging)
{
   GivenTheAmbientThermistorIs(Valid);
   GivenTheAmbientFilteredInternalTemperatureIs(SomeTemperatureInDegFx100);
   GivenTheApplicationHasBeenInitialized();

   TheResolvedFilteredInternalTemperatureShouldBe(SomeTemperatureInDegFx100);
   TheAmbientWindowAverageTemperatureFilterShouldBe(SomeTemperatureInDegFx100);

   After(AmbientTemperatureAveragingSampleFrequencyInMinutes * MSEC_PER_MIN);
   TheAmbientWindowAverageTemperatureFilterShouldBe(SomeTemperatureInDegFx100);

   WhenTheAmbientFilteredInternalTemperatureBecomes(SomeOtherTemperatureInDegFx100);
   After(AmbientTemperatureAveragingSampleFrequencyForTwoHoursInMinutes * MSEC_PER_MIN);
   TheAmbientWindowAverageTemperatureFilterShouldBe(SomeOtherTemperatureInDegFx100);
}

TEST(AmbientTemperatureIntegration, ShouldSetAmbientHumidityResolvedOnInit)
{
   GivenTheAmbientHumiditySensorIs(Valid);
   GivenTheAmbientInternalSensorDataInPercentx100Is(TenPercentInPercentx100);
   GivenTheApplicationHasBeenInitialized();

   TheResolvedFilteredHumidityShouldBe(TenPercentInPercentx100);
}

TEST(AmbientTemperatureIntegration, ShouldSetAmbientTemperaturePluginReadyAfterPluginInitialized)
{
   GivenTheApplicationHasBeenInitialized();

   AmbientTemperaturePluginReadyShouldBeSet();
}
