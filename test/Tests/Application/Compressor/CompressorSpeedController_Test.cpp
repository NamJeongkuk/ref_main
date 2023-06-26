/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "CompressorSpeedController.h"
#include "DataModelErdPointerAccess.h"
#include "CompressorData.h"
#include "Constants_Time.h"
#include "Constants_Binary.h"
#include "ValvePosition.h"
#include "SystemErds.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "uassert_test.h"
#include "ReferDataModel_TestDouble.h"
#include "TimerModule_TestDouble.h"
#include "CompressorSpeedFrequencyInHz.h"
#include "Vote.h"
#include "TddPersonality.h"

#define Given
#define When
#define And
#define Active true
#define Inactive false

enum
{
   SomeAmbientTemperatureAboveTheThreshold = 301,
   SomeAmbientTemperatureBelowTheThreshold = 299,
};

static const CompressorSpeedControllerConfiguration_t config = {
   .compressorStateErd = Erd_CompressorState,
   .compressorSpeedRequestErd = Erd_CompressorControllerSpeedRequest,
   .compressorSpeedResolvedVoteErd = Erd_CompressorSpeed_ResolvedVote,
   .valvePositionResolvedVoteErd = Erd_ValvePosition_ResolvedVote,
   .filteredAmbientTemperatureInDegFx100Erd = Erd_Ambient_FilteredInternalTemperatureResolvedInDegFx100,
   .disableMinimumCompressorTimeErd = Erd_DisableMinimumCompressorTimes,
};

TEST_GROUP(VariableSpeedCompressorSpeedController)
{
   ReferDataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;
   PersonalityParametricData_t *personalityParametricData;
   const CompressorData_t *compressorData;
   CompressorSpeedController_t instance;
   TimerModule_t *timerModule;
   TimerModule_TestDouble_t *timerModuleDouble;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelTestDouble);
      timerModuleDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelTestDouble);
      dataModel = dataModelTestDouble.dataModel;

      DataModel_Read(
         dataModel,
         Erd_PersonalityParametricData,
         &personalityParametricData);
      compressorData = personalityParametricData->compressorData;
   }

   void TheCompressorSpeedControllerIsInitialized()
   {
      CompressorSpeedController_Init(
         &instance,
         dataModel,
         &config);
   }

   void TheCompressorHsmStateShouldBe(CompressorState_t expected)
   {
      CompressorState_t actual;
      DataModel_Read(
         dataModel,
         Erd_CompressorState,
         &actual);
      CHECK_EQUAL(expected, actual);
   }

   void TheFilteredAmbientTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(
         dataModel,
         Erd_Ambient_FilteredInternalTemperatureResolvedInDegFx100,
         &temperature);
   }

   void TheValvePositionIs(ValvePosition_t position)
   {
      ValveVotedPosition_t votedPosition;
      votedPosition.care = true;
      votedPosition.position = position;
      DataModel_Write(
         dataModel,
         Erd_ValvePosition_ResolvedVote,
         &votedPosition);
   }

   ValvePosition_t GetValvePosition()
   {
      ValveVotedPosition_t actualValvePosition;
      DataModel_Read(
         dataModel,
         Erd_ValvePosition_ResolvedVote,
         &actualValvePosition);

      return actualValvePosition.position;
   }

   void TheValvePositionChanges()
   {
      if(GetValvePosition() == ValvePosition_B)
      {
         TheValvePositionIs(ValvePosition_C);
      }
      else
      {
         TheValvePositionIs(ValvePosition_B);
      }
   }

   void MinimumTimesAreDisabled()
   {
      bool active = true;
      DataModel_Write(
         dataModel,
         Erd_DisableMinimumCompressorTimes,
         &active);
   }

   void MinimumTimesAreEnabled()
   {
      bool inactive = false;
      DataModel_Write(
         dataModel,
         Erd_DisableMinimumCompressorTimes,
         &inactive);
   }

   void TheCompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_t requestedState)
   {
      if(requestedState == CompressorState_OffAndReadyToChange)
      {
         Given TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Off, Vote_Care);
         And TheCompressorSpeedControllerIsInitialized();
      }

      else if(requestedState == CompressorState_SabbathDelay)
      {
         Given TheCompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_OffAndReadyToChange);
         When TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);
      }

      else if(requestedState == CompressorState_Startup)
      {
         Given TheCompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_SabbathDelay);
         After(compressorData->compressorTimes.sabbathDelayTimeInSeconds * MSEC_PER_SEC);
      }

      else if(requestedState == CompressorState_MinimumOnTime)
      {
         Given TheCompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_Startup);
         After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC);
      }

      else if(requestedState == CompressorState_OnAndReadyToChange)
      {
         Given TheCompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_MinimumOnTime);
         After(compressorData->compressorTimes.minimumOnTimeInMinutes * MSEC_PER_MIN);
      }

      else if(requestedState == CompressorState_VariableSpeedMinimumRunTime)
      {
         Given TheCompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_OnAndReadyToChange);
         When TheResolvedVoteChangesToAnotherNonzeroSpeed();
      }

      else if(requestedState == CompressorState_MinimumOffTime)
      {
         Given TheCompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_OnAndReadyToChange);
         When TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Off, Vote_Care);
      }

      else if(requestedState == CompressorState_RemainOffAfterValveMove)
      {
         Given TheCompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_MinimumOffTime);
         When TheValvePositionChanges();
      }

      TheCompressorHsmStateShouldBe(requestedState);
   }

   void TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_t speed, bool care)
   {
      CompressorVotedSpeed_t vote;
      vote.speed = speed;
      vote.care = care;

      DataModel_Write(
         dataModel,
         Erd_CompressorSpeed_ResolvedVote,
         &vote);
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleDouble, ticks, 1000);
   }

   void TheCompressorControllerSpeedRequestShouldBe(CompressorSpeed_t expected)
   {
      CompressorSpeed_t actual;
      DataModel_Read(
         dataModel,
         Erd_CompressorControllerSpeedRequest,
         &actual);
      CHECK_EQUAL(expected, actual);
   }

   CompressorSpeed_t GetCompressorResolvedVotedSpeed()
   {
      CompressorVotedSpeed_t currentVotedSpeed;
      DataModel_Read(
         dataModel,
         Erd_CompressorSpeed_ResolvedVote,
         &currentVotedSpeed);
      return currentVotedSpeed.speed;
   }

   void TheResolvedVoteChangesToAnotherNonzeroSpeed()
   {
      if(GetCompressorResolvedVotedSpeed() == CompressorSpeed_Low)
      {
         TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Medium, Vote_Care);
      }
      else
      {
         TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);
      }
   }
};

