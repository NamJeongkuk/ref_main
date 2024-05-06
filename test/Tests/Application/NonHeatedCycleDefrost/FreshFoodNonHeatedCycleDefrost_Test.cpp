/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "FreshFoodNonHeatedCycleDefrost.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"
#include "UserSetpointData.h"
#include "SystemErds.h"
}

#include "CppUTest/TestHarness.h"
#include "ReferDataModel_TestDouble.h"
#include "TddPersonality.h"

static const FreshFoodNonHeatedCycleDefrostConfig_t config = {
   .freshFoodEvapFanSpeedNonHeatedCycleDefrostVoteErd = Erd_FreshFoodEvapFanSpeed_NonHeatedCycleDefrostVote,
   .compressorIsOnErd = Erd_CompressorIsOn,
   .defrostStateErd = Erd_DefrostState,
   .freshFoodSetpointResolvedVoteErd = Erd_FreshFoodSetpoint_ResolvedVote,
   .sealedSystemValvePreviousPositionErd = Erd_SealedSystemValvePreviousPosition,
   .sealedSystemValveCurrentPositionErd = Erd_SealedSystemValveCurrentPosition
};

static const FreshFoodNonHeatedCycleDefrostData_t freshFoodNonHeatedCycleDefrostData = {
   .fanSpeedCompressorOffEnable = true,
   .valveChangeEnable = true,
   .defaultTimeValveChangeInMinutes = 15,
   .valveChangeSlopeInMinutesX10PerDegF = 25,
   .maxTimeValveChangeInMinutes = 20,
   .minTimeValveChangeInMinutes = 10,
   .fanSpeedCompressorOff = FanSpeed_Low,
   .fanSpeedValveChange = FanSpeed_Medium
};

static const UserSetpointRangeData_t freshFoodUserSetpointRangeData = {
   .lowTemperatureSetpoint = 34,
   .highTemperatureSetpoint = 42,
   .defaultTemperatureSetpoint = 37
};

TEST_GROUP(FreshFoodNonHeatedCycleDefrost_SingleEvaporator)
{
   FreshFoodNonHeatedCycleDefrost_t instance;

   ReferDataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;

   TimerModule_TestDouble_t *timerModuleTestDouble;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelTestDouble, TddPersonality_DevelopmentSingleEvaporator);
      dataModel = dataModelTestDouble.dataModel;

      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelTestDouble);
   }

   void GivenTheModuleIsInitialized()
   {
      FreshFoodNonHeatedCycleDefrost_Init(
         &instance,
         dataModel,
         &config,
         &freshFoodNonHeatedCycleDefrostData,
         &freshFoodUserSetpointRangeData);
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks);
   }

   void FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare()
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_FreshFoodEvapFanSpeed_NonHeatedCycleDefrostVote, &actual);

      CHECK_FALSE(actual.care);
   }

   void FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBe(FanSpeed_t expected)
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_FreshFoodEvapFanSpeed_NonHeatedCycleDefrostVote, &actual);

      CHECK_EQUAL(expected, actual.speed);
      CHECK_TRUE(actual.care);
   }

   void GivenFreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteIs(FanSpeed_t speed)
   {
      FanVotedSpeed_t vote = {
         .speed = speed,
         .care = Vote_Care
      };

      DataModel_Write(dataModel, Erd_FreshFoodEvapFanSpeed_NonHeatedCycleDefrostVote, &vote);
   }

   void WhenCompressorChangesTo(bool state)
   {
      DataModel_Write(dataModel, Erd_CompressorIsOn, &state);
   }

   void GivenCompressorIs(bool state)
   {
      WhenCompressorChangesTo(state);
   }

   void GivenDefrostStateIs(DefrostState_t state)
   {
      DataModel_Write(dataModel, Erd_DefrostState, &state);
   }

   void WhenDefrostStateChangesTo(DefrostState_t state)
   {
      GivenDefrostStateIs(state);
   }

   void GivenFreshFoodResolvedSetpointIs(TemperatureDegFx100_t temperature)
   {
      SetpointVotedTemperature_t setpoint;
      setpoint.temperatureInDegFx100 = temperature;
      setpoint.care = Vote_Care;

      DataModel_Write(dataModel, Erd_FreshFoodSetpoint_ResolvedVote, &setpoint);
   }
};

TEST(FreshFoodNonHeatedCycleDefrost_SingleEvaporator, ShouldVoteDontCareForFreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteOnInit)
{
   GivenFreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteIs(FanSpeed_Low);
   GivenTheModuleIsInitialized();
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();
}

TEST(FreshFoodNonHeatedCycleDefrost_SingleEvaporator, ShouldVoteFanSpeedCompressorOffForFreshFoodEvapFanWhenCompressorChangesToOff)
{
   GivenCompressorIs(ON);
   GivenTheModuleIsInitialized();

   WhenCompressorChangesTo(OFF);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBe(freshFoodNonHeatedCycleDefrostData.fanSpeedCompressorOff);
}

TEST(FreshFoodNonHeatedCycleDefrost_SingleEvaporator, ShouldNotVoteFanSpeedCompressorOffForFreshFoodEvapFanWhenCompressorChangesToOffWhileDefrostStateIsHeaterOn)
{
   GivenDefrostStateIs(DefrostState_HeaterOn);
   GivenCompressorIs(ON);
   GivenTheModuleIsInitialized();

   WhenCompressorChangesTo(OFF);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();
}

TEST(FreshFoodNonHeatedCycleDefrost_SingleEvaporator, ShouldNotVoteForFreshFoodEvapFanWhenCompressorChangesToOn)
{
   GivenCompressorIs(OFF);
   GivenTheModuleIsInitialized();

   WhenCompressorChangesTo(ON);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();
}

TEST(FreshFoodNonHeatedCycleDefrost_SingleEvaporator, ShouldVoteDontCareForFreshFoodEvapFanWhenDefrostStateChangesToHeaterOnWhileCycleDefrosting)
{
   GivenCompressorIs(ON);
   GivenTheModuleIsInitialized();

   WhenCompressorChangesTo(OFF);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBe(freshFoodNonHeatedCycleDefrostData.fanSpeedCompressorOff);

   WhenDefrostStateChangesTo(DefrostState_HeaterOn);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();
}

TEST(FreshFoodNonHeatedCycleDefrost_SingleEvaporator, ShouldVoteDontCareForFreshFoodEvapFanWhenCompressorStateChangesToOnWhileCycleDefrosting)
{
   GivenCompressorIs(ON);
   GivenTheModuleIsInitialized();

   WhenCompressorChangesTo(OFF);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBe(freshFoodNonHeatedCycleDefrostData.fanSpeedCompressorOff);

   WhenCompressorChangesTo(ON);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();
}

TEST(FreshFoodNonHeatedCycleDefrost_SingleEvaporator, ShouldStopFreshFoodEvapFanAfterCalculatedTimeWithAFreshFoodResolvedSetpointLessThanDefaultSetpointSuchThatTimeWillBeClampedToMaxTime)
{
   GivenFreshFoodResolvedSetpointIs(3400);
   GivenCompressorIs(ON);
   GivenTheModuleIsInitialized();

   WhenCompressorChangesTo(OFF);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBe(freshFoodNonHeatedCycleDefrostData.fanSpeedCompressorOff);

   After(freshFoodNonHeatedCycleDefrostData.maxTimeValveChangeInMinutes * MSEC_PER_MIN - 1);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBe(freshFoodNonHeatedCycleDefrostData.fanSpeedCompressorOff);

   After(1);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();
}

TEST(FreshFoodNonHeatedCycleDefrost_SingleEvaporator, ShouldStopFreshFoodEvapFanAfterCalculatedTimeWithAFreshFoodResolvedSetpointWithASetpointThatRequiresRoundingUpForMaxTime)
{
   GivenFreshFoodResolvedSetpointIs(3600);
   GivenCompressorIs(ON);
   GivenTheModuleIsInitialized();

   WhenCompressorChangesTo(OFF);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBe(freshFoodNonHeatedCycleDefrostData.fanSpeedCompressorOff);

   After(18 * MSEC_PER_MIN - 1);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBe(freshFoodNonHeatedCycleDefrostData.fanSpeedCompressorOff);

   After(1);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();
}

