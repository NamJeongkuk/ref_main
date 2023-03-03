/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "FactoryMode.h"
#include "DataModelErdPointerAccess.h"
#include "VoteType.h"
}

#include "CppUTest/TestHarness.h"
#include "DataModel_TestDouble.h"
#include "uassert_test.h"

#define Given
#define When
#define Then
#define And

enum
{
   Inactive = false,
   Active = true
};

enum
{
   ResetDelayTimeInSeconds = 1
};

enum
{
   Erd_FactoryModeEnableRequest,
   Erd_DisableMinimumCompressorTimes,
   Erd_Reset,
   Erd_U8_FactoryVoteStruct,
   Erd_U16_FactoryVoteStruct,
   Erd_U32_FactoryVoteStruct,
   Erd_U64_FactoryVoteStruct,
   Erd_U8_AnotherFactoryVoteStruct,
   Erd_U16_AnotherFactoryVoteStruct,
   Erd_U32_AnotherFactoryVoteStruct,
   Erd_U64_AnotherFactoryVoteStruct
};

static uint8_t U8Value = UINT8_MAX;
static uint16_t U16Value = UINT16_MAX;
static uint32_t U32Value = UINT32_MAX;

static uint8_t U8OffValue = 0;
static uint16_t U16OffValue = 0;
static uint32_t U32OffValue = 0;

static uint8_t U8AnotherOffValue = 1;
static uint16_t U16AnotherOffValue = UINT8_MAX + 1;
static uint32_t U32AnotherOffValue = UINT16_MAX + 1;

typedef struct
{
   uint64_t value;
   Vote_t vote;
} U64Vote_t;

static const DataModel_TestDoubleConfigurationEntry_t erdTable[] = {
   { Erd_FactoryModeEnableRequest, sizeof(bool) },
   { Erd_DisableMinimumCompressorTimes, sizeof(bool) },
   { Erd_Reset, sizeof(uint8_t) },
   { Erd_U8_FactoryVoteStruct, sizeof(U8Vote_t) },
   { Erd_U16_FactoryVoteStruct, sizeof(U16Vote_t) },
   { Erd_U32_FactoryVoteStruct, sizeof(U32Vote_t) },
   { Erd_U64_FactoryVoteStruct, sizeof(U64Vote_t) },
   { Erd_U8_AnotherFactoryVoteStruct, sizeof(U8Vote_t) },
   { Erd_U16_AnotherFactoryVoteStruct, sizeof(U16Vote_t) },
   { Erd_U32_AnotherFactoryVoteStruct, sizeof(U32Vote_t) },
   { Erd_U64_AnotherFactoryVoteStruct, sizeof(U64Vote_t) }
};

static const FactoryVotePair_t factoryVotePairs[] = {
   { Erd_U8_FactoryVoteStruct, U8OffValue },
   { Erd_U16_FactoryVoteStruct, U16OffValue },
   { Erd_U32_FactoryVoteStruct, U32OffValue },
   { Erd_U8_AnotherFactoryVoteStruct, U8AnotherOffValue },
   { Erd_U16_AnotherFactoryVoteStruct, U16AnotherOffValue },
   { Erd_U32_AnotherFactoryVoteStruct, U32AnotherOffValue },
};

static const FactoryVotePair_t factoryVotePairsWithU64Vote[] = {
   { Erd_U64_FactoryVoteStruct, 0 },
   { Erd_U64_AnotherFactoryVoteStruct, 0 }
};

static const FactoryVoteList_t factoryVoteList = {
   .pairs = factoryVotePairs,
   .numberOfPairs = NUM_ELEMENTS(factoryVotePairs)
};

static const FactoryVoteList_t factoryVoteListWithU64Vote = {
   .pairs = factoryVotePairsWithU64Vote,
   .numberOfPairs = NUM_ELEMENTS(factoryVotePairsWithU64Vote)
};

static const FactoryModeConfiguration_t config = {
   .factoryModeActiveErd = Erd_FactoryModeEnableRequest,
   .disableMinimumCompressorTimesErd = Erd_DisableMinimumCompressorTimes,
   .resetErd = Erd_Reset,
   .factoryVoteList = factoryVoteList
};

static const FactoryModeConfiguration_t configU64Vote = {
   .factoryModeActiveErd = Erd_FactoryModeEnableRequest,
   .disableMinimumCompressorTimesErd = Erd_DisableMinimumCompressorTimes,
   .resetErd = Erd_Reset,
   .factoryVoteList = factoryVoteListWithU64Vote
};