TEST(VariableSpeedCompressorSpeedController, ShouldInitializeIntoOffAndReadyToChangeIfVotedSpeedIsZero)
{
   Given TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Off, Vote_Care);
   And TheCompressorSpeedControllerIsInitialized();
   TheCompressorHsmStateShouldBe(CompressorState_OffAndReadyToChange);
}

TEST(VariableSpeedCompressorSpeedController, ShouldInitializeIntoOnAndReadyToChangeIfVotedSpeedIsNonZeroAndMinimumTimesAreDisabled)
{
   Given TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);
   And TheCompressorSpeedControllerIsInitialized();
   And MinimumTimesAreDisabled();

   TheCompressorHsmStateShouldBe(CompressorState_OnAndReadyToChange);
}

TEST(VariableSpeedCompressorSpeedController, ShouldInitializeIntoSabbathDelayIfVotedSpeedIsNonZeroAndMinimumTimesAreEnabled)
{
   Given TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);
   Given TheCompressorSpeedControllerIsInitialized();
   TheCompressorHsmStateShouldBe(CompressorState_SabbathDelay);
}

TEST(VariableSpeedCompressorSpeedController, ShouldTransitionToOffStateOnInitWhenVotedCompressorSpeedIsOff)
{
   Given TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Off, Vote_Care);
   And TheCompressorSpeedControllerIsInitialized();

   TheCompressorHsmStateShouldBe(CompressorState_OffAndReadyToChange);
   And TheCompressorControllerSpeedRequestShouldBe(CompressorSpeed_Off);
}

TEST(VariableSpeedCompressorSpeedController, TheCompressorControllerSpeedRequestShouldBeOffWhenResolvedRequestBecomesNonZeroOnInit)
{
   Given TheCompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_OffAndReadyToChange);

   TheCompressorControllerSpeedRequestShouldBe(CompressorSpeed_Off);

   When TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_High, Vote_Care);
   TheCompressorControllerSpeedRequestShouldBe(CompressorSpeed_Off);
}

TEST(VariableSpeedCompressorSpeedController, ShouldHaveCompressorSpeedRequestOffWhenInSabbathDelayWithNonZeroResolvedSpeedVoteEvenIfSpeedChanges)
{
   Given TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);
   And TheCompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_SabbathDelay);
   TheCompressorControllerSpeedRequestShouldBe(CompressorSpeed_Off);

   When TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_High, Vote_Care);
   TheCompressorControllerSpeedRequestShouldBe(CompressorSpeed_Off);
}

TEST(VariableSpeedCompressorSpeedController, ShouldNotRequestACompressorSpeedForSabbathTimeWhileInSabbathStateThenShouldTransitionToStartupStateAfterStartupTime)
{
   Given TheCompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_SabbathDelay);
   TheCompressorControllerSpeedRequestShouldBe(CompressorSpeed_Off);

   After(compressorData->compressorTimes.sabbathDelayTimeInSeconds * MSEC_PER_SEC - 1);
   TheCompressorControllerSpeedRequestShouldBe(CompressorSpeed_Off);

   After(1);
   TheCompressorHsmStateShouldBe(CompressorState_Startup);
}