TEST(FreshFoodNonHeatedCycleDefrost_SingleEvaporator, ShouldStopFreshFoodEvapFanAfterCalculatedTimeWithAFreshFoodResolvedSetpointEqualToDefaultSetpoint)
{
   GivenFreshFoodResolvedSetpointIs(3700);
   GivenCompressorIs(ON);
   GivenTheModuleIsInitialized();

   WhenCompressorChangesTo(OFF);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBe(freshFoodNonHeatedCycleDefrostData.fanSpeedCompressorOff);

   After(15 * MSEC_PER_MIN - 1);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBe(freshFoodNonHeatedCycleDefrostData.fanSpeedCompressorOff);

   After(1);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();
}

TEST(FreshFoodNonHeatedCycleDefrost_SingleEvaporator, ShouldStopFreshFoodEvapFanAfterCalculatedTimeWithAFreshFoodResolvedSetpointGreaterThanDefaultSetpointSuchThatTimeWillBeClampedToMinTime)
{
   GivenFreshFoodResolvedSetpointIs(4000);
   GivenCompressorIs(ON);
   GivenTheModuleIsInitialized();

   WhenCompressorChangesTo(OFF);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBe(freshFoodNonHeatedCycleDefrostData.fanSpeedCompressorOff);

   After(freshFoodNonHeatedCycleDefrostData.minTimeValveChangeInMinutes * MSEC_PER_MIN - 1);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBe(freshFoodNonHeatedCycleDefrostData.fanSpeedCompressorOff);

   After(1);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();
}

TEST_GROUP(FreshFoodNonHeatedCycleDefrost_DualEvaporator)
{
   FreshFoodNonHeatedCycleDefrost_t instance;

   ReferDataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;

   TimerModule_TestDouble_t *timerModuleTestDouble;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelTestDouble, TddPersonality_DevelopmentDualEvapThreeDoor);
      dataModel = dataModelTestDouble.dataModel;

      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelTestDouble);
   }

   void GivenTheModuleIsInitialized()
   {
      FreshFoodNonHeatedCycleDefrost_Init(
         &instance,
         dataModel,
         &config,
         &freshFoodNonHeatedCycleDefrostData,
         &freshFoodUserSetpointRangeData);
   }

   void WhenCompressorChangesTo(bool state)
   {
      DataModel_Write(dataModel, Erd_CompressorIsOn, &state);
   }

   void GivenCompressorIs(bool state)
   {
      WhenCompressorChangesTo(state);
   }

   void FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare()
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_FreshFoodEvapFanSpeed_NonHeatedCycleDefrostVote, &actual);

      CHECK_FALSE(actual.care);
   }

   void FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBe(FanSpeed_t expected)
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_FreshFoodEvapFanSpeed_NonHeatedCycleDefrostVote, &actual);

      CHECK_EQUAL(expected, actual.speed);
      CHECK_TRUE(actual.care);
   }

   void WhenSealedSystemValvePreviousPositionChangesTo(SealedSystemValvePosition_t position)
   {
      DataModel_Write(dataModel, Erd_SealedSystemValvePreviousPosition, &position);
   }

   void GivenSealedSystemValvePreviousPositionIs(SealedSystemValvePosition_t position)
   {
      WhenSealedSystemValvePreviousPositionChangesTo(position);
   }

   void WhenSealedSystemValveCurrentPositionChangesTo(SealedSystemValvePosition_t position)
   {
      DataModel_Write(dataModel, Erd_SealedSystemValveCurrentPosition, &position);
   }

   void GivenSealedSystemValveCurrentPositionIs(SealedSystemValvePosition_t position)
   {
      WhenSealedSystemValveCurrentPositionChangesTo(position);
   }

   void GivenDefrostStateIs(DefrostState_t state)
   {
      DataModel_Write(dataModel, Erd_DefrostState, &state);
   }

   void GivenFreshFoodEvapFanIsRunningWithFanSpeedCompressorOff()
   {
      GivenDefrostStateIs(DefrostState_Idle);
      GivenCompressorIs(ON);
      GivenTheModuleIsInitialized();

      WhenSealedSystemValvePreviousPositionChangesTo(SealedSystemValvePosition_A);

      WhenCompressorChangesTo(OFF);
      FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBe(freshFoodNonHeatedCycleDefrostData.fanSpeedCompressorOff);
   }

   void GivenFreshFoodEvapFanIsRunningWithFanSpeedValveChange()
   {
      GivenDefrostStateIs(DefrostState_Idle);
      GivenCompressorIs(ON);
      GivenSealedSystemValvePreviousPositionIs(SealedSystemValvePosition_C);
      GivenTheModuleIsInitialized();

      WhenSealedSystemValveCurrentPositionChangesTo(SealedSystemValvePosition_B);
      FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBe(freshFoodNonHeatedCycleDefrostData.fanSpeedValveChange);
   }
};

TEST(FreshFoodNonHeatedCycleDefrost_DualEvaporator, ShouldVoteFanSpeedCompressorOffForFreshFoodEvapFanWhenCompressorChangesToOffAndSealedSystemValvePreviousPositionIsA)
{
   GivenDefrostStateIs(DefrostState_Idle);
   GivenCompressorIs(ON);
   GivenTheModuleIsInitialized();

   WhenSealedSystemValvePreviousPositionChangesTo(SealedSystemValvePosition_A);

   WhenCompressorChangesTo(OFF);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBe(freshFoodNonHeatedCycleDefrostData.fanSpeedCompressorOff);
}

TEST(FreshFoodNonHeatedCycleDefrost_DualEvaporator, ShouldVoteFanSpeedCompressorOffForFreshFoodEvapFanWhenCompressorChangesToOffAndSealedSystemValvePreviousPositionIsC)
{
   GivenDefrostStateIs(DefrostState_Idle);
   GivenCompressorIs(ON);
   GivenTheModuleIsInitialized();

   WhenSealedSystemValvePreviousPositionChangesTo(SealedSystemValvePosition_C);

   WhenCompressorChangesTo(OFF);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBe(freshFoodNonHeatedCycleDefrostData.fanSpeedCompressorOff);
}

TEST(FreshFoodNonHeatedCycleDefrost_DualEvaporator, ShouldNotVoteFanSpeedCompressorOffForFreshFoodEvapFanWhenCompressorChangesToOffAndSealedSystemValvePreviousPositionIsB)
{
   GivenDefrostStateIs(DefrostState_Idle);
   GivenCompressorIs(ON);
   GivenTheModuleIsInitialized();

   WhenSealedSystemValvePreviousPositionChangesTo(SealedSystemValvePosition_B);

   WhenCompressorChangesTo(OFF);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();
}

TEST(FreshFoodNonHeatedCycleDefrost_DualEvaporator, ShouldNotVoteFanSpeedCompressorOffForFreshFoodEvapFanWhenCompressorChangesToOffAndSealedSystemValvePreviousPositionIsD)
{
   GivenDefrostStateIs(DefrostState_Idle);
   GivenCompressorIs(ON);
   GivenTheModuleIsInitialized();

   WhenSealedSystemValvePreviousPositionChangesTo(SealedSystemValvePosition_D);

   WhenCompressorChangesTo(OFF);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();
}

TEST(FreshFoodNonHeatedCycleDefrost_DualEvaporator, ShouldVoteFanSpeedCompressorOffForFreshFoodEvapFanWhenSealedSystemValvePositionChangesToBAndPreviousValvePositionIsAAndCompressorOnIsFalse)
{
   GivenDefrostStateIs(DefrostState_Idle);
   GivenCompressorIs(OFF);
   GivenTheModuleIsInitialized();

   WhenSealedSystemValvePreviousPositionChangesTo(SealedSystemValvePosition_A);

   WhenSealedSystemValveCurrentPositionChangesTo(SealedSystemValvePosition_B);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBe(freshFoodNonHeatedCycleDefrostData.fanSpeedCompressorOff);
}

