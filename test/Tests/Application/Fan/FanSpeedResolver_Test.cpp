/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "FanSpeedResolver.h"
#include "FanSpeed.h"
#include "TemperatureDegFx100.h"
#include "Setpoint.h"
#include "SetpointZone.h"
#include "CoolingMode.h"
#include "PersonalityParametricData.h"
#include "Constants_Binary.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "DataModel_TestDouble.h"
#include "uassert_test.h"
#include "PersonalityParametricDataTestDouble.h"
#include "TddPersonality.h"

enum
{
   Invalid = 0,
   Valid,
   Inactive = 0,
   Active
};

enum
{
   Erd_ResolvedSpeedVote,
   Erd_CoolingMode,
   Erd_FreezerSetpointZone,
   Erd_CalculatedRequestFanControlSpeed,
   Erd_AmbientTemperature,
   Erd_AmbientHumidity,
   Erd_AmbientThermistorIsValid,
   Erd_AmbientHumidityIsValid,
   Erd_PullDownIsActive,
   Erd_CondenserFanAntiSweatBehaviorEnabledErd
};

static const DataModel_TestDoubleConfigurationEntry_t erds[] = {
   { Erd_ResolvedSpeedVote, sizeof(FanVotedSpeed_t) },
   { Erd_CoolingMode, sizeof(CoolingMode_t) },
   { Erd_FreezerSetpointZone, sizeof(SetpointZone_t) },
   { Erd_CalculatedRequestFanControlSpeed, sizeof(FanControl_t) },
   { Erd_AmbientTemperature, sizeof(TemperatureDegFx100_t) },
   { Erd_AmbientHumidity, sizeof(RelativeHumidityPercentx100_t) },
   { Erd_AmbientThermistorIsValid, sizeof(bool) },
   { Erd_AmbientHumidityIsValid, sizeof(bool) },
   { Erd_PullDownIsActive, sizeof(bool) },
   { Erd_CondenserFanAntiSweatBehaviorEnabledErd, sizeof(bool) }
};

static const FanSpeedResolverConfig_t nonCoolingModeConfig = {
   .resolvedFanSpeedVoteErd = Erd_ResolvedSpeedVote,
   .coolingModeErd = Erd_CoolingMode,
   .freezerSetpointErd = Erd_FreezerSetpointZone,
   .calculatedRequestFanControlErd = Erd_CalculatedRequestFanControlSpeed,
   .ambientFilteredTemperatureResolvedInDegFx100Erd = Erd_AmbientTemperature,
   .ambientFilteredHumidityPercentx100ResolvedErd = Erd_AmbientHumidity,
   .ambientThermistorIsValidErd = Erd_AmbientThermistorIsValid,
   .ambientHumiditySensorIsValidErd = Erd_AmbientHumidityIsValid,
   .pullDownIsActiveErd = Erd_PullDownIsActive,
   .fanAntiSweatBehaviorEnabledErd = Erd_CondenserFanAntiSweatBehaviorEnabledErd
};

static const FanSpeedResolverConfig_t coolingModeConfig = {
   .resolvedFanSpeedVoteErd = Erd_ResolvedSpeedVote,
   .coolingModeErd = Erd_CoolingMode,
   .freezerSetpointErd = Erd_FreezerSetpointZone,
   .calculatedRequestFanControlErd = Erd_CalculatedRequestFanControlSpeed,
   .ambientFilteredTemperatureResolvedInDegFx100Erd = Erd_AmbientTemperature,
   .ambientFilteredHumidityPercentx100ResolvedErd = Erd_AmbientHumidity,
   .ambientThermistorIsValidErd = Erd_AmbientThermistorIsValid,
   .ambientHumiditySensorIsValidErd = Erd_AmbientHumidityIsValid,
   .pullDownIsActiveErd = Erd_PullDownIsActive,
   .fanAntiSweatBehaviorEnabledErd = Erd_CondenserFanAntiSweatBehaviorEnabledErd
};

TEST_GROUP(FanSpeedResolver_NoCoolingMode)
{
   DataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;
   FanSpeedResolver_t instance;

   PersonalityParametricData_t *parametricData;
   const CombinedFanData_t *combinedFanData;
   const FanData_t *convertibleCompartmentFanData;

   void setup()
   {
      DataModel_TestDouble_Init(&dataModelTestDouble, erds, NUM_ELEMENTS(erds));
      dataModel = dataModelTestDouble.dataModel;

      parametricData = (PersonalityParametricData_t *)GivenThatTheApplicationParametricDataHasBeenLoadedIntoAPointer(TddPersonality_DevelopmentSingleEvaporator);
      combinedFanData = parametricData->fanData;
      convertibleCompartmentFanData = &combinedFanData->convertibleCompartmentFan;
   }

   void GivenInitialization()
   {
      FanSpeedResolver_Init(
         &instance,
         dataModel,
         convertibleCompartmentFanData,
         &nonCoolingModeConfig);
   }

   void WhenResolvedFanSpeedVoteIs(FanSpeed_t speed)
   {
      FanVotedSpeed_t votedSpeed = {
         .speed = speed,
         .care = true,
      };

      DataModel_Write(dataModel, Erd_ResolvedSpeedVote, &votedSpeed);
   }

   void GivenAmbientTemperatureIsSetTo(TemperatureDegFx100_t temp)
   {
      DataModel_Write(dataModel, Erd_AmbientTemperature, &temp);
   }

   void CalculatedFanControlSpeedShouldBe(uint16_t expectedSpeed)
   {
      FanControl_t actual;
      DataModel_Read(dataModel, Erd_CalculatedRequestFanControlSpeed, &actual);

      CHECK_EQUAL(expectedSpeed, actual.rpm);
   }
};

TEST(FanSpeedResolver_NoCoolingMode, ShouldSetCalculatedSpeedToLowWhenRequestedLow)
{
   GivenInitialization();

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(convertibleCompartmentFanData->speedData->lowSpeed.rpm);
}

TEST(FanSpeedResolver_NoCoolingMode, ShouldSetCalculatedSpeedToMediumWhenRequestedMedium)
{
   GivenInitialization();

   WhenResolvedFanSpeedVoteIs(FanSpeed_Medium);
   CalculatedFanControlSpeedShouldBe(convertibleCompartmentFanData->speedData->mediumSpeed.rpm);
}

TEST(FanSpeedResolver_NoCoolingMode, ShouldSetCalculatedSpeedToHighWhenRequestedHigh)
{
   GivenInitialization();

   WhenResolvedFanSpeedVoteIs(FanSpeed_High);
   CalculatedFanControlSpeedShouldBe(convertibleCompartmentFanData->speedData->highSpeed.rpm);
}

TEST(FanSpeedResolver_NoCoolingMode, ShouldSetCalculatedSpeedToMaxWhenRequestedSuperHigh)
{
   GivenInitialization();

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperHigh);
   CalculatedFanControlSpeedShouldBe(convertibleCompartmentFanData->speedData->superHighSpeed.rpm);
}

TEST(FanSpeedResolver_NoCoolingMode, ShouldSetCalculatedSpeedToSuperLowWhenRequestedSuperLow)
{
   GivenInitialization();

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(convertibleCompartmentFanData->speedData->superLowSpeed.rpm);
}

TEST(FanSpeedResolver_NoCoolingMode, ShouldSetCalculatedSpeedToZeroOnInitialization)
{
   GivenInitialization();
   CalculatedFanControlSpeedShouldBe(0);
}

TEST_GROUP(FanSpeedResolver_CoolingModeWithoutSetpoint)
{
   DataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;
   FanSpeedResolver_t instance;

   PersonalityParametricData_t *parametricData;
   const CombinedFanData_t *combinedFanData;
   const FanCareAboutCoolingModeSpeedData_t *coolingModeWithoutSetpointFanData;

   void setup()
   {
      DataModel_TestDouble_Init(&dataModelTestDouble, erds, NUM_ELEMENTS(erds));
      dataModel = dataModelTestDouble.dataModel;

      parametricData = (PersonalityParametricData_t *)GivenThatTheApplicationParametricDataHasBeenLoadedIntoAPointer(TddPersonality_DevelopmentSingleEvaporator);
      combinedFanData = parametricData->fanData;
      coolingModeWithoutSetpointFanData = combinedFanData->freshFoodEvapFan.careAboutCoolingModeSpeedData;
   }

   void WhenResolvedFanSpeedVoteIs(FanSpeed_t speed)
   {
      FanVotedSpeed_t votedSpeed = {
         .speed = speed,
         .care = true,
      };

      DataModel_Write(dataModel, Erd_ResolvedSpeedVote, &votedSpeed);
   }

   void GivenCoolingModeIs(CoolingMode_t coolingMode)
   {
      DataModel_Write(dataModel, Erd_CoolingMode, &coolingMode);
   }

   void WhenCoolingModeBecomes(CoolingMode_t coolingMode)
   {
      GivenCoolingModeIs(coolingMode);
   }

   void GivenFreezerSetpointIsSetTo(SetpointZone_t setpoint)
   {
      DataModel_Write(dataModel, Erd_FreezerSetpointZone, &setpoint);
   }

   void CalculatedFanControlSpeedShouldBe(uint16_t expectedSpeed)
   {
      FanControl_t actual;
      DataModel_Read(dataModel, Erd_CalculatedRequestFanControlSpeed, &actual);

      CHECK_EQUAL(expectedSpeed, actual.rpm);
   }

   void GivenInitializationWithoutCaringAboutSetpoint()
   {
      FanSpeedResolver_Init(
         &instance,
         dataModel,
         &combinedFanData->freshFoodEvapFan,
         &coolingModeConfig);
   }
};

