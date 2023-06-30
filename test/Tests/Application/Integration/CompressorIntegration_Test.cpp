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

   void WhenCompressorSpeedChangesTo(CompressorSpeed_t compressorSpeed)
   {
      CompressorVotedSpeed_t compressorVote;
      compressorVote.speed = compressorSpeed;
      compressorVote.care = Vote_Care;
      DataModel_Write(dataModel, Erd_CompressorSpeed_FactoryVote, &compressorVote);
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

      WhenCompressorSpeedChangesTo(CompressorSpeed_SuperLow);

      After(compressorData->compressorTimes.sabbathDelayTimeInSeconds * MSEC_PER_SEC);
      CompressorRelayShouldBe(ON);
   }

   void GivenDisableMinimumCompressorTimesIs(bool state)
   {
      BooleanVotedState_t votedState;
      votedState.state = state;
      votedState.care = Vote_Care;

      DataModel_Write(dataModel, Erd_DisableMinimumCompressorTimes_ResolvedVote, &votedState);
   }

   void WhenDisableMinimumCompressorTimesIs(bool state)
   {
      GivenDisableMinimumCompressorTimesIs(state);
   }
};

TEST(SingleSpeedCompressorIntegration, ShouldInitialize)
{
   GivenApplicationHasBeenInitialized();
}

TEST(SingleSpeedCompressorIntegration, ShouldTurnOnCompressorRelayAfterSabbathDelayTimeWhenCompressorSpeedChangesToSuperLow)
{
   GivenApplicationHasBeenInitialized();

   WhenCompressorSpeedChangesTo(CompressorSpeed_SuperLow);

   After(compressorData->compressorTimes.sabbathDelayTimeInSeconds * MSEC_PER_SEC - 1);
   CompressorRelayShouldBe(OFF);

   After(1);
   CompressorRelayShouldBe(ON);
}

TEST(SingleSpeedCompressorIntegration, ShouldTurnOffCompressorRelayAfterStartupOnAndMinimumOnTimesWhenCompressorSpeedChangesToOff)
{
   GivenApplicationHasBeenInitializedAndCompressorRelayIsOn();

   WhenCompressorSpeedChangesTo(CompressorSpeed_Off);

   After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC);

   After(compressorData->compressorTimes.minimumOnTimeInMinutes * MSEC_PER_MIN - 1);
   CompressorRelayShouldBe(ON);

   After(1);
   CompressorRelayShouldBe(OFF);
}

TEST(SingleSpeedCompressorIntegration, ShouldTurnOnCompressorRelayRightAfterUpdatingCompressorSpeedToSuperLowIfMinimumCompressorTimesAreDisabled)
{
   GivenApplicationHasBeenInitialized();
   GivenDisableMinimumCompressorTimesIs(SET);
   CompressorRelayShouldBe(OFF);

   WhenCompressorSpeedChangesTo(CompressorSpeed_SuperLow);
   CompressorRelayShouldBe(ON);
}

TEST(SingleSpeedCompressorIntegration, ShouldTurnOnCompressorRelayWhenDisableCompressorTimesIsSetDuringSabbathDelay)
{
   GivenApplicationHasBeenInitialized();

   WhenCompressorSpeedChangesTo(CompressorSpeed_SuperLow);

   After(compressorData->compressorTimes.sabbathDelayTimeInSeconds * MSEC_PER_SEC - 1);

   WhenDisableMinimumCompressorTimesIs(SET);
   CompressorRelayShouldBe(ON);
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

      WhenCompressorSpeedChangesTo(CompressorSpeed_SuperHigh);
      After(compressorData->compressorTimes.sabbathDelayTimeInSeconds * MSEC_PER_SEC);
      CompressorFrequencyShouldBe(compressorData->compressorSpeeds.startupSpeedFrequencyInHz);

      After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC);
      CompressorFrequencyShouldBe(compressorData->compressorSpeeds.superHighSpeedFrequencyInHz);
   }

   void WhenCompressorSpeedChangesTo(CompressorSpeed_t compressorSpeed)
   {
      CompressorVotedSpeed_t compressorVote;
      compressorVote.speed = compressorSpeed;
      compressorVote.care = Vote_Care;
      DataModel_Write(dataModel, Erd_CompressorSpeed_FactoryVote, &compressorVote);
   }

   void CompressorFrequencyShouldBe(PwmFrequency_t expected)
   {
      PwmFrequency_t actual;
      DataModel_Read(dataModel, Erd_CompressorInverterDriver, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks);
   }

   void GivenDisableMinimumCompressorTimesIs(bool state)
   {
      BooleanVotedState_t votedState;
      votedState.state = state;
      votedState.care = Vote_Care;

      DataModel_Write(dataModel, Erd_DisableMinimumCompressorTimes_ResolvedVote, &votedState);
   }

   void WhenDisableMinimumCompressorTimesIs(bool state)
   {
      GivenDisableMinimumCompressorTimesIs(state);
   }
};

