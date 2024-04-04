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

   void CalculatedFanControlTypeShouldBe(FanControlType_t expectedType)
   {
      FanControl_t actual;
      DataModel_Read(dataModel, Erd_CalculatedRequestFanControlSpeed, &actual);

      CHECK_EQUAL(expectedType, actual.type);
   }
};

TEST(FanSpeedResolver_NoCoolingMode, ShouldSetCalculatedSpeedToLowWhenRequestedLow)
{
   GivenInitialization();

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlTypeShouldBe(convertibleCompartmentFanData->speedData->lowSpeed.type);
   CalculatedFanControlSpeedShouldBe(convertibleCompartmentFanData->speedData->lowSpeed.rpm);
}

TEST(FanSpeedResolver_NoCoolingMode, ShouldSetCalculatedSpeedToMediumWhenRequestedMedium)
{
   GivenInitialization();

   WhenResolvedFanSpeedVoteIs(FanSpeed_Medium);
   CalculatedFanControlTypeShouldBe(convertibleCompartmentFanData->speedData->mediumSpeed.type);
   CalculatedFanControlSpeedShouldBe(convertibleCompartmentFanData->speedData->mediumSpeed.rpm);
}

TEST(FanSpeedResolver_NoCoolingMode, ShouldSetCalculatedSpeedToHighWhenRequestedHigh)
{
   GivenInitialization();

   WhenResolvedFanSpeedVoteIs(FanSpeed_High);
   CalculatedFanControlTypeShouldBe(convertibleCompartmentFanData->speedData->highSpeed.type);
   CalculatedFanControlSpeedShouldBe(convertibleCompartmentFanData->speedData->highSpeed.rpm);
}

TEST(FanSpeedResolver_NoCoolingMode, ShouldSetCalculatedSpeedToMaxWhenRequestedSuperHigh)
{
   GivenInitialization();

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperHigh);
   CalculatedFanControlTypeShouldBe(convertibleCompartmentFanData->speedData->superHighSpeed.type);
   CalculatedFanControlSpeedShouldBe(convertibleCompartmentFanData->speedData->superHighSpeed.rpm);
}

TEST(FanSpeedResolver_NoCoolingMode, ShouldSetCalculatedSpeedToSuperLowWhenRequestedSuperLow)
{
   GivenInitialization();

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlTypeShouldBe(convertibleCompartmentFanData->speedData->superLowSpeed.type);
   CalculatedFanControlSpeedShouldBe(convertibleCompartmentFanData->speedData->superLowSpeed.rpm);
}

TEST(FanSpeedResolver_NoCoolingMode, ShouldSetFanControlTypeToRpmAndFanControlSpeedToResolvedFanSpeedIfVotedSpeedIsEqualToNumberOfFanSpeeds)
{
   GivenInitialization();

   WhenResolvedFanSpeedVoteIs(FanSpeed_NumberOfSpeeds);
   CalculatedFanControlTypeShouldBe(FanControlType_Rpm);
   CalculatedFanControlSpeedShouldBe(FanSpeed_NumberOfSpeeds);
}

TEST(FanSpeedResolver_NoCoolingMode, ShouldSetFanControlTypeToRpmAndFanControlSpeedToResolvedFanSpeedIfVotedSpeedIsGreaterThanNumberOfFanSpeeds)
{
   GivenInitialization();

   WhenResolvedFanSpeedVoteIs(FanSpeed_NumberOfSpeeds + 1);
   CalculatedFanControlTypeShouldBe(FanControlType_Rpm);
   CalculatedFanControlSpeedShouldBe(FanSpeed_NumberOfSpeeds + 1);

   WhenResolvedFanSpeedVoteIs(FanSpeed_NumberOfSpeeds + 2);
   CalculatedFanControlTypeShouldBe(FanControlType_Rpm);
   CalculatedFanControlSpeedShouldBe(FanSpeed_NumberOfSpeeds + 2);
}

TEST(FanSpeedResolver_NoCoolingMode, ShouldSetFanControlTypeToRpmAndSetFanControlSpeedToResolvedFanSpeedIfVotedSpeedIsGreaterThanNumberOfFanSpeedsWhileSuperLowSpeedIsVoted)
{
   GivenInitialization();

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlTypeShouldBe(convertibleCompartmentFanData->speedData->superLowSpeed.type);
   CalculatedFanControlSpeedShouldBe(convertibleCompartmentFanData->speedData->superLowSpeed.rpm);

   WhenResolvedFanSpeedVoteIs(FanSpeed_NumberOfSpeeds + 10);
   CalculatedFanControlTypeShouldBe(FanControlType_Rpm);
   CalculatedFanControlSpeedShouldBe(FanSpeed_NumberOfSpeeds + 10);
}