TEST(FanSpeedResolver_CoolingModeWithoutSetpoint, ShouldSetCalculatedSpeedToLowFreshFoodWhenRequestedLowWithCoolingModeSetToFreshFood)
{
   GivenInitializationWithoutCaringAboutSetpoint();
   GivenCoolingModeIs(CoolingMode_FreshFood);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(coolingModeWithoutSetpointFanData->careAboutSetpointData.nonSetpointSpeeds.lowSpeedFreshFood.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithoutSetpoint, ShouldSetCalculatedSpeedToLowFreezerWhenRequestedLowWithCoolingModeSetToFreezer)
{
   GivenInitializationWithoutCaringAboutSetpoint();
   GivenCoolingModeIs(CoolingMode_Freezer);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(coolingModeWithoutSetpointFanData->careAboutSetpointData.nonSetpointSpeeds.lowSpeedFreezer.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithoutSetpoint, ShouldSetCalculatedSpeedToMediumFreshFoodWhenRequestedMediumWithCoolingModeSetToFreshFood)
{
   GivenInitializationWithoutCaringAboutSetpoint();
   GivenCoolingModeIs(CoolingMode_FreshFood);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Medium);
   CalculatedFanControlSpeedShouldBe(coolingModeWithoutSetpointFanData->careAboutSetpointData.nonSetpointSpeeds.mediumSpeedFreshFood.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithoutSetpoint, ShouldSetCalculatedSpeedToMediumFreezerWhenRequestedMediumWithCoolingModeSetToFreezer)
{
   GivenInitializationWithoutCaringAboutSetpoint();
   GivenCoolingModeIs(CoolingMode_Freezer);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Medium);
   CalculatedFanControlSpeedShouldBe(coolingModeWithoutSetpointFanData->careAboutSetpointData.nonSetpointSpeeds.mediumSpeedFreezer.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithoutSetpoint, ShouldSetCalculatedSpeedToHighFreshFoodWhenRequestedHighWithCoolingModeSetToFreshFood)
{
   GivenInitializationWithoutCaringAboutSetpoint();
   GivenCoolingModeIs(CoolingMode_FreshFood);

   WhenResolvedFanSpeedVoteIs(FanSpeed_High);
   CalculatedFanControlSpeedShouldBe(coolingModeWithoutSetpointFanData->careAboutSetpointData.nonSetpointSpeeds.highSpeedFreshFood.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithoutSetpoint, ShouldSetCalculatedSpeedToHighFreezerWhenRequestedHighWithCoolingModeSetToFreezer)
{
   GivenInitializationWithoutCaringAboutSetpoint();
   GivenCoolingModeIs(CoolingMode_Freezer);

   WhenResolvedFanSpeedVoteIs(FanSpeed_High);
   CalculatedFanControlSpeedShouldBe(coolingModeWithoutSetpointFanData->careAboutSetpointData.nonSetpointSpeeds.highSpeedFreezer.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithoutSetpoint, ShouldSetCalculatedSpeedToMaxWhenRequestedIsSuperHigh)
{
   GivenInitializationWithoutCaringAboutSetpoint();

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperHigh);
   CalculatedFanControlSpeedShouldBe(coolingModeWithoutSetpointFanData->careAboutSetpointData.nonSetpointSpeeds.superHighSpeed.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithoutSetpoint, ShouldSetCalculatedSpeedToLowFreezerWhenRequestedLowWithDefaultCaseCoolingMode)
{
   GivenInitializationWithoutCaringAboutSetpoint();
   GivenCoolingModeIs(CoolingMode_Off);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(coolingModeWithoutSetpointFanData->careAboutSetpointData.nonSetpointSpeeds.lowSpeedFreezer.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithoutSetpoint, ShouldSetCalculatedSpeedToMediumFreezerWhenRequestedLowWithDefaultCaseCoolingMode)
{
   GivenInitializationWithoutCaringAboutSetpoint();
   GivenCoolingModeIs(CoolingMode_Off);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Medium);
   CalculatedFanControlSpeedShouldBe(coolingModeWithoutSetpointFanData->careAboutSetpointData.nonSetpointSpeeds.mediumSpeedFreezer.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithoutSetpoint, ShouldSetCalculatedSpeedToHighFreezerWhenRequestedLowWithDefaultCaseCoolingMode)
{
   GivenInitializationWithoutCaringAboutSetpoint();
   GivenCoolingModeIs(CoolingMode_Off);

   WhenResolvedFanSpeedVoteIs(FanSpeed_High);
   CalculatedFanControlSpeedShouldBe(coolingModeWithoutSetpointFanData->careAboutSetpointData.nonSetpointSpeeds.highSpeedFreezer.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithoutSetpoint, ShouldChangeFanSpeedWhenCoolingModeChanges)
{
   GivenInitializationWithoutCaringAboutSetpoint();
   GivenCoolingModeIs(CoolingMode_FreshFood);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(coolingModeWithoutSetpointFanData->careAboutSetpointData.nonSetpointSpeeds.lowSpeedFreshFood.rpm);

   WhenCoolingModeBecomes(CoolingMode_Freezer);
   CalculatedFanControlSpeedShouldBe(coolingModeWithoutSetpointFanData->careAboutSetpointData.nonSetpointSpeeds.lowSpeedFreezer.rpm);
}

TEST_GROUP(FanSpeedResolver_CoolingModeWithSetpointAndHighAmbient)
{
   DataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;
   FanSpeedResolver_t instance;

   PersonalityParametricData_t *parametricData;
   const CombinedFanData_t *combinedFanData;
   const FanData_t *condenserFanData;
   const FanCareAboutCoolingModeSpeedData_t *coolingModeWithSetpointAndHighAmbientFanData;

   void setup()
   {
      DataModel_TestDouble_Init(&dataModelTestDouble, erds, NUM_ELEMENTS(erds));
      dataModel = dataModelTestDouble.dataModel;

      parametricData = (PersonalityParametricData_t *)GivenThatTheApplicationParametricDataHasBeenLoadedIntoAPointer(TddPersonality_DevelopmentSingleEvaporator);
      combinedFanData = parametricData->fanData;
      condenserFanData = &combinedFanData->condenserFan;
      coolingModeWithSetpointAndHighAmbientFanData = combinedFanData->condenserFan.careAboutCoolingModeSpeedData;
   }

   void WhenResolvedFanSpeedVoteIs(FanSpeed_t speed)
   {
      FanVotedSpeed_t votedSpeed = {
         .speed = speed,
         .care = true,
      };

      DataModel_Write(dataModel, Erd_ResolvedSpeedVote, &votedSpeed);
   }

   void GivenCoolingModeIs(CoolingMode_t coolingMode)
   {
      DataModel_Write(dataModel, Erd_CoolingMode, &coolingMode);
   }

   void WhenCoolingModeBecomes(CoolingMode_t coolingMode)
   {
      GivenCoolingModeIs(coolingMode);
   }

   void GivenFreezerSetpointIsSetTo(SetpointZone_t setpoint)
   {
      DataModel_Write(dataModel, Erd_FreezerSetpointZone, &setpoint);
   }

   void CalculatedFanControlSpeedShouldBe(uint16_t expectedSpeed)
   {
      FanControl_t actual;
      DataModel_Read(dataModel, Erd_CalculatedRequestFanControlSpeed, &actual);

      CHECK_EQUAL(expectedSpeed, actual.rpm);
   }

   void GivenInitializationCaringAboutSetpoint()
   {
      FanSpeedResolver_Init(
         &instance,
         dataModel,
         &combinedFanData->condenserFan,
         &coolingModeConfig);
   }

   void GivenPullDownIs(bool state)
   {
      DataModel_Write(dataModel, coolingModeConfig.pullDownIsActiveErd, &state);
   }

   void WhenPullDownIs(bool state)
   {
      GivenPullDownIs(state);
   }

   void GivenAmbientHumiditySensorIs(bool state)
   {
      DataModel_Write(dataModel, coolingModeConfig.ambientHumiditySensorIsValidErd, &state);
   }

   void WhenAmbientHumiditySensorIs(bool state)
   {
      GivenAmbientHumiditySensorIs(state);
   }

   void GivenAmbientThermistorIs(bool state)
   {
      DataModel_Write(dataModel, coolingModeConfig.ambientThermistorIsValidErd, &state);
   }

   void WhenAmbientThermistorSensorIs(bool state)
   {
      GivenAmbientThermistorIs(state);
   }

   void GivenAmbientTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, coolingModeConfig.ambientFilteredTemperatureResolvedInDegFx100Erd, &temperature);
   }

   void WhenAmbientTemperatureIs(TemperatureDegFx100_t temperature)
   {
      GivenAmbientTemperatureIs(temperature);
   }

   void GivenAmbientHumidityIs(RelativeHumidityPercentx100_t humidity)
   {
      DataModel_Write(dataModel, coolingModeConfig.ambientFilteredHumidityPercentx100ResolvedErd, &humidity);
   }

   void WhenAmbientHumidityIs(RelativeHumidityPercentx100_t humidity)
   {
      GivenAmbientHumidityIs(humidity);
   }

   void GivenAmbientSensorsAreValidAndPullDownIsActive()
   {
      GivenAmbientHumiditySensorIs(Valid);
      GivenAmbientThermistorIs(Valid);
      GivenPullDownIs(Active);
   }

   void GivenAmbientSensorsAreValidAndPullDownIsInactive()
   {
      GivenAmbientHumiditySensorIs(Valid);
      GivenAmbientThermistorIs(Valid);
      GivenPullDownIs(Inactive);
   }

   void GivenAmbientHumiditySensorIsInvalidAndAmbientThermistorIsValidAndPullDownIsInactive()
   {
      GivenAmbientHumiditySensorIs(Invalid);
      GivenAmbientThermistorIs(Valid);
      GivenPullDownIs(Inactive);
   }

   void GivenAmbientHumiditySensorIsValidAndAmbientThermistorIsInvalidAndPullDownIsInactive()
   {
      GivenAmbientHumiditySensorIs(Valid);
      GivenAmbientThermistorIs(Invalid);
      GivenPullDownIs(Inactive);
   }

   void WhenTheCondenserFanAntiSweatBehaviorIs(bool status)
   {
      DataModel_Write(dataModel, Erd_CondenserFanAntiSweatBehaviorEnabledErd, &status);
   }
};

TEST(FanSpeedResolver_CoolingModeWithSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowWhenRequestedSuperLowWithCoolingModeSetToFreshFoodAndSetPointIsCold)
{
   GivenInitializationCaringAboutSetpoint();
   GivenFreezerSetpointIsSetTo(SetpointZone_Cold);
   GivenCoolingModeIs(CoolingMode_FreshFood);

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(coolingModeWithSetpointAndHighAmbientFanData->careAboutSetpointData.setpointSpeeds.superLowSpeed.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowWhenRequestedSuperLowWithAmbientSensorsValidAndPullDownIsActive)
{
   GivenAmbientHumiditySensorIs(Valid);
   GivenAmbientThermistorIs(Valid);
   GivenPullDownIs(Active);
   GivenInitializationCaringAboutSetpoint();

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(coolingModeWithSetpointAndHighAmbientFanData->careAboutSetpointData.setpointSpeeds.superLowSpeed.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowWhenRequestedSuperLowWithAmbientHumiditySensorIsInvalidAndAmbientThermistorIsValidAndPullDownIsInactive)
{
   GivenAmbientHumiditySensorIs(Invalid);
   GivenAmbientThermistorIs(Valid);
   GivenPullDownIs(Inactive);
   GivenInitializationCaringAboutSetpoint();

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(coolingModeWithSetpointAndHighAmbientFanData->careAboutSetpointData.setpointSpeeds.superLowSpeed.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowWhenRequestedSuperLowWithAmbientHumiditySensorIsValidAndAmbientThermistorIsInvalidAndPullDownIsInactive)
{
   GivenAmbientHumiditySensorIs(Valid);
   GivenAmbientThermistorIs(Invalid);
   GivenPullDownIs(Inactive);
   GivenInitializationCaringAboutSetpoint();

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(coolingModeWithSetpointAndHighAmbientFanData->careAboutSetpointData.setpointSpeeds.superLowSpeed.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowWhenRequestedSuperLowWithAmbientSensorsValidAndPullDownIsInactiveAndAmbientHumidityIsGreaterThanHighTriggerHumidityAndAmbientTemperatureIsGreaterThanHighTriggerTemp)
{
   GivenAmbientHumiditySensorIs(Valid);
   GivenAmbientThermistorIs(Valid);
   GivenPullDownIs(Inactive);
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 + 1);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 + 1);
   GivenInitializationCaringAboutSetpoint();

   WhenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(condenserFanData->superLowAtHighAmbientHumidityAndHighTemperature.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedAtHighAmbientHumidityAndHighAmbientTemperatureIfAmbientSensorsAreValidAndPullDownIsInactiveAndAmbientHumidityIsEqualToHighTriggerHumidityAndAmbientTemperatureIsEqualToHighTriggerTemp)
{
   GivenAmbientHumiditySensorIs(Valid);
   GivenAmbientThermistorIs(Valid);
   GivenPullDownIs(Inactive);
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100);
   GivenInitializationCaringAboutSetpoint();

   WhenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(condenserFanData->superLowAtHighAmbientHumidityAndHighTemperature.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedAtHighAmbientHumidityAndLowAmbientTemperatureIfAmbientSensorsAreValidAndPullDownIsInactiveAndAmbientHumidityIsEqualToHighTriggerHumidityAndAmbientTemperatureIsLessThanTriggerTemp)
{
   GivenAmbientHumiditySensorIs(Valid);
   GivenAmbientThermistorIs(Valid);
   GivenPullDownIs(Inactive);
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 - 1);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100);
   GivenInitializationCaringAboutSetpoint();

   WhenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);

   CalculatedFanControlSpeedShouldBe(condenserFanData->superLowAtHighAmbientHumidityAndLowTemperature.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedIfAmbientSensorsAreValidAndPullDownIsInactiveAndAmbientHumidityIsLessThanHighTriggerHumidityAndAmbientTemperatureIsEqualToTriggerTemp)
{
   GivenAmbientHumiditySensorIs(Valid);
   GivenAmbientThermistorIs(Valid);
   GivenPullDownIs(Inactive);
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 - 1);
   GivenInitializationCaringAboutSetpoint();

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(coolingModeWithSetpointAndHighAmbientFanData->careAboutSetpointData.setpointSpeeds.superLowSpeed.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedIfAmbientHumiditySensorIsInvalidAndAmbientThermistorIsValidAndPullDownIsInactiveAndAmbientHumidityIsGreaterThanHighTriggerHumidityAndAmbientTemperatureIsGreaterThanHighTriggerTemp)
{
   GivenAmbientHumiditySensorIs(Invalid);
   GivenAmbientThermistorIs(Valid);
   GivenPullDownIs(Inactive);
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 + 1);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 + 1);
   GivenInitializationCaringAboutSetpoint();

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(coolingModeWithSetpointAndHighAmbientFanData->careAboutSetpointData.setpointSpeeds.superLowSpeed.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedIfAmbientHumiditySensorIsValidAndAmbientThermistorIsInvalidAndPullDownIsInactiveAndAmbientHumidityIsGreaterThanHighTriggerHumidityAndAmbientTemperatureIsGreaterThanHighTriggerTemp)
{
   GivenAmbientHumiditySensorIs(Valid);
   GivenAmbientThermistorIs(Invalid);
   GivenPullDownIs(Inactive);
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 + 1);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 + 1);
   GivenInitializationCaringAboutSetpoint();

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(coolingModeWithSetpointAndHighAmbientFanData->careAboutSetpointData.setpointSpeeds.superLowSpeed.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedIfAmbientHumiditySensorsAreValidAndPullDownIsActiveAndAmbientHumidityIsGreaterThanHighTriggerHumidityAndAmbientTemperatureIsGreaterThanHighTriggerTemp)
{
   GivenAmbientHumiditySensorIs(Valid);
   GivenAmbientThermistorIs(Valid);
   GivenPullDownIs(Active);
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 + 1);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 + 1);
   GivenInitializationCaringAboutSetpoint();

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(coolingModeWithSetpointAndHighAmbientFanData->careAboutSetpointData.setpointSpeeds.superLowSpeed.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedAtHighAmbientHumidityAndHighAmbientTemperatureWhenPullDownBecomesInactiveAndAmbientSensorsAreGreaterThanTriggerPoints)
{
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 + 1);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 + 1);
   GivenAmbientSensorsAreValidAndPullDownIsActive();
   GivenInitializationCaringAboutSetpoint();

   WhenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(coolingModeWithSetpointAndHighAmbientFanData->careAboutSetpointData.setpointSpeeds.superLowSpeed.rpm);

   WhenPullDownIs(Inactive);
   CalculatedFanControlSpeedShouldBe(condenserFanData->superLowAtHighAmbientHumidityAndHighTemperature.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedWhenPullDownBecomesActiveAndAmbientSensorsAreGreaterThanTriggerPoints)
{
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 + 1);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 + 1);
   GivenAmbientSensorsAreValidAndPullDownIsInactive();
   GivenInitializationCaringAboutSetpoint();

   WhenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(condenserFanData->superLowAtHighAmbientHumidityAndHighTemperature.rpm);

   WhenPullDownIs(Active);
   CalculatedFanControlSpeedShouldBe(coolingModeWithSetpointAndHighAmbientFanData->careAboutSetpointData.setpointSpeeds.superLowSpeed.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedAtHighAmbientHumidityAndHighAmbientTemperatureWhenAmbientHumiditySensorBecomesInvalidAndAmbientSensorsAreGreaterThanTriggerPoints)
{
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 + 1);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 + 1);
   GivenAmbientHumiditySensorIsInvalidAndAmbientThermistorIsValidAndPullDownIsInactive();
   GivenInitializationCaringAboutSetpoint();

   WhenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(coolingModeWithSetpointAndHighAmbientFanData->careAboutSetpointData.setpointSpeeds.superLowSpeed.rpm);

   WhenAmbientHumiditySensorIs(Valid);
   CalculatedFanControlSpeedShouldBe(condenserFanData->superLowAtHighAmbientHumidityAndHighTemperature.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedWhenAmbientHumiditySensorBecomesValidAndAmbientSensorsAreGreaterThanTriggerPoints)
{
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 + 1);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 + 1);
   GivenAmbientSensorsAreValidAndPullDownIsInactive();
   GivenInitializationCaringAboutSetpoint();

   WhenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(condenserFanData->superLowAtHighAmbientHumidityAndHighTemperature.rpm);

   WhenAmbientHumiditySensorIs(Invalid);
   CalculatedFanControlSpeedShouldBe(coolingModeWithSetpointAndHighAmbientFanData->careAboutSetpointData.setpointSpeeds.superLowSpeed.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedAtHighAmbientHumidityAndHighAmbientTemperatureWhenAmbientThermistorBecomesInvalidAndAmbientSensorsAreGreaterThanTriggerPoints)
{
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 + 1);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 + 1);
   GivenAmbientHumiditySensorIsValidAndAmbientThermistorIsInvalidAndPullDownIsInactive();
   GivenInitializationCaringAboutSetpoint();

   WhenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(coolingModeWithSetpointAndHighAmbientFanData->careAboutSetpointData.setpointSpeeds.superLowSpeed.rpm);

   WhenAmbientThermistorSensorIs(Valid);
   CalculatedFanControlSpeedShouldBe(condenserFanData->superLowAtHighAmbientHumidityAndHighTemperature.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedWhenAmbientThermistorBecomesValidAndAmbientSensorsAreGreaterThanTriggerPoints)
{
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 + 1);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 + 1);
   GivenAmbientSensorsAreValidAndPullDownIsInactive();
   GivenInitializationCaringAboutSetpoint();

   WhenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(condenserFanData->superLowAtHighAmbientHumidityAndHighTemperature.rpm);

   WhenAmbientThermistorSensorIs(Invalid);
   CalculatedFanControlSpeedShouldBe(coolingModeWithSetpointAndHighAmbientFanData->careAboutSetpointData.setpointSpeeds.superLowSpeed.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedAtHighAmbientHumidityAndLowAmbientTemperatureWhenAmbientTemperatureDropsBelowTriggerTempWhileAmbientHumidityIsEqualToTriggerHumidityAndAmbientSensorsAreValidAndPullDownIsInactive)
{
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100);
   GivenAmbientSensorsAreValidAndPullDownIsInactive();
   GivenInitializationCaringAboutSetpoint();

   WhenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(condenserFanData->superLowAtHighAmbientHumidityAndHighTemperature.rpm);

   WhenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 - 1);
   CalculatedFanControlSpeedShouldBe(condenserFanData->superLowAtHighAmbientHumidityAndLowTemperature.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedWhenAmbientHumidityDropsBelowTriggerTempWhileAmbientTemperatureIsEqualToTriggerTempAndAmbientSensorsAreValidAndPullDownIsInactive)
{
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100);
   GivenAmbientSensorsAreValidAndPullDownIsInactive();
   GivenInitializationCaringAboutSetpoint();

   WhenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(condenserFanData->superLowAtHighAmbientHumidityAndHighTemperature.rpm);

   WhenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 - 1);
   CalculatedFanControlSpeedShouldBe(coolingModeWithSetpointAndHighAmbientFanData->careAboutSetpointData.setpointSpeeds.superLowSpeed.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpointAndHighAmbient, ShouldSetCalculatedSpeedToLowColdFreshFoodWhenRequestedLowWithCoolingModeSetToFreshFoodAndSetPointIsCold)
{
   GivenInitializationCaringAboutSetpoint();
   GivenFreezerSetpointIsSetTo(SetpointZone_Cold);
   GivenCoolingModeIs(CoolingMode_FreshFood);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(coolingModeWithSetpointAndHighAmbientFanData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpointAndHighAmbient, ShouldSetCalculatedSpeedToLowColdFreezerWhenRequestedLowWithCoolingModeSetToFreezerAndSetPointIsCold)
{
   GivenInitializationCaringAboutSetpoint();
   GivenFreezerSetpointIsSetTo(SetpointZone_Cold);
   GivenCoolingModeIs(CoolingMode_Freezer);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(coolingModeWithSetpointAndHighAmbientFanData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpointAndHighAmbient, ShouldSetCalculatedSpeedToLowColdConvertibleCompartmentWhenRequestedLowWithCoolingModeSetToConvertibleCompartmentAndSetPointIsCold)
{
   GivenInitializationCaringAboutSetpoint();
   GivenFreezerSetpointIsSetTo(SetpointZone_Cold);
   GivenCoolingModeIs(CoolingMode_ConvertibleCompartment);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(coolingModeWithSetpointAndHighAmbientFanData->careAboutSetpointData.setpointSpeeds.lowSpeedConvertibleCompartmentWithColdSetpoint.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpointAndHighAmbient, ShouldSetCalculatedSpeedToLowMediumFreezerWhenRequestedLowWithCoolingModeSetToFreezerAndSetPointIsMedium)
{
   GivenInitializationCaringAboutSetpoint();
   GivenFreezerSetpointIsSetTo(SetpointZone_Middle);
   GivenCoolingModeIs(CoolingMode_Freezer);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(coolingModeWithSetpointAndHighAmbientFanData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithMediumSetpoint.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpointAndHighAmbient, ShouldSetCalculatedSpeedToLowMediumConvertibleCompartmentWhenRequestedLowWithCoolingModeSetToConvertibleCompartmentAndSetPointIsMedium)
{
   GivenInitializationCaringAboutSetpoint();
   GivenFreezerSetpointIsSetTo(SetpointZone_Middle);
   GivenCoolingModeIs(CoolingMode_ConvertibleCompartment);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(coolingModeWithSetpointAndHighAmbientFanData->careAboutSetpointData.setpointSpeeds.lowSpeedConvertibleCompartmentWithMediumSetpoint.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpointAndHighAmbient, ShouldSetCalculatedSpeedToLowWarmFreezerWhenRequestedLowWithCoolingModeSetToFreezerAndSetPointIsWarm)
{
   GivenInitializationCaringAboutSetpoint();
   GivenFreezerSetpointIsSetTo(SetpointZone_Warm);
   GivenCoolingModeIs(CoolingMode_Freezer);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(coolingModeWithSetpointAndHighAmbientFanData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithWarmSetpoint.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpointAndHighAmbient, ShouldSetCalculatedSpeedToLowWarmConvertibleCompartmentWhenRequestedLowWithCoolingModeSetToConvertibleCompartmentAndSetPointIsWarm)
{
   GivenInitializationCaringAboutSetpoint();
   GivenFreezerSetpointIsSetTo(SetpointZone_Warm);
   GivenCoolingModeIs(CoolingMode_ConvertibleCompartment);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(coolingModeWithSetpointAndHighAmbientFanData->careAboutSetpointData.setpointSpeeds.lowSpeedConvertibleCompartmentWithWarmSetpoint.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpointAndHighAmbient, ShouldSetCalculatedSpeedToLowColdFreezerWhenRequestedLowWithDefaultCaseCoolingModeAndSetPointIsCold)
{
   GivenInitializationCaringAboutSetpoint();
   GivenFreezerSetpointIsSetTo(SetpointZone_Cold);
   GivenCoolingModeIs(CoolingMode_Off);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(coolingModeWithSetpointAndHighAmbientFanData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpointAndHighAmbient, ShouldSetCalculatedSpeedToLowMediumFreezerWhenRequestedLowWithDefaultCaseCoolingModeAndSetPointIsMedium)
{
   GivenInitializationCaringAboutSetpoint();
   GivenFreezerSetpointIsSetTo(SetpointZone_Middle);
   GivenCoolingModeIs(CoolingMode_Off);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(coolingModeWithSetpointAndHighAmbientFanData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithMediumSetpoint.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpointAndHighAmbient, ShouldSetCalculatedSpeedToLowWarmFreezerWhenRequestedLowWithDefaultCaseCoolingModeAndSetPointIsWarm)
{
   GivenInitializationCaringAboutSetpoint();
   GivenFreezerSetpointIsSetTo(SetpointZone_Warm);
   GivenCoolingModeIs(CoolingMode_Off);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(coolingModeWithSetpointAndHighAmbientFanData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithWarmSetpoint.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpointAndHighAmbient, ShouldSetCalculatedSpeedToMediumFreshFoodWhenRequestedMediumWithCoolingModeSetToFreshFood)
{
   GivenInitializationCaringAboutSetpoint();
   GivenCoolingModeIs(CoolingMode_FreshFood);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Medium);
   CalculatedFanControlSpeedShouldBe(coolingModeWithSetpointAndHighAmbientFanData->careAboutSetpointData.setpointSpeeds.mediumSpeedFreshFood.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpointAndHighAmbient, ShouldSetCalculatedSpeedToMediumFreezerWhenRequestedMediumWithCoolingModeSetToFreezer)
{
   GivenInitializationCaringAboutSetpoint();
   GivenCoolingModeIs(CoolingMode_Freezer);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Medium);
   CalculatedFanControlSpeedShouldBe(coolingModeWithSetpointAndHighAmbientFanData->careAboutSetpointData.setpointSpeeds.mediumSpeedFreezer.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpointAndHighAmbient, ShouldSetCalculatedSpeedToMediumConvertibleCompartmentWhenRequestedMediumWithCoolingModeSetToConvertibleCompartment)
{
   GivenInitializationCaringAboutSetpoint();
   GivenCoolingModeIs(CoolingMode_ConvertibleCompartment);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Medium);
   CalculatedFanControlSpeedShouldBe(coolingModeWithSetpointAndHighAmbientFanData->careAboutSetpointData.setpointSpeeds.mediumSpeedConvertibleCompartment.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpointAndHighAmbient, ShouldSetCalculatedSpeedToHighFreshFoodWhenRequestedHighWithCoolingModeSetToFreshFood)
{
   GivenInitializationCaringAboutSetpoint();
   GivenCoolingModeIs(CoolingMode_FreshFood);

   WhenResolvedFanSpeedVoteIs(FanSpeed_High);
   CalculatedFanControlSpeedShouldBe(coolingModeWithSetpointAndHighAmbientFanData->careAboutSetpointData.setpointSpeeds.highSpeedFreshFood.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpointAndHighAmbient, ShouldSetCalculatedSpeedToHighFreezerWhenRequestedHighWithCoolingModeSetToFreezer)
{
   GivenInitializationCaringAboutSetpoint();
   GivenCoolingModeIs(CoolingMode_Freezer);

   WhenResolvedFanSpeedVoteIs(FanSpeed_High);
   CalculatedFanControlSpeedShouldBe(coolingModeWithSetpointAndHighAmbientFanData->careAboutSetpointData.setpointSpeeds.highSpeedFreezer.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpointAndHighAmbient, ShouldSetCalculatedSpeedToHighConvertibleCompartmentWhenRequestedHighWithCoolingModeSetToConvertibleCompartment)
{
   GivenInitializationCaringAboutSetpoint();
   GivenCoolingModeIs(CoolingMode_ConvertibleCompartment);

   WhenResolvedFanSpeedVoteIs(FanSpeed_High);
   CalculatedFanControlSpeedShouldBe(coolingModeWithSetpointAndHighAmbientFanData->careAboutSetpointData.setpointSpeeds.highSpeedConvertibleCompartment.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperHighWhenRequestedSuperHigh)
{
   GivenInitializationCaringAboutSetpoint();

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperHigh);
   CalculatedFanControlSpeedShouldBe(coolingModeWithSetpointAndHighAmbientFanData->careAboutSetpointData.setpointSpeeds.superHighSpeed.rpm);
}

TEST_GROUP(FanSpeedResolver_CoolingModeWithoutSetpointAndHighAmbient)
{
   DataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;
   FanSpeedResolver_t instance;

   PersonalityParametricData_t *parametricData;
   const CombinedFanData_t *combinedFanData;
   const FanData_t *condenserFanData;
   const FanCareAboutCoolingModeSpeedData_t *coolingModeWithoutSetpointAndHighAmbientFanData;

   void setup()
   {
      DataModel_TestDouble_Init(&dataModelTestDouble, erds, NUM_ELEMENTS(erds));
      dataModel = dataModelTestDouble.dataModel;

      parametricData = (PersonalityParametricData_t *)GivenThatTheApplicationParametricDataHasBeenLoadedIntoAPointer(TddPersonality_DevelopmentCondenserFanCoolingModeWithoutSetpoint);
      combinedFanData = parametricData->fanData;
      condenserFanData = &combinedFanData->condenserFan;
      coolingModeWithoutSetpointAndHighAmbientFanData = combinedFanData->condenserFan.careAboutCoolingModeSpeedData;
   }

   void WhenResolvedFanSpeedVoteIs(FanSpeed_t speed)
   {
      FanVotedSpeed_t votedSpeed = {
         .speed = speed,
         .care = true,
      };

      DataModel_Write(dataModel, Erd_ResolvedSpeedVote, &votedSpeed);
   }

   void GivenCoolingModeIs(CoolingMode_t coolingMode)
   {
      DataModel_Write(dataModel, Erd_CoolingMode, &coolingMode);
   }

   void WhenCoolingModeBecomes(CoolingMode_t coolingMode)
   {
      GivenCoolingModeIs(coolingMode);
   }

   void GivenFreezerSetpointIsSetTo(SetpointZone_t setpoint)
   {
      DataModel_Write(dataModel, Erd_FreezerSetpointZone, &setpoint);
   }

   void CalculatedFanControlSpeedShouldBe(uint16_t expectedSpeed)
   {
      FanControl_t actual;
      DataModel_Read(dataModel, Erd_CalculatedRequestFanControlSpeed, &actual);

      CHECK_EQUAL(expectedSpeed, actual.rpm);
   }

   void GivenInitializationNotCaringAboutSetpoint()
   {
      FanSpeedResolver_Init(
         &instance,
         dataModel,
         &combinedFanData->condenserFan,
         &coolingModeConfig);
   }

   void GivenPullDownIs(bool state)
   {
      DataModel_Write(dataModel, coolingModeConfig.pullDownIsActiveErd, &state);
   }

   void WhenPullDownIs(bool state)
   {
      GivenPullDownIs(state);
   }

   void GivenAmbientHumiditySensorIs(bool state)
   {
      DataModel_Write(dataModel, coolingModeConfig.ambientHumiditySensorIsValidErd, &state);
   }

   void WhenAmbientHumiditySensorIs(bool state)
   {
      GivenAmbientHumiditySensorIs(state);
   }

   void GivenAmbientThermistorIs(bool state)
   {
      DataModel_Write(dataModel, coolingModeConfig.ambientThermistorIsValidErd, &state);
   }

   void WhenAmbientThermistorSensorIs(bool state)
   {
      GivenAmbientThermistorIs(state);
   }

   void GivenAmbientTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, coolingModeConfig.ambientFilteredTemperatureResolvedInDegFx100Erd, &temperature);
   }

   void WhenAmbientTemperatureIs(TemperatureDegFx100_t temperature)
   {
      GivenAmbientTemperatureIs(temperature);
   }

   void GivenAmbientHumidityIs(RelativeHumidityPercentx100_t humidity)
   {
      DataModel_Write(dataModel, coolingModeConfig.ambientFilteredHumidityPercentx100ResolvedErd, &humidity);
   }

   void WhenAmbientHumidityIs(RelativeHumidityPercentx100_t humidity)
   {
      GivenAmbientHumidityIs(humidity);
   }

   void GivenAmbientSensorsAreValidAndPullDownIsActive()
   {
      GivenAmbientHumiditySensorIs(Valid);
      GivenAmbientThermistorIs(Valid);
      GivenPullDownIs(Active);
   }

   void GivenAmbientSensorsAreValidAndPullDownIsInactive()
   {
      GivenAmbientHumiditySensorIs(Valid);
      GivenAmbientThermistorIs(Valid);
      GivenPullDownIs(Inactive);
   }

   void GivenAmbientHumiditySensorIsInvalidAndAmbientThermistorIsValidAndPullDownIsInactive()
   {
      GivenAmbientHumiditySensorIs(Invalid);
      GivenAmbientThermistorIs(Valid);
      GivenPullDownIs(Inactive);
   }

   void GivenAmbientHumiditySensorIsValidAndAmbientThermistorIsInvalidAndPullDownIsInactive()
   {
      GivenAmbientHumiditySensorIs(Valid);
      GivenAmbientThermistorIs(Invalid);
      GivenPullDownIs(Inactive);
   }

   void WhenTheCondenserFanAntiSweatBehaviorIs(bool status)
   {
      DataModel_Write(dataModel, Erd_CondenserFanAntiSweatBehaviorEnabledErd, &status);
   }
};

TEST(FanSpeedResolver_CoolingModeWithoutSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowWhenRequestedSuperLowWithAmbientSensorsValidAndPullDownIsActive)
{
   GivenAmbientHumiditySensorIs(Valid);
   GivenAmbientThermistorIs(Valid);
   GivenPullDownIs(Active);
   GivenInitializationNotCaringAboutSetpoint();

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(coolingModeWithoutSetpointAndHighAmbientFanData->careAboutSetpointData.nonSetpointSpeeds.superLowSpeed.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithoutSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowWhenRequestedSuperLowWithAmbientHumiditySensorIsInvalidAndAmbientThermistorIsValidAndPullDownIsInactive)
{
   GivenAmbientHumiditySensorIs(Invalid);
   GivenAmbientThermistorIs(Valid);
   GivenPullDownIs(Inactive);
   GivenInitializationNotCaringAboutSetpoint();

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(coolingModeWithoutSetpointAndHighAmbientFanData->careAboutSetpointData.nonSetpointSpeeds.superLowSpeed.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithoutSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowWhenRequestedSuperLowWithAmbientHumiditySensorIsValidAndAmbientThermistorIsInvalidAndPullDownIsInactive)
{
   GivenAmbientHumiditySensorIs(Valid);
   GivenAmbientThermistorIs(Invalid);
   GivenPullDownIs(Inactive);
   GivenInitializationNotCaringAboutSetpoint();

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(coolingModeWithoutSetpointAndHighAmbientFanData->careAboutSetpointData.nonSetpointSpeeds.superLowSpeed.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithoutSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowWhenRequestedSuperLowWithAmbientSensorsValidAndPullDownIsInactiveAndAmbientHumidityIsGreaterThanHighTriggerHumidityAndAmbientTemperatureIsGreaterThanHighTriggerTemp)
{
   GivenAmbientHumiditySensorIs(Valid);
   GivenAmbientThermistorIs(Valid);
   GivenPullDownIs(Inactive);
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 + 1);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 + 1);
   GivenInitializationNotCaringAboutSetpoint();

   WhenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(condenserFanData->superLowAtHighAmbientHumidityAndHighTemperature.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithoutSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedAtHighAmbientHumidityAndHighAmbientTemperatureIfAmbientSensorsAreValidAndPullDownIsInactiveAndAmbientHumidityIsEqualToHighTriggerHumidityAndAmbientTemperatureIsEqualToHighTriggerTemp)
{
   GivenAmbientHumiditySensorIs(Valid);
   GivenAmbientThermistorIs(Valid);
   GivenPullDownIs(Inactive);
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100);
   GivenInitializationNotCaringAboutSetpoint();

   WhenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(condenserFanData->superLowAtHighAmbientHumidityAndHighTemperature.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithoutSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedAtHighAmbientHumidityAndLowAmbientTemperatureIfAmbientSensorsAreValidAndPullDownIsInactiveAndAmbientHumidityIsEqualToHighTriggerHumidityAndAmbientTemperatureIsLessThanTriggerTemp)
{
   GivenAmbientHumiditySensorIs(Valid);
   GivenAmbientThermistorIs(Valid);
   GivenPullDownIs(Inactive);
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 - 1);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100);
   GivenInitializationNotCaringAboutSetpoint();

   WhenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);

   CalculatedFanControlSpeedShouldBe(condenserFanData->superLowAtHighAmbientHumidityAndLowTemperature.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithoutSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedIfAmbientSensorsAreValidAndPullDownIsInactiveAndAmbientHumidityIsLessThanHighTriggerHumidityAndAmbientTemperatureIsEqualToTriggerTemp)
{
   GivenAmbientHumiditySensorIs(Valid);
   GivenAmbientThermistorIs(Valid);
   GivenPullDownIs(Inactive);
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 - 1);
   GivenInitializationNotCaringAboutSetpoint();

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(coolingModeWithoutSetpointAndHighAmbientFanData->careAboutSetpointData.nonSetpointSpeeds.superLowSpeed.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithoutSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedIfAmbientHumiditySensorIsInvalidAndAmbientThermistorIsValidAndPullDownIsInactiveAndAmbientHumidityIsGreaterThanHighTriggerHumidityAndAmbientTemperatureIsGreaterThanHighTriggerTemp)
{
   GivenAmbientHumiditySensorIs(Invalid);
   GivenAmbientThermistorIs(Valid);
   GivenPullDownIs(Inactive);
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 + 1);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 + 1);
   GivenInitializationNotCaringAboutSetpoint();

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(coolingModeWithoutSetpointAndHighAmbientFanData->careAboutSetpointData.nonSetpointSpeeds.superLowSpeed.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithoutSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedIfAmbientHumiditySensorIsValidAndAmbientThermistorIsInvalidAndPullDownIsInactiveAndAmbientHumidityIsGreaterThanHighTriggerHumidityAndAmbientTemperatureIsGreaterThanHighTriggerTemp)
{
   GivenAmbientHumiditySensorIs(Valid);
   GivenAmbientThermistorIs(Invalid);
   GivenPullDownIs(Inactive);
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 + 1);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 + 1);
   GivenInitializationNotCaringAboutSetpoint();

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(coolingModeWithoutSetpointAndHighAmbientFanData->careAboutSetpointData.nonSetpointSpeeds.superLowSpeed.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithoutSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedIfAmbientHumiditySensorsAreValidAndPullDownIsActiveAndAmbientHumidityIsGreaterThanHighTriggerHumidityAndAmbientTemperatureIsGreaterThanHighTriggerTemp)
{
   GivenAmbientHumiditySensorIs(Valid);
   GivenAmbientThermistorIs(Valid);
   GivenPullDownIs(Active);
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 + 1);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 + 1);
   GivenInitializationNotCaringAboutSetpoint();

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(coolingModeWithoutSetpointAndHighAmbientFanData->careAboutSetpointData.nonSetpointSpeeds.superLowSpeed.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithoutSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedAtHighAmbientHumidityAndHighAmbientTemperatureWhenPullDownBecomesInactiveAndAmbientSensorsAreGreaterThanTriggerPoints)
{
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 + 1);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 + 1);
   GivenAmbientSensorsAreValidAndPullDownIsActive();
   GivenInitializationNotCaringAboutSetpoint();

   WhenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(coolingModeWithoutSetpointAndHighAmbientFanData->careAboutSetpointData.nonSetpointSpeeds.superLowSpeed.rpm);

   WhenPullDownIs(Inactive);
   CalculatedFanControlSpeedShouldBe(condenserFanData->superLowAtHighAmbientHumidityAndHighTemperature.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithoutSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedWhenPullDownBecomesActiveAndAmbientSensorsAreGreaterThanTriggerPoints)
{
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 + 1);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 + 1);
   GivenAmbientSensorsAreValidAndPullDownIsInactive();
   GivenInitializationNotCaringAboutSetpoint();

   WhenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(condenserFanData->superLowAtHighAmbientHumidityAndHighTemperature.rpm);

   WhenPullDownIs(Active);
   CalculatedFanControlSpeedShouldBe(coolingModeWithoutSetpointAndHighAmbientFanData->careAboutSetpointData.nonSetpointSpeeds.superLowSpeed.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithoutSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedAtHighAmbientHumidityAndHighAmbientTemperatureWhenAmbientHumiditySensorBecomesInvalidAndAmbientSensorsAreGreaterThanTriggerPoints)
{
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 + 1);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 + 1);
   GivenAmbientHumiditySensorIsInvalidAndAmbientThermistorIsValidAndPullDownIsInactive();
   GivenInitializationNotCaringAboutSetpoint();

   WhenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(coolingModeWithoutSetpointAndHighAmbientFanData->careAboutSetpointData.nonSetpointSpeeds.superLowSpeed.rpm);

   WhenAmbientHumiditySensorIs(Valid);
   CalculatedFanControlSpeedShouldBe(condenserFanData->superLowAtHighAmbientHumidityAndHighTemperature.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithoutSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedWhenAmbientHumiditySensorBecomesValidAndAmbientSensorsAreGreaterThanTriggerPoints)
{
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 + 1);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 + 1);
   GivenAmbientSensorsAreValidAndPullDownIsInactive();
   GivenInitializationNotCaringAboutSetpoint();

   WhenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(condenserFanData->superLowAtHighAmbientHumidityAndHighTemperature.rpm);

   WhenAmbientHumiditySensorIs(Invalid);
   CalculatedFanControlSpeedShouldBe(coolingModeWithoutSetpointAndHighAmbientFanData->careAboutSetpointData.nonSetpointSpeeds.superLowSpeed.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithoutSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedAtHighAmbientHumidityAndHighAmbientTemperatureWhenAmbientThermistorBecomesInvalidAndAmbientSensorsAreGreaterThanTriggerPoints)
{
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 + 1);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 + 1);
   GivenAmbientHumiditySensorIsValidAndAmbientThermistorIsInvalidAndPullDownIsInactive();
   GivenInitializationNotCaringAboutSetpoint();

   WhenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(coolingModeWithoutSetpointAndHighAmbientFanData->careAboutSetpointData.nonSetpointSpeeds.superLowSpeed.rpm);

   WhenAmbientThermistorSensorIs(Valid);
   CalculatedFanControlSpeedShouldBe(condenserFanData->superLowAtHighAmbientHumidityAndHighTemperature.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithoutSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedWhenAmbientThermistorBecomesValidAndAmbientSensorsAreGreaterThanTriggerPoints)
{
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 + 1);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 + 1);
   GivenAmbientSensorsAreValidAndPullDownIsInactive();
   GivenInitializationNotCaringAboutSetpoint();

   WhenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(condenserFanData->superLowAtHighAmbientHumidityAndHighTemperature.rpm);

   WhenAmbientThermistorSensorIs(Invalid);
   CalculatedFanControlSpeedShouldBe(coolingModeWithoutSetpointAndHighAmbientFanData->careAboutSetpointData.nonSetpointSpeeds.superLowSpeed.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithoutSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedAtHighAmbientHumidityAndLowAmbientTemperatureWhenAmbientTemperatureDropsBelowTriggerTempWhileAmbientHumidityIsEqualToTriggerHumidityAndAmbientSensorsAreValidAndPullDownIsInactive)
{
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100);
   GivenAmbientSensorsAreValidAndPullDownIsInactive();
   GivenInitializationNotCaringAboutSetpoint();

   WhenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(condenserFanData->superLowAtHighAmbientHumidityAndHighTemperature.rpm);

   WhenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 - 1);
   CalculatedFanControlSpeedShouldBe(condenserFanData->superLowAtHighAmbientHumidityAndLowTemperature.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithoutSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedWhenAmbientHumidityDropsBelowTriggerTempWhileAmbientTemperatureIsEqualToTriggerTempAndAmbientSensorsAreValidAndPullDownIsInactive)
{
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100);
   GivenAmbientSensorsAreValidAndPullDownIsInactive();
   GivenInitializationNotCaringAboutSetpoint();

   WhenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(condenserFanData->superLowAtHighAmbientHumidityAndHighTemperature.rpm);

   WhenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 - 1);
   CalculatedFanControlSpeedShouldBe(coolingModeWithoutSetpointAndHighAmbientFanData->careAboutSetpointData.nonSetpointSpeeds.superLowSpeed.rpm);
}