TEST(VariableSpeedCompressorSpeedController, ShouldRequestStartupSpeedUponEntryToStartupState)
{
   Given TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);
   And TheCompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_Startup);
   TheCompressorControllerSpeedRequestShouldBe(CompressorSpeed_Startup);
}

TEST(VariableSpeedCompressorSpeedController, ShouldRequestCompressorStartupSpeedForStartupTimerWhileInStartupState)
{
   Given TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);
   And TheCompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_Startup);
   TheCompressorControllerSpeedRequestShouldBe(CompressorSpeed_Startup);

   When TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);
   TheCompressorControllerSpeedRequestShouldBe(CompressorSpeed_Startup);
}

TEST(VariableSpeedCompressorSpeedController, ShouldTransitionToMinimumOnTimeFromStartupStateAfterStartupTime)
{
   Given TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);
   And TheCompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_Startup);

   After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC - 1);
   TheCompressorHsmStateShouldBe(CompressorState_Startup);

   After(1);
   TheCompressorHsmStateShouldBe(CompressorState_MinimumOnTime);
}

TEST(VariableSpeedCompressorSpeedController, ShouldRequestCachedSpeedFromSabbathStateOnceInMinimumOnTimeEvenIfResolvedSpeedChanges)
{
   Given TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);
   And TheCompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_Startup);

   After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC - 1);
   TheCompressorHsmStateShouldBe(CompressorState_Startup);

   When TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Medium, Vote_Care);

   After(1);
   TheCompressorControllerSpeedRequestShouldBe(CompressorSpeed_Low);
}

TEST(VariableSpeedCompressorSpeedController, ShouldNotTransitionFromOffAndReadyToChangeIfVoteChangesFromOffAndDontCareToOffAndCare)
{
   Given TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Off, Vote_DontCare);
   And TheCompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_OffAndReadyToChange);

   When TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Off, Vote_Care);
   TheCompressorHsmStateShouldBe(CompressorState_OffAndReadyToChange);
}

TEST(VariableSpeedCompressorSpeedController, ShouldTransitionFromMinimumOnTimeToOnAndReadyToChangeAfterMinimumOnTimerExpires)
{
   Given TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);
   And TheCompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_MinimumOnTime);

   After(compressorData->compressorTimes.minimumOnTimeInMinutes * MSEC_PER_MIN - 1);
   TheCompressorHsmStateShouldBe(CompressorState_MinimumOnTime);

   After(1);
   TheCompressorHsmStateShouldBe(CompressorState_OnAndReadyToChange);
}

TEST(VariableSpeedCompressorSpeedController, ShouldNotRequestCompressorSpeedInTransitionFromMinimumOnTimeToOnAndReadyToChange)
{
   Given TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);
   And TheCompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_MinimumOnTime);
   TheCompressorControllerSpeedRequestShouldBe(CompressorSpeed_Low);

   After(compressorData->compressorTimes.minimumOnTimeInMinutes * MSEC_PER_MIN);
   TheCompressorControllerSpeedRequestShouldBe(CompressorSpeed_Low);
}

TEST(VariableSpeedCompressorSpeedController, ShouldTransitionFromOnAndReadyToChangeToMinimumOffTimeWhenResolvedSpeedVoteBecomesOff)
{
   Given TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);
   And TheCompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_OnAndReadyToChange);
   TheCompressorControllerSpeedRequestShouldBe(CompressorSpeed_Low);

   When TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Off, Vote_Care);
   TheCompressorHsmStateShouldBe(CompressorState_MinimumOffTime);
   And TheCompressorControllerSpeedRequestShouldBe(CompressorSpeed_Off);
}

TEST(VariableSpeedCompressorSpeedController, ShouldTransitionFromOnAndReadyToChangeToVariableSpeedMinimumRunTimeWhenResolvedSpeedVoteChangesToNonzeroSpeed)
{
   Given TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);
   And TheCompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_OnAndReadyToChange);
   TheCompressorControllerSpeedRequestShouldBe(CompressorSpeed_Low);

   When TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Medium, Vote_Care);
   TheCompressorHsmStateShouldBe(CompressorState_VariableSpeedMinimumRunTime);
   TheCompressorControllerSpeedRequestShouldBe(CompressorSpeed_Medium);
}

TEST(VariableSpeedCompressorSpeedController, ShouldTransitionFromVariableSpeedMinimumRunTimeToOnAndReadyToChangeWhenTheMinimumRunTimerCompletes)
{
   Given TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);
   And TheCompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_OnAndReadyToChange);
   And TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Medium, Vote_Care);

   After(compressorData->compressorTimes.variableSpeedMinimumRunTimeInMinutes * MSEC_PER_MIN - 1);
   TheCompressorHsmStateShouldBe(CompressorState_VariableSpeedMinimumRunTime);

   After(1);
   TheCompressorHsmStateShouldBe(CompressorState_OnAndReadyToChange);
}