TEST(FanSpeedResolver_NoCoolingMode, ShouldSetCalculatedSpeedToHighWhenRequestedHighWhileGreaterNumberIsVoted)
{
   GivenInitialization();

   WhenResolvedFanSpeedVoteIs(FanSpeed_NumberOfSpeeds + 20);
   CalculatedFanControlTypeShouldBe(FanControlType_Rpm);
   CalculatedFanControlSpeedShouldBe(FanSpeed_NumberOfSpeeds + 20);

   WhenResolvedFanSpeedVoteIs(FanSpeed_High);
   CalculatedFanControlTypeShouldBe(convertibleCompartmentFanData->speedData->highSpeed.type);
   CalculatedFanControlSpeedShouldBe(convertibleCompartmentFanData->speedData->highSpeed.rpm);
}

TEST(FanSpeedResolver_NoCoolingMode, ShouldSetCalculatedFanControlTypeToDutyCycleAndCalculatedFanControlSpeedToZeroWhenRequestedOff)
{
   GivenInitialization();

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlTypeShouldBe(convertibleCompartmentFanData->speedData->superLowSpeed.type);
   CalculatedFanControlSpeedShouldBe(convertibleCompartmentFanData->speedData->superLowSpeed.rpm);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Off);
   CalculatedFanControlTypeShouldBe(FanControlType_DutyCycle);
   CalculatedFanControlSpeedShouldBe(0);
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

   void CalculatedFanControlTypeShouldBe(FanControlType_t expectedType)
   {
      FanControl_t actual;
      DataModel_Read(dataModel, Erd_CalculatedRequestFanControlSpeed, &actual);

      CHECK_EQUAL(expectedType, actual.type);
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

TEST(FanSpeedResolver_CoolingModeWithoutSetpoint, ShouldSetFanControlTypeToRpmAndFanControlSpeedToResolvedFanSpeedIfVotedSpeedIsGreaterThanOrEqualToNumberOfFanSpeeds)
{
   GivenCoolingModeIs(CoolingMode_FreshFood);
   GivenInitializationWithoutCaringAboutSetpoint();

   WhenResolvedFanSpeedVoteIs(FanSpeed_NumberOfSpeeds);
   CalculatedFanControlTypeShouldBe(FanControlType_Rpm);
   CalculatedFanControlSpeedShouldBe(FanSpeed_NumberOfSpeeds);

   WhenResolvedFanSpeedVoteIs(FanSpeed_NumberOfSpeeds + 1);
   CalculatedFanControlTypeShouldBe(FanControlType_Rpm);
   CalculatedFanControlSpeedShouldBe(FanSpeed_NumberOfSpeeds + 1);
}

TEST(FanSpeedResolver_CoolingModeWithoutSetpoint, ShouldSetCalculatedFanControlTypeToDutyCycleAndCalculatedFanControlSpeedToZeroWhenRequestedOff)
{
   GivenInitializationWithoutCaringAboutSetpoint();
   GivenCoolingModeIs(CoolingMode_FreshFood);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(coolingModeWithoutSetpointFanData->careAboutSetpointData.nonSetpointSpeeds.lowSpeedFreshFood.rpm);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Off);
   CalculatedFanControlTypeShouldBe(FanControlType_DutyCycle);
   CalculatedFanControlSpeedShouldBe(0);
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

   void GivenAmbientSensorsAreValid()
   {
      GivenAmbientHumiditySensorIs(Valid);
      GivenAmbientThermistorIs(Valid);
   }

   void GivenAmbientHumiditySensorIsInvalidAndAmbientThermistorIsValid()
   {
      GivenAmbientHumiditySensorIs(Invalid);
      GivenAmbientThermistorIs(Valid);
   }

   void GivenAmbientHumiditySensorIsValidAndAmbientThermistorIsInvalid()
   {
      GivenAmbientHumiditySensorIs(Valid);
      GivenAmbientThermistorIs(Invalid);
   }

   void WhenTheCondenserFanAntiSweatBehaviorIs(bool status)
   {
      DataModel_Write(dataModel, Erd_CondenserFanAntiSweatBehaviorEnabledErd, &status);
   }

   void CalculatedFanControlTypeShouldBe(FanControlType_t expectedType)
   {
      FanControl_t actual;
      DataModel_Read(dataModel, Erd_CalculatedRequestFanControlSpeed, &actual);

      CHECK_EQUAL(expectedType, actual.type);
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

TEST(FanSpeedResolver_CoolingModeWithSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowWhenRequestedSuperLowWithAmbientSensorsValid)
{
   GivenAmbientHumiditySensorIs(Valid);
   GivenAmbientThermistorIs(Valid);
   GivenInitializationCaringAboutSetpoint();

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(coolingModeWithSetpointAndHighAmbientFanData->careAboutSetpointData.setpointSpeeds.superLowSpeed.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowWhenRequestedSuperLowWithAmbientHumiditySensorIsInvalidAndAmbientThermistorIsValid)
{
   GivenAmbientHumiditySensorIs(Invalid);
   GivenAmbientThermistorIs(Valid);
   GivenInitializationCaringAboutSetpoint();

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(coolingModeWithSetpointAndHighAmbientFanData->careAboutSetpointData.setpointSpeeds.superLowSpeed.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowWhenRequestedSuperLowWithAmbientHumiditySensorIsValidAndAmbientThermistorIsInvalid)
{
   GivenAmbientHumiditySensorIs(Valid);
   GivenAmbientThermistorIs(Invalid);
   GivenInitializationCaringAboutSetpoint();

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(coolingModeWithSetpointAndHighAmbientFanData->careAboutSetpointData.setpointSpeeds.superLowSpeed.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowWhenRequestedSuperLowWithAmbientSensorsValidAndAmbientHumidityIsGreaterThanHighTriggerHumidityAndAmbientTemperatureIsGreaterThanHighTriggerTemp)
{
   GivenAmbientHumiditySensorIs(Valid);
   GivenAmbientThermistorIs(Valid);
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 + 1);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 + 1);
   GivenInitializationCaringAboutSetpoint();

   WhenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(condenserFanData->superLowAtHighAmbientHumidityAndHighTemperature.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedAtHighAmbientHumidityAndHighAmbientTemperatureIfAmbientSensorsAreValidAndAmbientHumidityIsEqualToHighTriggerHumidityAndAmbientTemperatureIsEqualToHighTriggerTemp)
{
   GivenAmbientHumiditySensorIs(Valid);
   GivenAmbientThermistorIs(Valid);
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100);
   GivenInitializationCaringAboutSetpoint();

   WhenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(condenserFanData->superLowAtHighAmbientHumidityAndHighTemperature.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedAtHighAmbientHumidityAndLowAmbientTemperatureIfAmbientSensorsAreValidAndAmbientHumidityIsEqualToHighTriggerHumidityAndAmbientTemperatureIsLessThanTriggerTemp)
{
   GivenAmbientHumiditySensorIs(Valid);
   GivenAmbientThermistorIs(Valid);
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 - 1);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100);
   GivenInitializationCaringAboutSetpoint();

   WhenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);

   CalculatedFanControlSpeedShouldBe(condenserFanData->superLowAtHighAmbientHumidityAndLowTemperature.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedIfAmbientSensorsAreValidAndAmbientHumidityIsLessThanHighTriggerHumidityAndAmbientTemperatureIsEqualToTriggerTemp)
{
   GivenAmbientHumiditySensorIs(Valid);
   GivenAmbientThermistorIs(Valid);
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 - 1);
   GivenInitializationCaringAboutSetpoint();

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(coolingModeWithSetpointAndHighAmbientFanData->careAboutSetpointData.setpointSpeeds.superLowSpeed.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedIfAmbientHumiditySensorIsInvalidAndAmbientThermistorIsValidAndAmbientHumidityIsGreaterThanHighTriggerHumidityAndAmbientTemperatureIsGreaterThanHighTriggerTemp)
{
   GivenAmbientHumiditySensorIs(Invalid);
   GivenAmbientThermistorIs(Valid);
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 + 1);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 + 1);
   GivenInitializationCaringAboutSetpoint();

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(coolingModeWithSetpointAndHighAmbientFanData->careAboutSetpointData.setpointSpeeds.superLowSpeed.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedIfAmbientHumiditySensorIsValidAndAmbientThermistorIsInvalidAndAmbientHumidityIsGreaterThanHighTriggerHumidityAndAmbientTemperatureIsGreaterThanHighTriggerTemp)
{
   GivenAmbientHumiditySensorIs(Valid);
   GivenAmbientThermistorIs(Invalid);
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 + 1);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 + 1);
   GivenInitializationCaringAboutSetpoint();

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(coolingModeWithSetpointAndHighAmbientFanData->careAboutSetpointData.setpointSpeeds.superLowSpeed.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedIfAmbientHumiditySensorsAreValidAndAmbientHumidityIsGreaterThanHighTriggerHumidityAndAmbientTemperatureIsGreaterThanHighTriggerTemp)
{
   GivenAmbientHumiditySensorIs(Valid);
   GivenAmbientThermistorIs(Valid);
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 + 1);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 + 1);
   GivenInitializationCaringAboutSetpoint();

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(coolingModeWithSetpointAndHighAmbientFanData->careAboutSetpointData.setpointSpeeds.superLowSpeed.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedAtHighAmbientHumidityAndHighAmbientTemperatureWhenAmbientSensorsAreGreaterThanTriggerPoints)
{
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 + 1);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 + 1);
   GivenAmbientSensorsAreValid();
   GivenInitializationCaringAboutSetpoint();

   WhenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(condenserFanData->superLowAtHighAmbientHumidityAndHighTemperature.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedAtHighAmbientHumidityAndHighAmbientTemperatureWhenAmbientHumiditySensorBecomesInvalidAndAmbientSensorsAreGreaterThanTriggerPoints)
{
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 + 1);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 + 1);
   GivenAmbientHumiditySensorIsInvalidAndAmbientThermistorIsValid();
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
   GivenAmbientSensorsAreValid();
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
   GivenAmbientHumiditySensorIsValidAndAmbientThermistorIsInvalid();
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
   GivenAmbientSensorsAreValid();
   GivenInitializationCaringAboutSetpoint();

   WhenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(condenserFanData->superLowAtHighAmbientHumidityAndHighTemperature.rpm);

   WhenAmbientThermistorSensorIs(Invalid);
   CalculatedFanControlSpeedShouldBe(coolingModeWithSetpointAndHighAmbientFanData->careAboutSetpointData.setpointSpeeds.superLowSpeed.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedAtHighAmbientHumidityAndLowAmbientTemperatureWhenAmbientTemperatureDropsBelowTriggerTempWhileAmbientHumidityIsEqualToTriggerHumidityAndAmbientSensorsAreValid)
{
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100);
   GivenAmbientSensorsAreValid();
   GivenInitializationCaringAboutSetpoint();

   WhenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(condenserFanData->superLowAtHighAmbientHumidityAndHighTemperature.rpm);

   WhenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 - 1);
   CalculatedFanControlSpeedShouldBe(condenserFanData->superLowAtHighAmbientHumidityAndLowTemperature.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedWhenAmbientHumidityDropsBelowTriggerTempWhileAmbientTemperatureIsEqualToTriggerTempAndAmbientSensorsAreValid)
{
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100);
   GivenAmbientSensorsAreValid();
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