TEST_GROUP(FanSpeedResolver_NoCoolingModeAndHighAmbient)
{
   DataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;
   FanSpeedResolver_t instance;

   PersonalityParametricData_t *parametricData;
   const CombinedFanData_t *combinedFanData;
   const FanData_t *condenserFanData;
   const FanSpeedData_t *noCoolingModeAndHighAmbientFanData;

   void setup()
   {
      DataModel_TestDouble_Init(&dataModelTestDouble, erds, NUM_ELEMENTS(erds));
      dataModel = dataModelTestDouble.dataModel;

      parametricData = (PersonalityParametricData_t *)GivenThatTheApplicationParametricDataHasBeenLoadedIntoAPointer(TddPersonality_DevelopmentCondenserFanNoCoolingMode);
      combinedFanData = parametricData->fanData;
      condenserFanData = &combinedFanData->condenserFan;
      noCoolingModeAndHighAmbientFanData = combinedFanData->condenserFan.speedData;
   }

   void WhenResolvedFanSpeedVoteIs(FanSpeed_t speed)
   {
      FanVotedSpeed_t votedSpeed = {
         .speed = speed,
         .care = true,
      };

      DataModel_Write(dataModel, Erd_ResolvedSpeedVote, &votedSpeed);
   }

   void GivenCoolingModeIs(CoolingMode_t coolingMode)
   {
      DataModel_Write(dataModel, Erd_CoolingMode, &coolingMode);
   }

   void WhenCoolingModeBecomes(CoolingMode_t coolingMode)
   {
      GivenCoolingModeIs(coolingMode);
   }

   void GivenFreezerSetpointIsSetTo(SetpointZone_t setpoint)
   {
      DataModel_Write(dataModel, Erd_FreezerSetpointZone, &setpoint);
   }

   void CalculatedFanControlSpeedShouldBe(uint16_t expectedSpeed)
   {
      FanControl_t actual;
      DataModel_Read(dataModel, Erd_CalculatedRequestFanControlSpeed, &actual);

      CHECK_EQUAL(expectedSpeed, actual.rpm);
   }

   void GivenInitializationNotCaringAboutSetpoint()
   {
      FanSpeedResolver_Init(
         &instance,
         dataModel,
         &combinedFanData->condenserFan,
         &coolingModeConfig);
   }

   void GivenPullDownIs(bool state)
   {
      DataModel_Write(dataModel, coolingModeConfig.pullDownIsActiveErd, &state);
   }

   void WhenPullDownIs(bool state)
   {
      GivenPullDownIs(state);
   }

   void GivenAmbientHumiditySensorIs(bool state)
   {
      DataModel_Write(dataModel, coolingModeConfig.ambientHumiditySensorIsValidErd, &state);
   }

   void WhenAmbientHumiditySensorIs(bool state)
   {
      GivenAmbientHumiditySensorIs(state);
   }

   void GivenAmbientThermistorIs(bool state)
   {
      DataModel_Write(dataModel, coolingModeConfig.ambientThermistorIsValidErd, &state);
   }

   void WhenAmbientThermistorSensorIs(bool state)
   {
      GivenAmbientThermistorIs(state);
   }

   void GivenAmbientTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, coolingModeConfig.ambientFilteredTemperatureResolvedInDegFx100Erd, &temperature);
   }

   void WhenAmbientTemperatureIs(TemperatureDegFx100_t temperature)
   {
      GivenAmbientTemperatureIs(temperature);
   }

   void GivenAmbientHumidityIs(RelativeHumidityPercentx100_t humidity)
   {
      DataModel_Write(dataModel, coolingModeConfig.ambientFilteredHumidityPercentx100ResolvedErd, &humidity);
   }

   void WhenAmbientHumidityIs(RelativeHumidityPercentx100_t humidity)
   {
      GivenAmbientHumidityIs(humidity);
   }

   void GivenAmbientSensorsAreValidAndPullDownIsActive()
   {
      GivenAmbientHumiditySensorIs(Valid);
      GivenAmbientThermistorIs(Valid);
      GivenPullDownIs(Active);
   }

   void GivenAmbientSensorsAreValidAndPullDownIsInactive()
   {
      GivenAmbientHumiditySensorIs(Valid);
      GivenAmbientThermistorIs(Valid);
      GivenPullDownIs(Inactive);
   }

   void GivenAmbientHumiditySensorIsInvalidAndAmbientThermistorIsValidAndPullDownIsInactive()
   {
      GivenAmbientHumiditySensorIs(Invalid);
      GivenAmbientThermistorIs(Valid);
      GivenPullDownIs(Inactive);
   }

   void GivenAmbientHumiditySensorIsValidAndAmbientThermistorIsInvalidAndPullDownIsInactive()
   {
      GivenAmbientHumiditySensorIs(Valid);
      GivenAmbientThermistorIs(Invalid);
      GivenPullDownIs(Inactive);
   }

   void WhenTheCondenserFanAntiSweatBehaviorIs(bool status)
   {
      DataModel_Write(dataModel, Erd_CondenserFanAntiSweatBehaviorEnabledErd, &status);
   }
};