TEST(VariableSpeedCompressorSpeedController, ShouldNotChangeRequestedSpeedWhileInMinimumRunTimeEvenIfResolvedCompressorSpeedVoteChangesToNonzeroSpeed)
{
   Given TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);
   And TheCompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_OnAndReadyToChange);
   And TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Medium, Vote_Care);

   After(compressorData->compressorTimes.variableSpeedMinimumRunTimeInMinutes * MSEC_PER_MIN - 1);
   When TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);
   TheCompressorControllerSpeedRequestShouldBe(CompressorSpeed_Medium);

   After(1);
   TheCompressorControllerSpeedRequestShouldBe(CompressorSpeed_Medium);
   TheCompressorHsmStateShouldBe(CompressorState_OnAndReadyToChange);
}

TEST(VariableSpeedCompressorSpeedController, ShouldTransitionFromMinimumOffTimeToOffAndReadyToChangeAfterMinimumOffTimeInMinutes)
{
   Given TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);
   And TheCompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_MinimumOffTime);

   After(compressorData->compressorTimes.minimumOffTimeInMinutes * MSEC_PER_MIN - 1);
   TheCompressorHsmStateShouldBe(CompressorState_MinimumOffTime);
   TheCompressorControllerSpeedRequestShouldBe(CompressorSpeed_Off);

   After(1);
   TheCompressorHsmStateShouldBe(CompressorState_OffAndReadyToChange);
   TheCompressorControllerSpeedRequestShouldBe(CompressorSpeed_Off);
}

TEST(VariableSpeedCompressorSpeedController, ShouldTransitionFromMinimumOffTimeToRemainOffAfterValveMoveWhenValvePositionChanges)
{
   Given TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);
   And TheCompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_MinimumOffTime);

   When TheValvePositionChanges();
   TheCompressorHsmStateShouldBe(CompressorState_RemainOffAfterValveMove);
}

TEST(VariableSpeedCompressorSpeedController, ShouldTransitionFromRemainOffAfterValveMoveToMinimumOffTimeAfterRemainOffAfterValveMoveTimerElapses)
{
   Given TheFilteredAmbientTemperatureIs(SomeAmbientTemperatureAboveTheThreshold);
   And TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);
   And TheCompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_MinimumOffTime);

   When TheValvePositionChanges();
   TheCompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_RemainOffAfterValveMove);

   After(compressorData->compressorTimes.remainOffAfterValveMoveTimeInMinutes * MSEC_PER_MIN - 1);
   TheCompressorHsmStateShouldBe(CompressorState_RemainOffAfterValveMove);

   After(1);
   TheCompressorHsmStateShouldBe(CompressorState_MinimumOffTime);
}

TEST(VariableSpeedCompressorSpeedController, ShouldTransitionFromRemainOffAfterValveMoveToOffAndReadyAfterRemainOffAfterValveMoveTimerElapsesWithLowAmbientTemperature)
{
   Given TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);
   And TheFilteredAmbientTemperatureIs(SomeAmbientTemperatureBelowTheThreshold);
   And TheCompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_OffAndReadyToChange);

   When TheValvePositionChanges();
   After(compressorData->compressorTimes.remainOffAfterValveMoveLowAmbientTimeInMinutes * MSEC_PER_MIN - 1);
   TheCompressorHsmStateShouldBe(CompressorState_RemainOffAfterValveMove);

   After(1);
   TheCompressorHsmStateShouldBe(CompressorState_OffAndReadyToChange);
}

TEST(VariableSpeedCompressorSpeedController, ShouldTransitionFromRemainOffAfterValveMoveToMinimumOffTimeAfterRemainOffAfterValveMoveTimerElapsesWithLowAmbientTemperature)
{
   Given TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);
   And TheFilteredAmbientTemperatureIs(SomeAmbientTemperatureBelowTheThreshold);
   And TheCompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_MinimumOffTime);

   When TheValvePositionChanges();
   After(compressorData->compressorTimes.remainOffAfterValveMoveLowAmbientTimeInMinutes * MSEC_PER_MIN - 1);
   TheCompressorHsmStateShouldBe(CompressorState_RemainOffAfterValveMove);

   After(1);
   TheCompressorHsmStateShouldBe(CompressorState_MinimumOffTime);
}

