/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "SabbathDelayHandler.h"
#include "Constants_Time.h"
#include "VoteType.h"
#include "utils.h"
#include "DataModelErdPointerAccess.h"
#include "CompressorVotedSpeed.h"
#include "DamperVotedPosition.h"
#include "CompressorTimes.h"
#include "SystemErds.h"
}

#include "DataModel_TestDouble.h"
#include "TimerModule_TestDouble.h"
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "uassert_test.h"

enum
{
   SabbathDelayInSeconds = 20
};

const CompressorData_t compressorParametric = {
   .compressorTimes = { .sabbathDelayTimeInSeconds = SabbathDelayInSeconds }
};

enum
{
   Erd_U8Vote,
   Erd_U16Vote,
   Erd_U32Vote,
   Erd_U8VoteWithSabbathDelay,
   Erd_U16VoteWithSabbathDelay,
   Erd_U32VoteWithSabbathDelay,
   Erd_CompressorResolvedVote,
   Erd_CompressorResolvedVoteWithSabbathDelay,
};

static const DataModel_TestDoubleConfigurationEntry_t erdEntries[] = {
   { Erd_U8Vote, sizeof(U8Vote_t) },
   { Erd_U16Vote, sizeof(U16Vote_t) },
   { Erd_U32Vote, sizeof(U32Vote_t) },
   { Erd_U8VoteWithSabbathDelay, sizeof(U8Vote_t) },
   { Erd_U16VoteWithSabbathDelay, sizeof(U16Vote_t) },
   { Erd_U32VoteWithSabbathDelay, sizeof(U32Vote_t) },
   { Erd_TimerModule, sizeof(TimerModule_t *) },
   { Erd_CompressorResolvedVote, sizeof(CompressorVotedSpeed_t) },
   { Erd_CompressorResolvedVoteWithSabbathDelay, sizeof(CompressorVotedSpeed_t) },
   { Erd_SabbathModeEnable, sizeof(bool) },
   { Erd_EnhancedSabbathModeEnable, sizeof(bool) },
   { Erd_PersonalityParametricData, sizeof(PersonalityParametricData_t *) }
};

static const SabbathDelayHandlerConfiguration_t u8Config = {
   .compressorResolvedVote = Erd_CompressorResolvedVote,
   .loadResolvedVote = Erd_U8Vote,
   .loadResolvedVoteWithSabbathDelay = Erd_U8VoteWithSabbathDelay
};

static const SabbathDelayHandlerConfiguration_t u16Config = {
   .compressorResolvedVote = Erd_CompressorResolvedVote,
   .loadResolvedVote = Erd_U16Vote,
   .loadResolvedVoteWithSabbathDelay = Erd_U16VoteWithSabbathDelay
};

static const SabbathDelayHandlerConfiguration_t u32Config = {
   .compressorResolvedVote = Erd_CompressorResolvedVote,
   .loadResolvedVote = Erd_U32Vote,
   .loadResolvedVoteWithSabbathDelay = Erd_U32VoteWithSabbathDelay
};

static const SabbathDelayHandlerConfiguration_t compressorConfig = {
   .compressorResolvedVote = Erd_CompressorResolvedVote,
   .loadResolvedVote = Erd_CompressorResolvedVote,
   .loadResolvedVoteWithSabbathDelay = Erd_CompressorResolvedVoteWithSabbathDelay
};