TEST(FanSpeedResolver_NoCoolingModeAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowWhenRequestedSuperLowWithAmbientSensorsValidAndPullDownIsActive)
{
   GivenAmbientHumiditySensorIs(Valid);
   GivenAmbientThermistorIs(Valid);
   GivenPullDownIs(Active);
   GivenInitializationNotCaringAboutSetpoint();

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(noCoolingModeAndHighAmbientFanData->superLowSpeed.rpm);
}

TEST(FanSpeedResolver_NoCoolingModeAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowWhenRequestedSuperLowWithAmbientHumiditySensorIsInvalidAndAmbientThermistorIsValidAndPullDownIsInactive)
{
   GivenAmbientHumiditySensorIs(Invalid);
   GivenAmbientThermistorIs(Valid);
   GivenPullDownIs(Inactive);
   GivenInitializationNotCaringAboutSetpoint();

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(noCoolingModeAndHighAmbientFanData->superLowSpeed.rpm);
}

TEST(FanSpeedResolver_NoCoolingModeAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowWhenRequestedSuperLowWithAmbientHumiditySensorIsValidAndAmbientThermistorIsInvalidAndPullDownIsInactive)
{
   GivenAmbientHumiditySensorIs(Valid);
   GivenAmbientThermistorIs(Invalid);
   GivenPullDownIs(Inactive);
   GivenInitializationNotCaringAboutSetpoint();

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(noCoolingModeAndHighAmbientFanData->superLowSpeed.rpm);
}