TEST(FreshFoodNonHeatedCycleDefrost_DualEvaporator, ShouldNotVoteFanSpeedCompressorOffForFreshFoodEvapFanWhenSealedSystemValvePositionChangesToAnyPositionAndPreviousValvePositionIsBAndCompressorOnIsFalse)
{
   GivenDefrostStateIs(DefrostState_Idle);
   GivenCompressorIs(OFF);
   GivenTheModuleIsInitialized();

   WhenSealedSystemValvePreviousPositionChangesTo(SealedSystemValvePosition_B);

   WhenSealedSystemValveCurrentPositionChangesTo(SealedSystemValvePosition_A);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();

   WhenSealedSystemValveCurrentPositionChangesTo(SealedSystemValvePosition_B);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();

   WhenSealedSystemValveCurrentPositionChangesTo(SealedSystemValvePosition_C);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();

   WhenSealedSystemValveCurrentPositionChangesTo(SealedSystemValvePosition_D);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();
}

TEST(FreshFoodNonHeatedCycleDefrost_DualEvaporator, ShouldNotVoteFanSpeedCompressorOffForFreshFoodEvapFanWhenSealedSystemValvePositionChangesToAnyPositionAndPreviousValvePositionIsDAndCompressorOnIsFalse)
{
   GivenDefrostStateIs(DefrostState_Idle);
   GivenCompressorIs(OFF);
   GivenTheModuleIsInitialized();

   WhenSealedSystemValvePreviousPositionChangesTo(SealedSystemValvePosition_D);

   WhenSealedSystemValveCurrentPositionChangesTo(SealedSystemValvePosition_A);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();

   WhenSealedSystemValveCurrentPositionChangesTo(SealedSystemValvePosition_B);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();

   WhenSealedSystemValveCurrentPositionChangesTo(SealedSystemValvePosition_C);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();

   WhenSealedSystemValveCurrentPositionChangesTo(SealedSystemValvePosition_D);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();
}

TEST(FreshFoodNonHeatedCycleDefrost_DualEvaporator, ShouldVoteFanSpeedCompressorOffForFreshFoodEvapFanWhenSealedSystemValvePositionChangesToDAndPreviousValvePositionIsCAndCompressorOnIsFalse)
{
   GivenDefrostStateIs(DefrostState_Idle);
   GivenCompressorIs(OFF);
   GivenTheModuleIsInitialized();

   WhenSealedSystemValvePreviousPositionChangesTo(SealedSystemValvePosition_C);

   WhenSealedSystemValveCurrentPositionChangesTo(SealedSystemValvePosition_D);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBe(freshFoodNonHeatedCycleDefrostData.fanSpeedCompressorOff);
}

TEST(FreshFoodNonHeatedCycleDefrost_DualEvaporator, ShouldNotVoteFanSpeedCompressorOffForFreshFoodEvapFanWhenSealedSystemValvePositionChangesAndPreviousValvePositionIsAAndCompressorOnIsFalseAndDefrostStateIsHeaterOn)
{
   GivenDefrostStateIs(DefrostState_HeaterOn);
   GivenCompressorIs(OFF);
   GivenTheModuleIsInitialized();

   WhenSealedSystemValvePreviousPositionChangesTo(SealedSystemValvePosition_A);

   WhenSealedSystemValveCurrentPositionChangesTo(SealedSystemValvePosition_A);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();

   WhenSealedSystemValveCurrentPositionChangesTo(SealedSystemValvePosition_B);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();

   WhenSealedSystemValveCurrentPositionChangesTo(SealedSystemValvePosition_C);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();

   WhenSealedSystemValveCurrentPositionChangesTo(SealedSystemValvePosition_D);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();
}

TEST(FreshFoodNonHeatedCycleDefrost_DualEvaporator, ShouldNotVoteFanSpeedValveChangeForFreshFoodEvapFanWhenCompressorStateChangesToOnAndSealedSystemPreviousPositionIsAAndSealedSystemCurrentPositionIsB)
{
   GivenDefrostStateIs(DefrostState_Idle);
   GivenCompressorIs(OFF);
   GivenSealedSystemValvePreviousPositionIs(SealedSystemValvePosition_A);
   GivenSealedSystemValveCurrentPositionIs(SealedSystemValvePosition_B);
   GivenTheModuleIsInitialized();

   WhenCompressorChangesTo(ON);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();
}

TEST(FreshFoodNonHeatedCycleDefrost_DualEvaporator, ShouldNotVoteFanSpeedValveChangeForFreshFoodEvapFanWhenCompressorStateChangesToOnAndSealedSystemPreviousPositionIsAAndSealedSystemCurrentPositionIsC)
{
   GivenDefrostStateIs(DefrostState_Idle);
   GivenCompressorIs(OFF);
   GivenSealedSystemValvePreviousPositionIs(SealedSystemValvePosition_A);
   GivenSealedSystemValveCurrentPositionIs(SealedSystemValvePosition_C);
   GivenTheModuleIsInitialized();

   WhenCompressorChangesTo(ON);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();
}

TEST(FreshFoodNonHeatedCycleDefrost_DualEvaporator, ShouldNotVoteFanSpeedValveChangeForFreshFoodEvapFanWhenCompressorStateChangesToOnAndSealedSystemPreviousPositionIsCAndSealedSystemCurrentPositionIsB)
{
   GivenDefrostStateIs(DefrostState_Idle);
   GivenCompressorIs(OFF);
   GivenSealedSystemValvePreviousPositionIs(SealedSystemValvePosition_C);
   GivenSealedSystemValveCurrentPositionIs(SealedSystemValvePosition_B);
   GivenTheModuleIsInitialized();

   WhenCompressorChangesTo(ON);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();
}

TEST(FreshFoodNonHeatedCycleDefrost_DualEvaporator, ShouldNotVoteFanSpeedValveChangeForFreshFoodEvapFanWhenDefrostStateIsHeaterOnAndCompressorStateChangesToOnAndSealedSystemPreviousPositionIsCAndSealedSystemCurrentPositionIsB)
{
   GivenCompressorIs(OFF);
   GivenDefrostStateIs(DefrostState_HeaterOn);
   GivenSealedSystemValvePreviousPositionIs(SealedSystemValvePosition_C);
   GivenSealedSystemValveCurrentPositionIs(SealedSystemValvePosition_B);
   GivenTheModuleIsInitialized();

   WhenCompressorChangesTo(ON);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();
}

TEST(FreshFoodNonHeatedCycleDefrost_DualEvaporator, ShouldVoteFanSpeedValveChangeForFreshFoodEvapFanWhenSealedSystemCurrentValvePositionChangesToBAndSealedSystemPreviousPositionIsAAndCompressorIsOn)
{
   GivenDefrostStateIs(DefrostState_Idle);
   GivenCompressorIs(ON);
   GivenSealedSystemValvePreviousPositionIs(SealedSystemValvePosition_A);
   GivenTheModuleIsInitialized();

   WhenSealedSystemValveCurrentPositionChangesTo(SealedSystemValvePosition_B);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBe(freshFoodNonHeatedCycleDefrostData.fanSpeedValveChange);
}

TEST(FreshFoodNonHeatedCycleDefrost_DualEvaporator, ShouldVoteFanSpeedValveChangeForFreshFoodEvapFanWhenSealedSystemCurrentValvePositionChangesToBAndSealedSystemPreviousPositionIsCAndCompressorIsOn)
{
   GivenDefrostStateIs(DefrostState_Idle);
   GivenCompressorIs(ON);
   GivenSealedSystemValvePreviousPositionIs(SealedSystemValvePosition_C);
   GivenTheModuleIsInitialized();

   WhenSealedSystemValveCurrentPositionChangesTo(SealedSystemValvePosition_B);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBe(freshFoodNonHeatedCycleDefrostData.fanSpeedValveChange);
}