TEST(VariableSpeedCompressorSpeedController, ShouldTransitionToRemainOffAfterValveMoveThenToOffAndReadyToChangeWhenValveMovesLateInMinimumOffTime)
{
   Given TheFilteredAmbientTemperatureIs(SomeAmbientTemperatureAboveTheThreshold);
   And TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);
   And TheCompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_MinimumOffTime);

   After(compressorData->compressorTimes.minimumOffTimeInMinutes * MSEC_PER_MIN - 1);
   TheValvePositionChanges();
   TheCompressorHsmStateShouldBe(CompressorState_RemainOffAfterValveMove);

   After(compressorData->compressorTimes.remainOffAfterValveMoveTimeInMinutes * MSEC_PER_MIN - 1);
   TheCompressorHsmStateShouldBe(CompressorState_RemainOffAfterValveMove);

   After(1);
   TheCompressorHsmStateShouldBe(CompressorState_OffAndReadyToChange);
}

TEST(VariableSpeedCompressorSpeedController, ShouldTransitionToRemainOffAfterValveMoveThenToOffAndReadyToChangeWhenValveMovesLateInMinimumOffTimeWithLowAmbientTemperature)
{
   Given TheFilteredAmbientTemperatureIs(SomeAmbientTemperatureBelowTheThreshold);
   And TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);
   And TheCompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_MinimumOffTime);

   After(compressorData->compressorTimes.minimumOffTimeInMinutes * MSEC_PER_MIN - 1);
   TheValvePositionChanges();
   TheCompressorHsmStateShouldBe(CompressorState_RemainOffAfterValveMove);

   After(compressorData->compressorTimes.remainOffAfterValveMoveLowAmbientTimeInMinutes * MSEC_PER_MIN - 1);
   TheCompressorHsmStateShouldBe(CompressorState_RemainOffAfterValveMove);

   After(1);
   TheCompressorHsmStateShouldBe(CompressorState_OffAndReadyToChange);
}

TEST(VariableSpeedCompressorSpeedController, ShouldNotRequestACompressorSpeedInTransitionFromOffAndReadyToChangeToRemainOffAfterValveMove)
{
   When TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Off, Vote_Care);
   And TheCompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_OffAndReadyToChange);

   TheValvePositionChanges();
   And TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);

   TheCompressorHsmStateShouldBe(CompressorState_RemainOffAfterValveMove);
   TheCompressorControllerSpeedRequestShouldBe(CompressorSpeed_Off);
}

TEST(VariableSpeedCompressorSpeedController, ShouldTransitionToRemainOffAfterValveMoveFromOffAndReadyToChangeWhenValveMoves)
{
   When TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Off, Vote_Care);
   And TheCompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_OffAndReadyToChange);

   When TheValvePositionChanges();
   TheCompressorHsmStateShouldBe(CompressorState_RemainOffAfterValveMove);
}

TEST(VariableSpeedCompressorSpeedController, ShouldTransitionFromMinimumOnTimeToMinimumOffTimeAfterMinimumOnTimeIfCompressorVoteBecomesOff)
{
   Given TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);
   And TheCompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_MinimumOnTime);

   After(compressorData->compressorTimes.minimumOnTimeInMinutes * MSEC_PER_MIN - 1);
   TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Off, Vote_Care);

   After(1);
   TheCompressorHsmStateShouldBe(CompressorState_MinimumOffTime);
}

TEST(VariableSpeedCompressorSpeedController, ShouldTransitionFromSabbathDelayToOnAndReadyWhenDisableRequestIsSetAndResolvedVoteIsNonzero)
{
   Given TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);
   And TheCompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_SabbathDelay);

   And MinimumTimesAreDisabled();

   TheCompressorHsmStateShouldBe(CompressorState_OnAndReadyToChange);
   TheCompressorControllerSpeedRequestShouldBe(CompressorSpeed_Low);
}

TEST(VariableSpeedCompressorSpeedController, ShouldTransitionFromSabbathDelayToOffAndReadyWhenDisableRequestIsSetAndResolvedVoteIsZero)
{
   Given TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);
   And TheCompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_SabbathDelay);

   When TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Off, Vote_Care);
   And MinimumTimesAreDisabled();

   TheCompressorHsmStateShouldBe(CompressorState_OffAndReadyToChange);
   TheCompressorControllerSpeedRequestShouldBe(CompressorSpeed_Off);
}

TEST(VariableSpeedCompressorSpeedController, ShouldRequestResolvedCompressorSpeedIfMinimumTimesAreDisabledDuringSabbathDelay)
{
   Given TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);
   And TheCompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_SabbathDelay);

   When TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_High, Vote_Care);
   And MinimumTimesAreDisabled();

   TheCompressorControllerSpeedRequestShouldBe(CompressorSpeed_High);
}

TEST(VariableSpeedCompressorSpeedController, ShouldTransitionFromMinimumOnTimeToOnAndReadyWhenDisableRequestIsSetAndResolvedVoteIsNonZero)
{
   Given TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);
   And TheCompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_MinimumOnTime);

   When TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_High, Vote_Care);
   And MinimumTimesAreDisabled();

   TheCompressorHsmStateShouldBe(CompressorState_OnAndReadyToChange);
   TheCompressorControllerSpeedRequestShouldBe(CompressorSpeed_High);
}

