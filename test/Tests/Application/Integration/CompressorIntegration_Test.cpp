/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "Application.h"
#include "SystemErds.h"
#include "Rx2xxResetSource.h"
#include "Constants_Time.h"
#include "Constants_Binary.h"
}

#include "CppUTest/TestHarness.h"
#include "ReferDataModel_TestDouble.h"
#include "CppUTestExt/MockSupport.h"
#include "TddPersonality.h"

TEST_GROUP(SingleSpeedCompressorIntegration)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   I_DataModel_t *dataModel;
   Application_t instance;
   ResetReason_t resetReason;
   const CompressorData_t *compressorData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble, TddPersonality_DevelopmentSingleSpeedCompressor);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      compressorData = PersonalityParametricData_Get(dataModel)->compressorData;
   }

   void GivenApplicationHasBeenInitialized()
   {
      Application_Init(
         &instance,
         dataModel,
         resetReason);
   }

   void WhenTheGridCompressorVoteBecomes(CompressorSpeed_t compressorSpeed)
   {
      CompressorVotedSpeed_t compressorVote;
      compressorVote.speed = compressorSpeed;
      compressorVote.care = Vote_Care;
      DataModel_Write(dataModel, Erd_CompressorSpeed_GridVote, &compressorVote);
   }

   void CompressorRelayShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_CompressorRelay, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks);
   }

   void GivenApplicationHasBeenInitializedAndCompressorRelayIsOn()
   {
      GivenApplicationHasBeenInitialized();

      WhenTheGridCompressorVoteBecomes(CompressorSpeed_SuperLow);

      CompressorRelayShouldBe(ON);
   }
};

TEST(SingleSpeedCompressorIntegration, ShouldInitialize)
{
   GivenApplicationHasBeenInitialized();
}

TEST(SingleSpeedCompressorIntegration, ShouldTurnOnCompressorRelayWhenTheGridCompressorVoteIsSuperLow)
{
   GivenApplicationHasBeenInitialized();

   CompressorRelayShouldBe(OFF);
   WhenTheGridCompressorVoteBecomes(CompressorSpeed_SuperLow);

   CompressorRelayShouldBe(ON);
}

TEST(SingleSpeedCompressorIntegration, ShouldTurnOffCompressorRelayAfterMinimumOnTimeWhenCompressorSpeedChangesToOff)
{
   GivenApplicationHasBeenInitializedAndCompressorRelayIsOn();

   WhenTheGridCompressorVoteBecomes(CompressorSpeed_Off);

   After(compressorData->compressorTimes.minimumOnTimeInMinutes * MSEC_PER_MIN - 1);
   CompressorRelayShouldBe(ON);

   After(1);
   CompressorRelayShouldBe(OFF);
}

TEST_GROUP(VariableSpeedCompressorIntegration)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   I_DataModel_t *dataModel;
   Application_t instance;
   ResetReason_t resetReason;
   const CompressorData_t *compressorData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble, TddPersonality_DevelopmentVariableSpeedCoolingModeIndependentCompressor);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      compressorData = PersonalityParametricData_Get(dataModel)->compressorData;
   }

   void GivenApplicationHasBeenInitialized()
   {
      Application_Init(
         &instance,
         dataModel,
         resetReason);
   }

   void GivenApplicationHasBeenInitializedAndCompressorSpeedIsSuperHigh()
   {
      GivenApplicationHasBeenInitialized();
      GivenGridOverrideEnableIs(SET);

      WhenTheGridCompressorVoteBecomes(CompressorSpeed_SuperHigh);
      CompressorFrequencyShouldBe(compressorData->compressorSpeeds.startupSpeedFrequencyInHz);

      After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC);
      CompressorFrequencyShouldBe(compressorData->compressorSpeeds.superHighSpeedFrequencyInHz);
   }

   void WhenTheGridCompressorVoteBecomes(CompressorSpeed_t compressorSpeed)
   {
      CompressorVotedSpeed_t compressorVote;
      compressorVote.speed = compressorSpeed;
      compressorVote.care = Vote_Care;
      DataModel_Write(dataModel, Erd_CompressorSpeed_GridVote, &compressorVote);
   }

   void CompressorFrequencyShouldBe(PwmFrequency_t expected)
   {
      PwmFrequency_t actual;
      DataModel_Read(dataModel, Erd_CompressorInverterDriver, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void GivenGridOverrideEnableIs(bool enable)
   {
      DataModel_Write(dataModel, Erd_GridOverrideEnable, &enable);
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks);
   }
};