TEST(FanSpeedResolver_NoCoolingModeAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowWhenRequestedSuperLowWithAmbientSensorsValidAndPullDownIsInactiveAndAmbientHumidityIsGreaterThanHighTriggerHumidityAndAmbientTemperatureIsGreaterThanHighTriggerTemp)
{
   GivenAmbientHumiditySensorIs(Valid);
   GivenAmbientThermistorIs(Valid);
   GivenPullDownIs(Inactive);
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 + 1);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 + 1);
   GivenInitializationNotCaringAboutSetpoint();

   WhenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(condenserFanData->superLowAtHighAmbientHumidityAndHighTemperature.rpm);
}

TEST(FanSpeedResolver_NoCoolingModeAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedAtHighAmbientHumidityAndHighAmbientTemperatureIfAmbientSensorsAreValidAndPullDownIsInactiveAndAmbientHumidityIsEqualToHighTriggerHumidityAndAmbientTemperatureIsEqualToHighTriggerTemp)
{
   GivenAmbientHumiditySensorIs(Valid);
   GivenAmbientThermistorIs(Valid);
   GivenPullDownIs(Inactive);
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100);
   GivenInitializationNotCaringAboutSetpoint();

   WhenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(condenserFanData->superLowAtHighAmbientHumidityAndHighTemperature.rpm);
}