TEST(VariableSpeedCompressorSpeedController, ShouldTransitionFromSabbathDelayTimeToOffAndReadyWhenDisableRequestIsSetAndResolvedVoteIsZero)
{
   Given TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);
   And TheCompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_SabbathDelay);

   When TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Off, Vote_Care);
   And MinimumTimesAreDisabled();

   TheCompressorHsmStateShouldBe(CompressorState_OffAndReadyToChange);
   TheCompressorControllerSpeedRequestShouldBe(CompressorSpeed_Off);
}

TEST(VariableSpeedCompressorSpeedController, ShouldTransitionFromMinimumOffTimeToOnAndReadyWhenDisableRequestIsSetAndResolvedVoteIsNonZero)
{
   Given TheCompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_MinimumOffTime);

   When TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);
   And MinimumTimesAreDisabled();

   TheCompressorHsmStateShouldBe(CompressorState_OnAndReadyToChange);
   TheCompressorControllerSpeedRequestShouldBe(CompressorSpeed_Low);
}

TEST(VariableSpeedCompressorSpeedController, ShouldTransitionFromMinimumOffTimeToOffAndReadyWhenDisableRequestIsSetAndResolvedVoteIsZero)
{
   When TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);
   And TheCompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_MinimumOffTime);

   When TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Off, Vote_Care);
   And MinimumTimesAreDisabled();

   TheCompressorHsmStateShouldBe(CompressorState_OffAndReadyToChange);
   TheCompressorControllerSpeedRequestShouldBe(CompressorSpeed_Off);
}

TEST(VariableSpeedCompressorSpeedController, ShouldTransitionFromVariableSpeedMinimumRunTimeToOffAndReadyWhenDisableRequestIsSetAndResolvedVoteIsZero)
{
   Given TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);
   And TheCompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_OnAndReadyToChange);
   And TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Medium, Vote_Care);

   When TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Off, Vote_Care);
   And MinimumTimesAreDisabled();

   TheCompressorHsmStateShouldBe(CompressorState_OffAndReadyToChange);
   TheCompressorControllerSpeedRequestShouldBe(CompressorSpeed_Off);
}

TEST(VariableSpeedCompressorSpeedController, ShouldTransitionFromVariableSpeedMinimumRunTimeToOnAndReadyWhenDisableRequestIsSetAndResolvedVoteIsNonZero)
{
   Given TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);
   And TheCompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_OnAndReadyToChange);
   And TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Medium, Vote_Care);

   And MinimumTimesAreDisabled();

   TheCompressorHsmStateShouldBe(CompressorState_OnAndReadyToChange);
   TheCompressorControllerSpeedRequestShouldBe(CompressorSpeed_Medium);
}

TEST(VariableSpeedCompressorSpeedController, ShouldTransitionFromRemainOffAfterValveMoveToOffAndReadyWhenDisableRequestIsSetAndResolvedVoteIsZero)
{
   Given TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);
   And TheCompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_RemainOffAfterValveMove);

   When TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Off, Vote_Care);
   And MinimumTimesAreDisabled();

   TheCompressorHsmStateShouldBe(CompressorState_OffAndReadyToChange);
   TheCompressorControllerSpeedRequestShouldBe(CompressorSpeed_Off);
}

TEST(VariableSpeedCompressorSpeedController, ShouldTransitionFromVariableSpeedToOnAndReadyWhenDisableRequestIsSetAndResolvedVoteIsNonZero)
{
   Given TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);
   And TheCompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_OnAndReadyToChange);
   And TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Medium, Vote_Care);

   And MinimumTimesAreDisabled();

   TheCompressorHsmStateShouldBe(CompressorState_OnAndReadyToChange);
   And TheCompressorControllerSpeedRequestShouldBe(CompressorSpeed_Medium);
}

TEST(VariableSpeedCompressorSpeedController, ShouldTransitionFromOffAndReadyToChangeToOnAndReadyToChangeWhenMinimumTimeDisabledIsSet)
{
   Given TheCompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_OffAndReadyToChange);
   And MinimumTimesAreDisabled();

   When TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_High, Vote_Care);
   TheCompressorHsmStateShouldBe(CompressorState_OnAndReadyToChange);
   And TheCompressorControllerSpeedRequestShouldBe(CompressorSpeed_High);
}

TEST(VariableSpeedCompressorSpeedController, ShouldTransitionFromOnAndReadyToChangeToOffAndReadyToChangeWhenMinimumTimeDisabledIsSet)
{
   Given TheCompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_OnAndReadyToChange);
   And MinimumTimesAreDisabled();

   When TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Off, Vote_Care);
   TheCompressorHsmStateShouldBe(CompressorState_OffAndReadyToChange);
   And TheCompressorControllerSpeedRequestShouldBe(CompressorSpeed_Off);
}

