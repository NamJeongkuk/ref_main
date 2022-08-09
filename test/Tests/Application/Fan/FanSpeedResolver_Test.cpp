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
#include "FreezerSetpoint.h"
#include "CoolingMode.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "DataModel_TestDouble.h"
#include "uassert_test.h"

enum
{
   Erd_ResolvedSpeedVote,
   Erd_CoolingMode,
   Erd_FreezerSetpoint,
   Erd_CalculatedRequestFanControlSpeed,
   Erd_AmbientTemperature,

   SuperLowSpeed = 0,
   LowSpeed = 10,
   MediumSpeed = 1000,
   HighSpeed = 2000,
   MaxSpeed = 2500,

   SuperLowSpeedHighAmbientTemp = SuperLowSpeed + 1,
   LowSpeedFreshFood = LowSpeed + 10,
   LowSpeedFreezer = LowSpeed + 20,
   LowSpeedConvertibleCompartment = LowSpeed + 30,
   MediumSpeedFreshFood = MediumSpeed + 10,
   MediumSpeedFreezer = MediumSpeed + 20,
   MediumSpeedConvertibleCompartment = MediumSpeed + 30,
   HighSpeedFreshFood = HighSpeed + 10,
   HighSpeedFreezer = HighSpeed + 20,
   HighSpeedConvertibleCompartment = HighSpeed + 30,

   LowColdTempSpeedFreshFood = LowSpeedFreshFood + 10,
   LowColdTempSpeedFreezer = LowSpeedFreezer + 20,
   LowColdTempSpeedConvertibleCompartment = LowSpeedConvertibleCompartment + 30,
   LowMediumTempSpeedFreezer = LowColdTempSpeedFreezer + 20,
   LowMediumTempSpeedConvertibleCompartment = LowColdTempSpeedConvertibleCompartment + 30,
   LowWarmTempSpeedFreezer = LowMediumTempSpeedFreezer + 20,
   LowWarmTempSpeedConvertibleCompartment = LowMediumTempSpeedConvertibleCompartment + 30,

   HighAmbientTempDegFx100 = 4500
};

static const DataModel_TestDoubleConfigurationEntry_t erds[] = {
   { Erd_ResolvedSpeedVote, sizeof(FanVotedSpeed_t) },
   { Erd_CoolingMode, sizeof(CoolingMode_t) },
   { Erd_FreezerSetpoint, sizeof(FreezerSetpoint_t) },
   { Erd_CalculatedRequestFanControlSpeed, sizeof(FanControl_t) },
   { Erd_AmbientTemperature, sizeof(TemperatureDegFx100_t) },
};

static const PidControllerGains_t gains = {
   .kpValue = {
      .value = 4,
      .qBits = 0,
   },
   .kiValue = {
      .value = 4,
      .qBits = 0,
   },
   .kdValue = {
      .value = 4,
      .qBits = 0,
   }
};

static const FanSpeedData_t speedData = {
   .careAboutHighAmbientTemperature = true,
   .superLowSpeed = {
      .type = FanControlType_Rpm,
      .rpm = SuperLowSpeed,
   },
   .lowSpeed = {
      .type = FanControlType_Rpm,
      .rpm = LowSpeed,
   },
   .mediumSpeed = {
      .type = FanControlType_Rpm,
      .rpm = MediumSpeed,
   },
   .highSpeed = {
      .type = FanControlType_Rpm,
      .rpm = HighSpeed,
   },
   .superHighSpeed = {
      .type = FanControlType_Rpm,
      .rpm = MaxSpeed,
   },
   .superLowSpeedHighAmbientTemperature = {
      .type = FanControlType_Rpm,
      .rpm = SuperLowSpeedHighAmbientTemp,
   }
};

static const FanCareAboutCoolingModeSpeedData_t speedDataNoCareAboutSetpoint = {
   .careAboutSetpoint = false,
   .careAboutSetpointData = {
      .nonSetpointSpeeds = {
         .superLowSpeed = {
            .type = FanControlType_Rpm,
            .rpm = SuperLowSpeed,
         },
         .lowSpeedFreshFood = {
            .type = FanControlType_Rpm,
            .rpm = LowSpeedFreshFood,
         },
         .lowSpeedFreezer = {
            .type = FanControlType_Rpm,
            .rpm = LowSpeedFreezer,
         },
         .mediumSpeedFreshFood = {
            .type = FanControlType_Rpm,
            .rpm = MediumSpeedFreshFood,
         },
         .mediumSpeedFreezer = {
            .type = FanControlType_Rpm,
            .rpm = MediumSpeedFreezer,
         },
         .highSpeedSpeedFreshFood = {
            .type = FanControlType_Rpm,
            .rpm = HighSpeedFreshFood,
         },
         .highSpeedSpeedFreezer = {
            .type = FanControlType_Rpm,
            .rpm = HighSpeedFreezer,
         },
         .superHighSpeed = {
            .type = FanControlType_Rpm,
            .rpm = MaxSpeed,
         },
      },
   },
};