TEST(FanSpeedResolver_NoCoolingModeAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedAtHighAmbientHumidityAndLowAmbientTemperatureIfAmbientSensorsAreValidAndPullDownIsInactiveAndAmbientHumidityIsEqualToHighTriggerHumidityAndAmbientTemperatureIsLessThanTriggerTemp)
{
   GivenAmbientHumiditySensorIs(Valid);
   GivenAmbientThermistorIs(Valid);
   GivenPullDownIs(Inactive);
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 - 1);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100);
   GivenInitializationNotCaringAboutSetpoint();

   WhenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);

   CalculatedFanControlSpeedShouldBe(condenserFanData->superLowAtHighAmbientHumidityAndLowTemperature.rpm);
}

TEST(FanSpeedResolver_NoCoolingModeAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedIfAmbientSensorsAreValidAndPullDownIsInactiveAndAmbientHumidityIsLessThanHighTriggerHumidityAndAmbientTemperatureIsEqualToTriggerTemp)
{
   GivenAmbientHumiditySensorIs(Valid);
   GivenAmbientThermistorIs(Valid);
   GivenPullDownIs(Inactive);
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 - 1);
   GivenInitializationNotCaringAboutSetpoint();

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(noCoolingModeAndHighAmbientFanData->superLowSpeed.rpm);
}