TEST(VariableSpeedCompressorSpeedController, ShouldNotTransitionFromOffAndReadyToChangeWhenValveMovesWhenDisableMinimumTimeIsSet)
{
   Given TheCompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_OffAndReadyToChange);
   And MinimumTimesAreDisabled();

   When TheValvePositionChanges();
   TheCompressorHsmStateShouldBe(CompressorState_OffAndReadyToChange);
}

TEST(VariableSpeedCompressorSpeedController, ShouldNotLeaveOnAndReadyToChangeWhenResolvedSpeedVoteChangesWhenDisableMinimumTimeIsSet)
{
   Given TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);
   When TheCompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_OnAndReadyToChange);
   And MinimumTimesAreDisabled();

   When TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_High, Vote_Care);
   TheCompressorHsmStateShouldBe(CompressorState_OnAndReadyToChange);
   And TheCompressorControllerSpeedRequestShouldBe(CompressorSpeed_High);

   When TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_SuperLow, Vote_Care);
   TheCompressorHsmStateShouldBe(CompressorState_OnAndReadyToChange);
   And TheCompressorControllerSpeedRequestShouldBe(CompressorSpeed_SuperLow);
}

TEST(VariableSpeedCompressorSpeedController, ShouldTransitionFromOnAndReadyToMinimumOffTimeTimeWhenSpeedVoteBecomesNonZeroAfterReEnablingMinimumOnTime)
{
   When TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);
   Given TheCompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_OnAndReadyToChange);
   And MinimumTimesAreDisabled();

   When TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_High, Vote_Care);
   TheCompressorHsmStateShouldBe(CompressorState_OnAndReadyToChange);

   When MinimumTimesAreEnabled();

   When TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Off, Vote_Care);
   TheCompressorHsmStateShouldBe(CompressorState_MinimumOffTime);
}

TEST(VariableSpeedCompressorSpeedController, ShouldTransitionFromOffAndReadyToSabbathDelayTimeWhenSpeedVoteBecomesNonZeroAfterReEnablingMinimumOnTime)
{
   Given TheCompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_OffAndReadyToChange);
   And MinimumTimesAreDisabled();

   TheCompressorHsmStateShouldBe(CompressorState_OffAndReadyToChange);

   When MinimumTimesAreEnabled();
   And TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_High, Vote_Care);
   And TheCompressorHsmStateShouldBe(CompressorState_SabbathDelay);
   And TheCompressorControllerSpeedRequestShouldBe(CompressorSpeed_Off);
}

TEST(VariableSpeedCompressorSpeedController, ShouldNotRestartMinimumOffTimeIfComingFromRemainOnAfterValveMoveState)
{
   Given TheFilteredAmbientTemperatureIs(SomeAmbientTemperatureAboveTheThreshold);
   And TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);
   And TheCompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_MinimumOffTime);

   When TheValvePositionChanges();
   TheCompressorHsmStateShouldBe(CompressorState_RemainOffAfterValveMove);

   After(compressorData->compressorTimes.remainOffAfterValveMoveTimeInMinutes * MSEC_PER_MIN);
   TheCompressorHsmStateShouldBe(CompressorState_MinimumOffTime);

   After((compressorData->compressorTimes.minimumOffTimeInMinutes * MSEC_PER_MIN) - (compressorData->compressorTimes.remainOffAfterValveMoveTimeInMinutes * MSEC_PER_MIN) - 1);
   TheCompressorHsmStateShouldBe(CompressorState_MinimumOffTime);

   After(1);
   TheCompressorHsmStateShouldBe(CompressorState_OffAndReadyToChange);
}

TEST(VariableSpeedCompressorSpeedController, ShouldTransitionFromMinimumOffTimeToSabbathDelayIfResolvedVoteBecomesNonZeroDuringMinimumOffTime)
{
   Given TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);
   And TheCompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_MinimumOffTime);

   After(compressorData->compressorTimes.minimumOffTimeInMinutes * MSEC_PER_MIN - 1);
   TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_High, Vote_Care);
   TheCompressorHsmStateShouldBe(CompressorState_MinimumOffTime);

   After(1);
   TheCompressorHsmStateShouldBe(CompressorState_SabbathDelay);
}

TEST(VariableSpeedCompressorSpeedController, ShouldInitializeIntoSabbathDelayStateIfResolvedVoteIsNonZeroOnInit)
{
   Given TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);

   When TheCompressorSpeedControllerIsInitialized();
   TheCompressorHsmStateShouldBe(CompressorState_SabbathDelay);
}

