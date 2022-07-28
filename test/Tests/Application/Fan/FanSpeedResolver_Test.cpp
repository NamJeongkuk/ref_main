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
#include "ValvePosition.h"
#include "TemperatureDegFx100.h"
#include "Setpoint.h"
#include "FreezerSetpoint.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "DataModel_TestDouble.h"
#include "uassert_test.h"

enum
{
   Erd_ResolvedSpeedVote,
   Erd_ResolvedValvePostionVote,
   Erd_FreezerSetpoint,
   Erd_CalculatedRequestFanControlSpeed,
   Erd_AmbientTemperature,

   MinSpeed = 0,
   LowSpeed = 10,
   MediumSpeed = 1000,
   HighSpeed = 2000,
   MaxSpeed = 2500,

   MinSpeedHighAmbientTemp = MinSpeed + 1,
   LowSpeedA = LowSpeed + 10,
   LowSpeedB = LowSpeed + 20,
   LowSpeedC = LowSpeed + 30,
   MediumSpeedA = MediumSpeed + 10,
   MediumSpeedB = MediumSpeed + 20,
   MediumSpeedC = MediumSpeed + 30,
   HighSpeedA = HighSpeed + 10,
   HighSpeedB = HighSpeed + 20,
   HighSpeedC = HighSpeed + 30,

   ColdTempSpeedA = LowSpeedA + 10,
   ColdTempSpeedB = LowSpeedB + 20,
   ColdTempSpeedC = LowSpeedC + 30,
   MediumTempSpeedA = ColdTempSpeedA + 10,
   MediumTempSpeedB = ColdTempSpeedB + 20,
   MediumTempSpeedC = ColdTempSpeedC + 30,
   WarmTempSpeedA = MediumTempSpeedA + 10,
   WarmTempSpeedB = MediumTempSpeedB + 20,
   WarmTempSpeedC = MediumTempSpeedC + 30,

   HighAmbientTempDegFx100 = 4500
};