static const FanCareAboutCoolingModeSpeedData_t speedDataCareAboutSetpoint = {
   .careAboutSetpoint = true,
   .careAboutSetpointData = {
      .setpointSpeeds = {
         .superLowSpeed = {
            .type = FanControlType_Rpm,
            .rpm = SuperLowSpeed,
         },
         .lowSpeedFreshFood = {
            .type = FanControlType_Rpm,
            .rpm = LowColdTempSpeedFreshFood,
         },
         .lowSpeedFreezerWithColdSetpoint = {
            .type = FanControlType_Rpm,
            .rpm = LowColdTempSpeedFreezer,
         },
         .lowSpeedFreezerWithMediumSetpoint = {
            .type = FanControlType_Rpm,
            .rpm = LowMediumTempSpeedFreezer,
         },
         .lowSpeedFreezerWithWarmSetpoint = {
            .type = FanControlType_Rpm,
            .rpm = LowWarmTempSpeedFreezer,
         },
         .lowSpeedConvertibleCompartmentWithColdSetpoint = {
            .type = FanControlType_Rpm,
            .rpm = LowColdTempSpeedConvertibleCompartment,
         },
         .lowSpeedConvertibleCompartmentWithMediumSetpoint = {
            .type = FanControlType_Rpm,
            .rpm = LowMediumTempSpeedConvertibleCompartment,
         },
         .lowSpeedConvertibleCompartmentWithWarmSetpoint = {
            .type = FanControlType_Rpm,
            .rpm = LowWarmTempSpeedConvertibleCompartment,
         },
         .mediumSpeedFreshFood = {
            .type = FanControlType_Rpm,
            .rpm = MediumSpeedFreshFood,
         },
         .mediumSpeedFreezer = {
            .type = FanControlType_Rpm,
            .rpm = MediumSpeedFreezer,
         },
         .mediumSpeedConvertibleCompartment = {
            .type = FanControlType_Rpm,
            .rpm = MediumSpeedConvertibleCompartment,
         },
         .highSpeedSpeedFreshFood = {
            .type = FanControlType_Rpm,
            .rpm = HighSpeedFreshFood,
         },
         .highSpeedSpeedFreezer = {
            .type = FanControlType_Rpm,
            .rpm = HighSpeedFreezer,
         },
         .highSpeedSpeedConvertibleCompartment = {
            .type = FanControlType_Rpm,
            .rpm = HighSpeedConvertibleCompartment,
         },
         .superHighSpeed = {
            .type = FanControlType_Rpm,
            .rpm = MaxSpeed,
         },
      },
   },
};

static const FanData_t speedDataWithoutCoolingMode = {
   .fanId = 2,
   .pulsesPerRevolution = 2,
   .careAboutCoolingMode = false,
   .powerUsageInWatts = 8,
   .gains = gains,
   .lowerLimit = 0,
   .upperLimit = 0xFFFF,
   .fanMissedTargetFaultTimeoutInSeconds = 60,
   .missingFanFeedbackFaultTimeoutInSeconds = 60,
   .feedbackPresentWhenFanIsOffTimeoutInSeconds = 60,
   .speedData = speedData
};

static const FanData_t coolingModeSpeedDataNoCareAboutSetpoint = {
   .fanId = 2,
   .pulsesPerRevolution = 2,
   .careAboutCoolingMode = true,
   .powerUsageInWatts = 8,
   .gains = gains,
   .lowerLimit = 0xFFFF,
   .upperLimit = 0,
   .fanMissedTargetFaultTimeoutInSeconds = 60,
   .missingFanFeedbackFaultTimeoutInSeconds = 60,
   .feedbackPresentWhenFanIsOffTimeoutInSeconds = 60,
   .careAboutCoolingModeSpeedData = speedDataNoCareAboutSetpoint
};