TEST(VariableSpeedCompressorIntegration, ShouldInitialize)
{
   GivenApplicationHasBeenInitialized();
}

TEST(VariableSpeedCompressorIntegration, ShouldSetCompressorFrequencyToSuperLowAfterStartupOnTimeWhenCompressorSpeedChangesToSuperLow)
{
   GivenApplicationHasBeenInitialized();

   WhenTheGridCompressorVoteBecomes(CompressorSpeed_SuperLow);

   CompressorFrequencyShouldBe(compressorData->compressorSpeeds.startupSpeedFrequencyInHz);

   After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC - 1);
   CompressorFrequencyShouldBe(compressorData->compressorSpeeds.startupSpeedFrequencyInHz);

   After(1);
   CompressorFrequencyShouldBe(compressorData->compressorSpeeds.superLowSpeedFrequencyInHz);
}

TEST(VariableSpeedCompressorIntegration, ShouldSetCompressorFrequencyToLowAfterStartupOnTimeWhenCompressorSpeedChangesToLow)
{
   GivenApplicationHasBeenInitialized();

   WhenTheGridCompressorVoteBecomes(CompressorSpeed_Low);

   CompressorFrequencyShouldBe(compressorData->compressorSpeeds.startupSpeedFrequencyInHz);

   After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC);
   CompressorFrequencyShouldBe(
      compressorData->compressorSpeeds.coolingModeIndependentCompressorSpeeds.lowSpeedFrequencyInHz);
}

TEST(VariableSpeedCompressorIntegration, ShouldSetCompressorFrequencyToMediumAfterStartupOnTimeWhenCompressorSpeedChangesToMedium)
{
   GivenApplicationHasBeenInitialized();

   WhenTheGridCompressorVoteBecomes(CompressorSpeed_Medium);

   CompressorFrequencyShouldBe(compressorData->compressorSpeeds.startupSpeedFrequencyInHz);

   After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC);
   CompressorFrequencyShouldBe(
      compressorData->compressorSpeeds.coolingModeIndependentCompressorSpeeds.mediumSpeedFrequencyInHz);
}

TEST(VariableSpeedCompressorIntegration, ShouldSetCompressorFrequencyToHighAndStartupOnTimeWhenCompressorSpeedChangesToHigh)
{
   GivenApplicationHasBeenInitialized();

   WhenTheGridCompressorVoteBecomes(CompressorSpeed_High);

   CompressorFrequencyShouldBe(compressorData->compressorSpeeds.startupSpeedFrequencyInHz);

   After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC);
   CompressorFrequencyShouldBe(
      compressorData->compressorSpeeds.coolingModeIndependentCompressorSpeeds.highSpeedFrequencyInHz);
}

TEST(VariableSpeedCompressorIntegration, ShouldSetCompressorFrequencyToSuperHighStartupOnTimeWhenCompressorSpeedChangesToSuperHigh)
{
   GivenApplicationHasBeenInitialized();

   WhenTheGridCompressorVoteBecomes(CompressorSpeed_SuperHigh);

   CompressorFrequencyShouldBe(compressorData->compressorSpeeds.startupSpeedFrequencyInHz);

   After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC);
   CompressorFrequencyShouldBe(compressorData->compressorSpeeds.superHighSpeedFrequencyInHz);
}

TEST(VariableSpeedCompressorIntegration, ShouldSetCompressorFrequencyToZeroAfterMinimumOnTimeWhenCompressorSpeedChangesToOff)
{
   GivenApplicationHasBeenInitializedAndCompressorSpeedIsSuperHigh();

   WhenTheGridCompressorVoteBecomes(CompressorSpeed_Off);

   After(compressorData->compressorTimes.minimumOnTimeInMinutes * MSEC_PER_MIN - compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC - 1);
   CompressorFrequencyShouldBe(compressorData->compressorSpeeds.superHighSpeedFrequencyInHz);

   After(1);
   CompressorFrequencyShouldBe(0);
}