static const DataModel_TestDoubleConfigurationEntry_t erds[] = {
   { Erd_ResolvedSpeedVote, sizeof(FanVotedSpeed_t) },
   { Erd_ResolvedValvePostionVote, sizeof(ValveVotedPosition_t) },
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

static const ValveIndependentFanData_t independentValveData = {
   .minSpeed = {
      .type = FanControlType_Rpm,
      .rpm = MinSpeed,
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
   .maxSpeed = {
      .type = FanControlType_Rpm,
      .rpm = MaxSpeed,
   }
};

static const ValveDependentFanData_t dependentValveDataNoCareAboutSetpoint = {
   .careAboutSetpoint = false,
   .minSpeed = {
      .type = FanControlType_Rpm,
      .rpm = MinSpeed,
   },
   .minSpeedHighAmbientTemperature = {
      .type = FanControlType_Rpm,
      .rpm = MinSpeedHighAmbientTemp,
   },
   .careAboutSetpointData = {
      .nonSetpointSpeeds = {
         .lowSpeedValveA = {
            .type = FanControlType_Rpm,
            .rpm = LowSpeedA,
         },
         .lowSpeedValveB = {
            .type = FanControlType_Rpm,
            .rpm = LowSpeedB,
         },
         .lowSpeedValveC = {
            .type = FanControlType_Rpm,
            .rpm = LowSpeedC,
         },
      },
   },
   .mediumSpeedValveA = {
      .type = FanControlType_Rpm,
      .rpm = MediumSpeedA,
   },
   .mediumSpeedValveB = {
      .type = FanControlType_Rpm,
      .rpm = MediumSpeedB,
   },
   .mediumSpeedValveC = {
      .type = FanControlType_Rpm,
      .rpm = MediumSpeedC,
   },
   .highSpeedValveA = {
      .type = FanControlType_Rpm,
      .rpm = HighSpeedA,
   },
   .highSpeedValveB = {
      .type = FanControlType_Rpm,
      .rpm = HighSpeedB,
   },
   .highSpeedValveC = {
      .type = FanControlType_Rpm,
      .rpm = HighSpeedC,
   },
   .maxSpeed = {
      .type = FanControlType_Rpm,
      .rpm = MaxSpeed,
   },
};

static const ValveDependentFanData_t dependentValveDataCareAboutSetpoint = {
   .careAboutSetpoint = true,
   .minSpeed = {
      .type = FanControlType_Rpm,
      .rpm = MinSpeed,
   },
   .minSpeedHighAmbientTemperature = {
      .type = FanControlType_Rpm,
      .rpm = MinSpeedHighAmbientTemp,
   },
   .careAboutSetpointData = {
      .setpointSpeeds = {
         .lowSpeedWithColdSetpointValveA = {
            .type = FanControlType_Rpm,
            .rpm = ColdTempSpeedA,
         },
         .lowSpeedWithColdSetpointValveB = {
            .type = FanControlType_Rpm,
            .rpm = ColdTempSpeedB,
         },
         .lowSpeedWithColdSetpointValveC = {
            .type = FanControlType_Rpm,
            .rpm = ColdTempSpeedC,
         },
         .lowSpeedWithMediumSetpointValveA = {
            .type = FanControlType_Rpm,
            .rpm = MediumTempSpeedA,
         },
         .lowSpeedWithMediumSetpointValveB = {
            .type = FanControlType_Rpm,
            .rpm = MediumTempSpeedB,
         },
         .lowSpeedWithMediumSetpointValveC = {
            .type = FanControlType_Rpm,
            .rpm = MediumTempSpeedC,
         },
         .lowSpeedWithWarmSetpointValveA = {
            .type = FanControlType_Rpm,
            .rpm = WarmTempSpeedA,
         },
         .lowSpeedWithWarmSetpointValveB = {
            .type = FanControlType_Rpm,
            .rpm = WarmTempSpeedB,
         },
         .lowSpeedWithWarmSetpointValveC = {
            .type = FanControlType_Rpm,
            .rpm = WarmTempSpeedC,
         },
      },
   },
   .mediumSpeedValveA = {
      .type = FanControlType_Rpm,
      .rpm = MediumSpeedA,
   },
   .mediumSpeedValveB = {
      .type = FanControlType_Rpm,
      .rpm = MediumSpeedB,
   },
   .mediumSpeedValveC = {
      .type = FanControlType_Rpm,
      .rpm = MediumSpeedC,
   },
   .highSpeedValveA = {
      .type = FanControlType_Rpm,
      .rpm = HighSpeedA,
   },
   .highSpeedValveB = {
      .type = FanControlType_Rpm,
      .rpm = HighSpeedB,
   },
   .highSpeedValveC = {
      .type = FanControlType_Rpm,
      .rpm = HighSpeedC,
   },
   .maxSpeed = {
      .type = FanControlType_Rpm,
      .rpm = MaxSpeed,
   },
};

static const ValveFanData_t independentValve = {
   .fanId = 2,
   .numberOfPoles = 2,
   .valveDependent = false,
   .powerUsageInWatts = 8,
   .gains = gains,
   .lowerLimit = 0,
   .upperLimit = 0xFFFF,
   .fanMissedTargetFaultTimeoutInSeconds = 60,
   .missingFanFeedbackFaultTimeoutInSeconds = 60,
   .feedbackPresentWhenFanIsOffTimeoutInSeconds = 60,
   .independentData = independentValveData
};

static const ValveFanData_t dependentValveNoCareAboutSetpoint = {
   .fanId = 2,
   .numberOfPoles = 2,
   .valveDependent = true,
   .powerUsageInWatts = 8,
   .gains = gains,
   .lowerLimit = 0xFFFF,
   .upperLimit = 0,
   .fanMissedTargetFaultTimeoutInSeconds = 60,
   .missingFanFeedbackFaultTimeoutInSeconds = 60,
   .feedbackPresentWhenFanIsOffTimeoutInSeconds = 60,
   .dependentData = dependentValveDataNoCareAboutSetpoint
};

static const ValveFanData_t dependentValveCareAboutSetpoint = {
   .fanId = 2,
   .numberOfPoles = 2,
   .valveDependent = true,
   .powerUsageInWatts = 8,
   .gains = gains,
   .lowerLimit = 0,
   .upperLimit = 0xFFFF,
   .fanMissedTargetFaultTimeoutInSeconds = 60,
   .missingFanFeedbackFaultTimeoutInSeconds = 60,
   .feedbackPresentWhenFanIsOffTimeoutInSeconds = 60,
   .dependentData = dependentValveDataCareAboutSetpoint
};

static const FanSpeedResolverConfig_t independentConfig = {
   .resolvedFanSpeedVoteErd = Erd_ResolvedSpeedVote,
   .resolvedValvePositionVoteErd = Erd_ResolvedValvePostionVote,
   .freezerSetpointErd = Erd_FreezerSetpoint,
   .calculatedRequestFanControlErd = Erd_CalculatedRequestFanControlSpeed,
   .ambientTempErd = Erd_AmbientTemperature,
};

static const FanSpeedResolverConfig_t dependentConfig = {
   .resolvedFanSpeedVoteErd = Erd_ResolvedSpeedVote,
   .resolvedValvePositionVoteErd = Erd_ResolvedValvePostionVote,
   .freezerSetpointErd = Erd_FreezerSetpoint,
   .calculatedRequestFanControlErd = Erd_CalculatedRequestFanControlSpeed,
   .ambientTempErd = Erd_AmbientTemperature,
};

TEST_GROUP(FanSpeedResolver_Independent)
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
         &independentValve,
         &independentConfig);
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
};