static const FanData_t coolingModeSpeedDataCareAboutSetpoint = {
   .fanId = 2,
   .pulsesPerRevolution = 2,
   .careAboutCoolingMode = true,
   .powerUsageInWatts = 8,
   .gains = gains,
   .lowerLimit = 0,
   .upperLimit = 0xFFFF,
   .fanMissedTargetFaultTimeoutInSeconds = 60,
   .missingFanFeedbackFaultTimeoutInSeconds = 60,
   .feedbackPresentWhenFanIsOffTimeoutInSeconds = 60,
   .careAboutCoolingModeSpeedData = speedDataCareAboutSetpoint
};

static const FanSpeedResolverConfig_t nonCoolingModeConfig = {
   .resolvedFanSpeedVoteErd = Erd_ResolvedSpeedVote,
   .coolingModeErd = Erd_CoolingMode,
   .freezerSetpointErd = Erd_FreezerSetpoint,
   .calculatedRequestFanControlErd = Erd_CalculatedRequestFanControlSpeed,
   .ambientTempErd = Erd_AmbientTemperature,
};

static const FanSpeedResolverConfig_t coolingModeConfig = {
   .resolvedFanSpeedVoteErd = Erd_ResolvedSpeedVote,
   .coolingModeErd = Erd_CoolingMode,
   .freezerSetpointErd = Erd_FreezerSetpoint,
   .calculatedRequestFanControlErd = Erd_CalculatedRequestFanControlSpeed,
   .ambientTempErd = Erd_AmbientTemperature,
};

TEST_GROUP(FanSpeedResolver_NoCoolingMode)
{
   DataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;
   FanSpeedResolver_t instance;

   void setup()
   {
      DataModel_TestDouble_Init(&dataModelTestDouble, erds, NUM_ELEMENTS(erds));
      dataModel = dataModelTestDouble.dataModel;
   }

   void GivenInitialization()
   {
      FanSpeedResolver_Init(
         &instance,
         dataModel,
         &speedDataWithoutCoolingMode,
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
   CalculatedFanControlSpeedShouldBe(LowSpeed);
}

TEST(FanSpeedResolver_NoCoolingMode, CalculatedSpeedShouldBeMediumWhenRequestedMedium)
{
   GivenInitialization();

   WhenResolvedFanSpeedVoteIs(FanSpeed_Medium);
   CalculatedFanControlSpeedShouldBe(MediumSpeed);
}

TEST(FanSpeedResolver_NoCoolingMode, CalculatedSpeedShouldBeHighWhenRequestedHigh)
{
   GivenInitialization();

   WhenResolvedFanSpeedVoteIs(FanSpeed_High);
   CalculatedFanControlSpeedShouldBe(HighSpeed);
}

TEST(FanSpeedResolver_NoCoolingMode, CalculatedSpeedShouldBeMaxWhenRequestedSuperHigh)
{
   GivenInitialization();

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperHigh);
   CalculatedFanControlSpeedShouldBe(MaxSpeed);
}

TEST(FanSpeedResolver_NoCoolingMode, CalculatedSpeedShouldBeSuperLowWhenRequestedSuperLow)
{
   GivenInitialization();

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(SuperLowSpeed);
}

TEST(FanSpeedResolver_NoCoolingMode, CalculatedSpeedShouldBeSuperLowWithHighAmbientTemperatureWhenRequestedSuperLowAndAmbientTemperatureIsHigh)
{
   GivenInitialization();
   GivenAmbientTemperatureIsSetTo(HighAmbientTempDegFx100);

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(SuperLowSpeedHighAmbientTemp);
}

TEST(FanSpeedResolver_NoCoolingMode, CalculatedSpeedShouldBeSuperLowWhenRequestedIsSuperLowAndAmbientTemperatureIsBelowThreshold)
{
   GivenInitialization();
   GivenAmbientTemperatureIsSetTo(HighAmbientTempDegFx100 - 1);

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperLow);
   CalculatedFanControlSpeedShouldBe(SuperLowSpeed);
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

   void setup()
   {
      DataModel_TestDouble_Init(&dataModelTestDouble, erds, NUM_ELEMENTS(erds));
      dataModel = dataModelTestDouble.dataModel;
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

   void GivenFreezerSetpointIsSetTo(FreezerSetpoint_t setpoint)
   {
      DataModel_Write(dataModel, Erd_FreezerSetpoint, &setpoint);
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
         &coolingModeSpeedDataNoCareAboutSetpoint,
         &coolingModeConfig);
   }

   void GivenInitializationCaringAboutSetpoint()
   {
      FanSpeedResolver_Init(
         &instance,
         dataModel,
         &coolingModeSpeedDataCareAboutSetpoint,
         &coolingModeConfig);
   }
};

