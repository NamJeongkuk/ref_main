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
#define The

enum
{
   SomeSabbathTimeInSeconds = 20
};

static const CompressorSpeedControllerConfiguration_t config = {
   .compressorStateErd = Erd_CompressorState,
   .compressorSpeedRequestErd = Erd_CompressorControllerSpeedRequest,
   .compressorSpeedResolvedVoteErd = Erd_CompressorSpeed_ResolvedVote,
   .sabbathDelayTimeInSeconds = SomeSabbathTimeInSeconds
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

   void CompressorSpeedControllerIsInitialized()
   {
      CompressorSpeedController_Init(
         &instance,
         dataModel,
         &config);
   }

   void CompressorHsmStateShouldBe(CompressorState_t expected)
   {
      CompressorState_t actual;
      DataModel_Read(
         dataModel,
         Erd_CompressorState,
         &actual);
      CHECK_EQUAL(expected, actual);
   }

   void ValvePositionIs(ValvePosition_t position)
   {
      ValveVotedPosition_t votedPosition;
      votedPosition.care = true;
      votedPosition.position = position;
      DataModel_Write(
         dataModel,
         Erd_ValvePosition_ResolvedVote,
         &votedPosition);
   }

   void CompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_t requestedState)
   {
      if(requestedState == CompressorState_OffAndReadyToChange)
      {
         Given ResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Off, Vote_Care);
         And The CompressorSpeedControllerIsInitialized();
      }

      else if(requestedState == CompressorState_SabbathDelay)
      {
         Given The CompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_OffAndReadyToChange);
         When The ResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);
      }

      else if(requestedState == CompressorState_Startup)
      {
         Given The CompressorSpeedControllerIsInitialized();
         After(SomeSabbathTimeInSeconds * MSEC_PER_SEC);
      }

      else if(requestedState == CompressorState_MinimumOnTime)
      {
         Given The CompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_Startup);
         After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC);
      }

      else if(requestedState == CompressorState_OnAndReadyToChange)
      {
         Given The CompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_MinimumOnTime);
         After(compressorData->compressorTimes.minimumOnTimeInMinutes * MSEC_PER_MIN);
      }

      else if(requestedState == CompressorState_VariableSpeedMinimumRunTime)
      {
         Given The CompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_OnAndReadyToChange);
         When The ResolvedVoteChangesToAnotherNonzeroSpeed();
      }

      else if(requestedState == CompressorState_MinimumOffTime)
      {
         Given The CompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_OnAndReadyToChange);
         When The ResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Off, Vote_Care);
      }

      CompressorHsmStateShouldBe(requestedState);
   }

   void ResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_t speed, bool care)
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

   void CompressorControllerSpeedRequestShouldBe(CompressorSpeed_t expected)
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

   void ResolvedVoteChangesToAnotherNonzeroSpeed()
   {
      if(GetCompressorResolvedVotedSpeed() == CompressorSpeed_Low)
      {
         ResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Medium, Vote_Care);
      }
      else
      {
         ResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);
      }
   }
};

TEST(VariableSpeedCompressorSpeedController, ShouldInitialize)
{
   Given CompressorSpeedControllerIsInitialized();
}

TEST(VariableSpeedCompressorSpeedController, ShouldTransitionToOffStateOnInitWhenVotedCompressorSpeedIsOff)
{
   Given ResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Off, Vote_Care);
   And The CompressorSpeedControllerIsInitialized();

   The CompressorHsmStateShouldBe(CompressorState_OffAndReadyToChange);
   And The CompressorControllerSpeedRequestShouldBe(CompressorSpeed_Off);
}

TEST(VariableSpeedCompressorSpeedController, CompressorControllerSpeedRequestShouldBeOffWhenResolvedRequestBecomesNonZero)
{
   Given The CompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_OffAndReadyToChange);

   The CompressorControllerSpeedRequestShouldBe(CompressorSpeed_Off);

   When ResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_High, Vote_Care);
   The CompressorControllerSpeedRequestShouldBe(CompressorSpeed_Off);
}