TEST(FanSpeedResolver_CoolingModeWithSetpointAndHighAmbient, ShouldSetFanControlTypeToRpmAndFanControlSpeedToResolvedFanSpeedIfVotedSpeedIsGreaterThanOrEqualToNumberOfFanSpeeds)
{
   GivenInitializationCaringAboutSetpoint();

   WhenResolvedFanSpeedVoteIs(FanSpeed_NumberOfSpeeds);
   CalculatedFanControlTypeShouldBe(FanControlType_Rpm);
   CalculatedFanControlSpeedShouldBe(FanSpeed_NumberOfSpeeds);

   WhenResolvedFanSpeedVoteIs(FanSpeed_NumberOfSpeeds + 1);
   CalculatedFanControlTypeShouldBe(FanControlType_Rpm);
   CalculatedFanControlSpeedShouldBe(FanSpeed_NumberOfSpeeds + 1);
}

TEST(FanSpeedResolver_CoolingModeWithSetpointAndHighAmbient, ShouldSetCalculatedFanControlTypeToDutyCycleAndCalculatedFanControlSpeedToZeroWhenRequestedOff)
{
   GivenInitializationCaringAboutSetpoint();

   WhenResolvedFanSpeedVoteIs(FanSpeed_NumberOfSpeeds);
   CalculatedFanControlTypeShouldBe(FanControlType_Rpm);
   CalculatedFanControlSpeedShouldBe(FanSpeed_NumberOfSpeeds);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Off);
   CalculatedFanControlTypeShouldBe(FanControlType_DutyCycle);
   CalculatedFanControlSpeedShouldBe(0);
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

   void GivenAmbientSensorsAreValid()
   {
      GivenAmbientHumiditySensorIs(Valid);
      GivenAmbientThermistorIs(Valid);
   }

   void GivenAmbientHumiditySensorIsInvalidAndAmbientThermistorIsValid()
   {
      GivenAmbientHumiditySensorIs(Invalid);
      GivenAmbientThermistorIs(Valid);
   }

   void GivenAmbientHumiditySensorIsValidAndAmbientThermistorIsInvalid()
   {
      GivenAmbientHumiditySensorIs(Valid);
      GivenAmbientThermistorIs(Invalid);
   }

   void WhenTheCondenserFanAntiSweatBehaviorIs(bool status)
   {
      DataModel_Write(dataModel, Erd_CondenserFanAntiSweatBehaviorEnabledErd, &status);
   }

   void CalculatedFanControlTypeShouldBe(FanControlType_t expectedType)
   {
      FanControl_t actual;
      DataModel_Read(dataModel, Erd_CalculatedRequestFanControlSpeed, &actual);

      CHECK_EQUAL(expectedType, actual.type);
   }
};