TEST(FanSpeedResolver_CoolingMode, CalculatedSpeedShouldBeLowFreshFoodWhenRequestedLowWithCoolingModeSetToFreshFood)
{
   GivenInitializationWithoutCaringAboutSetpoint();
   GivenCoolingModeIs(CoolingMode_FreshFood);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(LowSpeedFreshFood);
}

TEST(FanSpeedResolver_CoolingMode, CalculatedSpeedShouldBeLowFreezerWhenRequestedLowWithCoolingModeSetToFreezer)
{
   GivenInitializationWithoutCaringAboutSetpoint();
   GivenCoolingModeIs(CoolingMode_Freezer);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(LowSpeedFreezer);
}

TEST(FanSpeedResolver_CoolingMode, CalculatedSpeedShouldBeMediumFreezerWhenRequestedMediumWithCoolingModeSetToFreezer)
{
   GivenInitializationWithoutCaringAboutSetpoint();
   GivenCoolingModeIs(CoolingMode_Freezer);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Medium);
   CalculatedFanControlSpeedShouldBe(MediumSpeedFreezer);
}

TEST(FanSpeedResolver_CoolingMode, CalculatedSpeedShouldBeHighFreshFoodWhenRequestedHighWithCoolingModeSetToFreshFood)
{
   GivenInitializationWithoutCaringAboutSetpoint();
   GivenCoolingModeIs(CoolingMode_FreshFood);

   WhenResolvedFanSpeedVoteIs(FanSpeed_High);
   CalculatedFanControlSpeedShouldBe(HighSpeedFreshFood);
}

TEST(FanSpeedResolver_CoolingMode, CalculatedSpeedShouldBeHighFreezerWhenRequestedHighWithCoolingModeSetToFreezer)
{
   GivenInitializationWithoutCaringAboutSetpoint();
   GivenCoolingModeIs(CoolingMode_Freezer);

   WhenResolvedFanSpeedVoteIs(FanSpeed_High);
   CalculatedFanControlSpeedShouldBe(HighSpeedFreezer);
}

TEST(FanSpeedResolver_CoolingMode, CalculatedSpeedShouldBeMaxWhenRequestedIsSuperHigh)
{
   GivenInitializationWithoutCaringAboutSetpoint();

   WhenResolvedFanSpeedVoteIs(FanSpeed_SuperHigh);
   CalculatedFanControlSpeedShouldBe(MaxSpeed);
}

TEST(FanSpeedResolver_CoolingMode, CalculatedSpeedShouldBeLowColdFreshFoodWhenRequestedLowWithCoolingModeSetToFreshFoodAndSetPointIsCold)
{
   GivenInitializationCaringAboutSetpoint();
   GivenFreezerSetpointIsSetTo(FreezerSetpoint_Cold);
   GivenCoolingModeIs(CoolingMode_FreshFood);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(LowColdTempSpeedFreshFood);
}

TEST(FanSpeedResolver_CoolingMode, CalculatedSpeedShouldBeLowColdFreezerWhenRequestedLowWithCoolingModeSetToFreezerAndSetPointIsCold)
{
   GivenInitializationCaringAboutSetpoint();
   GivenFreezerSetpointIsSetTo(FreezerSetpoint_Cold);
   GivenCoolingModeIs(CoolingMode_Freezer);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(LowColdTempSpeedFreezer);
}

TEST(FanSpeedResolver_CoolingMode, CalculatedSpeedShouldBeLowColdConvertibleCompartmentWhenRequestedLowWithCoolingModeSetToConvertibleCompartmentAndSetPointIsCold)
{
   GivenInitializationCaringAboutSetpoint();
   GivenFreezerSetpointIsSetTo(FreezerSetpoint_Cold);
   GivenCoolingModeIs(CoolingMode_ConvertibleCompartment);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(LowColdTempSpeedConvertibleCompartment);
}