TEST(FanSpeedResolver_NoCoolingModeAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedIfAmbientHumiditySensorIsInvalidAndAmbientThermistorIsValidAndPullDownIsInactiveAndAmbientHumidityIsGreaterThanHighTriggerHumidityAndAmbientTemperatureIsGreaterThanHighTriggerTemp)
{
   GivenAmbientHumiditySensorIs(Invalid);
   GivenAmbientThermistorIs(Valid);
   GivenPullDownIs(Inactive);
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 + 1);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 + 1);
   GivenInitializationNotCaringAboutSetpoint();

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(noCoolingModeAndHighAmbientFanData->superLowSpeed.rpm);
}

TEST(FanSpeedResolver_NoCoolingModeAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedIfAmbientHumiditySensorIsValidAndAmbientThermistorIsInvalidAndPullDownIsInactiveAndAmbientHumidityIsGreaterThanHighTriggerHumidityAndAmbientTemperatureIsGreaterThanHighTriggerTemp)
{
   GivenAmbientHumiditySensorIs(Valid);
   GivenAmbientThermistorIs(Invalid);
   GivenPullDownIs(Inactive);
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 + 1);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 + 1);
   GivenInitializationNotCaringAboutSetpoint();

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(noCoolingModeAndHighAmbientFanData->superLowSpeed.rpm);
}