TEST(VariableSpeedCompressorIntegration, ShouldInitialize)
{
   GivenApplicationHasBeenInitialized();
}

TEST(VariableSpeedCompressorIntegration, ShouldSetCompressorFrequencyToSuperLowAfterSabbathDelayAndStartupOnTimesWhenCompressorSpeedChangesToSuperLow)
{
   GivenApplicationHasBeenInitialized();

   WhenCompressorSpeedChangesTo(CompressorSpeed_SuperLow);
   CompressorFrequencyShouldBe(0);

   After(compressorData->compressorTimes.sabbathDelayTimeInSeconds * MSEC_PER_SEC);
   CompressorFrequencyShouldBe(compressorData->compressorSpeeds.startupSpeedFrequencyInHz);

   After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC - 1);
   CompressorFrequencyShouldBe(compressorData->compressorSpeeds.startupSpeedFrequencyInHz);

   After(1);
   CompressorFrequencyShouldBe(compressorData->compressorSpeeds.superLowSpeedFrequencyInHz);
}

TEST(VariableSpeedCompressorIntegration, ShouldSetCompressorFrequencyToLowAfterSabbathDelayAndStartupOnTimesWhenCompressorSpeedChangesToLow)
{
   GivenApplicationHasBeenInitialized();

   WhenCompressorSpeedChangesTo(CompressorSpeed_Low);
   CompressorFrequencyShouldBe(0);

   After(compressorData->compressorTimes.sabbathDelayTimeInSeconds * MSEC_PER_SEC);
   CompressorFrequencyShouldBe(compressorData->compressorSpeeds.startupSpeedFrequencyInHz);

   After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC);
   CompressorFrequencyShouldBe(
      compressorData->compressorSpeeds.coolingModeIndependentCompressorSpeeds.lowSpeedFrequencyInHz);
}

TEST(VariableSpeedCompressorIntegration, ShouldSetCompressorFrequencyToMediumAfterSabbathDelayAndStartupOnTimesWhenCompressorSpeedChangesToMedium)
{
   GivenApplicationHasBeenInitialized();

   WhenCompressorSpeedChangesTo(CompressorSpeed_Medium);
   CompressorFrequencyShouldBe(0);

   After(compressorData->compressorTimes.sabbathDelayTimeInSeconds * MSEC_PER_SEC);
   CompressorFrequencyShouldBe(compressorData->compressorSpeeds.startupSpeedFrequencyInHz);

   After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC);
   CompressorFrequencyShouldBe(
      compressorData->compressorSpeeds.coolingModeIndependentCompressorSpeeds.mediumSpeedFrequencyInHz);
}

TEST(VariableSpeedCompressorIntegration, ShouldSetCompressorFrequencyToHighAfterSabbathDelayAndStartupOnTimesWhenCompressorSpeedChangesToHigh)
{
   GivenApplicationHasBeenInitialized();

   WhenCompressorSpeedChangesTo(CompressorSpeed_High);
   CompressorFrequencyShouldBe(0);

   After(compressorData->compressorTimes.sabbathDelayTimeInSeconds * MSEC_PER_SEC);
   CompressorFrequencyShouldBe(compressorData->compressorSpeeds.startupSpeedFrequencyInHz);

   After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC);
   CompressorFrequencyShouldBe(
      compressorData->compressorSpeeds.coolingModeIndependentCompressorSpeeds.highSpeedFrequencyInHz);
}

