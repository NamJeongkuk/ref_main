/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "CondenserFanAntiSweatBehavior.h"
#include "SystemErds.h"
};

#include "CppUTest/TestHarness.h"
#include "ReferDataModel_TestDouble.h"

static const CondenserFanAntiSweatBehaviorConfig_t config = {
   .ambientFilteredTemperatureResolvedInDegFx100Erd = Erd_Ambient_FilteredTemperatureResolvedInDegFx100,
   .ambientFilteredHumidityPercentx100ResolvedErd = Erd_Ambient_FilteredHumidityResolvedPercentx100,
   .condenserFanAntiSweatBehaviorVoteErd = Erd_CondenserFanSpeed_AntiSweatBehaviorVote,
   .ambientTemperatureSensorIsValidErd = Erd_AmbientTemperature_IsValidResolved,
   .ambientHumiditySensorIsValidErd = Erd_AmbientHumidity_IsValidResolved,
   .condenserFanAntiSweatBehaviorEnabledByGridErd = Erd_CondenserFanAntiSweatBehaviorEnabledByGrid,
   .coolingModeErd = Erd_CoolingMode
};

TEST_GROUP(CondenserFanAntiSweatBehavior)
{
   CondenserFanAntiSweatBehavior_t instance;

   ReferDataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;

   const CombinedFanData_t *combinedFanData;
   const FanData_t *condenserFanData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelTestDouble);
      dataModel = dataModelTestDouble.dataModel;

      combinedFanData = PersonalityParametricData_Get(dataModel)->fanData;
      condenserFanData = &combinedFanData->condenserFan;
   }

   void GivenTheModuleIsInitialized()
   {
      CondenserFanAntiSweatBehavior_Init(&instance, dataModel, &config, condenserFanData);
   }

   void GivenAmbientHumidityIs(RelativeHumidityPercentx100_t humidity)
   {
      DataModel_Write(dataModel, Erd_Ambient_FilteredHumidityResolvedPercentx100, &humidity);
   }

   void WhenAmbientHumidityIs(RelativeHumidityPercentx100_t humidity)
   {
      GivenAmbientHumidityIs(humidity);
   }

   void GivenAmbientTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_Ambient_FilteredTemperatureResolvedInDegFx100, &temperature);
   }

   void WhenAmbientTemperatureIs(TemperatureDegFx100_t temperature)
   {
      GivenAmbientTemperatureIs(temperature);
   }

   void CondenserFanSpeedForAntiSweatBehaviorVoteShouldBe(FanRpm_t expected)
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_CondenserFanSpeed_AntiSweatBehaviorVote, &actual);
      CHECK_EQUAL(expected, actual.speed);
   }

   void CondenserFanSpeedForAntiSweatBehaviorVoteShouldBeDontCare()
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_CondenserFanSpeed_AntiSweatBehaviorVote, &actual);

      CHECK_FALSE(actual.care);
   }

   void GivenAmbientTemperatureSensorValidityIs(bool state)
   {
      DataModel_Write(dataModel, Erd_AmbientTemperature_IsValidResolved, &state);
   }

   void WhenAmbientTemperatureSensorValidityChangesTo(bool state)
   {
      GivenAmbientTemperatureSensorValidityIs(state);
   }

   void GivenAmbientHumiditySensorValidityIs(bool state)
   {
      DataModel_Write(dataModel, Erd_AmbientHumidity_IsValidResolved, &state);
   }

   void WhenAmbientHumiditySensorValidityChangesTo(bool state)
   {
      GivenAmbientHumiditySensorValidityIs(state);
   }

   void GivenCondenserFanAntiSweatBehaviorEnabledByGridIs(bool state)
   {
      DataModel_Write(dataModel, Erd_CondenserFanAntiSweatBehaviorEnabledByGrid, &state);
   }

   void WhenCondenserFanAntiSweatBehaviorEnabledByGridIs(bool state)
   {
      GivenCondenserFanAntiSweatBehaviorEnabledByGridIs(state);
   }

   void GivenCoolingModeIs(CoolingMode_t mode)
   {
      DataModel_Write(dataModel, Erd_CoolingMode, &mode);
   }

   void WhenCoolingModeChangesTo(CoolingMode_t mode)
   {
      GivenCoolingModeIs(mode);
   }
};

