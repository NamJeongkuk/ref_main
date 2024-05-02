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
   Erd_CalculatedRequestFanControlSpeed
};

static const DataModel_TestDoubleConfigurationEntry_t erds[] = {
   { Erd_ResolvedSpeedVote, sizeof(FanVotedSpeed_t) },
   { Erd_CoolingMode, sizeof(CoolingMode_t) },
   { Erd_FreezerSetpointZone, sizeof(SetpointZone_t) },
   { Erd_CalculatedRequestFanControlSpeed, sizeof(FanControl_t) }
};

static const FanSpeedResolverConfig_t nonCoolingModeConfig = {
   .resolvedFanSpeedVoteErd = Erd_ResolvedSpeedVote,
   .coolingModeErd = Erd_CoolingMode,
   .freezerSetpointErd = Erd_FreezerSetpointZone,
   .calculatedRequestFanControlErd = Erd_CalculatedRequestFanControlSpeed
};

static const FanSpeedResolverConfig_t coolingModeConfig = {
   .resolvedFanSpeedVoteErd = Erd_ResolvedSpeedVote,
   .coolingModeErd = Erd_CoolingMode,
   .freezerSetpointErd = Erd_FreezerSetpointZone,
   .calculatedRequestFanControlErd = Erd_CalculatedRequestFanControlSpeed
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

TEST_GROUP(FanSpeedResolver_CoolingModeWithSetpoint)
{
   DataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;
   FanSpeedResolver_t instance;

   PersonalityParametricData_t *parametricData;
   const CombinedFanData_t *combinedFanData;
   const FanData_t *condenserFanData;
   const FanCareAboutCoolingModeSpeedData_t *coolingModeWithSetpointFanData;

   void setup()
   {
      DataModel_TestDouble_Init(&dataModelTestDouble, erds, NUM_ELEMENTS(erds));
      dataModel = dataModelTestDouble.dataModel;

      parametricData = (PersonalityParametricData_t *)GivenThatTheApplicationParametricDataHasBeenLoadedIntoAPointer(TddPersonality_DevelopmentSingleEvaporator);
      combinedFanData = parametricData->fanData;
      condenserFanData = &combinedFanData->condenserFan;
      coolingModeWithSetpointFanData = combinedFanData->condenserFan.careAboutCoolingModeSpeedData;
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

   void GivenFreezerSetpointIsSetTo(SetpointZone_t setpointZone)
   {
      DataModel_Write(dataModel, Erd_FreezerSetpointZone, &setpointZone);
   }

   void WhenFreezerSetpointChangesTo(SetpointZone_t setpointZone)
   {
      GivenFreezerSetpointIsSetTo(setpointZone);
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

   void CalculatedFanControlTypeShouldBe(FanControlType_t expectedType)
   {
      FanControl_t actual;
      DataModel_Read(dataModel, Erd_CalculatedRequestFanControlSpeed, &actual);

      CHECK_EQUAL(expectedType, actual.type);
   }
};

TEST(FanSpeedResolver_CoolingModeWithSetpoint, ShouldSetCalculatedSpeedToSuperLowWhenRequestedSuperLowWithCoolingModeSetToFreshFoodAndSetPointIsCold)
{
   GivenInitializationCaringAboutSetpoint();
   GivenFreezerSetpointIsSetTo(SetpointZone_Cold);
   GivenCoolingModeIs(CoolingMode_FreshFood);

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(coolingModeWithSetpointFanData->careAboutSetpointData.setpointSpeeds.superLowSpeed.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpoint, ShouldSetCalculatedSpeedToLowColdFreshFoodWhenRequestedLowWithCoolingModeSetToFreshFoodAndSetPointIsCold)
{
   GivenInitializationCaringAboutSetpoint();
   GivenFreezerSetpointIsSetTo(SetpointZone_Cold);
   GivenCoolingModeIs(CoolingMode_FreshFood);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(coolingModeWithSetpointFanData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpoint, ShouldSetCalculatedSpeedToLowColdFreezerWhenRequestedLowWithCoolingModeSetToFreezerAndSetPointIsCold)
{
   GivenInitializationCaringAboutSetpoint();
   GivenFreezerSetpointIsSetTo(SetpointZone_Cold);
   GivenCoolingModeIs(CoolingMode_Freezer);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(coolingModeWithSetpointFanData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpoint, ShouldSetCalculatedSpeedToLowColdConvertibleCompartmentWhenRequestedLowWithCoolingModeSetToConvertibleCompartmentAndSetPointIsCold)
{
   GivenInitializationCaringAboutSetpoint();
   GivenFreezerSetpointIsSetTo(SetpointZone_Cold);
   GivenCoolingModeIs(CoolingMode_ConvertibleCompartment);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(coolingModeWithSetpointFanData->careAboutSetpointData.setpointSpeeds.lowSpeedConvertibleCompartmentWithColdSetpoint.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpoint, ShouldSetCalculatedSpeedToLowMediumFreezerWhenRequestedLowWithCoolingModeSetToFreezerAndSetPointIsMedium)
{
   GivenInitializationCaringAboutSetpoint();
   GivenFreezerSetpointIsSetTo(SetpointZone_Middle);
   GivenCoolingModeIs(CoolingMode_Freezer);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(coolingModeWithSetpointFanData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithMediumSetpoint.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpoint, ShouldSetCalculatedSpeedToLowMediumConvertibleCompartmentWhenRequestedLowWithCoolingModeSetToConvertibleCompartmentAndSetPointIsMedium)
{
   GivenInitializationCaringAboutSetpoint();
   GivenFreezerSetpointIsSetTo(SetpointZone_Middle);
   GivenCoolingModeIs(CoolingMode_ConvertibleCompartment);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(coolingModeWithSetpointFanData->careAboutSetpointData.setpointSpeeds.lowSpeedConvertibleCompartmentWithMediumSetpoint.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpoint, ShouldSetCalculatedSpeedToLowWarmWhenFreezerSetpointChangesToSetPointZoneWarm)
{
   GivenInitializationCaringAboutSetpoint();
   GivenFreezerSetpointIsSetTo(SetpointZone_Middle);
   GivenCoolingModeIs(CoolingMode_ConvertibleCompartment);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(coolingModeWithSetpointFanData->careAboutSetpointData.setpointSpeeds.lowSpeedConvertibleCompartmentWithMediumSetpoint.rpm);

   WhenFreezerSetpointChangesTo(SetpointZone_Warm);
   CalculatedFanControlSpeedShouldBe(coolingModeWithSetpointFanData->careAboutSetpointData.setpointSpeeds.lowSpeedConvertibleCompartmentWithWarmSetpoint.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpoint, ShouldSetCalculatedSpeedToLowWarmFreezerWhenRequestedLowWithCoolingModeSetToFreezerAndSetPointIsWarm)
{
   GivenInitializationCaringAboutSetpoint();
   GivenFreezerSetpointIsSetTo(SetpointZone_Warm);
   GivenCoolingModeIs(CoolingMode_Freezer);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(coolingModeWithSetpointFanData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithWarmSetpoint.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpoint, ShouldSetCalculatedSpeedToLowWarmConvertibleCompartmentWhenRequestedLowWithCoolingModeSetToConvertibleCompartmentAndSetPointIsWarm)
{
   GivenInitializationCaringAboutSetpoint();
   GivenFreezerSetpointIsSetTo(SetpointZone_Warm);
   GivenCoolingModeIs(CoolingMode_ConvertibleCompartment);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(coolingModeWithSetpointFanData->careAboutSetpointData.setpointSpeeds.lowSpeedConvertibleCompartmentWithWarmSetpoint.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpoint, ShouldSetCalculatedSpeedToLowColdFreezerWhenRequestedLowWithDefaultCaseCoolingModeAndSetPointIsCold)
{
   GivenInitializationCaringAboutSetpoint();
   GivenFreezerSetpointIsSetTo(SetpointZone_Cold);
   GivenCoolingModeIs(CoolingMode_Off);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(coolingModeWithSetpointFanData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpoint, ShouldSetCalculatedSpeedToLowMediumFreezerWhenRequestedLowWithDefaultCaseCoolingModeAndSetPointIsMedium)
{
   GivenInitializationCaringAboutSetpoint();
   GivenFreezerSetpointIsSetTo(SetpointZone_Middle);
   GivenCoolingModeIs(CoolingMode_Off);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(coolingModeWithSetpointFanData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithMediumSetpoint.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpoint, ShouldSetCalculatedSpeedToLowWarmFreezerWhenRequestedLowWithDefaultCaseCoolingModeAndSetPointIsWarm)
{
   GivenInitializationCaringAboutSetpoint();
   GivenFreezerSetpointIsSetTo(SetpointZone_Warm);
   GivenCoolingModeIs(CoolingMode_Off);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(coolingModeWithSetpointFanData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithWarmSetpoint.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpoint, ShouldSetCalculatedSpeedToMediumFreshFoodWhenRequestedMediumWithCoolingModeSetToFreshFood)
{
   GivenInitializationCaringAboutSetpoint();
   GivenCoolingModeIs(CoolingMode_FreshFood);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Medium);
   CalculatedFanControlSpeedShouldBe(coolingModeWithSetpointFanData->careAboutSetpointData.setpointSpeeds.mediumSpeedFreshFood.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpoint, ShouldSetCalculatedSpeedToMediumFreezerWhenRequestedMediumWithCoolingModeSetToFreezer)
{
   GivenInitializationCaringAboutSetpoint();
   GivenCoolingModeIs(CoolingMode_Freezer);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Medium);
   CalculatedFanControlSpeedShouldBe(coolingModeWithSetpointFanData->careAboutSetpointData.setpointSpeeds.mediumSpeedFreezer.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpoint, ShouldSetCalculatedSpeedToMediumConvertibleCompartmentWhenRequestedMediumWithCoolingModeSetToConvertibleCompartment)
{
   GivenInitializationCaringAboutSetpoint();
   GivenCoolingModeIs(CoolingMode_ConvertibleCompartment);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Medium);
   CalculatedFanControlSpeedShouldBe(coolingModeWithSetpointFanData->careAboutSetpointData.setpointSpeeds.mediumSpeedConvertibleCompartment.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpoint, ShouldSetCalculatedSpeedToHighFreshFoodWhenRequestedHighWithCoolingModeSetToFreshFood)
{
   GivenInitializationCaringAboutSetpoint();
   GivenCoolingModeIs(CoolingMode_FreshFood);

   WhenResolvedFanSpeedVoteIs(FanSpeed_High);
   CalculatedFanControlSpeedShouldBe(coolingModeWithSetpointFanData->careAboutSetpointData.setpointSpeeds.highSpeedFreshFood.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpoint, ShouldSetCalculatedSpeedToHighFreezerWhenRequestedHighWithCoolingModeSetToFreezer)
{
   GivenInitializationCaringAboutSetpoint();
   GivenCoolingModeIs(CoolingMode_Freezer);

   WhenResolvedFanSpeedVoteIs(FanSpeed_High);
   CalculatedFanControlSpeedShouldBe(coolingModeWithSetpointFanData->careAboutSetpointData.setpointSpeeds.highSpeedFreezer.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpoint, ShouldSetCalculatedSpeedToHighConvertibleCompartmentWhenRequestedHighWithCoolingModeSetToConvertibleCompartment)
{
   GivenInitializationCaringAboutSetpoint();
   GivenCoolingModeIs(CoolingMode_ConvertibleCompartment);

   WhenResolvedFanSpeedVoteIs(FanSpeed_High);
   CalculatedFanControlSpeedShouldBe(coolingModeWithSetpointFanData->careAboutSetpointData.setpointSpeeds.highSpeedConvertibleCompartment.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpoint, ShouldSetCalculatedSpeedToSuperHighWhenRequestedSuperHigh)
{
   GivenInitializationCaringAboutSetpoint();

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperHigh);
   CalculatedFanControlSpeedShouldBe(coolingModeWithSetpointFanData->careAboutSetpointData.setpointSpeeds.superHighSpeed.rpm);
}

TEST(FanSpeedResolver_CoolingModeWithSetpoint, ShouldSetFanControlTypeToRpmAndFanControlSpeedToResolvedFanSpeedIfVotedSpeedIsGreaterThanOrEqualToNumberOfFanSpeeds)
{
   GivenInitializationCaringAboutSetpoint();

   WhenResolvedFanSpeedVoteIs(FanSpeed_NumberOfSpeeds);
   CalculatedFanControlTypeShouldBe(FanControlType_Rpm);
   CalculatedFanControlSpeedShouldBe(FanSpeed_NumberOfSpeeds);

   WhenResolvedFanSpeedVoteIs(FanSpeed_NumberOfSpeeds + 1);
   CalculatedFanControlTypeShouldBe(FanControlType_Rpm);
   CalculatedFanControlSpeedShouldBe(FanSpeed_NumberOfSpeeds + 1);
}

TEST(FanSpeedResolver_CoolingModeWithSetpoint, ShouldSetCalculatedFanControlTypeToDutyCycleAndCalculatedFanControlSpeedToZeroWhenRequestedOff)
{
   GivenInitializationCaringAboutSetpoint();

   WhenResolvedFanSpeedVoteIs(FanSpeed_NumberOfSpeeds);
   CalculatedFanControlTypeShouldBe(FanControlType_Rpm);
   CalculatedFanControlSpeedShouldBe(FanSpeed_NumberOfSpeeds);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Off);
   CalculatedFanControlTypeShouldBe(FanControlType_DutyCycle);
   CalculatedFanControlSpeedShouldBe(0);
}