TEST(FreshFoodNonHeatedCycleDefrost_DualEvaporator, ShouldNotVoteFanSpeedValveChangeForFreshFoodEvapFanWhenSealedSystemCurrentValvePositionChangesToCAndSealedSystemPreviousPositionIsAAndCompressorIsOn)
{
   GivenDefrostStateIs(DefrostState_Idle);
   GivenCompressorIs(ON);
   GivenSealedSystemValvePreviousPositionIs(SealedSystemValvePosition_A);
   GivenTheModuleIsInitialized();

   WhenSealedSystemValveCurrentPositionChangesTo(SealedSystemValvePosition_C);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();
}

TEST(FreshFoodNonHeatedCycleDefrost_DualEvaporator, ShouldVoteDontCareForFreshFoodEvapFanWhenCompressorStateChangesToOnAndCurrentValvePositionIsAWhileFreshFoodEvapFanIsRunningWithFanSpeedCompressorOff)
{
   GivenFreshFoodEvapFanIsRunningWithFanSpeedCompressorOff();

   WhenSealedSystemValveCurrentPositionChangesTo(SealedSystemValvePosition_A);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBe(freshFoodNonHeatedCycleDefrostData.fanSpeedCompressorOff);

   WhenCompressorChangesTo(ON);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();
}

TEST(FreshFoodNonHeatedCycleDefrost_DualEvaporator, ShouldNotVoteDontCareForFreshFoodEvapFanWhenCompressorStateChangesToOnAndCurrentValvePositionIsBWhileFreshFoodEvapFanIsRunningWithFanSpeedCompressorOff)
{
   GivenFreshFoodEvapFanIsRunningWithFanSpeedCompressorOff();

   WhenSealedSystemValveCurrentPositionChangesTo(SealedSystemValvePosition_B);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBe(freshFoodNonHeatedCycleDefrostData.fanSpeedCompressorOff);

   WhenCompressorChangesTo(ON);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBe(freshFoodNonHeatedCycleDefrostData.fanSpeedCompressorOff);
}

TEST(FreshFoodNonHeatedCycleDefrost_DualEvaporator, ShouldVoteDontCareForFreshFoodEvapFanWhenCompressorStateChangesToOnAndCurrentValvePositionIsCWhileFreshFoodEvapFanIsRunningWithFanSpeedCompressorOff)
{
   GivenFreshFoodEvapFanIsRunningWithFanSpeedCompressorOff();

   WhenSealedSystemValveCurrentPositionChangesTo(SealedSystemValvePosition_C);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBe(freshFoodNonHeatedCycleDefrostData.fanSpeedCompressorOff);

   WhenCompressorChangesTo(ON);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();
}

TEST(FreshFoodNonHeatedCycleDefrost_DualEvaporator, ShouldVoteDontCareForFreshFoodEvapFanWhenSealedSystemValveCurrentPositionChangesToAAndCompressorIsOn)
{
   GivenFreshFoodEvapFanIsRunningWithFanSpeedValveChange();
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBe(freshFoodNonHeatedCycleDefrostData.fanSpeedValveChange);

   WhenSealedSystemValveCurrentPositionChangesTo(SealedSystemValvePosition_A);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();
}

TEST(FreshFoodNonHeatedCycleDefrost_DualEvaporator, ShouldNotVoteDontCareForFreshFoodEvapFanWhenSealedSystemValveCurrentPositionChangesToDAndCompressorIsOn)
{
   GivenFreshFoodEvapFanIsRunningWithFanSpeedValveChange();
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBe(freshFoodNonHeatedCycleDefrostData.fanSpeedValveChange);

   WhenSealedSystemValveCurrentPositionChangesTo(SealedSystemValvePosition_D);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBe(freshFoodNonHeatedCycleDefrostData.fanSpeedValveChange);
}

TEST(FreshFoodNonHeatedCycleDefrost_DualEvaporator, ShouldVoteDontCareForFreshFoodEvapFanWhenSealedSystemValveCurrentPositionChangesToCAndCompressorIsOn)
{
   GivenFreshFoodEvapFanIsRunningWithFanSpeedValveChange();
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBe(freshFoodNonHeatedCycleDefrostData.fanSpeedValveChange);

   WhenSealedSystemValveCurrentPositionChangesTo(SealedSystemValvePosition_C);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();
}

TEST_GROUP(FreshFoodNonHeatedCycleDefrost_TripleEvaporator)
{
   FreshFoodNonHeatedCycleDefrost_t instance;

   ReferDataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;

   TimerModule_TestDouble_t *timerModuleTestDouble;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelTestDouble, TddPersonality_DevelopmentTripleEvaporator);
      dataModel = dataModelTestDouble.dataModel;

      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelTestDouble);
   }

   void GivenTheModuleIsInitialized()
   {
      FreshFoodNonHeatedCycleDefrost_Init(
         &instance,
         dataModel,
         &config,
         &freshFoodNonHeatedCycleDefrostData,
         &freshFoodUserSetpointRangeData);
   }

   void WhenCompressorChangesTo(bool state)
   {
      DataModel_Write(dataModel, Erd_CompressorIsOn, &state);
   }

   void GivenCompressorIs(bool state)
   {
      WhenCompressorChangesTo(state);
   }

   void FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare()
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_FreshFoodEvapFanSpeed_NonHeatedCycleDefrostVote, &actual);

      CHECK_FALSE(actual.care);
   }

   void FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBe(FanSpeed_t expected)
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_FreshFoodEvapFanSpeed_NonHeatedCycleDefrostVote, &actual);

      CHECK_EQUAL(expected, actual.speed);
      CHECK_TRUE(actual.care);
   }

   void WhenSealedSystemValvePreviousPositionChangesTo(SealedSystemValvePosition_t position)
   {
      DataModel_Write(dataModel, Erd_SealedSystemValvePreviousPosition, &position);
   }

   void GivenSealedSystemValvePreviousPositionIs(SealedSystemValvePosition_t position)
   {
      WhenSealedSystemValvePreviousPositionChangesTo(position);
   }

   void WhenSealedSystemValveCurrentPositionChangesTo(SealedSystemValvePosition_t position)
   {
      DataModel_Write(dataModel, Erd_SealedSystemValveCurrentPosition, &position);
   }

   void GivenSealedSystemValveCurrentPositionIs(SealedSystemValvePosition_t position)
   {
      WhenSealedSystemValveCurrentPositionChangesTo(position);
   }

   void GivenDefrostStateIs(DefrostState_t state)
   {
      DataModel_Write(dataModel, Erd_DefrostState, &state);
   }

   void GivenFreshFoodEvapFanIsRunningWithFanSpeedCompressorOff()
   {
      GivenDefrostStateIs(DefrostState_Idle);
      GivenCompressorIs(ON);
      GivenTheModuleIsInitialized();

      WhenSealedSystemValvePreviousPositionChangesTo(SealedSystemValvePosition_A);

      WhenCompressorChangesTo(OFF);
      FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBe(freshFoodNonHeatedCycleDefrostData.fanSpeedCompressorOff);
   }

   void GivenFreshFoodEvapFanIsRunningWithFanSpeedValveChange()
   {
      GivenDefrostStateIs(DefrostState_Idle);
      GivenCompressorIs(ON);
      GivenSealedSystemValvePreviousPositionIs(SealedSystemValvePosition_D);
      GivenTheModuleIsInitialized();

      WhenSealedSystemValveCurrentPositionChangesTo(SealedSystemValvePosition_C);
      FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBe(freshFoodNonHeatedCycleDefrostData.fanSpeedValveChange);
   }
};