TEST(FanSpeedResolver_Independent, CalculatedSpeedShouldBeLowWhenRequestedLow)
{
   GivenInitialization();

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(LowSpeed);
}

TEST(FanSpeedResolver_Independent, CalculatedSpeedShouldBeMediumWhenRequestedMedium)
{
   GivenInitialization();

   WhenResolvedFanSpeedVoteIs(FanSpeed_Medium);
   CalculatedFanControlSpeedShouldBe(MediumSpeed);
}

TEST(FanSpeedResolver_Independent, CalculatedSpeedShouldBeHighWhenRequestedHigh)
{
   GivenInitialization();

   WhenResolvedFanSpeedVoteIs(FanSpeed_High);
   CalculatedFanControlSpeedShouldBe(HighSpeed);
}

TEST(FanSpeedResolver_Independent, CalculatedSpeedShouldBeMaxWhenRequestedSuperHigh)
{
   GivenInitialization();

   WhenResolvedFanSpeedVoteIs(FanSpeed_Max);
   CalculatedFanControlSpeedShouldBe(MaxSpeed);
}

TEST(FanSpeedResolver_Independent, CalculatedSpeedShouldBeMinWhenRequestedSuperLow)
{
   GivenInitialization();

   WhenResolvedFanSpeedVoteIs(FanSpeed_Min);
   CalculatedFanControlSpeedShouldBe(MinSpeed);
}

TEST(FanSpeedResolver_Independent, CalculatedSpeedShouldBeZeroOnInitialization)
{
   GivenInitialization();
   CalculatedFanControlSpeedShouldBe(0);
}

TEST_GROUP(FanSpeedResolver_Dependent)
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

   void GivenResolvedValvePositionIsSetTo(ValvePosition_t position)
   {
      ValveVotedPosition_t votedPosition = {
         .position = position,
         .care = true,
      };

      DataModel_Write(dataModel, Erd_ResolvedValvePostionVote, &votedPosition);
   }

   void GivenAmbientTemperatureIsSetTo(TemperatureDegFx100_t temp)
   {
      DataModel_Write(dataModel, Erd_AmbientTemperature, &temp);
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
         &dependentValveNoCareAboutSetpoint,
         &dependentConfig);
   }

   void GivenInitializationCaringAboutSetpoint()
   {
      FanSpeedResolver_Init(
         &instance,
         dataModel,
         &dependentValveCareAboutSetpoint,
         &dependentConfig);
   }
};

TEST(FanSpeedResolver_Dependent, CalculatedSpeedShouldBeLowWithValveAWhenRequestedLowWithValveSetToA)
{
   GivenInitializationWithoutCaringAboutSetpoint();
   GivenResolvedValvePositionIsSetTo(ValvePosition_A);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(LowSpeedA);
}