TEST(VariableSpeedCompressorSpeedController, ShouldInitializeIntoSabbathDelayWithNonZeroResolvedSpeedVote)
{
   Given ResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);
   And The CompressorSpeedControllerIsInitialized();

   The CompressorHsmStateShouldBe(CompressorState_SabbathDelay);
   And The CompressorControllerSpeedRequestShouldBe(CompressorSpeed_Off);
}

TEST(VariableSpeedCompressorSpeedController, ShouldHaveCompressorSpeedRequestOffWhenInSabbathDelayWithNonZeroResolvedSpeedVote)
{
   Given ResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);
   And The CompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_SabbathDelay);

   When The ResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_High, Vote_Care);
   The CompressorControllerSpeedRequestShouldBe(CompressorSpeed_Off);
}

TEST(VariableSpeedCompressorSpeedController, ShouldNotRequestACompressorSpeedForSabbathTimeWhileInSabbathStateThenShouldTransitionToStartupStateAfterStartupTime)
{
   Given The CompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_SabbathDelay);
   The CompressorControllerSpeedRequestShouldBe(CompressorSpeed_Off);

   After(SomeSabbathTimeInSeconds * MSEC_PER_SEC - 1);
   The CompressorControllerSpeedRequestShouldBe(CompressorSpeed_Off);

   After(1);
   The CompressorHsmStateShouldBe(CompressorState_Startup);
}

TEST(VariableSpeedCompressorSpeedController, ShouldRequestStartupSpeedUponEntryToStartupState)
{
   Given The ResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);
   And The CompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_Startup);
   The CompressorControllerSpeedRequestShouldBe(CompressorSpeed_Startup);
}

TEST(VariableSpeedCompressorSpeedController, ShouldRequestCompressorStartupSpeedForStartupTimerWhileInStartupState)
{
   Given The ResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);
   And The CompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_Startup);
   CompressorControllerSpeedRequestShouldBe(CompressorSpeed_Startup);

   When ResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);
   CompressorControllerSpeedRequestShouldBe(CompressorSpeed_Startup);
}

TEST(VariableSpeedCompressorSpeedController, ShouldTransitionToMinimumOnTimeFromStartupStateAfterStartupTime)
{
   Given The ResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);
   And The CompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_Startup);

   After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC - 1);
   The CompressorHsmStateShouldBe(CompressorState_Startup);

   After(1);
   The CompressorHsmStateShouldBe(CompressorState_MinimumOnTime);
}

TEST(VariableSpeedCompressorSpeedController, ShouldRequestCachedSpeedFromSabbathStateOnceInMinimumOnTimeEvenIfResolvedSpeedChanges)
{
   Given The ResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);
   And CompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_Startup);

   After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC - 1);
   The CompressorHsmStateShouldBe(CompressorState_Startup);

   When The ResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Medium, Vote_Care);

   After(1);
   CompressorControllerSpeedRequestShouldBe(CompressorSpeed_Low);
}

TEST(VariableSpeedCompressorSpeedController, ShouldNotTransitionFromOffAndReadyToChangeIfVoteChangesFromOffAndDontCareToOffAndCare)
{
   Given The ResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Off, Vote_DontCare);
   And CompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_OffAndReadyToChange);

   When The ResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Off, Vote_Care);
   The CompressorHsmStateShouldBe(CompressorState_OffAndReadyToChange);
}

TEST(VariableSpeedCompressorSpeedController, ShouldTransitionFromMinimumOnTimeToOnAndReadyToChangeAfterMinimumOnTimerExpires)
{
   Given The ResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);
   And CompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_MinimumOnTime);

   After(compressorData->compressorTimes.minimumOnTimeInMinutes * MSEC_PER_MIN - 1);
   The CompressorHsmStateShouldBe(CompressorState_MinimumOnTime);

   After(1);
   The CompressorHsmStateShouldBe(CompressorState_OnAndReadyToChange);
}

TEST(VariableSpeedCompressorSpeedController, ShouldNotRequestCompressorSpeedInTransitionFromMinimumOnTimeToOnAndReadyToChange)
{
   Given The ResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);
   And CompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_MinimumOnTime);
   CompressorControllerSpeedRequestShouldBe(CompressorSpeed_Low);

   After(compressorData->compressorTimes.minimumOnTimeInMinutes * MSEC_PER_MIN);
   CompressorControllerSpeedRequestShouldBe(CompressorSpeed_Low);
}