TEST(VariableSpeedCompressorIntegration, ShouldSetCompressorFrequencyToSuperHighAfterSabbathDelayAndStartupOnTimesWhenCompressorSpeedChangesToSuperHigh)
{
   GivenApplicationHasBeenInitialized();

   WhenCompressorSpeedChangesTo(CompressorSpeed_SuperHigh);
   CompressorFrequencyShouldBe(0);

   After(compressorData->compressorTimes.sabbathDelayTimeInSeconds * MSEC_PER_SEC);
   CompressorFrequencyShouldBe(compressorData->compressorSpeeds.startupSpeedFrequencyInHz);

   After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC);
   CompressorFrequencyShouldBe(compressorData->compressorSpeeds.superHighSpeedFrequencyInHz);
}

TEST(VariableSpeedCompressorIntegration, ShouldSetCompressorFrequencyToZeroAfterMinimumOnTimeWhenCompressorSpeedChangesToOff)
{
   GivenApplicationHasBeenInitializedAndCompressorSpeedIsSuperHigh();

   WhenCompressorSpeedChangesTo(CompressorSpeed_Off);

   After(compressorData->compressorTimes.minimumOnTimeInMinutes * MSEC_PER_MIN - 1);
   CompressorFrequencyShouldBe(compressorData->compressorSpeeds.superHighSpeedFrequencyInHz);

   After(1);
   CompressorFrequencyShouldBe(0);
}

TEST(VariableSpeedCompressorIntegration, ShouldUpdateCompressorSpeedFromSuperHighToSuperLowWhenCompressorSpeedChangesToSuperLowAfterMinimumOnTimeInMinutes)
{
   GivenApplicationHasBeenInitializedAndCompressorSpeedIsSuperHigh();

   After(compressorData->compressorTimes.minimumOnTimeInMinutes * MSEC_PER_MIN);
   CompressorFrequencyShouldBe(compressorData->compressorSpeeds.superHighSpeedFrequencyInHz);

   WhenCompressorSpeedChangesTo(CompressorSpeed_SuperLow);
   CompressorFrequencyShouldBe(compressorData->compressorSpeeds.superLowSpeedFrequencyInHz);
}

TEST(VariableSpeedCompressorIntegration, ShouldUpdateCompressorFrequencyRightAfterUpdatingCompressorSpeedWhenDisableMinimumCompressorTimesIsSet)
{
   GivenApplicationHasBeenInitialized();
   GivenDisableMinimumCompressorTimesIs(SET);
   CompressorFrequencyShouldBe(0);

   WhenCompressorSpeedChangesTo(CompressorSpeed_SuperLow);
   CompressorFrequencyShouldBe(compressorData->compressorSpeeds.superLowSpeedFrequencyInHz);

   WhenCompressorSpeedChangesTo(CompressorSpeed_SuperHigh);
   CompressorFrequencyShouldBe(compressorData->compressorSpeeds.superHighSpeedFrequencyInHz);
}

TEST(VariableSpeedCompressorIntegration, ShouldUpdateCompressorFrequencyWhenDisableCompressorTimesIsSetDuringSabbathDelayTime)
{
   GivenApplicationHasBeenInitialized();

   WhenCompressorSpeedChangesTo(CompressorSpeed_Low);
   CompressorFrequencyShouldBe(0);

   After(compressorData->compressorTimes.sabbathDelayTimeInSeconds * MSEC_PER_SEC - 1);

   WhenDisableMinimumCompressorTimesIs(SET);
   CompressorFrequencyShouldBe(
      compressorData->compressorSpeeds.coolingModeIndependentCompressorSpeeds.lowSpeedFrequencyInHz);
}

TEST(VariableSpeedCompressorIntegration, ShouldUpdateCompressorFrequencyWhenDisableCompressorTimesIsSetDuringStartupOnTime)
{
   GivenApplicationHasBeenInitialized();

   WhenCompressorSpeedChangesTo(CompressorSpeed_Low);
   CompressorFrequencyShouldBe(0);

   After(compressorData->compressorTimes.sabbathDelayTimeInSeconds * MSEC_PER_SEC);
   CompressorFrequencyShouldBe(compressorData->compressorSpeeds.startupSpeedFrequencyInHz);

   After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC - 1);

   WhenDisableMinimumCompressorTimesIs(SET);
   CompressorFrequencyShouldBe(
      compressorData->compressorSpeeds.coolingModeIndependentCompressorSpeeds.lowSpeedFrequencyInHz);
}