TEST_GROUP(FactoryMode)
{
   DataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   FactoryMode_t instance;

   void setup()
   {
      DataModel_TestDouble_Init(&dataModelDouble, erdTable, NUM_ELEMENTS(erdTable));
      dataModel = dataModelDouble.dataModel;
   }

   void ModuleIsInitialized()
   {
      FactoryMode_Init(&instance, dataModel, &config);
   }

   void ModuleIsInitializedWithU64Vote()
   {
      FactoryMode_Init(&instance, dataModel, &configU64Vote);
   }

   void FactoryModeIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FactoryModeEnableRequest, &state);
   }

   void U8VoteStructErdHasValue(Erd_t erd, uint8_t value, Vote_t vote)
   {
      U8Vote_t offVote = { value, vote };
      DataModel_Write(dataModel, erd, &offVote);
   }

   void U16VoteStructErdHasValue(Erd_t erd, uint16_t value, Vote_t vote)
   {
      U16Vote_t offVote = { value, vote };
      DataModel_Write(dataModel, erd, &offVote);
   }

   void U32VoteStructErdHasValue(Erd_t erd, uint32_t value, Vote_t vote)
   {
      U32Vote_t offVote = { value, vote };
      DataModel_Write(dataModel, erd, &offVote);
   }

   void U64VoteStructErdHasValue(Erd_t erd, uint64_t value, Vote_t vote)
   {
      U64Vote_t offVote = { value, vote };
      DataModel_Write(dataModel, erd, &offVote);
   }

   void AllFactoryVoteErdsAreUninitialized(void)
   {
      Given U8VoteStructErdHasValue(Erd_U8_FactoryVoteStruct, U8Value, Vote_DontCare);
      Given U16VoteStructErdHasValue(Erd_U16_FactoryVoteStruct, U16Value, Vote_DontCare);
      Given U32VoteStructErdHasValue(Erd_U32_FactoryVoteStruct, U32Value, Vote_DontCare);
      Given U8VoteStructErdHasValue(Erd_U8_AnotherFactoryVoteStruct, U8Value, Vote_DontCare);
      Given U16VoteStructErdHasValue(Erd_U16_AnotherFactoryVoteStruct, U16Value, Vote_DontCare);
      Given U32VoteStructErdHasValue(Erd_U32_AnotherFactoryVoteStruct, U32Value, Vote_DontCare);
   }

   void TheU8VoteStructErdShouldBe(Erd_t erd, uint8_t expectedValue, Vote_t expectedVote)
   {
      U8Vote_t actual;
      DataModel_Read(dataModel, erd, &actual);

      CHECK_EQUAL(expectedValue, actual.value);
      CHECK_EQUAL(expectedVote, actual.vote);
   }

   void TheU16VoteStructErdShouldBe(Erd_t erd, uint16_t expectedValue, Vote_t expectedVote)
   {
      U16Vote_t actual;
      DataModel_Read(dataModel, erd, &actual);

      CHECK_EQUAL(expectedValue, actual.value);
      CHECK_EQUAL(expectedVote, actual.vote);
   }

   void TheU32VoteStructErdShouldBe(Erd_t erd, uint32_t expectedValue, Vote_t expectedVote)
   {
      U32Vote_t actual;
      DataModel_Read(dataModel, erd, &actual);

      CHECK_EQUAL(expectedValue, actual.value);
      CHECK_EQUAL(expectedVote, actual.vote);
   }

   void TheU64VoteStructErdShouldBe(Erd_t erd, uint64_t expectedValue, Vote_t expectedVote)
   {
      U64Vote_t actual;
      DataModel_Read(dataModel, erd, &actual);

      CHECK_EQUAL(expectedValue, actual.value);
      CHECK_EQUAL(expectedVote, actual.vote);
   }

   void AllFactoryVotesShouldBeOff(void)
   {
      TheU8VoteStructErdShouldBe(Erd_U8_FactoryVoteStruct, U8OffValue, Vote_Care);
      TheU16VoteStructErdShouldBe(Erd_U16_FactoryVoteStruct, U16OffValue, Vote_Care);
      TheU32VoteStructErdShouldBe(Erd_U32_FactoryVoteStruct, U32OffValue, Vote_Care);
      TheU8VoteStructErdShouldBe(Erd_U8_AnotherFactoryVoteStruct, U8AnotherOffValue, Vote_Care);
      TheU16VoteStructErdShouldBe(Erd_U16_AnotherFactoryVoteStruct, U16AnotherOffValue, Vote_Care);
      TheU32VoteStructErdShouldBe(Erd_U32_AnotherFactoryVoteStruct, U32AnotherOffValue, Vote_Care);
   }

   void ResetRequestShouldBe(uint8_t expected)
   {
      uint8_t actual;
      DataModel_Read(dataModel, Erd_Reset, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void MinimumCompressorTimesShouldBeDisabled()
   {
      bool actual;
      DataModel_Read(dataModel, Erd_DisableMinimumCompressorTimes, &actual);
      CHECK_EQUAL(true, actual);
   }

   void MinimumCompressorTimesShouldBeEnabled()
   {
      bool actual;
      DataModel_Read(dataModel, Erd_DisableMinimumCompressorTimes, &actual);
      CHECK_EQUAL(false, actual);
   }
};

TEST(FactoryMode, ShouldInitialize)
{
   ModuleIsInitialized();
}

TEST(FactoryMode, ShouldVoteOffForAllTheLoadsWhenFactoryModeIsChangedFromInactiveToActive)
{
   Given AllFactoryVoteErdsAreUninitialized();
   Given FactoryModeIs(Inactive);
   Given ModuleIsInitialized();

   When FactoryModeIs(Active);
   AllFactoryVotesShouldBeOff();
}

TEST(FactoryMode, ShouldAssertWhenOverSizeVoteErdIsIncludedInConfig)
{
   Given FactoryModeIs(Inactive);
   Given ModuleIsInitializedWithU64Vote();

   ShouldFailAssertionWhen(FactoryModeIs(Active));
}

TEST(FactoryMode, ShouldRequestResetWhenFactoryModeIsInactive)
{
   Given ModuleIsInitialized();
   Given FactoryModeIs(Active);

   When FactoryModeIs(Inactive);
   ResetRequestShouldBe(ResetDelayTimeInSeconds);
}

TEST(FactoryMode, ShouldDisableMinimumCompressorTimesWhenFactoryModeIsActive)
{
   Given ModuleIsInitialized();
   MinimumCompressorTimesShouldBeEnabled();

   When FactoryModeIs(Active);
   MinimumCompressorTimesShouldBeDisabled();
}