TEST(FanSpeedResolver_Dependent, CalculatedSpeedShouldBeLowWithValveBWhenRequestedLowWithValveSetToB)
{
   GivenInitializationWithoutCaringAboutSetpoint();
   GivenResolvedValvePositionIsSetTo(ValvePosition_B);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(LowSpeedB);
}

TEST(FanSpeedResolver_Dependent, CalculatedSpeedShouldBeLowWithValveCWhenRequestedLowWithValveSetToC)
{
   GivenInitializationWithoutCaringAboutSetpoint();
   GivenResolvedValvePositionIsSetTo(ValvePosition_C);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(LowSpeedC);
}

TEST(FanSpeedResolver_Dependent, CalculatedSpeedShouldBeMediumWithValveAWhenRequestedMediumWithValveSetToA)
{
   GivenInitializationWithoutCaringAboutSetpoint();
   GivenResolvedValvePositionIsSetTo(ValvePosition_A);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Medium);
   CalculatedFanControlSpeedShouldBe(MediumSpeedA);
}

TEST(FanSpeedResolver_Dependent, CalculatedSpeedShouldBeMediumWithValveBWhenRequestedMediumWithValveSetToB)
{
   GivenInitializationWithoutCaringAboutSetpoint();
   GivenResolvedValvePositionIsSetTo(ValvePosition_B);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Medium);
   CalculatedFanControlSpeedShouldBe(MediumSpeedB);
}

TEST(FanSpeedResolver_Dependent, CalculatedSpeedShouldBeMediumWithValveCWhenRequestedMediumWithValveSetToC)
{
   GivenInitializationWithoutCaringAboutSetpoint();
   GivenResolvedValvePositionIsSetTo(ValvePosition_C);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Medium);
   CalculatedFanControlSpeedShouldBe(MediumSpeedC);
}

TEST(FanSpeedResolver_Dependent, CalculatedSpeedShouldBeHighWithValveAWhenRequestedHighWithValveSetToA)
{
   GivenInitializationWithoutCaringAboutSetpoint();
   GivenResolvedValvePositionIsSetTo(ValvePosition_A);

   WhenResolvedFanSpeedVoteIs(FanSpeed_High);
   CalculatedFanControlSpeedShouldBe(HighSpeedA);
}

TEST(FanSpeedResolver_Dependent, CalculatedSpeedShouldBeHighWithValveBWhenRequestedHighWithValveSetToB)
{
   GivenInitializationWithoutCaringAboutSetpoint();
   GivenResolvedValvePositionIsSetTo(ValvePosition_B);

   WhenResolvedFanSpeedVoteIs(FanSpeed_High);
   CalculatedFanControlSpeedShouldBe(HighSpeedB);
}

TEST(FanSpeedResolver_Dependent, CalculatedSpeedShouldBeHighWithValveCWhenRequestedHighWithValveSetToC)
{
   GivenInitializationWithoutCaringAboutSetpoint();
   GivenResolvedValvePositionIsSetTo(ValvePosition_C);

   WhenResolvedFanSpeedVoteIs(FanSpeed_High);
   CalculatedFanControlSpeedShouldBe(HighSpeedC);
}

TEST(FanSpeedResolver_Dependent, CalculatedSpeedShouldBeMaxWhenRequestedIsSuperHigh)
{
   GivenInitializationWithoutCaringAboutSetpoint();

   WhenResolvedFanSpeedVoteIs(FanSpeed_Max);
   CalculatedFanControlSpeedShouldBe(MaxSpeed);
}

TEST(FanSpeedResolver_Dependent, CalculatedSpeedShouldBeMinWhenRequestedIsSuperLowAndAmbientTemperatureIsBelowThreshold)
{
   GivenInitializationWithoutCaringAboutSetpoint();
   GivenAmbientTemperatureIsSetTo(HighAmbientTempDegFx100 - 1);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Min);
   CalculatedFanControlSpeedShouldBe(MinSpeed);
}

TEST(FanSpeedResolver_Dependent, CalculatedSpeedShouldBeMinWithHighAmbientTempWhenRequestedIsSuperLowAndAmbientTemperatureIsAtThreshold)
{
   GivenInitializationWithoutCaringAboutSetpoint();
   GivenAmbientTemperatureIsSetTo(HighAmbientTempDegFx100);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Min);
   CalculatedFanControlSpeedShouldBe(MinSpeedHighAmbientTemp);
}