TEST(VariableSpeedCompressorIntegration, ShouldUpdateCompressorSpeedFromSuperHighToSuperLowWhenCompressorSpeedChangesToSuperLowAfterMinimumOnTimeInMinutes)
{
   GivenApplicationHasBeenInitializedAndCompressorSpeedIsSuperHigh();

   After(compressorData->compressorTimes.minimumOnTimeInMinutes * MSEC_PER_MIN - compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC - 1);
   CompressorFrequencyShouldBe(compressorData->compressorSpeeds.superHighSpeedFrequencyInHz);

   After(1);
   WhenTheGridCompressorVoteBecomes(CompressorSpeed_SuperLow);
   CompressorFrequencyShouldBe(compressorData->compressorSpeeds.superLowSpeedFrequencyInHz);
}

TEST_GROUP(VariableSpeedCompressorIntegrationWithSealedSystemValve)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   I_DataModel_t *dataModel;
   Application_t instance;
   ResetReason_t resetReason;
   const CompressorData_t *compressorData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble, TddPersonality_DevelopmentDualEvapFourDoor);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      compressorData = PersonalityParametricData_Get(dataModel)->compressorData;
   }

   void GivenApplicationHasBeenInitialized()
   {
      Application_Init(
         &instance,
         dataModel,
         resetReason);
   }

   void WhenCompressorSpeedGridVoteChangesTo(CompressorSpeed_t compressorSpeed)
   {
      CompressorVotedSpeed_t compressorVote;
      compressorVote.speed = compressorSpeed;
      compressorVote.care = Vote_Care;
      DataModel_Write(dataModel, Erd_CompressorSpeed_GridVote, &compressorVote);
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks);
   }

   void GivenTheSealedSystemValveVoteIs(SealedSystemValvePosition_t position)
   {
      SealedSystemValveVotedPosition_t vote;
      vote.position = position;
      vote.care = Vote_Care;
      DataModel_Write(dataModel, Erd_SealedSystemValvePosition_FactoryVote, &vote);
   }

   void WhenTheSealedSystemValveVoteBecomes(SealedSystemValvePosition_t position)
   {
      GivenTheSealedSystemValveVoteIs(position);
   }

   void CompressorFrequencyShouldBe(PwmFrequency_t expected)
   {
      PwmFrequency_t actual;
      DataModel_Read(dataModel, Erd_CompressorInverterDriver, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void GivenGridOverrideEnableIs(bool enable)
   {
      DataModel_Write(dataModel, Erd_GridOverrideEnable, &enable);
   }
};

TEST(VariableSpeedCompressorIntegrationWithSealedSystemValve, ShouldNotDelayCompressorStartOnStartupEvenIfSealedSystemValveMoves)
{
   GivenTheSealedSystemValveVoteIs(SealedSystemValvePosition_A);
   GivenApplicationHasBeenInitialized();

   WhenTheSealedSystemValveVoteBecomes(SealedSystemValvePosition_B);
   WhenCompressorSpeedGridVoteChangesTo(CompressorSpeed_Medium);
   CompressorFrequencyShouldBe(compressorData->compressorSpeeds.startupSpeedFrequencyInHz);
}

TEST(VariableSpeedCompressorIntegrationWithSealedSystemValve, ShouldDelayCompressorStartOnStartupIfSealedSystemValveMovesOnSecondCompressorCycle)
{
   GivenApplicationHasBeenInitialized();
   GivenTheSealedSystemValveVoteIs(SealedSystemValvePosition_A);
   GivenGridOverrideEnableIs(SET);

   WhenCompressorSpeedGridVoteChangesTo(CompressorSpeed_Medium);
   After(compressorData->compressorTimes.minimumOnTimeInMinutes * MSEC_PER_MIN);

   WhenCompressorSpeedGridVoteChangesTo(CompressorSpeed_Off);
   After(compressorData->compressorTimes.minimumOffTimeInMinutes * MSEC_PER_MIN);

   WhenTheSealedSystemValveVoteBecomes(SealedSystemValvePosition_B);
   WhenCompressorSpeedGridVoteChangesTo(CompressorSpeed_SuperHigh);
   CompressorFrequencyShouldBe(0);

   After(compressorData->compressorTimes.remainOffAfterValveMoveTimeInMinutes * MSEC_PER_MIN - 1);
   CompressorFrequencyShouldBe(0);

   After(1);
   CompressorFrequencyShouldBe(compressorData->compressorSpeeds.startupSpeedFrequencyInHz);
}