TEST(CondenserFanAntiSweatBehavior, ShouldSetCondenserFanSpeedToAntiSweatHighTemperatureSpeedWhenAmbientHumidityIsEqualToHighAmbientHumidityTriggerAndAmbientTemperatureIsEqualToHighAmbientTemperatureTriggerOnInit)
{
   GivenAmbientTemperatureSensorValidityIs(true);
   GivenAmbientHumiditySensorValidityIs(true);
   GivenCondenserFanAntiSweatBehaviorEnabledByGridIs(true);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100);
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100);
   GivenTheModuleIsInitialized();

   CondenserFanSpeedForAntiSweatBehaviorVoteShouldBe(condenserFanData->highAmbientHumidityAndHighTemperature.rpm);
}

TEST(CondenserFanAntiSweatBehavior, ShouldSetCondenserFanSpeedToAntiSweatHighTemperatureSpeedWhenAmbientHumidityIsEqualToHighAmbientHumidityTriggerAndAmbientTemperatureIsEqualToHighAmbientTemperatureTrigger)
{
   GivenAmbientTemperatureSensorValidityIs(true);
   GivenAmbientHumiditySensorValidityIs(true);
   GivenCondenserFanAntiSweatBehaviorEnabledByGridIs(true);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100);
   GivenTheModuleIsInitialized();
   CondenserFanSpeedForAntiSweatBehaviorVoteShouldBe(condenserFanData->highAmbientHumidityAndLowTemperature.rpm);

   WhenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100);
   CondenserFanSpeedForAntiSweatBehaviorVoteShouldBe(condenserFanData->highAmbientHumidityAndHighTemperature.rpm);
}

TEST(CondenserFanAntiSweatBehavior, ShouldSetCondenserFanSpeedToAntiSweatHighTemperatureSpeedWhenAmbientHumidityIsGreaterThanHighAmbientHumidityTriggerAndAmbientTemperatureIsEqualToHighAmbientTemperatureTrigger)
{
   GivenAmbientTemperatureSensorValidityIs(true);
   GivenAmbientHumiditySensorValidityIs(true);
   GivenCondenserFanAntiSweatBehaviorEnabledByGridIs(true);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 + 1);
   GivenTheModuleIsInitialized();
   CondenserFanSpeedForAntiSweatBehaviorVoteShouldBe(condenserFanData->highAmbientHumidityAndLowTemperature.rpm);

   WhenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100);
   CondenserFanSpeedForAntiSweatBehaviorVoteShouldBe(condenserFanData->highAmbientHumidityAndHighTemperature.rpm);
}

TEST(CondenserFanAntiSweatBehavior, ShouldSetCondenserFanSpeedToAntiSweatHighTemperatureSpeedWhenAmbientHumidityIsGreaterThanHighAmbientHumidityTriggerAndAmbientTemperatureIsGreaterThanHighAmbientTemperatureTrigger)
{
   GivenAmbientTemperatureSensorValidityIs(true);
   GivenAmbientHumiditySensorValidityIs(true);
   GivenCondenserFanAntiSweatBehaviorEnabledByGridIs(true);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 + 1);
   GivenTheModuleIsInitialized();
   CondenserFanSpeedForAntiSweatBehaviorVoteShouldBe(condenserFanData->highAmbientHumidityAndLowTemperature.rpm);

   WhenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 + 1);
   CondenserFanSpeedForAntiSweatBehaviorVoteShouldBe(condenserFanData->highAmbientHumidityAndHighTemperature.rpm);
}