TEST_GROUP(SabbathDelayHandler)
{
   SabbathDelayHandler_t instance;

   DataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t timerModuleTestDouble;

   PersonalityParametricData_t parametric = {
      .compressorData = &compressorParametric
   };

   void setup()
   {
      DataModel_TestDouble_Init(&dataModelTestDouble, erdEntries, NUM_ELEMENTS(erdEntries));
      dataModel = dataModelTestDouble.dataModel;

      TimerModule_TestDouble_Init(&timerModuleTestDouble);
      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &timerModuleTestDouble.timerModule);

      DataModelErdPointerAccess_Write(dataModel, Erd_PersonalityParametricData, &parametric);
   }

   void GivenModuleIsInitializedWithU8Vote()
   {
      SabbathDelayHandler_Init(&instance, dataModel, &u8Config);
   }

   void GivenModuleIsInitializedWithU16Vote()
   {
      SabbathDelayHandler_Init(&instance, dataModel, &u16Config);
   }

   void GivenModuleIsInitializedWithU32Vote()
   {
      SabbathDelayHandler_Init(&instance, dataModel, &u32Config);
   }

   void GivenModuleIsInitializedWithCompressorConfig()
   {
      SabbathDelayHandler_Init(&instance, dataModel, &compressorConfig);
   }

   void GivenU8VoteIs(U8Vote_t vote)
   {
      DataModel_Write(dataModel, Erd_U8Vote, &vote);
   }

   void WhenU8VoteBecomes(U8Vote_t vote)
   {
      GivenU8VoteIs(vote);
   }

   void GivenU16VoteIs(U16Vote_t vote)
   {
      DataModel_Write(dataModel, Erd_U16Vote, &vote);
   }

   void WhenU16VoteBecomes(U16Vote_t vote)
   {
      GivenU16VoteIs(vote);
   }

   void GivenU32VoteIs(U32Vote_t vote)
   {
      DataModel_Write(dataModel, Erd_U32Vote, &vote);
   }

   void WhenU32VoteBecomes(U32Vote_t vote)
   {
      GivenU32VoteIs(vote);
   }

   void GivenSabbathModeIs(bool enable)
   {
      DataModel_Write(dataModel, Erd_SabbathModeEnable, &enable);
   }

   void WhenSabbathModeIs(bool enable)
   {
      GivenSabbathModeIs(enable);
   }

   void GivenEnhancedSabbathModeIs(bool enable)
   {
      DataModel_Write(dataModel, Erd_EnhancedSabbathModeEnable, &enable);
   }

   void WhenEnhancedSabbathModeIs(bool enable)
   {
      GivenEnhancedSabbathModeIs(enable);
   }

   void GivenCompressorIs(CompressorSpeed_t speed)
   {
      CompressorVotedSpeed_t vote = { .speed = speed, .care = Vote_Care };
      DataModel_Write(dataModel, Erd_CompressorResolvedVote, &vote);
   }

   void WhenCompressorBecomes(CompressorSpeed_t speed)
   {
      GivenCompressorIs(speed);
   }

   void GivenCompressorIs(CompressorSpeed_t speed, Vote_t care)
   {
      CompressorVotedSpeed_t vote = { .speed = speed, .care = care };
      DataModel_Write(dataModel, Erd_CompressorResolvedVote, &vote);
   }

   void WhenCompressorBecomes(CompressorSpeed_t speed, Vote_t care)
   {
      GivenCompressorIs(speed, care);
   }

   void TheU8VoteWithSabbathDelayShouldBe(U8Vote_t expected)
   {
      U8Vote_t actual;
      DataModel_Read(dataModel, Erd_U8VoteWithSabbathDelay, &actual);
      CHECK_EQUAL(expected.value, actual.value);
      CHECK_EQUAL(expected.vote, actual.vote);
   }

   void TheU16VoteWithSabbathDelayShouldBe(U16Vote_t expected)
   {
      U16Vote_t actual;
      DataModel_Read(dataModel, Erd_U16VoteWithSabbathDelay, &actual);
      CHECK_EQUAL(expected.value, actual.value);
      CHECK_EQUAL(expected.vote, actual.vote);
   }

   void TheU32VoteWithSabbathDelayShouldBe(U32Vote_t expected)
   {
      U32Vote_t actual;
      DataModel_Read(dataModel, Erd_U32VoteWithSabbathDelay, &actual);
      CHECK_EQUAL(expected.value, actual.value);
      CHECK_EQUAL(expected.vote, actual.vote);
   }

   void TheCompressorVoteWithSabbathDelayShouldBe(CompressorVotedSpeed_t expected)
   {
      CompressorVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_CompressorResolvedVoteWithSabbathDelay, &actual);
      CHECK_EQUAL(expected.speed, actual.speed);
      CHECK_EQUAL(expected.care, actual.care);
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(&timerModuleTestDouble, ticks);
   }
};

TEST(SabbathDelayHandler, ShouldPassThroughU8VoteIfNotInSabbath)
{
   GivenSabbathModeIs(false);
   GivenEnhancedSabbathModeIs(false);
   GivenU8VoteIs({ 1, Vote_DontCare });
   GivenModuleIsInitializedWithU8Vote();

   WhenU8VoteBecomes({ 2, Vote_Care });
   TheU8VoteWithSabbathDelayShouldBe({ 2, Vote_Care });
}

TEST(SabbathDelayHandler, ShouldPassThroughU16VoteIfNotInSabbath)
{
   GivenSabbathModeIs(false);
   GivenEnhancedSabbathModeIs(false);
   GivenU16VoteIs({ 1, Vote_DontCare });
   GivenModuleIsInitializedWithU16Vote();

   WhenU16VoteBecomes({ 2, Vote_Care });
   TheU16VoteWithSabbathDelayShouldBe({ 2, Vote_Care });
}