TEST(FanSpeedResolver_CoolingModeWithoutSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowWhenRequestedSuperLowWithAmbientSensorsValid)
{
   GivenAmbientHumiditySensorIs(Valid);
   GivenAmbientThermistorIs(Valid);
   GivenInitializationNotCaringAboutSetpoint();

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(coolingModeWithoutSetpointAndHighAmbientFanData->careAboutSetpointData.nonSetpointSpeeds.superLowSpeed.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithoutSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowWhenRequestedSuperLowWithAmbientHumiditySensorIsInvalidAndAmbientThermistorIsValid)
{
   GivenAmbientHumiditySensorIs(Invalid);
   GivenAmbientThermistorIs(Valid);
   GivenInitializationNotCaringAboutSetpoint();

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(coolingModeWithoutSetpointAndHighAmbientFanData->careAboutSetpointData.nonSetpointSpeeds.superLowSpeed.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithoutSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowWhenRequestedSuperLowWithAmbientHumiditySensorIsValidAndAmbientThermistorIsInvalid)
{
   GivenAmbientHumiditySensorIs(Valid);
   GivenAmbientThermistorIs(Invalid);
   GivenInitializationNotCaringAboutSetpoint();

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(coolingModeWithoutSetpointAndHighAmbientFanData->careAboutSetpointData.nonSetpointSpeeds.superLowSpeed.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithoutSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowWhenRequestedSuperLowWithAmbientSensorsValidAndAmbientHumidityIsGreaterThanHighTriggerHumidityAndAmbientTemperatureIsGreaterThanHighTriggerTemp)
{
   GivenAmbientHumiditySensorIs(Valid);
   GivenAmbientThermistorIs(Valid);
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 + 1);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 + 1);
   GivenInitializationNotCaringAboutSetpoint();

   WhenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(condenserFanData->superLowAtHighAmbientHumidityAndHighTemperature.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithoutSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedAtHighAmbientHumidityAndHighAmbientTemperatureIfAmbientSensorsAreValidAndAmbientHumidityIsEqualToHighTriggerHumidityAndAmbientTemperatureIsEqualToHighTriggerTemp)
{
   GivenAmbientHumiditySensorIs(Valid);
   GivenAmbientThermistorIs(Valid);
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100);
   GivenInitializationNotCaringAboutSetpoint();

   WhenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(condenserFanData->superLowAtHighAmbientHumidityAndHighTemperature.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithoutSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedAtHighAmbientHumidityAndLowAmbientTemperatureIfAmbientSensorsAreValidAndAmbientHumidityIsEqualToHighTriggerHumidityAndAmbientTemperatureIsLessThanTriggerTemp)
{
   GivenAmbientHumiditySensorIs(Valid);
   GivenAmbientThermistorIs(Valid);
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 - 1);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100);
   GivenInitializationNotCaringAboutSetpoint();

   WhenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);

   CalculatedFanControlSpeedShouldBe(condenserFanData->superLowAtHighAmbientHumidityAndLowTemperature.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithoutSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedIfAmbientSensorsAreValidAndAmbientHumidityIsLessThanHighTriggerHumidityAndAmbientTemperatureIsEqualToTriggerTemp)
{
   GivenAmbientHumiditySensorIs(Valid);
   GivenAmbientThermistorIs(Valid);
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 - 1);
   GivenInitializationNotCaringAboutSetpoint();

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(coolingModeWithoutSetpointAndHighAmbientFanData->careAboutSetpointData.nonSetpointSpeeds.superLowSpeed.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithoutSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedIfAmbientHumiditySensorIsInvalidAndAmbientThermistorIsValidAndAmbientHumidityIsGreaterThanHighTriggerHumidityAndAmbientTemperatureIsGreaterThanHighTriggerTemp)
{
   GivenAmbientHumiditySensorIs(Invalid);
   GivenAmbientThermistorIs(Valid);
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 + 1);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 + 1);
   GivenInitializationNotCaringAboutSetpoint();

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(coolingModeWithoutSetpointAndHighAmbientFanData->careAboutSetpointData.nonSetpointSpeeds.superLowSpeed.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithoutSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedIfAmbientHumiditySensorIsValidAndAmbientThermistorIsInvalidAndAmbientHumidityIsGreaterThanHighTriggerHumidityAndAmbientTemperatureIsGreaterThanHighTriggerTemp)
{
   GivenAmbientHumiditySensorIs(Valid);
   GivenAmbientThermistorIs(Invalid);
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 + 1);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 + 1);
   GivenInitializationNotCaringAboutSetpoint();

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(coolingModeWithoutSetpointAndHighAmbientFanData->careAboutSetpointData.nonSetpointSpeeds.superLowSpeed.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithoutSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedIfAmbientHumiditySensorsAreValidAndAmbientHumidityIsGreaterThanHighTriggerHumidityAndAmbientTemperatureIsGreaterThanHighTriggerTemp)
{
   GivenAmbientHumiditySensorIs(Valid);
   GivenAmbientThermistorIs(Valid);
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 + 1);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 + 1);
   GivenInitializationNotCaringAboutSetpoint();

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(coolingModeWithoutSetpointAndHighAmbientFanData->careAboutSetpointData.nonSetpointSpeeds.superLowSpeed.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithoutSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedAtHighAmbientHumidityAndHighAmbientTemperatureWhenAmbientSensorsAreGreaterThanTriggerPoints)
{
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 + 1);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 + 1);
   GivenAmbientSensorsAreValid();
   GivenInitializationNotCaringAboutSetpoint();

   WhenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(condenserFanData->superLowAtHighAmbientHumidityAndHighTemperature.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithoutSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedAtHighAmbientHumidityAndHighAmbientTemperatureWhenAmbientHumiditySensorBecomesInvalidAndAmbientSensorsAreGreaterThanTriggerPoints)
{
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 + 1);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 + 1);
   GivenAmbientHumiditySensorIsInvalidAndAmbientThermistorIsValid();
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
   GivenAmbientSensorsAreValid();
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
   GivenAmbientHumiditySensorIsValidAndAmbientThermistorIsInvalid();
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
   GivenAmbientSensorsAreValid();
   GivenInitializationNotCaringAboutSetpoint();

   WhenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(condenserFanData->superLowAtHighAmbientHumidityAndHighTemperature.rpm);

   WhenAmbientThermistorSensorIs(Invalid);
   CalculatedFanControlSpeedShouldBe(coolingModeWithoutSetpointAndHighAmbientFanData->careAboutSetpointData.nonSetpointSpeeds.superLowSpeed.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithoutSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedAtHighAmbientHumidityAndLowAmbientTemperatureWhenAmbientTemperatureDropsBelowTriggerTempWhileAmbientHumidityIsEqualToTriggerHumidityAndAmbientSensorsAreValid)
{
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100);
   GivenAmbientSensorsAreValid();
   GivenInitializationNotCaringAboutSetpoint();

   WhenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(condenserFanData->superLowAtHighAmbientHumidityAndHighTemperature.rpm);

   WhenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 - 1);
   CalculatedFanControlSpeedShouldBe(condenserFanData->superLowAtHighAmbientHumidityAndLowTemperature.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithoutSetpointAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedWhenAmbientHumidityDropsBelowTriggerTempWhileAmbientTemperatureIsEqualToTriggerTempAndAmbientSensorsAreValid)
{
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100);
   GivenAmbientSensorsAreValid();
   GivenInitializationNotCaringAboutSetpoint();

   WhenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(condenserFanData->superLowAtHighAmbientHumidityAndHighTemperature.rpm);

   WhenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 - 1);
   CalculatedFanControlSpeedShouldBe(coolingModeWithoutSetpointAndHighAmbientFanData->careAboutSetpointData.nonSetpointSpeeds.superLowSpeed.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithoutSetpointAndHighAmbient, ShouldSetFanControlTypeToRpmAndFanControlSpeedToResolvedFanSpeedIfVotedSpeedIsGreaterThanOrEqualToNumberOfFanSpeeds)
{
   GivenAmbientHumiditySensorIs(Valid);
   GivenAmbientThermistorIs(Valid);
   GivenInitializationNotCaringAboutSetpoint();

   WhenResolvedFanSpeedVoteIs(FanSpeed_NumberOfSpeeds);
   CalculatedFanControlTypeShouldBe(FanControlType_Rpm);
   CalculatedFanControlSpeedShouldBe(FanSpeed_NumberOfSpeeds);

   WhenResolvedFanSpeedVoteIs(FanSpeed_NumberOfSpeeds + 1);
   CalculatedFanControlTypeShouldBe(FanControlType_Rpm);
   CalculatedFanControlSpeedShouldBe(FanSpeed_NumberOfSpeeds + 1);
}