TEST(FreshFoodNonHeatedCycleDefrost_TripleEvaporator, ShouldVoteFanSpeedCompressorOffForFreshFoodEvapFanWhenCompressorChangesToOffAndSealedSystemValvePreviousPositionIsA)
{
   GivenDefrostStateIs(DefrostState_Idle);
   GivenCompressorIs(ON);
   GivenTheModuleIsInitialized();

   WhenSealedSystemValvePreviousPositionChangesTo(SealedSystemValvePosition_A);

   WhenCompressorChangesTo(OFF);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBe(freshFoodNonHeatedCycleDefrostData.fanSpeedCompressorOff);
}

TEST(FreshFoodNonHeatedCycleDefrost_TripleEvaporator, ShouldNotVoteFanSpeedCompressorOffForFreshFoodEvapFanWhenCompressorChangesToOffAndSealedSystemValvePreviousPositionIsB)
{
   GivenDefrostStateIs(DefrostState_Idle);
   GivenCompressorIs(ON);
   GivenTheModuleIsInitialized();

   WhenSealedSystemValvePreviousPositionChangesTo(SealedSystemValvePosition_B);

   WhenCompressorChangesTo(OFF);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();
}

TEST(FreshFoodNonHeatedCycleDefrost_TripleEvaporator, ShouldNotVoteFanSpeedCompressorOffForFreshFoodEvapFanWhenCompressorChangesToOffAndSealedSystemValvePreviousPositionIsC)
{
   GivenDefrostStateIs(DefrostState_Idle);
   GivenCompressorIs(ON);
   GivenTheModuleIsInitialized();

   WhenSealedSystemValvePreviousPositionChangesTo(SealedSystemValvePosition_C);

   WhenCompressorChangesTo(OFF);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();
}

TEST(FreshFoodNonHeatedCycleDefrost_TripleEvaporator, ShouldVoteFanSpeedCompressorOffForFreshFoodEvapFanWhenCompressorChangesToOffAndSealedSystemValvePreviousPositionIsD)
{
   GivenDefrostStateIs(DefrostState_Idle);
   GivenCompressorIs(ON);
   GivenTheModuleIsInitialized();

   WhenSealedSystemValvePreviousPositionChangesTo(SealedSystemValvePosition_D);

   WhenCompressorChangesTo(OFF);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBe(freshFoodNonHeatedCycleDefrostData.fanSpeedCompressorOff);
}

TEST(FreshFoodNonHeatedCycleDefrost_TripleEvaporator, ShouldVoteFanSpeedCompressorOffForFreshFoodEvapFanWhenSealedSystemValvePositionChangesToBAndPreviousValvePositionIsAAndCompressorOnIsFalse)
{
   GivenDefrostStateIs(DefrostState_Idle);
   GivenCompressorIs(OFF);
   GivenTheModuleIsInitialized();

   WhenSealedSystemValvePreviousPositionChangesTo(SealedSystemValvePosition_A);

   WhenSealedSystemValveCurrentPositionChangesTo(SealedSystemValvePosition_B);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBe(freshFoodNonHeatedCycleDefrostData.fanSpeedCompressorOff);
}

TEST(FreshFoodNonHeatedCycleDefrost_TripleEvaporator, ShouldNotVoteFanSpeedCompressorOffForFreshFoodEvapFanWhenSealedSystemValvePositionChangesToAnyPositionAndPreviousValvePositionIsBAndCompressorOnIsFalse)
{
   GivenDefrostStateIs(DefrostState_Idle);
   GivenCompressorIs(OFF);
   GivenTheModuleIsInitialized();

   WhenSealedSystemValvePreviousPositionChangesTo(SealedSystemValvePosition_B);

   WhenSealedSystemValveCurrentPositionChangesTo(SealedSystemValvePosition_A);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();

   WhenSealedSystemValveCurrentPositionChangesTo(SealedSystemValvePosition_B);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();

   WhenSealedSystemValveCurrentPositionChangesTo(SealedSystemValvePosition_C);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();

   WhenSealedSystemValveCurrentPositionChangesTo(SealedSystemValvePosition_D);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();
}

TEST(FreshFoodNonHeatedCycleDefrost_TripleEvaporator, ShouldNotVoteFanSpeedCompressorOffForFreshFoodEvapFanWhenSealedSystemValvePositionChangesToAnyPositionAndPreviousValvePositionIsCAndCompressorOnIsFalse)
{
   GivenDefrostStateIs(DefrostState_Idle);
   GivenCompressorIs(OFF);
   GivenTheModuleIsInitialized();

   WhenSealedSystemValvePreviousPositionChangesTo(SealedSystemValvePosition_C);

   WhenSealedSystemValveCurrentPositionChangesTo(SealedSystemValvePosition_A);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();

   WhenSealedSystemValveCurrentPositionChangesTo(SealedSystemValvePosition_B);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();

   WhenSealedSystemValveCurrentPositionChangesTo(SealedSystemValvePosition_C);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();

   WhenSealedSystemValveCurrentPositionChangesTo(SealedSystemValvePosition_D);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();
}

TEST(FreshFoodNonHeatedCycleDefrost_TripleEvaporator, ShouldVoteFanSpeedCompressorOffForFreshFoodEvapFanWhenSealedSystemValvePositionChangesToBAndPreviousValvePositionIsDAndCompressorOnIsFalse)
{
   GivenDefrostStateIs(DefrostState_Idle);
   GivenCompressorIs(OFF);
   GivenTheModuleIsInitialized();

   WhenSealedSystemValvePreviousPositionChangesTo(SealedSystemValvePosition_D);

   WhenSealedSystemValveCurrentPositionChangesTo(SealedSystemValvePosition_B);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBe(freshFoodNonHeatedCycleDefrostData.fanSpeedCompressorOff);
}

TEST(FreshFoodNonHeatedCycleDefrost_TripleEvaporator, ShouldNotVoteFanSpeedCompressorOffForFreshFoodEvapFanWhenSealedSystemValvePositionChangesAndPreviousValvePositionIsAAndCompressorOnIsFalseAndDefrostStateIsHeaterOn)
{
   GivenDefrostStateIs(DefrostState_HeaterOn);
   GivenCompressorIs(OFF);
   GivenTheModuleIsInitialized();

   WhenSealedSystemValvePreviousPositionChangesTo(SealedSystemValvePosition_A);

   WhenSealedSystemValveCurrentPositionChangesTo(SealedSystemValvePosition_A);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();

   WhenSealedSystemValveCurrentPositionChangesTo(SealedSystemValvePosition_B);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();

   WhenSealedSystemValveCurrentPositionChangesTo(SealedSystemValvePosition_C);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();

   WhenSealedSystemValveCurrentPositionChangesTo(SealedSystemValvePosition_D);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();
}

TEST(FreshFoodNonHeatedCycleDefrost_TripleEvaporator, ShouldNotVoteFanSpeedValveChangeForFreshFoodEvapFanWhenCompressorStateChangesToOnAndSealedSystemPreviousPositionIsAAndSealedSystemCurrentPositionIsB)
{
   GivenDefrostStateIs(DefrostState_Idle);
   GivenCompressorIs(OFF);
   GivenSealedSystemValvePreviousPositionIs(SealedSystemValvePosition_A);
   GivenSealedSystemValveCurrentPositionIs(SealedSystemValvePosition_B);
   GivenTheModuleIsInitialized();

   WhenCompressorChangesTo(ON);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();
}

TEST(FreshFoodNonHeatedCycleDefrost_TripleEvaporator, ShouldNotVoteFanSpeedValveChangeForFreshFoodEvapFanWhenCompressorStateChangesToOnAndSealedSystemPreviousPositionIsAAndSealedSystemCurrentPositionIsC)
{
   GivenDefrostStateIs(DefrostState_Idle);
   GivenCompressorIs(OFF);
   GivenSealedSystemValvePreviousPositionIs(SealedSystemValvePosition_A);
   GivenSealedSystemValveCurrentPositionIs(SealedSystemValvePosition_C);
   GivenTheModuleIsInitialized();

   WhenCompressorChangesTo(ON);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();
}