TEST(VariableSpeedCompressorSpeedController, ShouldTransitionFromOnAndReadyToChangeToMinimumOffTimeWhenResolvedSpeedVoteBecomesOff)
{
   Given The ResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);
   And CompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_OnAndReadyToChange);
   CompressorControllerSpeedRequestShouldBe(CompressorSpeed_Low);

   When The ResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Off, Vote_Care);
   The CompressorHsmStateShouldBe(CompressorState_MinimumOffTime);
   And CompressorControllerSpeedRequestShouldBe(CompressorSpeed_Off);
}

TEST(VariableSpeedCompressorSpeedController, ShouldTransitionFromOnAndReadyToChangeToVariableSpeedMinimumRunTimeWhenResolvedSpeedVoteChangesToNonzeroSpeed)
{
   Given The ResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);
   And CompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_OnAndReadyToChange);
   CompressorControllerSpeedRequestShouldBe(CompressorSpeed_Low);

   When The ResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Medium, Vote_Care);
   The CompressorHsmStateShouldBe(CompressorState_VariableSpeedMinimumRunTime);
   CompressorControllerSpeedRequestShouldBe(CompressorSpeed_Medium);
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

   void CompressorSpeedControllerIsInitialized()
   {
      CompressorSpeedController_Init(
         &instance,
         dataModel,
         &config);
   }

   void CompressorHsmStateShouldBe(CompressorState_t expected)
   {
      CompressorState_t actual;
      DataModel_Read(
         dataModel,
         Erd_CompressorState,
         &actual);
      CHECK_EQUAL(expected, actual);
   }

   void CompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_t requestedState)
   {
      if(requestedState == CompressorState_OffAndReadyToChange)
      {
         Given ResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Off, Vote_Care);
         And The CompressorSpeedControllerIsInitialized();
      }

      else if(requestedState == CompressorState_SabbathDelay)
      {
         Given The CompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_OffAndReadyToChange);
         When The ResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);
      }

      else if(requestedState == CompressorState_Startup)
      {
         Given The CompressorSpeedControllerIsInitialized();
         After(SomeSabbathTimeInSeconds * MSEC_PER_SEC);
      }

      else if(requestedState == CompressorState_MinimumOnTime)
      {
         Given The CompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_Startup);
         After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC);
      }

      else if(requestedState == CompressorState_OnAndReadyToChange)
      {
         Given The CompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_MinimumOnTime);
         After(compressorData->compressorTimes.minimumOnTimeInMinutes * MSEC_PER_MIN);
      }

      else if(requestedState == CompressorState_VariableSpeedMinimumRunTime)
      {
         Given The CompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_OnAndReadyToChange);
         When The ResolvedVoteChangesToAnotherNonzeroSpeed();
      }

      else if(requestedState == CompressorState_MinimumOffTime)
      {
         Given The CompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_OnAndReadyToChange);
         When The ResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Off, Vote_Care);
      }

      CompressorHsmStateShouldBe(requestedState);
   }

   void ResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_t speed, bool care)
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

   void CompressorControllerSpeedRequestShouldBe(CompressorSpeed_t expected)
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

   void ResolvedVoteChangesToAnotherNonzeroSpeed()
   {
      if(GetCompressorResolvedVotedSpeed() == CompressorSpeed_Low)
      {
         ResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Medium, Vote_Care);
      }
      else
      {
         ResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);
      }
   }
};

TEST(SingleSpeedCompressorSpeedController, ShouldNotTransitionFromOnAndReadyToChangeToMinimumVariableSpeedRunTimeWhenResolvedSpeedVoteChangesToNonzeroSpeed)
{
   Given The ResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Low, Vote_Care);
   And CompressorSpeedControllerIsInitializedAndGetsIntoState(CompressorState_OnAndReadyToChange);
   CompressorControllerSpeedRequestShouldBe(CompressorSpeed_Low);

   When The ResolvedCompressorSpeedVoteAndCareIs(CompressorSpeed_Medium, Vote_Care);
   The CompressorHsmStateShouldBe(CompressorState_OnAndReadyToChange);
}
