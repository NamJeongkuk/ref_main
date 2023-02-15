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
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "DataModel_TestDouble.h"
#include "uassert_test.h"
#include "PersonalityParametricDataTestDouble.h"
#include "TddPersonality.h"

enum
{
   Erd_ResolvedSpeedVote,
   Erd_CoolingMode,
   Erd_FreezerSetpointZone,
   Erd_CalculatedRequestFanControlSpeed,
   Erd_AmbientTemperature,

   HighAmbientTempDegFx100 = 4500
};

static const DataModel_TestDoubleConfigurationEntry_t erds[] = {
   { Erd_ResolvedSpeedVote, sizeof(FanVotedSpeed_t) },
   { Erd_CoolingMode, sizeof(CoolingMode_t) },
   { Erd_FreezerSetpointZone, sizeof(SetpointZone_t) },
   { Erd_CalculatedRequestFanControlSpeed, sizeof(FanControl_t) },
   { Erd_AmbientTemperature, sizeof(TemperatureDegFx100_t) },
};

static const FanSpeedResolverConfig_t nonCoolingModeConfig = {
   .resolvedFanSpeedVoteErd = Erd_ResolvedSpeedVote,
   .coolingModeErd = Erd_CoolingMode,
   .freezerSetpointErd = Erd_FreezerSetpointZone,
   .calculatedRequestFanControlErd = Erd_CalculatedRequestFanControlSpeed,
   .ambientTempErd = Erd_AmbientTemperature,
};

static const FanSpeedResolverConfig_t coolingModeConfig = {
   .resolvedFanSpeedVoteErd = Erd_ResolvedSpeedVote,
   .coolingModeErd = Erd_CoolingMode,
   .freezerSetpointErd = Erd_FreezerSetpointZone,
   .calculatedRequestFanControlErd = Erd_CalculatedRequestFanControlSpeed,
   .ambientTempErd = Erd_AmbientTemperature,
};