TEST(FanSpeedResolver_CoolingMode, CalculatedSpeedShouldBeLowMediumFreezerWhenRequestedLowWithCoolingModeSetToFreezerAndSetPointIsMedium)
{
   GivenInitializationCaringAboutSetpoint();
   GivenFreezerSetpointIsSetTo(FreezerSetpoint_Medium);
   GivenCoolingModeIs(CoolingMode_Freezer);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(LowMediumTempSpeedFreezer);
}

TEST(FanSpeedResolver_CoolingMode, CalculatedSpeedShouldBeLowMediumConvertibleCompartmentWhenRequestedLowWithCoolingModeSetToConvertibleCompartmentAndSetPointIsMedium)
{
   GivenInitializationCaringAboutSetpoint();
   GivenFreezerSetpointIsSetTo(FreezerSetpoint_Medium);
   GivenCoolingModeIs(CoolingMode_ConvertibleCompartment);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(LowMediumTempSpeedConvertibleCompartment);
}

TEST(FanSpeedResolver_CoolingMode, CalculatedSpeedShouldBeLowWarmFreezerWhenRequestedLowWithCoolingModeSetToFreezerAndSetPointIsWarm)
{
   GivenInitializationCaringAboutSetpoint();
   GivenFreezerSetpointIsSetTo(FreezerSetpoint_Warm);
   GivenCoolingModeIs(CoolingMode_Freezer);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(LowWarmTempSpeedFreezer);
}

TEST(FanSpeedResolver_CoolingMode, CalculatedSpeedShouldBeLowWarmConvertibleCompartmentWhenRequestedLowWithCoolingModeSetToConvertibleCompartmentAndSetPointIsWarm)
{
   GivenInitializationCaringAboutSetpoint();
   GivenFreezerSetpointIsSetTo(FreezerSetpoint_Warm);
   GivenCoolingModeIs(CoolingMode_ConvertibleCompartment);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(LowWarmTempSpeedConvertibleCompartment);
}

TEST(FanSpeedResolver_CoolingMode, CalculatedSpeedShouldBeLowColdFreezerWhenRequestedLowWithDefaultCaseCoolingModeAndSetPointIsCold)
{
   GivenInitializationCaringAboutSetpoint();
   GivenFreezerSetpointIsSetTo(FreezerSetpoint_Cold);
   GivenCoolingModeIs(CoolingMode_Unknown);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(LowColdTempSpeedFreezer);
}

TEST(FanSpeedResolver_CoolingMode, CalculatedSpeedShouldBeLowMediumFreezerWhenRequestedLowWithDefaultCaseCoolingModeAndSetPointIsMedium)
{
   GivenInitializationCaringAboutSetpoint();
   GivenFreezerSetpointIsSetTo(FreezerSetpoint_Medium);
   GivenCoolingModeIs(CoolingMode_Unknown);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(LowMediumTempSpeedFreezer);
}

TEST(FanSpeedResolver_CoolingMode, CalculatedSpeedShouldBeLowWarmFreezerWhenRequestedLowWithDefaultCaseCoolingModeAndSetPointIsWarm)
{
   GivenInitializationCaringAboutSetpoint();
   GivenFreezerSetpointIsSetTo(FreezerSetpoint_Warm);
   GivenCoolingModeIs(CoolingMode_Unknown);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(LowWarmTempSpeedFreezer);
}

TEST(FanSpeedResolver_CoolingMode, CalculatedSpeedShouldBeLowFreezerWhenRequestedLowWithDefaultCaseCoolingMode)
{
   GivenInitializationWithoutCaringAboutSetpoint();
   GivenCoolingModeIs(CoolingMode_Unknown);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(LowSpeedFreezer);
}

TEST(FanSpeedResolver_CoolingMode, CalculatedSpeedShouldBeMediumFreezerWhenRequestedLowWithDefaultCaseCoolingMode)
{
   GivenInitializationWithoutCaringAboutSetpoint();
   GivenCoolingModeIs(CoolingMode_Unknown);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Medium);
   CalculatedFanControlSpeedShouldBe(MediumSpeedFreezer);
}

TEST(FanSpeedResolver_CoolingMode, CalculatedSpeedShouldBeHighFreezerWhenRequestedLowWithDefaultCaseCoolingMode)
{
   GivenInitializationWithoutCaringAboutSetpoint();
   GivenCoolingModeIs(CoolingMode_Unknown);

   WhenResolvedFanSpeedVoteIs(FanSpeed_High);
   CalculatedFanControlSpeedShouldBe(HighSpeedFreezer);
}