TEST(FreshFoodNonHeatedCycleDefrost_TripleEvaporator, ShouldNotVoteFanSpeedValveChangeForFreshFoodEvapFanWhenCompressorStateChangesToOnAndSealedSystemPreviousPositionIsDAndSealedSystemCurrentPositionIsB)
{
   GivenDefrostStateIs(DefrostState_Idle);
   GivenCompressorIs(OFF);
   GivenSealedSystemValvePreviousPositionIs(SealedSystemValvePosition_D);
   GivenSealedSystemValveCurrentPositionIs(SealedSystemValvePosition_B);
   GivenTheModuleIsInitialized();

   WhenCompressorChangesTo(ON);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();
}

TEST(FreshFoodNonHeatedCycleDefrost_TripleEvaporator, ShouldNotVoteFanSpeedValveChangeForFreshFoodEvapFanWhenCompressorStateChangesToOnAndSealedSystemPreviousPositionIsDAndSealedSystemCurrentPositionIsC)
{
   GivenDefrostStateIs(DefrostState_Idle);
   GivenCompressorIs(OFF);
   GivenSealedSystemValvePreviousPositionIs(SealedSystemValvePosition_D);
   GivenSealedSystemValveCurrentPositionIs(SealedSystemValvePosition_C);
   GivenTheModuleIsInitialized();

   WhenCompressorChangesTo(ON);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();
}

TEST(FreshFoodNonHeatedCycleDefrost_TripleEvaporator, ShouldNotVoteFanSpeedValveChangeForFreshFoodEvapFanWhenCompressorStateChangesToOnAndSealedSystemPreviousPositionIsAAndSealedSystemCurrentPositionIsD)
{
   GivenDefrostStateIs(DefrostState_Idle);
   GivenCompressorIs(OFF);
   GivenSealedSystemValvePreviousPositionIs(SealedSystemValvePosition_A);
   GivenSealedSystemValveCurrentPositionIs(SealedSystemValvePosition_D);
   GivenTheModuleIsInitialized();

   WhenCompressorChangesTo(ON);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();
}

TEST(FreshFoodNonHeatedCycleDefrost_TripleEvaporator, ShouldNotVoteFanSpeedValveChangeForFreshFoodEvapFanWhenCompressorStateChangesToOnAndSealedSystemPreviousPositionIsDAndSealedSystemCurrentPositionIsA)
{
   GivenDefrostStateIs(DefrostState_Idle);
   GivenCompressorIs(OFF);
   GivenSealedSystemValvePreviousPositionIs(SealedSystemValvePosition_D);
   GivenSealedSystemValveCurrentPositionIs(SealedSystemValvePosition_A);
   GivenTheModuleIsInitialized();

   WhenCompressorChangesTo(ON);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();
}

TEST(FreshFoodNonHeatedCycleDefrost_TripleEvaporator, ShouldVoteFanSpeedValveChangeForFreshFoodEvapFanWhenSealedSystemCurrentValvePositionChangesToBAndSealedSystemPreviousPositionIsAAndCompressorIsOn)
{
   GivenDefrostStateIs(DefrostState_Idle);
   GivenCompressorIs(ON);
   GivenSealedSystemValvePreviousPositionIs(SealedSystemValvePosition_A);
   GivenTheModuleIsInitialized();

   WhenSealedSystemValveCurrentPositionChangesTo(SealedSystemValvePosition_B);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBe(freshFoodNonHeatedCycleDefrostData.fanSpeedValveChange);
}

TEST(FreshFoodNonHeatedCycleDefrost_TripleEvaporator, ShouldVoteFanSpeedValveChangeForFreshFoodEvapFanWhenSealedSystemCurrentValvePositionChangesToCAndSealedSystemPreviousPositionIsAAndCompressorIsOn)
{
   GivenDefrostStateIs(DefrostState_Idle);
   GivenCompressorIs(ON);
   GivenSealedSystemValvePreviousPositionIs(SealedSystemValvePosition_A);
   GivenTheModuleIsInitialized();

   WhenSealedSystemValveCurrentPositionChangesTo(SealedSystemValvePosition_C);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBe(freshFoodNonHeatedCycleDefrostData.fanSpeedValveChange);
}

TEST(FreshFoodNonHeatedCycleDefrost_TripleEvaporator, ShouldVoteFanSpeedValveChangeForFreshFoodEvapFanWhenSealedSystemCurrentValvePositionChangesToBAndSealedSystemPreviousPositionIsDAndCompressorIsOn)
{
   GivenDefrostStateIs(DefrostState_Idle);
   GivenCompressorIs(ON);
   GivenSealedSystemValvePreviousPositionIs(SealedSystemValvePosition_D);
   GivenTheModuleIsInitialized();

   WhenSealedSystemValveCurrentPositionChangesTo(SealedSystemValvePosition_B);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBe(freshFoodNonHeatedCycleDefrostData.fanSpeedValveChange);
}

TEST(FreshFoodNonHeatedCycleDefrost_TripleEvaporator, ShouldVoteFanSpeedValveChangeForFreshFoodEvapFanWhenSealedSystemCurrentValvePositionChangesToCAndSealedSystemPreviousPositionIsDAndCompressorIsOn)
{
   GivenDefrostStateIs(DefrostState_Idle);
   GivenCompressorIs(ON);
   GivenSealedSystemValvePreviousPositionIs(SealedSystemValvePosition_D);
   GivenTheModuleIsInitialized();

   WhenSealedSystemValveCurrentPositionChangesTo(SealedSystemValvePosition_C);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBe(freshFoodNonHeatedCycleDefrostData.fanSpeedValveChange);
}

TEST(FreshFoodNonHeatedCycleDefrost_TripleEvaporator, ShouldVoteFanSpeedValveChangeForFreshFoodEvapFanWhenSealedSystemCurrentValvePositionChangesToAAndSealedSystemPreviousPositionIsDAndCompressorIsOn)
{
   GivenDefrostStateIs(DefrostState_Idle);
   GivenCompressorIs(ON);
   GivenSealedSystemValvePreviousPositionIs(SealedSystemValvePosition_D);
   GivenTheModuleIsInitialized();

   WhenSealedSystemValveCurrentPositionChangesTo(SealedSystemValvePosition_A);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();
}

TEST(FreshFoodNonHeatedCycleDefrost_TripleEvaporator, ShouldVoteDontCareForFreshFoodEvapFanWhenCompressorStateChangesToOnAndCurrentValvePositionIsAWhileFreshFoodEvapFanIsRunningWithFanSpeedCompressorOff)
{
   GivenFreshFoodEvapFanIsRunningWithFanSpeedCompressorOff();

   WhenSealedSystemValveCurrentPositionChangesTo(SealedSystemValvePosition_A);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBe(freshFoodNonHeatedCycleDefrostData.fanSpeedCompressorOff);

   WhenCompressorChangesTo(ON);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();
}

TEST(FreshFoodNonHeatedCycleDefrost_TripleEvaporator, ShouldVoteDontCareForFreshFoodEvapFanWhenCompressorStateChangesToOnAndCurrentValvePositionIsDWhileFreshFoodEvapFanIsRunningWithFanSpeedCompressorOff)
{
   GivenFreshFoodEvapFanIsRunningWithFanSpeedCompressorOff();

   WhenSealedSystemValveCurrentPositionChangesTo(SealedSystemValvePosition_D);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBe(freshFoodNonHeatedCycleDefrostData.fanSpeedCompressorOff);

   WhenCompressorChangesTo(ON);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();
}