TEST(SabbathDelayHandler, ShouldPassThroughU32VoteIfNotInSabbath)
{
   GivenSabbathModeIs(false);
   GivenEnhancedSabbathModeIs(false);
   GivenU32VoteIs({ 1, Vote_DontCare });
   GivenModuleIsInitializedWithU32Vote();

   WhenU32VoteBecomes({ 2, Vote_Care });
   TheU32VoteWithSabbathDelayShouldBe({ 2, Vote_Care });
}

TEST(SabbathDelayHandler, ShouldDelayVoteIfSabbathIsActiveAndCompressorSwitchedFromOffToOn)
{
   GivenSabbathModeIs(true);
   GivenCompressorIs(CompressorSpeed_Off);
   GivenU32VoteIs({ 1, Vote_DontCare });
   GivenModuleIsInitializedWithU32Vote();

   WhenCompressorBecomes(CompressorSpeed_High);
   WhenU32VoteBecomes({ 2, Vote_Care });
   TheU32VoteWithSabbathDelayShouldBe({ 1, Vote_DontCare });

   After(SabbathDelayInSeconds * MSEC_PER_SEC - 1);
   TheU32VoteWithSabbathDelayShouldBe({ 1, Vote_DontCare });

   After(1);
   TheU32VoteWithSabbathDelayShouldBe({ 2, Vote_Care });
}

TEST(SabbathDelayHandler, ShouldNotDelayVoteWhenCompressorVoteBecomesOnUnlessInSabbath)
{
   GivenSabbathModeIs(false);
   GivenEnhancedSabbathModeIs(false);
   WhenCompressorBecomes(CompressorSpeed_Off);
   GivenU32VoteIs({ 1, Vote_DontCare });
   GivenModuleIsInitializedWithU32Vote();

   WhenCompressorBecomes(CompressorSpeed_High);
   WhenU32VoteBecomes({ 2, Vote_Care });
   TheU32VoteWithSabbathDelayShouldBe({ 2, Vote_Care });
}

TEST(SabbathDelayHandler, ShouldNotDelayVoteWhenCompressorVoteChangeIsStillOff)
{
   GivenSabbathModeIs(true);
   GivenEnhancedSabbathModeIs(true);
   WhenCompressorBecomes(CompressorSpeed_Off, Vote_DontCare);
   GivenU32VoteIs({ 1, Vote_DontCare });
   GivenModuleIsInitializedWithU32Vote();

   WhenCompressorBecomes(CompressorSpeed_Off, Vote_Care);
   WhenU32VoteBecomes({ 2, Vote_Care });
   TheU32VoteWithSabbathDelayShouldBe({ 2, Vote_Care });
}

TEST(SabbathDelayHandler, ShouldDelayCompressorIfResolvedLoadVoteIsCompressorResolvedVote)
{
   GivenEnhancedSabbathModeIs(true);
   GivenCompressorIs(CompressorSpeed_Off);
   GivenModuleIsInitializedWithCompressorConfig();

   WhenCompressorBecomes(CompressorSpeed_High);
   TheCompressorVoteWithSabbathDelayShouldBe({ CompressorSpeed_Off, Vote_Care });

   After(SabbathDelayInSeconds * MSEC_PER_SEC);
   TheCompressorVoteWithSabbathDelayShouldBe({ CompressorSpeed_High, Vote_Care });
}

TEST(SabbathDelayHandler, ShouldNotDelayVoteIfCompressorIsJustChangingSpeeds)
{
   GivenSabbathModeIs(true);
   GivenCompressorIs(CompressorSpeed_Off);
   GivenU32VoteIs({ 1, Vote_DontCare });
   GivenModuleIsInitializedWithU32Vote();

   WhenCompressorBecomes(CompressorSpeed_High);
   WhenU32VoteBecomes({ 2, Vote_Care });
   TheU32VoteWithSabbathDelayShouldBe({ 1, Vote_DontCare });

   After(SabbathDelayInSeconds * MSEC_PER_SEC);
   TheU32VoteWithSabbathDelayShouldBe({ 2, Vote_Care });

   WhenCompressorBecomes(CompressorSpeed_Medium);
   WhenU32VoteBecomes({ 3, Vote_Care });
   TheU32VoteWithSabbathDelayShouldBe({ 3, Vote_Care });
}