TEST(FanSpeedResolver_CoolingModeWithoutSetpointAndHighAmbient, ShouldSetCalculatedFanControlTypeToDutyCycleAndCalculatedFanControlSpeedToZeroWhenRequestedOff)
{
   GivenAmbientHumiditySensorIs(Valid);
   GivenAmbientThermistorIs(Valid);
   GivenInitializationNotCaringAboutSetpoint();

   WhenResolvedFanSpeedVoteIs(FanSpeed_NumberOfSpeeds);
   CalculatedFanControlTypeShouldBe(FanControlType_Rpm);
   CalculatedFanControlSpeedShouldBe(FanSpeed_NumberOfSpeeds);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Off);
   CalculatedFanControlTypeShouldBe(FanControlType_DutyCycle);
   CalculatedFanControlSpeedShouldBe(0);
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

   void GivenAmbientSensorsAreValid()
   {
      GivenAmbientHumiditySensorIs(Valid);
      GivenAmbientThermistorIs(Valid);
   }

   void GivenAmbientHumiditySensorIsInvalidAndAmbientThermistorIsValid()
   {
      GivenAmbientHumiditySensorIs(Invalid);
      GivenAmbientThermistorIs(Valid);
   }

   void GivenAmbientHumiditySensorIsValidAndAmbientThermistorIsInvalid()
   {
      GivenAmbientHumiditySensorIs(Valid);
      GivenAmbientThermistorIs(Invalid);
   }

   void WhenTheCondenserFanAntiSweatBehaviorIs(bool status)
   {
      DataModel_Write(dataModel, Erd_CondenserFanAntiSweatBehaviorEnabledErd, &status);
   }

   void CalculatedFanControlTypeShouldBe(FanControlType_t expectedType)
   {
      FanControl_t actual;
      DataModel_Read(dataModel, Erd_CalculatedRequestFanControlSpeed, &actual);

      CHECK_EQUAL(expectedType, actual.type);
   }
};