TEST(FreshFoodNonHeatedCycleDefrost_TripleEvaporator, ShouldNotVoteDontCareForFreshFoodEvapFanWhenCompressorStateChangesToOnAndCurrentValvePositionIsBWhileFreshFoodEvapFanIsRunningWithFanSpeedCompressorOff)
{
   GivenFreshFoodEvapFanIsRunningWithFanSpeedCompressorOff();

   WhenSealedSystemValveCurrentPositionChangesTo(SealedSystemValvePosition_B);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBe(freshFoodNonHeatedCycleDefrostData.fanSpeedCompressorOff);

   WhenCompressorChangesTo(ON);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBe(freshFoodNonHeatedCycleDefrostData.fanSpeedCompressorOff);
}

TEST(FreshFoodNonHeatedCycleDefrost_TripleEvaporator, ShouldNotVoteDontCareForFreshFoodEvapFanWhenCompressorStateChangesToOnAndCurrentValvePositionIsCWhileFreshFoodEvapFanIsRunningWithFanSpeedCompressorOff)
{
   GivenFreshFoodEvapFanIsRunningWithFanSpeedCompressorOff();

   WhenSealedSystemValveCurrentPositionChangesTo(SealedSystemValvePosition_C);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBe(freshFoodNonHeatedCycleDefrostData.fanSpeedCompressorOff);

   WhenCompressorChangesTo(ON);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBe(freshFoodNonHeatedCycleDefrostData.fanSpeedCompressorOff);
}

TEST(FreshFoodNonHeatedCycleDefrost_TripleEvaporator, ShouldVoteDontCareForFreshFoodEvapFanWhenSealedSystemValveCurrentPositionChangesToAAndCompressorIsOn)
{
   GivenFreshFoodEvapFanIsRunningWithFanSpeedValveChange();
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBe(freshFoodNonHeatedCycleDefrostData.fanSpeedValveChange);

   WhenSealedSystemValveCurrentPositionChangesTo(SealedSystemValvePosition_A);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();
}

TEST(FreshFoodNonHeatedCycleDefrost_TripleEvaporator, ShouldNotVoteDontCareForFreshFoodEvapFanWhenSealedSystemValveCurrentPositionChangesToBAndCompressorIsOn)
{
   GivenFreshFoodEvapFanIsRunningWithFanSpeedValveChange();
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBe(freshFoodNonHeatedCycleDefrostData.fanSpeedValveChange);

   WhenSealedSystemValveCurrentPositionChangesTo(SealedSystemValvePosition_B);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBe(freshFoodNonHeatedCycleDefrostData.fanSpeedValveChange);
}

TEST(FreshFoodNonHeatedCycleDefrost_TripleEvaporator, ShouldNotVoteDontCareForFreshFoodEvapFanWhenSealedSystemValveCurrentPositionChangesToCAndCompressorIsOn)
{
   GivenFreshFoodEvapFanIsRunningWithFanSpeedValveChange();
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBe(freshFoodNonHeatedCycleDefrostData.fanSpeedValveChange);

   WhenSealedSystemValveCurrentPositionChangesTo(SealedSystemValvePosition_C);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBe(freshFoodNonHeatedCycleDefrostData.fanSpeedValveChange);
}

TEST(FreshFoodNonHeatedCycleDefrost_TripleEvaporator, ShouldVoteDontCareForFreshFoodEvapFanWhenSealedSystemValveCurrentPositionChangesToDAndCompressorIsOn)
{
   GivenFreshFoodEvapFanIsRunningWithFanSpeedValveChange();
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBe(freshFoodNonHeatedCycleDefrostData.fanSpeedValveChange);

   WhenSealedSystemValveCurrentPositionChangesTo(SealedSystemValvePosition_D);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();
}

static const FreshFoodNonHeatedCycleDefrostData_t freshFoodNonHeatedCycleDefrostDataWithFanSpeedEnableOff = {
   .fanSpeedCompressorOffEnable = false,
   .valveChangeEnable = true,
   .defaultTimeValveChangeInMinutes = 10,
   .valveChangeSlopeInMinutesX10PerDegF = 20,
   .maxTimeValveChangeInMinutes = 100,
   .minTimeValveChangeInMinutes = 1,
   .fanSpeedCompressorOff = FanSpeed_Low,
   .fanSpeedValveChange = FanSpeed_Medium
};

TEST_GROUP(FreshFoodNonHeatedCycleDefrost_SingleEvapFanSpeedEnableOff)
{
   FreshFoodNonHeatedCycleDefrost_t instance;

   ReferDataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelTestDouble, TddPersonality_DevelopmentSingleEvaporatorFanEnableOff);
      dataModel = dataModelTestDouble.dataModel;
   }

   void WhenCompressorChangesTo(bool state)
   {
      DataModel_Write(dataModel, Erd_CompressorIsOn, &state);
   }

   void GivenCompressorIs(bool state)
   {
      WhenCompressorChangesTo(state);
   }

   void GivenTheModuleIsInitialized()
   {
      FreshFoodNonHeatedCycleDefrost_Init(
         &instance,
         dataModel,
         &config,
         &freshFoodNonHeatedCycleDefrostDataWithFanSpeedEnableOff,
         &freshFoodUserSetpointRangeData);
   }

   void FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare()
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_FreshFoodEvapFanSpeed_NonHeatedCycleDefrostVote, &actual);

      CHECK_FALSE(actual.care);
   }

   void GivenDefrostStateIs(DefrostState_t state)
   {
      DataModel_Write(dataModel, Erd_DefrostState, &state);
   }
};

TEST(FreshFoodNonHeatedCycleDefrost_SingleEvapFanSpeedEnableOff, ShouldNotVoteFanSpeedCompressorOffForFreshFoodEvapFanWhenCompressorChangesToOff)
{
   GivenDefrostStateIs(DefrostState_Idle);
   GivenCompressorIs(ON);
   GivenTheModuleIsInitialized();

   WhenCompressorChangesTo(OFF);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();
}

TEST_GROUP(FreshFoodNonHeatedCycleDefrost_DualEvapFanEnableOff)
{
   FreshFoodNonHeatedCycleDefrost_t instance;

   ReferDataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;

   const FreshFoodNonHeatedCycleDefrostData_t *freshFoodNonHeatedCycleDefrostData;
   const UserSetpointRangeData_t *freshFoodUserSetpointRangeData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelTestDouble, TddPersonality_DevelopmentDualEvaporatorFanEnableOff);
      dataModel = dataModelTestDouble.dataModel;

      freshFoodNonHeatedCycleDefrostData =
         PersonalityParametricData_Get(dataModel)->freshFoodNonHeatedCycleDefrost;
      freshFoodUserSetpointRangeData =
         PersonalityParametricData_Get(dataModel)->setpointData->userSetpointData->freshFoodUserSetpoint;
   }

   void WhenCompressorChangesTo(bool state)
   {
      DataModel_Write(dataModel, Erd_CompressorIsOn, &state);
   }

   void GivenCompressorIs(bool state)
   {
      WhenCompressorChangesTo(state);
   }

   void GivenTheModuleIsInitialized()
   {
      FreshFoodNonHeatedCycleDefrost_Init(
         &instance,
         dataModel,
         &config,
         freshFoodNonHeatedCycleDefrostData,
         freshFoodUserSetpointRangeData);
   }

   void FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare()
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_FreshFoodEvapFanSpeed_NonHeatedCycleDefrostVote, &actual);

      CHECK_FALSE(actual.care);
   }

   void WhenSealedSystemValvePreviousPositionChangesTo(SealedSystemValvePosition_t position)
   {
      DataModel_Write(dataModel, Erd_SealedSystemValvePreviousPosition, &position);
   }

   void GivenSealedSystemValvePreviousPositionIs(SealedSystemValvePosition_t position)
   {
      WhenSealedSystemValvePreviousPositionChangesTo(position);
   }

   void WhenSealedSystemValveCurrentPositionChangesTo(SealedSystemValvePosition_t position)
   {
      DataModel_Write(dataModel, Erd_SealedSystemValveCurrentPosition, &position);
   }

   void GivenSealedSystemValveCurrentPositionIs(SealedSystemValvePosition_t position)
   {
      WhenSealedSystemValveCurrentPositionChangesTo(position);
   }

   void GivenDefrostStateIs(DefrostState_t state)
   {
      DataModel_Write(dataModel, Erd_DefrostState, &state);
   }
};