TEST(CondenserFanAntiSweatBehavior, ShouldSetCondenserFanSpeedToAntiSweatLowTemperatureSpeedWhenAmbientHumidityIsGreaterThanOrEqualToHighAmbientHumidityTriggerAndAmbientTemperatureIsLessThanHighAmbientTemperatureTrigger)
{
   GivenAmbientTemperatureSensorValidityIs(true);
   GivenAmbientHumiditySensorValidityIs(true);
   GivenCondenserFanAntiSweatBehaviorEnabledByGridIs(true);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 + 1);
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100);
   GivenTheModuleIsInitialized();
   CondenserFanSpeedForAntiSweatBehaviorVoteShouldBe(condenserFanData->highAmbientHumidityAndHighTemperature.rpm);

   WhenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 - 1);
   CondenserFanSpeedForAntiSweatBehaviorVoteShouldBe(condenserFanData->highAmbientHumidityAndLowTemperature.rpm);
}

TEST(CondenserFanAntiSweatBehavior, ShouldVoteDontCareWhenAmbientHumidityIsLessThanHighAmbientHumidityTrigger)
{
   GivenAmbientTemperatureSensorValidityIs(true);
   GivenAmbientHumiditySensorValidityIs(true);
   GivenCondenserFanAntiSweatBehaviorEnabledByGridIs(true);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100);
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100);
   GivenTheModuleIsInitialized();
   CondenserFanSpeedForAntiSweatBehaviorVoteShouldBe(condenserFanData->highAmbientHumidityAndHighTemperature.rpm);

   WhenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 - 1);
   CondenserFanSpeedForAntiSweatBehaviorVoteShouldBe(condenserFanData->highAmbientHumidityAndLowTemperature.rpm);

   WhenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 - 1);
   CondenserFanSpeedForAntiSweatBehaviorVoteShouldBeDontCare();
}

TEST(CondenserFanAntiSweatBehavior, ShouldVoteDontCareWhenAmbientHumiditySensorIsInvalid)
{
   GivenAmbientTemperatureSensorValidityIs(true);
   GivenAmbientHumiditySensorValidityIs(true);
   GivenCondenserFanAntiSweatBehaviorEnabledByGridIs(true);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100);
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100);
   GivenTheModuleIsInitialized();
   CondenserFanSpeedForAntiSweatBehaviorVoteShouldBe(condenserFanData->highAmbientHumidityAndHighTemperature.rpm);

   WhenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 - 1);
   CondenserFanSpeedForAntiSweatBehaviorVoteShouldBe(condenserFanData->highAmbientHumidityAndLowTemperature.rpm);

   WhenAmbientHumiditySensorValidityChangesTo(false);
   CondenserFanSpeedForAntiSweatBehaviorVoteShouldBeDontCare();
}

TEST(CondenserFanAntiSweatBehavior, ShouldVoteDontCareWhenAmbientTemperatureSensorIsInvalid)
{
   GivenAmbientTemperatureSensorValidityIs(true);
   GivenAmbientHumiditySensorValidityIs(true);
   GivenCondenserFanAntiSweatBehaviorEnabledByGridIs(true);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100);
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100);
   GivenTheModuleIsInitialized();
   CondenserFanSpeedForAntiSweatBehaviorVoteShouldBe(condenserFanData->highAmbientHumidityAndHighTemperature.rpm);

   WhenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 - 1);
   CondenserFanSpeedForAntiSweatBehaviorVoteShouldBe(condenserFanData->highAmbientHumidityAndLowTemperature.rpm);

   WhenAmbientTemperatureSensorValidityChangesTo(false);
   CondenserFanSpeedForAntiSweatBehaviorVoteShouldBeDontCare();
}