TEST_GROUP(FanSpeedResolver_NoCoolingMode)
{
   DataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;
   FanSpeedResolver_t instance;

   PersonalityParametricData_t *parametricData;
   const CombinedFanData_t *combinedFanData;
   const FanData_t *nonCoolingModeFanData;

   void setup()
   {
      DataModel_TestDouble_Init(&dataModelTestDouble, erds, NUM_ELEMENTS(erds));
      dataModel = dataModelTestDouble.dataModel;

      parametricData = (PersonalityParametricData_t *)GivenThatTheApplicationParametricDataHasBeenLoadedIntoAPointer(TddPersonality_DevelopmentSingleEvaporator);
      combinedFanData = parametricData->fanData;
      nonCoolingModeFanData = &combinedFanData->convertibleCompartmentFan;
   }

   void GivenInitialization()
   {
      FanSpeedResolver_Init(
         &instance,
         dataModel,
         nonCoolingModeFanData,
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

TEST(FanSpeedResolver_NoCoolingMode, CalculatedSpeedShouldBeLowWhenRequestedLow)
{
   GivenInitialization();

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(nonCoolingModeFanData->speedData.lowSpeed.rpm);
}

TEST(FanSpeedResolver_NoCoolingMode, CalculatedSpeedShouldBeMediumWhenRequestedMedium)
{
   GivenInitialization();

   WhenResolvedFanSpeedVoteIs(FanSpeed_Medium);
   CalculatedFanControlSpeedShouldBe(nonCoolingModeFanData->speedData.mediumSpeed.rpm);
}

TEST(FanSpeedResolver_NoCoolingMode, CalculatedSpeedShouldBeHighWhenRequestedHigh)
{
   GivenInitialization();

   WhenResolvedFanSpeedVoteIs(FanSpeed_High);
   CalculatedFanControlSpeedShouldBe(nonCoolingModeFanData->speedData.highSpeed.rpm);
}

TEST(FanSpeedResolver_NoCoolingMode, CalculatedSpeedShouldBeMaxWhenRequestedSuperHigh)
{
   GivenInitialization();

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperHigh);
   CalculatedFanControlSpeedShouldBe(nonCoolingModeFanData->speedData.superHighSpeed.rpm);
}

TEST(FanSpeedResolver_NoCoolingMode, CalculatedSpeedShouldBeSuperLowWhenRequestedSuperLow)
{
   GivenInitialization();

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(nonCoolingModeFanData->speedData.superLowSpeed.rpm);
}

TEST(FanSpeedResolver_NoCoolingMode, CalculatedSpeedShouldBeSuperLowWithHighAmbientTemperatureWhenRequestedSuperLowAndAmbientTemperatureIsHigh)
{
   GivenInitialization();
   GivenAmbientTemperatureIsSetTo(HighAmbientTempDegFx100);

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(nonCoolingModeFanData->speedData.superLowSpeedHighAmbientTemperature.rpm);
}

TEST(FanSpeedResolver_NoCoolingMode, CalculatedSpeedShouldBeSuperLowWhenRequestedIsSuperLowAndAmbientTemperatureIsBelowThreshold)
{
   GivenInitialization();
   GivenAmbientTemperatureIsSetTo(HighAmbientTempDegFx100 - 1);

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(nonCoolingModeFanData->speedData.superLowSpeed.rpm);
}

TEST(FanSpeedResolver_NoCoolingMode, CalculatedSpeedShouldBeZeroOnInitialization)
{
   GivenInitialization();
   CalculatedFanControlSpeedShouldBe(0);
}

TEST_GROUP(FanSpeedResolver_CoolingMode)
{
   DataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;
   FanSpeedResolver_t instance;

   PersonalityParametricData_t *parametricData;
   const CombinedFanData_t *combinedFanData;
   const FanCareAboutCoolingModeSpeedData_t *coolingModeFanData;
   const FanCareAboutCoolingModeSpeedData_t *coolingModeCareSetpointData;

   void setup()
   {
      DataModel_TestDouble_Init(&dataModelTestDouble, erds, NUM_ELEMENTS(erds));
      dataModel = dataModelTestDouble.dataModel;

      parametricData = (PersonalityParametricData_t *)GivenThatTheApplicationParametricDataHasBeenLoadedIntoAPointer(TddPersonality_DevelopmentSingleEvaporator);
      combinedFanData = parametricData->fanData;
      coolingModeFanData = &combinedFanData->freshFoodEvapFan.careAboutCoolingModeSpeedData;
      coolingModeCareSetpointData = &combinedFanData->freezerEvapFan.careAboutCoolingModeSpeedData;
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

   void GivenInitializationCaringAboutSetpoint()
   {
      FanSpeedResolver_Init(
         &instance,
         dataModel,
         &combinedFanData->freezerEvapFan,
         &coolingModeConfig);
   }
};

TEST(FanSpeedResolver_CoolingMode, CalculatedSpeedShouldBeLowFreshFoodWhenRequestedLowWithCoolingModeSetToFreshFood)
{
   GivenInitializationWithoutCaringAboutSetpoint();
   GivenCoolingModeIs(CoolingMode_FreshFood);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(coolingModeFanData->careAboutSetpointData.nonSetpointSpeeds.lowSpeedFreshFood.rpm);
}

TEST(FanSpeedResolver_CoolingMode, CalculatedSpeedShouldBeLowFreezerWhenRequestedLowWithCoolingModeSetToFreezer)
{
   GivenInitializationWithoutCaringAboutSetpoint();
   GivenCoolingModeIs(CoolingMode_Freezer);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(coolingModeFanData->careAboutSetpointData.nonSetpointSpeeds.lowSpeedFreezer.rpm);
}

TEST(FanSpeedResolver_CoolingMode, CalculatedSpeedShouldBeMediumFreezerWhenRequestedMediumWithCoolingModeSetToFreezer)
{
   GivenInitializationWithoutCaringAboutSetpoint();
   GivenCoolingModeIs(CoolingMode_Freezer);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Medium);
   CalculatedFanControlSpeedShouldBe(coolingModeFanData->careAboutSetpointData.nonSetpointSpeeds.mediumSpeedFreezer.rpm);
}

TEST(FanSpeedResolver_CoolingMode, CalculatedSpeedShouldBeHighFreshFoodWhenRequestedHighWithCoolingModeSetToFreshFood)
{
   GivenInitializationWithoutCaringAboutSetpoint();
   GivenCoolingModeIs(CoolingMode_FreshFood);

   WhenResolvedFanSpeedVoteIs(FanSpeed_High);
   CalculatedFanControlSpeedShouldBe(coolingModeFanData->careAboutSetpointData.nonSetpointSpeeds.highSpeedSpeedFreshFood.rpm);
}

TEST(FanSpeedResolver_CoolingMode, CalculatedSpeedShouldBeHighFreezerWhenRequestedHighWithCoolingModeSetToFreezer)
{
   GivenInitializationWithoutCaringAboutSetpoint();
   GivenCoolingModeIs(CoolingMode_Freezer);

   WhenResolvedFanSpeedVoteIs(FanSpeed_High);
   CalculatedFanControlSpeedShouldBe(coolingModeFanData->careAboutSetpointData.nonSetpointSpeeds.highSpeedSpeedFreezer.rpm);
}

TEST(FanSpeedResolver_CoolingMode, CalculatedSpeedShouldBeMaxWhenRequestedIsSuperHigh)
{
   GivenInitializationWithoutCaringAboutSetpoint();

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperHigh);
   CalculatedFanControlSpeedShouldBe(coolingModeFanData->careAboutSetpointData.nonSetpointSpeeds.superHighSpeed.rpm);
}

TEST(FanSpeedResolver_CoolingMode, CalculatedSpeedShouldBeLowColdFreshFoodWhenRequestedLowWithCoolingModeSetToFreshFoodAndSetPointIsCold)
{
   GivenInitializationCaringAboutSetpoint();
   GivenFreezerSetpointIsSetTo(SetpointZone_Cold);
   GivenCoolingModeIs(CoolingMode_FreshFood);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(coolingModeCareSetpointData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood.rpm);
}

TEST(FanSpeedResolver_CoolingMode, CalculatedSpeedShouldBeLowColdFreezerWhenRequestedLowWithCoolingModeSetToFreezerAndSetPointIsCold)
{
   GivenInitializationCaringAboutSetpoint();
   GivenFreezerSetpointIsSetTo(SetpointZone_Cold);
   GivenCoolingModeIs(CoolingMode_Freezer);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(coolingModeCareSetpointData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint.rpm);
}

TEST(FanSpeedResolver_CoolingMode, CalculatedSpeedShouldBeLowColdConvertibleCompartmentWhenRequestedLowWithCoolingModeSetToConvertibleCompartmentAndSetPointIsCold)
{
   GivenInitializationCaringAboutSetpoint();
   GivenFreezerSetpointIsSetTo(SetpointZone_Cold);
   GivenCoolingModeIs(CoolingMode_ConvertibleCompartment);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(coolingModeCareSetpointData->careAboutSetpointData.setpointSpeeds.lowSpeedConvertibleCompartmentWithColdSetpoint.rpm);
}

TEST(FanSpeedResolver_CoolingMode, CalculatedSpeedShouldBeLowMediumFreezerWhenRequestedLowWithCoolingModeSetToFreezerAndSetPointIsMedium)
{
   GivenInitializationCaringAboutSetpoint();
   GivenFreezerSetpointIsSetTo(SetpointZone_Middle);
   GivenCoolingModeIs(CoolingMode_Freezer);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(coolingModeCareSetpointData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithMediumSetpoint.rpm);
}

TEST(FanSpeedResolver_CoolingMode, CalculatedSpeedShouldBeLowMediumConvertibleCompartmentWhenRequestedLowWithCoolingModeSetToConvertibleCompartmentAndSetPointIsMedium)
{
   GivenInitializationCaringAboutSetpoint();
   GivenFreezerSetpointIsSetTo(SetpointZone_Middle);
   GivenCoolingModeIs(CoolingMode_ConvertibleCompartment);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(coolingModeCareSetpointData->careAboutSetpointData.setpointSpeeds.lowSpeedConvertibleCompartmentWithMediumSetpoint.rpm);
}

TEST(FanSpeedResolver_CoolingMode, CalculatedSpeedShouldBeLowWarmFreezerWhenRequestedLowWithCoolingModeSetToFreezerAndSetPointIsWarm)
{
   GivenInitializationCaringAboutSetpoint();
   GivenFreezerSetpointIsSetTo(SetpointZone_Warm);
   GivenCoolingModeIs(CoolingMode_Freezer);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(coolingModeCareSetpointData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithWarmSetpoint.rpm);
}

TEST(FanSpeedResolver_CoolingMode, CalculatedSpeedShouldBeLowWarmConvertibleCompartmentWhenRequestedLowWithCoolingModeSetToConvertibleCompartmentAndSetPointIsWarm)
{
   GivenInitializationCaringAboutSetpoint();
   GivenFreezerSetpointIsSetTo(SetpointZone_Warm);
   GivenCoolingModeIs(CoolingMode_ConvertibleCompartment);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(coolingModeCareSetpointData->careAboutSetpointData.setpointSpeeds.lowSpeedConvertibleCompartmentWithWarmSetpoint.rpm);
}

TEST(FanSpeedResolver_CoolingMode, CalculatedSpeedShouldBeLowColdFreezerWhenRequestedLowWithDefaultCaseCoolingModeAndSetPointIsCold)
{
   GivenInitializationCaringAboutSetpoint();
   GivenFreezerSetpointIsSetTo(SetpointZone_Cold);
   GivenCoolingModeIs(CoolingMode_Off);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(coolingModeCareSetpointData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint.rpm);
}

TEST(FanSpeedResolver_CoolingMode, CalculatedSpeedShouldBeLowMediumFreezerWhenRequestedLowWithDefaultCaseCoolingModeAndSetPointIsMedium)
{
   GivenInitializationCaringAboutSetpoint();
   GivenFreezerSetpointIsSetTo(SetpointZone_Middle);
   GivenCoolingModeIs(CoolingMode_Off);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(coolingModeCareSetpointData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithMediumSetpoint.rpm);
}

TEST(FanSpeedResolver_CoolingMode, CalculatedSpeedShouldBeLowWarmFreezerWhenRequestedLowWithDefaultCaseCoolingModeAndSetPointIsWarm)
{
   GivenInitializationCaringAboutSetpoint();
   GivenFreezerSetpointIsSetTo(SetpointZone_Warm);
   GivenCoolingModeIs(CoolingMode_Off);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(coolingModeCareSetpointData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithWarmSetpoint.rpm);
}

TEST(FanSpeedResolver_CoolingMode, CalculatedSpeedShouldBeLowFreezerWhenRequestedLowWithDefaultCaseCoolingMode)
{
   GivenInitializationWithoutCaringAboutSetpoint();
   GivenCoolingModeIs(CoolingMode_Off);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(coolingModeCareSetpointData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint.rpm);
}

TEST(FanSpeedResolver_CoolingMode, CalculatedSpeedShouldBeMediumFreezerWhenRequestedLowWithDefaultCaseCoolingMode)
{
   GivenInitializationWithoutCaringAboutSetpoint();
   GivenCoolingModeIs(CoolingMode_Off);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Medium);
   CalculatedFanControlSpeedShouldBe(coolingModeCareSetpointData->careAboutSetpointData.setpointSpeeds.mediumSpeedFreezer.rpm);
}

TEST(FanSpeedResolver_CoolingMode, CalculatedSpeedShouldBeHighFreezerWhenRequestedLowWithDefaultCaseCoolingMode)
{
   GivenInitializationWithoutCaringAboutSetpoint();
   GivenCoolingModeIs(CoolingMode_Off);

   WhenResolvedFanSpeedVoteIs(FanSpeed_High);
   CalculatedFanControlSpeedShouldBe(coolingModeCareSetpointData->careAboutSetpointData.setpointSpeeds.highSpeedSpeedFreezer.rpm);
}

TEST(FanSpeedResolver_CoolingMode, ShouldChangeFanSpeedWhenCoolingModeChanges)
{
   GivenInitializationWithoutCaringAboutSetpoint();
   GivenCoolingModeIs(CoolingMode_FreshFood);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(coolingModeFanData->careAboutSetpointData.nonSetpointSpeeds.lowSpeedFreshFood.rpm);

   WhenCoolingModeBecomes(CoolingMode_Freezer);
   CalculatedFanControlSpeedShouldBe(coolingModeFanData->careAboutSetpointData.nonSetpointSpeeds.lowSpeedFreezer.rpm);
}