TEST(FreshFoodNonHeatedCycleDefrost_DualEvapFanEnableOff, ShouldNotVoteFanSpeedCompressorOffForFreshFoodEvapFanWhenCompressorChangesToOffAndSealedSystemValvePreviousPositionIsA)
{
   GivenDefrostStateIs(DefrostState_Idle);
   GivenCompressorIs(ON);
   GivenTheModuleIsInitialized();

   WhenSealedSystemValvePreviousPositionChangesTo(SealedSystemValvePosition_A);

   WhenCompressorChangesTo(OFF);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();
}

TEST(FreshFoodNonHeatedCycleDefrost_DualEvapFanEnableOff, ShouldNotVoteFanSpeedCompressorOffForFreshFoodEvapFanWhenSealedSystemValvePositionChangesToBAndPreviousValvePositionIsAAndCompressorOnIsFalse)
{
   GivenDefrostStateIs(DefrostState_Idle);
   GivenCompressorIs(OFF);
   GivenTheModuleIsInitialized();

   WhenSealedSystemValvePreviousPositionChangesTo(SealedSystemValvePosition_A);

   WhenSealedSystemValveCurrentPositionChangesTo(SealedSystemValvePosition_B);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();
}

TEST(FreshFoodNonHeatedCycleDefrost_DualEvapFanEnableOff, ShouldNotVoteFanSpeedValveChangeForFreshFoodEvapFanWhenCompressorStateChangesToOnAndSealedSystemPreviousPositionIsAAndSealedSystemCurrentPositionIsB)
{
   GivenDefrostStateIs(DefrostState_Idle);
   GivenCompressorIs(OFF);
   GivenSealedSystemValvePreviousPositionIs(SealedSystemValvePosition_A);
   GivenSealedSystemValveCurrentPositionIs(SealedSystemValvePosition_B);
   GivenTheModuleIsInitialized();

   WhenCompressorChangesTo(ON);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();
}

TEST(FreshFoodNonHeatedCycleDefrost_DualEvapFanEnableOff, ShouldNotVoteFanSpeedValveChangeForFreshFoodEvapFanWhenSealedSystemCurrentValvePositionChangesToBAndSealedSystemPreviousPositionIsAAndCompressorIsOn)
{
   GivenDefrostStateIs(DefrostState_Idle);
   GivenCompressorIs(ON);
   GivenSealedSystemValvePreviousPositionIs(SealedSystemValvePosition_A);
   GivenTheModuleIsInitialized();

   WhenSealedSystemValveCurrentPositionChangesTo(SealedSystemValvePosition_B);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();
}

TEST_GROUP(FreshFoodNonHeatedCycleDefrost_TripleEvapFanEnableOff)
{
   FreshFoodNonHeatedCycleDefrost_t instance;

   ReferDataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;

   const FreshFoodNonHeatedCycleDefrostData_t *freshFoodNonHeatedCycleDefrostData;
   const UserSetpointRangeData_t *freshFoodUserSetpointRangeData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelTestDouble, TddPersonality_DevelopmentTripleEvaporatorFanEnableOff);
      dataModel = dataModelTestDouble.dataModel;

      freshFoodNonHeatedCycleDefrostData =
         PersonalityParametricData_Get(dataModel)->freshFoodNonHeatedCycleDefrost;
      freshFoodUserSetpointRangeData =
         PersonalityParametricData_Get(dataModel)->setpointData->userSetpointData->freshFoodUserSetpoint;
   }

   void WhenCompressorChangesTo(bool state)
   {
      DataModel_Write(dataModel, Erd_CompressorIsOn, &state);
   }

   void GivenCompressorIs(bool state)
   {
      WhenCompressorChangesTo(state);
   }

   void GivenTheModuleIsInitialized()
   {
      FreshFoodNonHeatedCycleDefrost_Init(
         &instance,
         dataModel,
         &config,
         freshFoodNonHeatedCycleDefrostData,
         freshFoodUserSetpointRangeData);
   }

   void FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare()
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_FreshFoodEvapFanSpeed_NonHeatedCycleDefrostVote, &actual);

      CHECK_FALSE(actual.care);
   }

   void WhenSealedSystemValvePreviousPositionChangesTo(SealedSystemValvePosition_t position)
   {
      DataModel_Write(dataModel, Erd_SealedSystemValvePreviousPosition, &position);
   }

   void GivenSealedSystemValvePreviousPositionIs(SealedSystemValvePosition_t position)
   {
      WhenSealedSystemValvePreviousPositionChangesTo(position);
   }

   void WhenSealedSystemValveCurrentPositionChangesTo(SealedSystemValvePosition_t position)
   {
      DataModel_Write(dataModel, Erd_SealedSystemValveCurrentPosition, &position);
   }

   void GivenSealedSystemValveCurrentPositionIs(SealedSystemValvePosition_t position)
   {
      WhenSealedSystemValveCurrentPositionChangesTo(position);
   }

   void GivenDefrostStateIs(DefrostState_t state)
   {
      DataModel_Write(dataModel, Erd_DefrostState, &state);
   }
};

TEST(FreshFoodNonHeatedCycleDefrost_TripleEvapFanEnableOff, ShouldNotVoteFanSpeedCompressorOffForFreshFoodEvapFanWhenCompressorChangesToOffAndSealedSystemValvePreviousPositionIsA)
{
   GivenDefrostStateIs(DefrostState_Idle);
   GivenCompressorIs(ON);
   GivenTheModuleIsInitialized();

   WhenSealedSystemValvePreviousPositionChangesTo(SealedSystemValvePosition_A);

   WhenCompressorChangesTo(OFF);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();
}

TEST(FreshFoodNonHeatedCycleDefrost_TripleEvapFanEnableOff, ShouldNotVoteFanSpeedCompressorOffForFreshFoodEvapFanWhenSealedSystemValvePositionChangesToBAndPreviousValvePositionIsAAndCompressorOnIsFalse)
{
   GivenDefrostStateIs(DefrostState_Idle);
   GivenCompressorIs(OFF);
   GivenTheModuleIsInitialized();

   WhenSealedSystemValvePreviousPositionChangesTo(SealedSystemValvePosition_A);

   WhenSealedSystemValveCurrentPositionChangesTo(SealedSystemValvePosition_B);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();
}

TEST(FreshFoodNonHeatedCycleDefrost_TripleEvapFanEnableOff, ShouldNotVoteFanSpeedValveChangeForFreshFoodEvapFanWhenCompressorStateChangesToOnAndSealedSystemPreviousPositionIsAAndSealedSystemCurrentPositionIsB)
{
   GivenDefrostStateIs(DefrostState_Idle);
   GivenCompressorIs(OFF);
   GivenSealedSystemValvePreviousPositionIs(SealedSystemValvePosition_A);
   GivenSealedSystemValveCurrentPositionIs(SealedSystemValvePosition_B);
   GivenTheModuleIsInitialized();

   WhenCompressorChangesTo(ON);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();
}

TEST(FreshFoodNonHeatedCycleDefrost_TripleEvapFanEnableOff, ShouldNotVoteFanSpeedValveChangeForFreshFoodEvapFanWhenSealedSystemCurrentValvePositionChangesToBAndSealedSystemPreviousPositionIsAAndCompressorIsOn)
{
   GivenDefrostStateIs(DefrostState_Idle);
   GivenCompressorIs(ON);
   GivenSealedSystemValvePreviousPositionIs(SealedSystemValvePosition_A);
   GivenTheModuleIsInitialized();

   WhenSealedSystemValveCurrentPositionChangesTo(SealedSystemValvePosition_B);
   FreshFoodEvapFanSpeedNonHeatedCycleDefrostVoteShouldBeDontCare();
}