TEST(FanSpeedResolver_NoCoolingModeAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowWhenRequestedSuperLowWithAmbientSensorsValid)
{
   GivenAmbientHumiditySensorIs(Valid);
   GivenAmbientThermistorIs(Valid);
   GivenInitializationNotCaringAboutSetpoint();

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(noCoolingModeAndHighAmbientFanData->superLowSpeed.rpm);
}

TEST(FanSpeedResolver_NoCoolingModeAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowWhenRequestedSuperLowWithAmbientHumiditySensorIsInvalidAndAmbientThermistorIsValid)
{
   GivenAmbientHumiditySensorIs(Invalid);
   GivenAmbientThermistorIs(Valid);
   GivenInitializationNotCaringAboutSetpoint();

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(noCoolingModeAndHighAmbientFanData->superLowSpeed.rpm);
}

TEST(FanSpeedResolver_NoCoolingModeAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowWhenRequestedSuperLowWithAmbientHumiditySensorIsValidAndAmbientThermistorIsInvalid)
{
   GivenAmbientHumiditySensorIs(Valid);
   GivenAmbientThermistorIs(Invalid);
   GivenInitializationNotCaringAboutSetpoint();

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(noCoolingModeAndHighAmbientFanData->superLowSpeed.rpm);
}

TEST(FanSpeedResolver_NoCoolingModeAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowWhenRequestedSuperLowWithAmbientSensorsValidAndAmbientHumidityIsGreaterThanHighTriggerHumidityAndAmbientTemperatureIsGreaterThanHighTriggerTemp)
{
   GivenAmbientHumiditySensorIs(Valid);
   GivenAmbientThermistorIs(Valid);
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 + 1);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 + 1);
   GivenInitializationNotCaringAboutSetpoint();

   WhenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(condenserFanData->superLowAtHighAmbientHumidityAndHighTemperature.rpm);
}