TEST(VariableSpeedCompressorSpeedController, ShouldDisableCompressorTimesWhenMinimumTimesAreDisabled)
{
   Given TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);
   And TheCompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_MinimumOnTime);

   When TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Off, Vote_Care);

   When MinimumTimesAreDisabled();
   And TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);
   TheCompressorHsmStateShouldBe(CompressorState_OnAndReadyToChange);

   When MinimumTimesAreEnabled();
   And TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Off, Vote_Care);
   TheCompressorHsmStateShouldBe(CompressorState_MinimumOffTime);

   After(compressorData->compressorTimes.minimumOffTimeInMinutes * MSEC_PER_MIN - 1);
   TheCompressorHsmStateShouldBe(CompressorState_MinimumOffTime);

   After(1);
   TheCompressorHsmStateShouldBe(CompressorState_OffAndReadyToChange);
}

TEST_GROUP(SingleSpeedCompressorSpeedController)
{
   ReferDataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;
   PersonalityParametricData_t *personalityParametricData;
   const CompressorData_t *compressorData;
   CompressorSpeedController_t instance;
   TimerModule_t *timerModule;
   TimerModule_TestDouble_t *timerModuleDouble;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelTestDouble, TddPersonality_DevelopmentSingleSpeedCompressor);
      timerModuleDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelTestDouble);
      dataModel = dataModelTestDouble.dataModel;

      DataModel_Read(
         dataModel,
         Erd_PersonalityParametricData,
         &personalityParametricData);
      compressorData = personalityParametricData->compressorData;
   }

   void TheCompressorSpeedControllerIsInitialized()
   {
      CompressorSpeedController_Init(
         &instance,
         dataModel,
         &config);
   }

   void TheCompressorHsmStateShouldBe(CompressorState_t expected)
   {
      CompressorState_t actual;
      DataModel_Read(
         dataModel,
         Erd_CompressorState,
         &actual);
      CHECK_EQUAL(expected, actual);
   }

   void TheCompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_t requestedState)
   {
      if(requestedState == CompressorState_OffAndReadyToChange)
      {
         Given TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Off, Vote_Care);
         And TheCompressorSpeedControllerIsInitialized();
      }

      else if(requestedState == CompressorState_SabbathDelay)
      {
         Given TheCompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_OffAndReadyToChange);
         When TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);
      }

      else if(requestedState == CompressorState_Startup)
      {
         Given TheCompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_SabbathDelay);
         After(compressorData->compressorTimes.sabbathDelayTimeInSeconds * MSEC_PER_SEC);
      }

      else if(requestedState == CompressorState_MinimumOnTime)
      {
         Given TheCompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_Startup);
         After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC);
      }

      else if(requestedState == CompressorState_OnAndReadyToChange)
      {
         Given TheCompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_MinimumOnTime);
         After(compressorData->compressorTimes.minimumOnTimeInMinutes * MSEC_PER_MIN);
      }

      else if(requestedState == CompressorState_VariableSpeedMinimumRunTime)
      {
         Given TheCompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_OnAndReadyToChange);
         When TheResolvedVoteChangesToAnotherNonzeroSpeed();
      }

      else if(requestedState == CompressorState_MinimumOffTime)
      {
         Given TheCompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_OnAndReadyToChange);
         When TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Off, Vote_Care);
      }

      TheCompressorHsmStateShouldBe(requestedState);
   }

   void TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_t speed, bool care)
   {
      CompressorVotedSpeed_t vote;
      vote.speed = speed;
      vote.care = care;

      DataModel_Write(
         dataModel,
         Erd_CompressorSpeed_ResolvedVote,
         &vote);
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleDouble, ticks, 1000);
   }

   void TheCompressorControllerSpeedRequestShouldBe(CompressorSpeed_t expected)
   {
      CompressorSpeed_t actual;
      DataModel_Read(
         dataModel,
         Erd_CompressorControllerSpeedRequest,
         &actual);
      CHECK_EQUAL(expected, actual);
   }

   CompressorSpeed_t GetCompressorResolvedVotedSpeed()
   {
      CompressorVotedSpeed_t currentVotedSpeed;
      DataModel_Read(
         dataModel,
         Erd_CompressorSpeed_ResolvedVote,
         &currentVotedSpeed);
      return currentVotedSpeed.speed;
   }

   void TheResolvedVoteChangesToAnotherNonzeroSpeed()
   {
      if(GetCompressorResolvedVotedSpeed() == CompressorSpeed_Low)
      {
         TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Medium, Vote_Care);
      }
      else
      {
         TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);
      }
   }
};

TEST(SingleSpeedCompressorSpeedController, ShouldNotTransitionFromOnAndReadyToChangeToMinimumVariableSpeedRunTimeWhenResolvedSpeedVoteChangesToNonzeroSpeed)
{
   Given TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);
   And TheCompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_OnAndReadyToChange);
   TheCompressorControllerSpeedRequestShouldBe(CompressorSpeed_Low);

   When TheResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Medium, Vote_Care);
   TheCompressorHsmStateShouldBe(CompressorState_OnAndReadyToChange);
}