TEST(CondenserFanAntiSweatBehavior, ShouldVoteDontCareWhenCondenserFanAntiSweatIsDisabledByGrid)
{
   GivenAmbientTemperatureSensorValidityIs(true);
   GivenAmbientHumiditySensorValidityIs(true);
   GivenCondenserFanAntiSweatBehaviorEnabledByGridIs(true);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100);
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100);
   GivenTheModuleIsInitialized();
   CondenserFanSpeedForAntiSweatBehaviorVoteShouldBe(condenserFanData->highAmbientHumidityAndHighTemperature.rpm);

   WhenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 - 1);
   CondenserFanSpeedForAntiSweatBehaviorVoteShouldBe(condenserFanData->highAmbientHumidityAndLowTemperature.rpm);

   WhenCondenserFanAntiSweatBehaviorEnabledByGridIs(false);
   CondenserFanSpeedForAntiSweatBehaviorVoteShouldBeDontCare();
}

TEST(CondenserFanAntiSweatBehavior, ShouldSetCondenserFanSpeedToAntiSweatHighTemperatureFreshFoodCoolingModeSpeedWhenCoolingModeIsFreshFoodAndHumidityIsGreaterThanHighAmbientHumidityTriggerAndTemperatureIsGreaterThanHighAmbientTriggerTemperature)
{
   GivenCoolingModeIs(CoolingMode_FreshFood);
   GivenAmbientTemperatureSensorValidityIs(true);
   GivenAmbientHumiditySensorValidityIs(true);
   GivenCondenserFanAntiSweatBehaviorEnabledByGridIs(true);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 + 1);
   GivenTheModuleIsInitialized();
   CondenserFanSpeedForAntiSweatBehaviorVoteShouldBe(condenserFanData->highAmbientHumidityAndLowTemperatureFreshFoodCoolingMode.rpm);

   WhenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 + 1);
   CondenserFanSpeedForAntiSweatBehaviorVoteShouldBe(condenserFanData->highAmbientHumidityAndHighTemperatureFreshFoodCoolingMode.rpm);
}

TEST(CondenserFanAntiSweatBehavior, ShouldSetCondenserFanSpeedToAntiSweatLowTemperatureFreshFoodCoolingModeSpeedWhenCoolingModeIsFreshFoodAndHumidityIsGreaterThanHighAmbientHumidityTriggerAndTemperatureIsLessThanHighAmbientTriggerTemperature)
{
   GivenCoolingModeIs(CoolingMode_FreshFood);
   GivenAmbientTemperatureSensorValidityIs(true);
   GivenAmbientHumiditySensorValidityIs(true);
   GivenCondenserFanAntiSweatBehaviorEnabledByGridIs(true);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 + 1);
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100);
   GivenTheModuleIsInitialized();
   CondenserFanSpeedForAntiSweatBehaviorVoteShouldBe(condenserFanData->highAmbientHumidityAndHighTemperatureFreshFoodCoolingMode.rpm);

   WhenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 - 1);
   CondenserFanSpeedForAntiSweatBehaviorVoteShouldBe(condenserFanData->highAmbientHumidityAndLowTemperatureFreshFoodCoolingMode.rpm);
}

TEST(CondenserFanAntiSweatBehavior, ShouldSetCondenserFanSpeedToAntiSweatHighTemperatureFreshFoodCoolingModeSpeedWhenCoolingModeChangesToFreshFoodAndHumidityIsGreaterThanHighAmbientHumidityTriggerAndTemperatureIsGreaterThanHighAmbientTriggerTemperature)
{
   GivenCoolingModeIs(CoolingMode_Freezer);
   GivenAmbientTemperatureSensorValidityIs(true);
   GivenAmbientHumiditySensorValidityIs(true);
   GivenCondenserFanAntiSweatBehaviorEnabledByGridIs(true);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 + 1);
   WhenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 + 1);
   GivenTheModuleIsInitialized();
   CondenserFanSpeedForAntiSweatBehaviorVoteShouldBe(condenserFanData->highAmbientHumidityAndHighTemperature.rpm);

   WhenCoolingModeChangesTo(CoolingMode_FreshFood);
   CondenserFanSpeedForAntiSweatBehaviorVoteShouldBe(condenserFanData->highAmbientHumidityAndHighTemperatureFreshFoodCoolingMode.rpm);
}