TEST(FanSpeedResolver_NoCoolingModeAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedAtHighAmbientHumidityAndHighAmbientTemperatureIfAmbientSensorsAreValidAndAmbientHumidityIsEqualToHighTriggerHumidityAndAmbientTemperatureIsEqualToHighTriggerTemp)
{
   GivenAmbientHumiditySensorIs(Valid);
   GivenAmbientThermistorIs(Valid);
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100);
   GivenInitializationNotCaringAboutSetpoint();

   WhenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(condenserFanData->superLowAtHighAmbientHumidityAndHighTemperature.rpm);
}

TEST(FanSpeedResolver_NoCoolingModeAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedAtHighAmbientHumidityAndLowAmbientTemperatureIfAmbientSensorsAreValidAndAmbientHumidityIsEqualToHighTriggerHumidityAndAmbientTemperatureIsLessThanTriggerTemp)
{
   GivenAmbientHumiditySensorIs(Valid);
   GivenAmbientThermistorIs(Valid);
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 - 1);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100);
   GivenInitializationNotCaringAboutSetpoint();

   WhenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);

   CalculatedFanControlSpeedShouldBe(condenserFanData->superLowAtHighAmbientHumidityAndLowTemperature.rpm);
}

TEST(FanSpeedResolver_NoCoolingModeAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedIfAmbientSensorsAreValidAndAmbientHumidityIsLessThanHighTriggerHumidityAndAmbientTemperatureIsEqualToTriggerTemp)
{
   GivenAmbientHumiditySensorIs(Valid);
   GivenAmbientThermistorIs(Valid);
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 - 1);
   GivenInitializationNotCaringAboutSetpoint();

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(noCoolingModeAndHighAmbientFanData->superLowSpeed.rpm);
}

TEST(FanSpeedResolver_NoCoolingModeAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedIfAmbientHumiditySensorIsInvalidAndAmbientThermistorIsValidAndAmbientHumidityIsGreaterThanHighTriggerHumidityAndAmbientTemperatureIsGreaterThanHighTriggerTemp)
{
   GivenAmbientHumiditySensorIs(Invalid);
   GivenAmbientThermistorIs(Valid);
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 + 1);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 + 1);
   GivenInitializationNotCaringAboutSetpoint();

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(noCoolingModeAndHighAmbientFanData->superLowSpeed.rpm);
}

TEST(FanSpeedResolver_NoCoolingModeAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedIfAmbientHumiditySensorIsValidAndAmbientThermistorIsInvalidAndAmbientHumidityIsGreaterThanHighTriggerHumidityAndAmbientTemperatureIsGreaterThanHighTriggerTemp)
{
   GivenAmbientHumiditySensorIs(Valid);
   GivenAmbientThermistorIs(Invalid);
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 + 1);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 + 1);
   GivenInitializationNotCaringAboutSetpoint();

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(noCoolingModeAndHighAmbientFanData->superLowSpeed.rpm);
}

TEST(FanSpeedResolver_NoCoolingModeAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedIfAmbientHumiditySensorsAreValidAndAmbientHumidityIsGreaterThanHighTriggerHumidityAndAmbientTemperatureIsGreaterThanHighTriggerTemp)
{
   GivenAmbientHumiditySensorIs(Valid);
   GivenAmbientThermistorIs(Valid);
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 + 1);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 + 1);
   GivenInitializationNotCaringAboutSetpoint();

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(noCoolingModeAndHighAmbientFanData->superLowSpeed.rpm);
}

