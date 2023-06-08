/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "CoolingSystemRequestHandler.h"
#include "DataModelErdPointerAccess.h"
#include "VoteType.h"
}

#include "CppUTest/TestHarness.h"
#include "DataModel_TestDouble.h"
#include "TimerModule_TestDouble.h"
#include "Constants_Time.h"
#include "uassert_test.h"

enum
{
   Erd_CoolingSystemOffRequest,
   Erd_CoolingSystemOffStatus,
   Erd_U8_CoolingSystemRequestVoteStruct,
   Erd_U16_CoolingSystemRequestVoteStruct,
   Erd_U32_CoolingSystemRequestVoteStruct,
   Erd_U64_CoolingSystemRequestVoteStruct,
   Erd_U8_AnotherCoolingSystemRequestVoteStruct,
   Erd_U16_AnotherCoolingSystemRequestVoteStruct,
   Erd_U32_AnotherCoolingSystemRequestVoteStruct,
   Erd_U64_AnotherCoolingSystemRequestVoteStruct,
   Erd_DisableDefrost,

   ClearValue = UINT8_MAX
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

static uint8_t U8ChangeValue = 127;
static uint16_t U16ChangeValue = UINT8_MAX + 10;
static uint32_t U32ChangeValue = UINT16_MAX + 10;

typedef struct
{
   uint64_t value;
   Vote_t vote;
} U64Vote_t;

static const DataModel_TestDoubleConfigurationEntry_t erdTable[] = {
   { Erd_CoolingSystemOffRequest, sizeof(uint8_t) },
   { Erd_CoolingSystemOffStatus, sizeof(bool) },
   { Erd_U8_CoolingSystemRequestVoteStruct, sizeof(U8Vote_t) },
   { Erd_U16_CoolingSystemRequestVoteStruct, sizeof(U16Vote_t) },
   { Erd_U32_CoolingSystemRequestVoteStruct, sizeof(U32Vote_t) },
   { Erd_U64_CoolingSystemRequestVoteStruct, sizeof(U64Vote_t) },
   { Erd_U8_AnotherCoolingSystemRequestVoteStruct, sizeof(U8Vote_t) },
   { Erd_U16_AnotherCoolingSystemRequestVoteStruct, sizeof(U16Vote_t) },
   { Erd_U32_AnotherCoolingSystemRequestVoteStruct, sizeof(U32Vote_t) },
   { Erd_U64_AnotherCoolingSystemRequestVoteStruct, sizeof(U64Vote_t) },
   { Erd_DisableDefrost, sizeof(bool) }
};

static const CoolingSystemRequestVotePair_t coolingSystemVotePairs[] = {
   { Erd_U8_CoolingSystemRequestVoteStruct, U8OffValue },
   { Erd_U16_CoolingSystemRequestVoteStruct, U16OffValue },
   { Erd_U32_CoolingSystemRequestVoteStruct, U32OffValue },
   { Erd_U8_AnotherCoolingSystemRequestVoteStruct, U8AnotherOffValue },
   { Erd_U16_AnotherCoolingSystemRequestVoteStruct, U16AnotherOffValue },
   { Erd_U32_AnotherCoolingSystemRequestVoteStruct, U32AnotherOffValue },
};

static const CoolingSystemRequestVotePair_t coolingSystemVotePairsWithU64Vote[] = {
   { Erd_U64_CoolingSystemRequestVoteStruct, 0 },
   { Erd_U64_AnotherCoolingSystemRequestVoteStruct, 0 }
};

static const CoolingSystemRequestVoteList_t coolingSystemVoteList = {
   .pairs = coolingSystemVotePairs,
   .numberOfPairs = NUM_ELEMENTS(coolingSystemVotePairs)
};

static const CoolingSystemRequestVoteList_t coolingSystemVoteListWithU64Vote = {
   .pairs = coolingSystemVotePairsWithU64Vote,
   .numberOfPairs = NUM_ELEMENTS(coolingSystemVotePairsWithU64Vote)
};

static const CoolingSystemRequestHandlerConfiguration_t config = {
   .requestErd = Erd_CoolingSystemOffRequest,
   .statusErd = Erd_CoolingSystemOffStatus,
   .disableDefrostErd = Erd_DisableDefrost,
   .coolingSystemRequestVoteList = coolingSystemVoteList
};

static const CoolingSystemRequestHandlerConfiguration_t configU64Vote = {
   .requestErd = Erd_CoolingSystemOffRequest,
   .statusErd = Erd_CoolingSystemOffStatus,
   .disableDefrostErd = Erd_DisableDefrost,
   .coolingSystemRequestVoteList = coolingSystemVoteListWithU64Vote
};

TEST_GROUP(CoolingSystemRequestHandler)
{
   DataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   CoolingSystemRequestHandler_t instance;

   void setup()
   {
      DataModel_TestDouble_Init(&dataModelDouble, erdTable, NUM_ELEMENTS(erdTable));
      dataModel = dataModelDouble.dataModel;
   }

   void GivenTheModuleIsInitialized()
   {
      CoolingSystemRequestHandler_Init(
         &instance,
         dataModel,
         &config);
   }

   void GivenTheModuleIsInitializedWithU64Vote()
   {
      CoolingSystemRequestHandler_Init(
         &instance,
         dataModel,
         &configU64Vote);
   }

   void WhenTheCoolingSystemOffRequestChangesTo(uint8_t state)
   {
      DataModel_Write(dataModel, Erd_CoolingSystemOffRequest, &state);
   }

   void WhenTheCoolingSystemOffRequestShouldBe(uint8_t expected)
   {
      uint8_t actual;
      DataModel_Read(dataModel, Erd_CoolingSystemOffRequest, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void CoolingSystemRequestHandlerEnableRequestInMinutesShouldBe(uint8_t expected)
   {
      uint8_t actual;
      DataModel_Read(dataModel, Erd_CoolingSystemOffRequest, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void GivenTheU8VoteStructErdHasValue(Erd_t erd, uint8_t value, Vote_t vote)
   {
      U8Vote_t offVote = { value, vote };
      DataModel_Write(dataModel, erd, &offVote);
   }

   void GivenTheU16VoteStructErdHasValue(Erd_t erd, uint16_t value, Vote_t vote)
   {
      U16Vote_t offVote = { value, vote };
      DataModel_Write(dataModel, erd, &offVote);
   }

   void GivenTheU32VoteStructErdHasValue(Erd_t erd, uint32_t value, Vote_t vote)
   {
      U32Vote_t offVote = { value, vote };
      DataModel_Write(dataModel, erd, &offVote);
   }

   void AllFactoryVoteErdsAreUninitialized(void)
   {
      GivenTheU8VoteStructErdHasValue(Erd_U8_CoolingSystemRequestVoteStruct, U8Value, Vote_DontCare);
      GivenTheU16VoteStructErdHasValue(Erd_U16_CoolingSystemRequestVoteStruct, U16Value, Vote_DontCare);
      GivenTheU32VoteStructErdHasValue(Erd_U32_CoolingSystemRequestVoteStruct, U32Value, Vote_DontCare);
      GivenTheU8VoteStructErdHasValue(Erd_U8_AnotherCoolingSystemRequestVoteStruct, U8Value, Vote_DontCare);
      GivenTheU16VoteStructErdHasValue(Erd_U16_AnotherCoolingSystemRequestVoteStruct, U16Value, Vote_DontCare);
      GivenTheU32VoteStructErdHasValue(Erd_U32_AnotherCoolingSystemRequestVoteStruct, U32Value, Vote_DontCare);
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

   void AllCoolingSystemRequestVotesShouldBeOffAndCare(void)
   {
      TheU8VoteStructErdShouldBe(Erd_U8_CoolingSystemRequestVoteStruct, U8OffValue, Vote_Care);
      TheU16VoteStructErdShouldBe(Erd_U16_CoolingSystemRequestVoteStruct, U16OffValue, Vote_Care);
      TheU32VoteStructErdShouldBe(Erd_U32_CoolingSystemRequestVoteStruct, U32OffValue, Vote_Care);
      TheU8VoteStructErdShouldBe(Erd_U8_AnotherCoolingSystemRequestVoteStruct, U8AnotherOffValue, Vote_Care);
      TheU16VoteStructErdShouldBe(Erd_U16_AnotherCoolingSystemRequestVoteStruct, U16AnotherOffValue, Vote_Care);
      TheU32VoteStructErdShouldBe(Erd_U32_AnotherCoolingSystemRequestVoteStruct, U32AnotherOffValue, Vote_Care);
   }

   void AllCoolingSystemRequestVotesShouldBeOffAndDontCare(void)
   {
      TheU8VoteStructErdShouldBe(Erd_U8_CoolingSystemRequestVoteStruct, U8OffValue, Vote_DontCare);
      TheU16VoteStructErdShouldBe(Erd_U16_CoolingSystemRequestVoteStruct, U16OffValue, Vote_DontCare);
      TheU32VoteStructErdShouldBe(Erd_U32_CoolingSystemRequestVoteStruct, U32OffValue, Vote_DontCare);
      TheU8VoteStructErdShouldBe(Erd_U8_AnotherCoolingSystemRequestVoteStruct, U8AnotherOffValue, Vote_DontCare);
      TheU16VoteStructErdShouldBe(Erd_U16_AnotherCoolingSystemRequestVoteStruct, U16AnotherOffValue, Vote_DontCare);
      TheU32VoteStructErdShouldBe(Erd_U32_AnotherCoolingSystemRequestVoteStruct, U32AnotherOffValue, Vote_DontCare);
   }

   void GivenTheCoolingSystemOffStatusIs(bool state)
   {
      DataModel_Write(dataModel, Erd_CoolingSystemOffStatus, &state);
   }

   void TheCoolingSystemOffStatusShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_CoolingSystemOffStatus, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void DefrostDisabledShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_DisableDefrost, &actual);
      CHECK_EQUAL(expected, actual);
   }
};

TEST(CoolingSystemRequestHandler, ShouldVoteForAllTheLoadsAndDisableDefrostWhenTheCoolingSystemOffRequestChangesToTrue)
{
   GivenTheModuleIsInitialized();
   GivenTheU8VoteStructErdHasValue(Erd_U8_CoolingSystemRequestVoteStruct, U8ChangeValue, Vote_DontCare);
   GivenTheU16VoteStructErdHasValue(Erd_U16_CoolingSystemRequestVoteStruct, U16ChangeValue, Vote_DontCare);
   GivenTheU32VoteStructErdHasValue(Erd_U32_CoolingSystemRequestVoteStruct, U32ChangeValue, Vote_DontCare);
   GivenTheU8VoteStructErdHasValue(Erd_U8_AnotherCoolingSystemRequestVoteStruct, U8ChangeValue, Vote_DontCare);
   GivenTheU16VoteStructErdHasValue(Erd_U16_AnotherCoolingSystemRequestVoteStruct, U16ChangeValue, Vote_DontCare);
   GivenTheU32VoteStructErdHasValue(Erd_U32_AnotherCoolingSystemRequestVoteStruct, U32ChangeValue, Vote_DontCare);

   WhenTheCoolingSystemOffRequestChangesTo(true);
   TheCoolingSystemOffStatusShouldBe(true);
   DefrostDisabledShouldBe(true);
   AllCoolingSystemRequestVotesShouldBeOffAndCare();
}

TEST(CoolingSystemRequestHandler, ShouldVoteForAllTheLoadsWithCareAndDisableDefrostWhenTheCoolingSystemOffStatusIsTrueOnInitialization)
{
   GivenTheCoolingSystemOffStatusIs(true);
   GivenTheModuleIsInitialized();

   TheCoolingSystemOffStatusShouldBe(true);
   DefrostDisabledShouldBe(true);
   AllCoolingSystemRequestVotesShouldBeOffAndCare();
}

TEST(CoolingSystemRequestHandler, ShouldNotVoteAtAllOrEnableDefrostIfCoolingSystemRequestIsClearValue)
{
   GivenTheCoolingSystemOffStatusIs(true);
   GivenTheModuleIsInitialized();

   TheCoolingSystemOffStatusShouldBe(true);
   DefrostDisabledShouldBe(true);
   AllCoolingSystemRequestVotesShouldBeOffAndCare();

   WhenTheCoolingSystemOffRequestChangesTo(ClearValue);
   TheCoolingSystemOffStatusShouldBe(true);
   DefrostDisabledShouldBe(true);
   AllCoolingSystemRequestVotesShouldBeOffAndCare();
}

TEST(CoolingSystemRequestHandler, ShouldVoteForAllTheLoadsWithNoCareWhenTheCoolingSystemOffStatusIsFalseOnInitialization)
{
   GivenTheCoolingSystemOffStatusIs(false);
   GivenTheModuleIsInitialized();

   TheCoolingSystemOffStatusShouldBe(false);
   AllCoolingSystemRequestVotesShouldBeOffAndDontCare();
}

TEST(CoolingSystemRequestHandler, ShouldVoteForAllTheLoadsWithNoCareAndEnableDefrostWhenTheCoolingSystemOffRequestChangesToTrueThenFalse)
{
   GivenTheModuleIsInitialized();
   GivenTheU8VoteStructErdHasValue(Erd_U8_CoolingSystemRequestVoteStruct, U8ChangeValue, Vote_DontCare);
   GivenTheU16VoteStructErdHasValue(Erd_U16_CoolingSystemRequestVoteStruct, U16ChangeValue, Vote_DontCare);
   GivenTheU32VoteStructErdHasValue(Erd_U32_CoolingSystemRequestVoteStruct, U32ChangeValue, Vote_DontCare);
   GivenTheU8VoteStructErdHasValue(Erd_U8_AnotherCoolingSystemRequestVoteStruct, U8ChangeValue, Vote_DontCare);
   GivenTheU16VoteStructErdHasValue(Erd_U16_AnotherCoolingSystemRequestVoteStruct, U16ChangeValue, Vote_DontCare);
   GivenTheU32VoteStructErdHasValue(Erd_U32_AnotherCoolingSystemRequestVoteStruct, U32ChangeValue, Vote_DontCare);

   WhenTheCoolingSystemOffRequestChangesTo(true);
   TheCoolingSystemOffStatusShouldBe(true);
   DefrostDisabledShouldBe(true);
   AllCoolingSystemRequestVotesShouldBeOffAndCare();

   WhenTheCoolingSystemOffRequestChangesTo(false);
   TheCoolingSystemOffStatusShouldBe(false);
   DefrostDisabledShouldBe(false);
   AllCoolingSystemRequestVotesShouldBeOffAndDontCare();
}

TEST(CoolingSystemRequestHandler, ShouldClearRequestErdOnceARequestIsMade)
{
   GivenTheModuleIsInitialized();

   WhenTheCoolingSystemOffRequestChangesTo(true);
   WhenTheCoolingSystemOffRequestShouldBe(ClearValue);

   WhenTheCoolingSystemOffRequestChangesTo(false);
   WhenTheCoolingSystemOffRequestShouldBe(ClearValue);
}

TEST(CoolingSystemRequestHandler, ShouldAssertWhenOverSizeVoteErdIsIncludedInConfig)
{
   ShouldFailAssertionWhen(GivenTheModuleIsInitializedWithU64Vote());
}