TEST(FanSpeedResolver_Dependent, CalculatedSpeedShouldBeColdLowWithValveAWhenRequestedLowWithValveSetToAndSetPointIsCold)
{
   GivenInitializationCaringAboutSetpoint();
   GivenFreezerSetpointIsSetTo(FreezerSetpoint_Cold);
   GivenResolvedValvePositionIsSetTo(ValvePosition_A);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(ColdTempSpeedA);
}

TEST(FanSpeedResolver_Dependent, CalculatedSpeedShouldBeColdLowWithValveBWhenRequestedLowWithValveSetToBAndSetPointIsCold)
{
   GivenInitializationCaringAboutSetpoint();
   GivenFreezerSetpointIsSetTo(FreezerSetpoint_Cold);
   GivenResolvedValvePositionIsSetTo(ValvePosition_B);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(ColdTempSpeedB);
}

TEST(FanSpeedResolver_Dependent, CalculatedSpeedShouldBeColdLowWithValveCWhenRequestedLowWithValveSetToCAndSetPointIsCold)
{
   GivenInitializationCaringAboutSetpoint();
   GivenFreezerSetpointIsSetTo(FreezerSetpoint_Cold);
   GivenResolvedValvePositionIsSetTo(ValvePosition_C);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(ColdTempSpeedC);
}

TEST(FanSpeedResolver_Dependent, CalculatedSpeedShouldBeMediumLowWithValveAWhenRequestedLowWithValveSetToAndSetPointIsMedium)
{
   GivenInitializationCaringAboutSetpoint();
   GivenFreezerSetpointIsSetTo(FreezerSetpoint_Medium);
   GivenResolvedValvePositionIsSetTo(ValvePosition_A);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(MediumTempSpeedA);
}

TEST(FanSpeedResolver_Dependent, CalculatedSpeedShouldBeMediumLowWithValveBWhenRequestedLowWithValveSetToBAndSetPointIsMedium)
{
   GivenInitializationCaringAboutSetpoint();
   GivenFreezerSetpointIsSetTo(FreezerSetpoint_Medium);
   GivenResolvedValvePositionIsSetTo(ValvePosition_B);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(MediumTempSpeedB);
}

TEST(FanSpeedResolver_Dependent, CalculatedSpeedShouldBeMediumLowWithValveCWhenRequestedLowWithValveSetToCAndSetPointIsMedium)
{
   GivenInitializationCaringAboutSetpoint();
   GivenFreezerSetpointIsSetTo(FreezerSetpoint_Medium);
   GivenResolvedValvePositionIsSetTo(ValvePosition_C);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(MediumTempSpeedC);
}

TEST(FanSpeedResolver_Dependent, CalculatedSpeedShouldBeWarmLowWithValveAWhenRequestedLowWithValveSetToAndSetPointIsWarm)
{
   GivenInitializationCaringAboutSetpoint();
   GivenFreezerSetpointIsSetTo(FreezerSetpoint_Warm);
   GivenResolvedValvePositionIsSetTo(ValvePosition_A);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(WarmTempSpeedA);
}

TEST(FanSpeedResolver_Dependent, CalculatedSpeedShouldBeWarmLowWithValveBWhenRequestedLowWithValveSetToBAndSetPointIsWarm)
{
   GivenInitializationCaringAboutSetpoint();
   GivenFreezerSetpointIsSetTo(FreezerSetpoint_Warm);
   GivenResolvedValvePositionIsSetTo(ValvePosition_B);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(WarmTempSpeedB);
}

TEST(FanSpeedResolver_Dependent, CalculatedSpeedShouldBeWarmLowWithValveCWhenRequestedLowWithValveSetToCAndSetPointIsWarm)
{
   GivenInitializationCaringAboutSetpoint();
   GivenFreezerSetpointIsSetTo(FreezerSetpoint_Warm);
   GivenResolvedValvePositionIsSetTo(ValvePosition_C);

   WhenResolvedFanSpeedVoteIs(FanSpeed_Low);
   CalculatedFanControlSpeedShouldBe(WarmTempSpeedC);
}