TEST(FanSpeedResolver_NoCoolingModeAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedAtHighAmbientHumidityAndHighAmbientTemperatureWhenAmbientSensorsAreGreaterThanTriggerPoints)
{
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 + 1);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 + 1);
   GivenAmbientSensorsAreValid();
   GivenInitializationNotCaringAboutSetpoint();

   WhenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(condenserFanData->superLowAtHighAmbientHumidityAndHighTemperature.rpm);
}

TEST(FanSpeedResolver_NoCoolingModeAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedAtHighAmbientHumidityAndHighAmbientTemperatureWhenAmbientHumiditySensorBecomesInvalidAndAmbientSensorsAreGreaterThanTriggerPoints)
{
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 + 1);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100 + 1);
   GivenAmbientHumiditySensorIsInvalidAndAmbientThermistorIsValid();
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
   GivenAmbientSensorsAreValid();
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
   GivenAmbientHumiditySensorIsValidAndAmbientThermistorIsInvalid();
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
   GivenAmbientSensorsAreValid();
   GivenInitializationNotCaringAboutSetpoint();

   WhenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(condenserFanData->superLowAtHighAmbientHumidityAndHighTemperature.rpm);

   WhenAmbientThermistorSensorIs(Invalid);
   CalculatedFanControlSpeedShouldBe(noCoolingModeAndHighAmbientFanData->superLowSpeed.rpm);
}

TEST(FanSpeedResolver_NoCoolingModeAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedAtHighAmbientHumidityAndLowAmbientTemperatureWhenAmbientTemperatureDropsBelowTriggerTempWhileAmbientHumidityIsEqualToTriggerHumidityAndAmbientSensorsAreValid)
{
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100);
   GivenAmbientSensorsAreValid();
   GivenInitializationNotCaringAboutSetpoint();

   WhenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(condenserFanData->superLowAtHighAmbientHumidityAndHighTemperature.rpm);

   WhenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100 - 1);
   CalculatedFanControlSpeedShouldBe(condenserFanData->superLowAtHighAmbientHumidityAndLowTemperature.rpm);
}

TEST(FanSpeedResolver_NoCoolingModeAndHighAmbient, ShouldSetCalculatedSpeedToSuperLowSpeedWhenAmbientHumidityDropsBelowTriggerTempWhileAmbientTemperatureIsEqualToTriggerTempAndAmbientSensorsAreValid)
{
   GivenAmbientTemperatureIs(condenserFanData->highAmbientTriggerTemperatureInDegFx100);
   GivenAmbientHumidityIs(condenserFanData->highAmbientTriggerHumidityInPercentx100);
   GivenAmbientSensorsAreValid();
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
   GivenAmbientSensorsAreValid();
   GivenInitializationNotCaringAboutSetpoint();

   WhenTheCondenserFanAntiSweatBehaviorIs(DISABLED);
   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(condenserFanData->speedData->superLowSpeed.rpm);
}

TEST(FanSpeedResolver_NoCoolingModeAndHighAmbient, ShouldSetFanControlTypeToRpmAndFanControlSpeedToResolvedFanSpeedIfVotedSpeedIsGreaterThanOrEqualToNumberOfFanSpeeds)
{
   GivenAmbientHumiditySensorIs(Valid);
   GivenAmbientThermistorIs(Valid);
   GivenInitializationNotCaringAboutSetpoint();

   WhenResolvedFanSpeedVoteIs(FanSpeed_NumberOfSpeeds);
   CalculatedFanControlTypeShouldBe(FanControlType_Rpm);
   CalculatedFanControlSpeedShouldBe(FanSpeed_NumberOfSpeeds);

   WhenResolvedFanSpeedVoteIs(FanSpeed_NumberOfSpeeds + 1);
   CalculatedFanControlTypeShouldBe(FanControlType_Rpm);
   CalculatedFanControlSpeedShouldBe(FanSpeed_NumberOfSpeeds + 1);
}

TEST(FanSpeedResolver_NoCoolingModeAndHighAmbient, ShouldSetCalculatedFanControlTypeToDutyCycleAndCalculatedFanControlSpeedToZeroWhenRequestedOff)
{
   GivenAmbientHumiditySensorIs(Valid);
   GivenAmbientThermistorIs(Valid);
   GivenInitializationNotCaringAboutSetpoint();

   WhenResolvedFanSpeedVoteIs(FanSpeed_NumberOfSpeeds);
   CalculatedFanControlTypeShouldBe(FanControlType_Rpm);
   CalculatedFanControlSpeedShouldBe(FanSpeed_NumberOfSpeeds);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Off);
   CalculatedFanControlTypeShouldBe(FanControlType_DutyCycle);
   CalculatedFanControlSpeedShouldBe(0);
}