TEST(FanSpeedResolver_NoCoolingModeAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedIfAmbientHumiditySensorsAreValidAndPullDownIsActiveAndAmbientHumidityIsGreaterThanHighTriggerHumidityAndAmbientTemperatureIsGreaterThanHighTriggerTemp)
{
   GivenAmbientHumiditySensorIs(Valid);
   GivenAmbientThermistorIs(Valid);
   GivenPullDownIs(Active);
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 + 1);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 + 1);
   GivenInitializationNotCaringAboutSetpoint();

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(noCoolingModeAndHighAmbientFanData->superLowSpeed.rpm);
}

TEST(FanSpeedResolver_NoCoolingModeAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedAtHighAmbientHumidityAndHighAmbientTemperatureWhenPullDownBecomesInactiveAndAmbientSensorsAreGreaterThanTriggerPoints)
{
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 + 1);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 + 1);
   GivenAmbientSensorsAreValidAndPullDownIsActive();
   GivenInitializationNotCaringAboutSetpoint();

   WhenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(noCoolingModeAndHighAmbientFanData->superLowSpeed.rpm);

   WhenPullDownIs(Inactive);
   CalculatedFanControlSpeedShouldBe(condenserFanData->superLowAtHighAmbientHumidityAndHighTemperature.rpm);
}

TEST(FanSpeedResolver_NoCoolingModeAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedWhenPullDownBecomesActiveAndAmbientSensorsAreGreaterThanTriggerPoints)
{
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 + 1);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 + 1);
   GivenAmbientSensorsAreValidAndPullDownIsInactive();
   GivenInitializationNotCaringAboutSetpoint();

   WhenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(condenserFanData->superLowAtHighAmbientHumidityAndHighTemperature.rpm);

   WhenPullDownIs(Active);
   CalculatedFanControlSpeedShouldBe(noCoolingModeAndHighAmbientFanData->superLowSpeed.rpm);
}

TEST(FanSpeedResolver_NoCoolingModeAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedAtHighAmbientHumidityAndHighAmbientTemperatureWhenAmbientHumiditySensorBecomesInvalidAndAmbientSensorsAreGreaterThanTriggerPoints)
{
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 + 1);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 + 1);
   GivenAmbientHumiditySensorIsInvalidAndAmbientThermistorIsValidAndPullDownIsInactive();
   GivenInitializationNotCaringAboutSetpoint();

   WhenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(noCoolingModeAndHighAmbientFanData->superLowSpeed.rpm);

   WhenAmbientHumiditySensorIs(Valid);
   CalculatedFanControlSpeedShouldBe(condenserFanData->superLowAtHighAmbientHumidityAndHighTemperature.rpm);
}

TEST(FanSpeedResolver_NoCoolingModeAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedWhenAmbientHumiditySensorBecomesValidAndAmbientSensorsAreGreaterThanTriggerPoints)
{
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 + 1);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 + 1);
   GivenAmbientSensorsAreValidAndPullDownIsInactive();
   GivenInitializationNotCaringAboutSetpoint();

   WhenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(condenserFanData->superLowAtHighAmbientHumidityAndHighTemperature.rpm);

   WhenAmbientHumiditySensorIs(Invalid);
   CalculatedFanControlSpeedShouldBe(noCoolingModeAndHighAmbientFanData->superLowSpeed.rpm);
}

TEST(FanSpeedResolver_NoCoolingModeAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedAtHighAmbientHumidityAndHighAmbientTemperatureWhenAmbientThermistorBecomesInvalidAndAmbientSensorsAreGreaterThanTriggerPoints)
{
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 + 1);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 + 1);
   GivenAmbientHumiditySensorIsValidAndAmbientThermistorIsInvalidAndPullDownIsInactive();
   GivenInitializationNotCaringAboutSetpoint();

   WhenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(noCoolingModeAndHighAmbientFanData->superLowSpeed.rpm);

   WhenAmbientThermistorSensorIs(Valid);
   CalculatedFanControlSpeedShouldBe(condenserFanData->superLowAtHighAmbientHumidityAndHighTemperature.rpm);
}

TEST(FanSpeedResolver_NoCoolingModeAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedWhenAmbientThermistorBecomesValidAndAmbientSensorsAreGreaterThanTriggerPoints)
{
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 + 1);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 + 1);
   GivenAmbientSensorsAreValidAndPullDownIsInactive();
   GivenInitializationNotCaringAboutSetpoint();

   WhenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(condenserFanData->superLowAtHighAmbientHumidityAndHighTemperature.rpm);

   WhenAmbientThermistorSensorIs(Invalid);
   CalculatedFanControlSpeedShouldBe(noCoolingModeAndHighAmbientFanData->superLowSpeed.rpm);
}

TEST(FanSpeedResolver_NoCoolingModeAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedAtHighAmbientHumidityAndLowAmbientTemperatureWhenAmbientTemperatureDropsBelowTriggerTempWhileAmbientHumidityIsEqualToTriggerHumidityAndAmbientSensorsAreValidAndPullDownIsInactive)
{
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100);
   GivenAmbientSensorsAreValidAndPullDownIsInactive();
   GivenInitializationNotCaringAboutSetpoint();

   WhenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(condenserFanData->superLowAtHighAmbientHumidityAndHighTemperature.rpm);

   WhenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 - 1);
   CalculatedFanControlSpeedShouldBe(condenserFanData->superLowAtHighAmbientHumidityAndLowTemperature.rpm);
}

TEST(FanSpeedResolver_NoCoolingModeAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedWhenAmbientHumidityDropsBelowTriggerTempWhileAmbientTemperatureIsEqualToTriggerTempAndAmbientSensorsAreValidAndPullDownIsInactive)
{
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100);
   GivenAmbientSensorsAreValidAndPullDownIsInactive();
   GivenInitializationNotCaringAboutSetpoint();

   WhenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(condenserFanData->superLowAtHighAmbientHumidityAndHighTemperature.rpm);

   WhenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 - 1);
   CalculatedFanControlSpeedShouldBe(noCoolingModeAndHighAmbientFanData->superLowSpeed.rpm);
}

TEST(FanSpeedResolver_NoCoolingModeAndHighAmbient, ShouldNotRunCalculatedSpeedLogicWhenCondenserFanAntiSweatBehaviorIsDisabledByGrid)
{
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100);
   GivenAmbientSensorsAreValidAndPullDownIsInactive();
   GivenInitializationNotCaringAboutSetpoint();

   WhenTheCondenserFanAntiSweatBehaviorIs(DISABLED);
   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(condenserFanData->speedData->superLowSpeed.rpm);
}
