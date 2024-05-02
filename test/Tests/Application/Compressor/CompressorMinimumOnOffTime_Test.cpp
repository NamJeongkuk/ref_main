/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "CompressorMinimumOnOffTime.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"
}

#include "ReferDataModel_TestDouble.h"
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "uassert_test.h"

static const CompressorTimes_t timesData = {
   .minimumOffTimeInMinutes = 1,
   .minimumOnTimeInMinutes = 2
};

static const CompressorMinimumOnOffTimeConfiguration_t config = {
   .resolvedVoteErd = Erd_CompressorSpeed_ResolvedVote,
   .minimumOnOffTimeVoteErd = Erd_CompressorSpeed_MinimumOnOffTimeVote
};

TEST_GROUP(CompressorMinimumOnOffTime)
{
   CompressorMinimumOnOffTime_t instance;
   ReferDataModel_TestDouble_t referTestDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&referTestDouble);
      dataModel = referTestDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&referTestDouble);
   }

   void GivenTheModuleIsInitialized()
   {
      CompressorMinimumOnOffTime_Init(
         &instance,
         dataModel,
         &timesData,
         &config);
   }

   void GivenResolvedVoteIs(CompressorSpeed_t speed)
   {
      CompressorVotedSpeed_t votedSpeed;
      votedSpeed.speed = speed;
      votedSpeed.care = Vote_Care;

      DataModel_Write(
         dataModel,
         config.resolvedVoteErd,
         &votedSpeed);
   }

   void WhenTheResolvedVoteBecomes(CompressorSpeed_t speed)
   {
      GivenResolvedVoteIs(speed);
   }

   void WhenTheResolvedVoteBecomes(CompressorSpeed_t speed, Vote_t care)
   {
      GivenResolvedVoteIs(speed, care);
   }

   void GivenResolvedVoteIs(CompressorSpeed_t speed, Vote_t care)
   {
      CompressorVotedSpeed_t votedSpeed;
      votedSpeed.speed = speed;
      votedSpeed.care = care;

      DataModel_Write(
         dataModel,
         config.resolvedVoteErd,
         &votedSpeed);
   }

   void GivenMinimumOnOffTimeIs(CompressorSpeed_t speed, Vote_t care)
   {
      CompressorVotedSpeed_t votedSpeed;
      votedSpeed.speed = speed;
      votedSpeed.care = care;

      DataModel_Write(
         dataModel,
         config.minimumOnOffTimeVoteErd,
         &votedSpeed);
   }

   void TheMinimumOnOffVoteShouldBe(CompressorSpeed_t expectedSpeed, Vote_t expectedCare)
   {
      CompressorVotedSpeed_t actual;
      DataModel_Read(
         dataModel,
         config.minimumOnOffTimeVoteErd,
         &actual);

      CHECK_EQUAL(expectedSpeed, actual.speed);
      CHECK_EQUAL(expectedCare, actual.care);
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks);
   }
};

TEST(CompressorMinimumOnOffTime, ShouldVoteOffWhenResolvedVoteChangesToOff)
{
   GivenResolvedVoteIs(CompressorSpeed_High);
   GivenMinimumOnOffTimeIs(CompressorSpeed_High, Vote_DontCare);
   GivenTheModuleIsInitialized();

   WhenTheResolvedVoteBecomes(CompressorSpeed_Off);
   TheMinimumOnOffVoteShouldBe(CompressorSpeed_Off, Vote_Care);
}

TEST(CompressorMinimumOnOffTime, ShouldVoteOnWhenResolvedVoteChangesToOnAndCompressorWasPreviouslyOff)
{
   GivenResolvedVoteIs(CompressorSpeed_Off);
   GivenMinimumOnOffTimeIs(CompressorSpeed_Off, Vote_DontCare);
   GivenTheModuleIsInitialized();

   WhenTheResolvedVoteBecomes(CompressorSpeed_Low);
   TheMinimumOnOffVoteShouldBe(CompressorSpeed_Low, Vote_Care);
}

TEST(CompressorMinimumOnOffTime, ShouldNotVoteOnWhenResolvedVoteChangesToOnAndCompressorWasPreviouslyOn)
{
   GivenResolvedVoteIs(CompressorSpeed_Low);
   GivenMinimumOnOffTimeIs(CompressorSpeed_Off, Vote_DontCare);
   GivenTheModuleIsInitialized();

   WhenTheResolvedVoteBecomes(CompressorSpeed_High);
   TheMinimumOnOffVoteShouldBe(CompressorSpeed_Off, Vote_DontCare);
}

TEST(CompressorMinimumOnOffTime, ShouldVoteOffWhenResolvedVoteChangesToOffForTimeout)
{
   GivenResolvedVoteIs(CompressorSpeed_High);
   GivenTheModuleIsInitialized();

   WhenTheResolvedVoteBecomes(CompressorSpeed_Off);
   TheMinimumOnOffVoteShouldBe(CompressorSpeed_Off, Vote_Care);

   After(timesData.minimumOffTimeInMinutes * MSEC_PER_MIN - 1);
   TheMinimumOnOffVoteShouldBe(CompressorSpeed_Off, Vote_Care);

   After(1);
   TheMinimumOnOffVoteShouldBe(CompressorSpeed_Off, Vote_DontCare);
}

TEST(CompressorMinimumOnOffTime, ShouldVoteOnWhenResolvedVoteChangesToOnForTimeout)
{
   GivenResolvedVoteIs(CompressorSpeed_Off);
   GivenTheModuleIsInitialized();

   WhenTheResolvedVoteBecomes(CompressorSpeed_High);
   TheMinimumOnOffVoteShouldBe(CompressorSpeed_High, Vote_Care);

   After(timesData.minimumOnTimeInMinutes * MSEC_PER_MIN - 1);
   TheMinimumOnOffVoteShouldBe(CompressorSpeed_High, Vote_Care);

   After(1);
   TheMinimumOnOffVoteShouldBe(CompressorSpeed_Off, Vote_DontCare);
}

TEST(CompressorMinimumOnOffTime, ShouldChangeVotesIfResolvedVoteChangesBeforeTimeout)
{
   GivenResolvedVoteIs(CompressorSpeed_Off);
   GivenMinimumOnOffTimeIs(CompressorSpeed_Off, Vote_DontCare);
   GivenTheModuleIsInitialized();

   WhenTheResolvedVoteBecomes(CompressorSpeed_High);
   TheMinimumOnOffVoteShouldBe(CompressorSpeed_High, Vote_Care);

   WhenTheResolvedVoteBecomes(CompressorSpeed_Off);
   TheMinimumOnOffVoteShouldBe(CompressorSpeed_Off, Vote_Care);

   After(timesData.minimumOffTimeInMinutes * MSEC_PER_MIN);
   TheMinimumOnOffVoteShouldBe(CompressorSpeed_Off, Vote_DontCare);
}

TEST(CompressorMinimumOnOffTime, ShouldNotVoteForMinimumOffTimeIfNotPreviouslyOn)
{
   GivenResolvedVoteIs(CompressorSpeed_Off, Vote_Care);
   GivenMinimumOnOffTimeIs(CompressorSpeed_Off, Vote_DontCare);
   GivenTheModuleIsInitialized();

   WhenTheResolvedVoteBecomes(CompressorSpeed_Off, Vote_DontCare);
   TheMinimumOnOffVoteShouldBe(CompressorSpeed_Off, Vote_DontCare);
}
