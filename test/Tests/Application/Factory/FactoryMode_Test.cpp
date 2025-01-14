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
#include "BooleanVotedState.h"
#include "RampingPwmDutyCyclePercentageVote.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "DataModel_TestDouble.h"
#include "TimerModule_TestDouble.h"
#include "Signal.h"
#include "Constants_Time.h"
#include "uassert_test.h"
#include "DataModel_Mock.h"

#define Given
#define When
#define Then
#define And

enum
{
   TwoMinutes = 2,
   OneMinute = 1,
   ZeroMinutes = 0,
};

enum
{
   Erd_FactoryModeEnableRequestInMinutes,
   Erd_BroadcastResetRequestSignal,
   Erd_Boolean_FactoryVoteStruct,
   Erd_U8_FactoryVoteStruct,
   Erd_U16_FactoryVoteStruct,
   Erd_U32_FactoryVoteStruct,
   Erd_U64_FactoryVoteStruct,
   Erd_U8_AnotherFactoryVoteStruct,
   Erd_U16_AnotherFactoryVoteStruct,
   Erd_U32_AnotherFactoryVoteStruct,
   Erd_U64_AnotherFactoryVoteStruct,
   Erd_FreshFoodBackWallLightFactoryVote,
   Erd_FreshFoodTopLightFactoryVote,
   Erd_FreezerBackWallLightFactoryVote,
   Erd_FreezerTopLightFactoryVote,
   Erd_SealedSystemValveHomingRequest,
   Erd_U16_NonVotingStruct,
   Erd_U32_NonVotingStruct
};

static bool booleanOnValue = true;
static uint8_t U8Value = UINT8_MAX;
static uint16_t U16Value = UINT16_MAX;
static uint32_t U32Value = UINT32_MAX;

static bool booleanOffValue = false;
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
   { Erd_FactoryModeEnableRequestInMinutes, sizeof(uint8_t) },
   { Erd_BroadcastResetRequestSignal, sizeof(Signal_t) },
   { Erd_Boolean_FactoryVoteStruct, sizeof(BooleanVotedState_t) },
   { Erd_U8_FactoryVoteStruct, sizeof(U8Vote_t) },
   { Erd_U16_FactoryVoteStruct, sizeof(U16Vote_t) },
   { Erd_U32_FactoryVoteStruct, sizeof(U32Vote_t) },
   { Erd_U64_FactoryVoteStruct, sizeof(U64Vote_t) },
   { Erd_U8_AnotherFactoryVoteStruct, sizeof(U8Vote_t) },
   { Erd_U16_AnotherFactoryVoteStruct, sizeof(U16Vote_t) },
   { Erd_U32_AnotherFactoryVoteStruct, sizeof(U32Vote_t) },
   { Erd_U64_AnotherFactoryVoteStruct, sizeof(U64Vote_t) },
   { Erd_FreshFoodBackWallLightFactoryVote, sizeof(RampingPwmDutyCyclePercentageVote_t) },
   { Erd_FreshFoodTopLightFactoryVote, sizeof(RampingPwmDutyCyclePercentageVote_t) },
   { Erd_FreezerBackWallLightFactoryVote, sizeof(RampingPwmDutyCyclePercentageVote_t) },
   { Erd_FreezerTopLightFactoryVote, sizeof(RampingPwmDutyCyclePercentageVote_t) },
   { Erd_SealedSystemValveHomingRequest, sizeof(bool) },
   { Erd_U16_NonVotingStruct, sizeof(uint16_t) },
   { Erd_U32_NonVotingStruct, sizeof(uint32_t) }
};

static const ErdOffValuePair_t factoryVotePairs[] = {
   { Erd_Boolean_FactoryVoteStruct, booleanOffValue },
   { Erd_U8_FactoryVoteStruct, U8OffValue },
   { Erd_U16_FactoryVoteStruct, U16OffValue },
   { Erd_U32_FactoryVoteStruct, U32OffValue },
   { Erd_U8_AnotherFactoryVoteStruct, U8AnotherOffValue },
   { Erd_U16_AnotherFactoryVoteStruct, U16AnotherOffValue },
   { Erd_U32_AnotherFactoryVoteStruct, U32AnotherOffValue }
};

static const ErdOffValuePair_t factoryVotePairsWithU64Vote[] = {
   { Erd_U64_FactoryVoteStruct, 0 },
   { Erd_U64_AnotherFactoryVoteStruct, 0 }
};

static const FactoryModeList_t factoryVoteList = {
   .pairs = factoryVotePairs,
   .numberOfPairs = NUM_ELEMENTS(factoryVotePairs)
};

static const ErdOffValuePair_t nonVotingPairs[] = {
   { Erd_SealedSystemValveHomingRequest, true },
   { Erd_U16_NonVotingStruct, U16AnotherOffValue },
   { Erd_U32_NonVotingStruct, U32AnotherOffValue }
};

static const FactoryModeList_t nonVotingPairsList = {
   .pairs = nonVotingPairs,
   .numberOfPairs = NUM_ELEMENTS(nonVotingPairs)
};

static const FactoryModeList_t factoryVoteListWithU64Vote = {
   .pairs = factoryVotePairsWithU64Vote,
   .numberOfPairs = NUM_ELEMENTS(factoryVotePairsWithU64Vote)
};

static const Erd_t lightVoteErds[] = {
   Erd_FreshFoodBackWallLightFactoryVote,
   Erd_FreshFoodTopLightFactoryVote,
   Erd_FreezerBackWallLightFactoryVote,
   Erd_FreezerTopLightFactoryVote
};

static const ErdList_t lightVoteErdList = {
   .erds = lightVoteErds,
   .numberOfErds = NUM_ELEMENTS(lightVoteErds)
};

static const FactoryModeConfiguration_t config = {
   .factoryModeTimeErd = Erd_FactoryModeEnableRequestInMinutes,
   .broadcastResetRequestErd = Erd_BroadcastResetRequestSignal,
   .factoryVoteList = factoryVoteList,
   .lightVoteErdList = &lightVoteErdList,
   .erdValuePairList = &nonVotingPairsList
};

static const FactoryModeConfiguration_t configU64Vote = {
   .factoryModeTimeErd = Erd_FactoryModeEnableRequestInMinutes,
   .broadcastResetRequestErd = Erd_BroadcastResetRequestSignal,
   .factoryVoteList = factoryVoteListWithU64Vote,
   .lightVoteErdList = &lightVoteErdList,
   .erdValuePairList = NULL
};

static const FactoryModeConfiguration_t configU64NonVote = {
   .factoryModeTimeErd = Erd_FactoryModeEnableRequestInMinutes,
   .broadcastResetRequestErd = Erd_BroadcastResetRequestSignal,
   .factoryVoteList = factoryVoteList,
   .lightVoteErdList = &lightVoteErdList,
   .erdValuePairList = &factoryVoteListWithU64Vote
};

static const FactoryModeConfiguration_t votingOnlyErdConfig = {
   .factoryModeTimeErd = Erd_FactoryModeEnableRequestInMinutes,
   .broadcastResetRequestErd = Erd_BroadcastResetRequestSignal,
   .factoryVoteList = factoryVoteList,
   .lightVoteErdList = &lightVoteErdList,
   .erdValuePairList = NULL
};

static void OnDataModelChange(void *context, const void *_args)
{
   REINTERPRET(dataModel, context, I_DataModel_t *);
   REINTERPRET(args, _args, const DataModelOnDataChangeArgs_t *);

   if(args->erd == nonVotingPairs[0].erd)
   {
      REINTERPRET(data, args->data, const bool *);

      mock()
         .actualCall("Non Voting Pairs Write")
         .onObject(dataModel)
         .withParameter("Erd", args->erd)
         .withParameter("Data", *data);
   }
   else if(args->erd == factoryVotePairs[0].erd)
   {
      REINTERPRET(data, args->data, const BooleanVotedState_t *);

      mock()
         .actualCall("Voting Pairs Write")
         .onObject(dataModel)
         .withParameter("Erd", args->erd)
         .withParameter("Care", data->care)
         .withParameter("State", data->state);
   }
}

TEST_GROUP(FactoryMode)
{
   DataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   FactoryMode_t instance;
   TimerModule_TestDouble_t timerModuleDouble;
   EventSubscription_t dataModelTestDoubleOnChangeEventSubscription;

   void setup()
   {
      DataModel_TestDouble_Init(&dataModelDouble, erdTable, NUM_ELEMENTS(erdTable));
      dataModel = dataModelDouble.dataModel;
      TimerModule_TestDouble_Init(&timerModuleDouble);
   }

   void ModuleIsInitialized()
   {
      FactoryMode_Init(&instance, dataModel, &config, &timerModuleDouble.timerModule);
   }

   void ModuleIsInitializedWithDataModelMock()
   {
      EventSubscription_Init(
         &dataModelTestDoubleOnChangeEventSubscription,
         dataModel,
         OnDataModelChange);
      Event_Subscribe(
         dataModel->OnDataChange,
         &dataModelTestDoubleOnChangeEventSubscription);

      ModuleIsInitialized();

      mock().strictOrder();
   }

   void ModuleIsInitializedWithU64Vote()
   {
      FactoryMode_Init(&instance, dataModel, &configU64Vote, &timerModuleDouble.timerModule);
   }

   void ModuleIsInitializedWithU64NonVote()
   {
      FactoryMode_Init(&instance, dataModel, &configU64NonVote, &timerModuleDouble.timerModule);
   }

   void ModuleIsInitializedWithVotingOnlyConfig()
   {
      FactoryMode_Init(&instance, dataModel, &votingOnlyErdConfig, &timerModuleDouble.timerModule);
   }

   void FactoryModeEnableRequestInMinutesIs(uint8_t value)
   {
      DataModel_Write(dataModel, Erd_FactoryModeEnableRequestInMinutes, &value);
   }

   void FactoryModeEnableRequestInMinutesShouldBe(uint8_t expected)
   {
      uint8_t actual;
      DataModel_Read(dataModel, Erd_FactoryModeEnableRequestInMinutes, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(&timerModuleDouble, ticks);
   }

   void BooleanVoteStructErdHasValue(Erd_t erd, bool value, Vote_t vote)
   {
      BooleanVotedState_t offVote = { value, vote };
      DataModel_Write(dataModel, erd, &offVote);
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
      Given BooleanVoteStructErdHasValue(Erd_Boolean_FactoryVoteStruct, booleanOnValue, Vote_DontCare);
      Given U8VoteStructErdHasValue(Erd_U8_FactoryVoteStruct, U8Value, Vote_DontCare);
      Given U16VoteStructErdHasValue(Erd_U16_FactoryVoteStruct, U16Value, Vote_DontCare);
      Given U32VoteStructErdHasValue(Erd_U32_FactoryVoteStruct, U32Value, Vote_DontCare);
      Given U8VoteStructErdHasValue(Erd_U8_AnotherFactoryVoteStruct, U8Value, Vote_DontCare);
      Given U16VoteStructErdHasValue(Erd_U16_AnotherFactoryVoteStruct, U16Value, Vote_DontCare);
      Given U32VoteStructErdHasValue(Erd_U32_AnotherFactoryVoteStruct, U32Value, Vote_DontCare);
   }

   void TheBooleanVoteStructErdShouldBe(Erd_t erd, bool expectedValue, Vote_t expectedVote)
   {
      BooleanVotedState_t actual;
      DataModel_Read(dataModel, erd, &actual);

      CHECK_EQUAL(expectedValue, actual.state);
      CHECK_EQUAL(expectedVote, actual.care);
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

   void TheLightVoteErdShouldBeOff(Erd_t erd)
   {
      RampingPwmDutyCyclePercentageVote_t actual;
      DataModel_Read(dataModel, erd, &actual);

      CHECK_EQUAL(PercentageDutyCycle_Min, actual.rampingPwmDutyCyclePercentage.pwmDutyCyclePercentage);
      CHECK_EQUAL(UINT8_MAX, actual.rampingPwmDutyCyclePercentage.rampingDownCountInMsec);
      CHECK_EQUAL(UINT8_MAX, actual.rampingPwmDutyCyclePercentage.rampingUpCountInMsec);
      CHECK_EQUAL(Vote_Care, actual.care);
   }

   void AllFactoryVotesShouldBeOff(void)
   {
      TheBooleanVoteStructErdShouldBe(Erd_Boolean_FactoryVoteStruct, booleanOffValue, Vote_Care);
      TheU8VoteStructErdShouldBe(Erd_U8_FactoryVoteStruct, U8OffValue, Vote_Care);
      TheU16VoteStructErdShouldBe(Erd_U16_FactoryVoteStruct, U16OffValue, Vote_Care);
      TheU32VoteStructErdShouldBe(Erd_U32_FactoryVoteStruct, U32OffValue, Vote_Care);
      TheU8VoteStructErdShouldBe(Erd_U8_AnotherFactoryVoteStruct, U8AnotherOffValue, Vote_Care);
      TheU16VoteStructErdShouldBe(Erd_U16_AnotherFactoryVoteStruct, U16AnotherOffValue, Vote_Care);
      TheU32VoteStructErdShouldBe(Erd_U32_AnotherFactoryVoteStruct, U32AnotherOffValue, Vote_Care);

      for(uint8_t index = 0; index < lightVoteErdList.numberOfErds; index++)
      {
         TheLightVoteErdShouldBeOff(lightVoteErdList.erds[index]);
      }
   }

   void BroadcastResetRequestShouldBe(uint8_t expected)
   {
      uint8_t actual;
      DataModel_Read(dataModel, Erd_BroadcastResetRequestSignal, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void GetBroadcastResetSignalValue(Signal_t * currentValue)
   {
      DataModel_Read(dataModel, Erd_BroadcastResetRequestSignal, currentValue);
   }

   void SealedSystemHomeRequestShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_SealedSystemValveHomingRequest, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void GivenSealedSystemHomeRequestTo(bool request)
   {
      DataModel_Write(dataModel, Erd_SealedSystemValveHomingRequest, &request);
   }

   void DoNothingWithErdValuePairs()
   {
   }

   void GivenU16StructSetTo(uint16_t value)
   {
      DataModel_Write(dataModel, Erd_U16_NonVotingStruct, &value);
   }

   void GivenU16StructShouldBe(uint16_t value)
   {
      uint16_t actual;
      DataModel_Read(dataModel, Erd_U16_NonVotingStruct, &actual);
      CHECK_EQUAL(value, actual);
   }

   void GivenU32StructSetTo(uint32_t value)
   {
      DataModel_Write(dataModel, Erd_U32_NonVotingStruct, &value);
   }

   void GivenU32StructShouldBe(uint32_t value)
   {
      uint32_t actual;
      DataModel_Read(dataModel, Erd_U32_NonVotingStruct, &actual);
      CHECK_EQUAL(value, actual);
   }

   void ErdValuePairsShouldBeWrittenTo()
   {
      mock()
         .expectOneCall("Non Voting Pairs Write")
         .onObject(dataModel)
         .withParameter("Erd", nonVotingPairs[0].erd)
         .withParameter("Data", true);
   }

   void VotingErdsShouldBeWrittenTo()
   {
      mock()
         .expectOneCall("Voting Pairs Write")
         .onObject(dataModel)
         .withParameter("Erd", factoryVotePairs[0].erd)
         .withParameter("Care", Vote_Care)
         .withParameter("State", booleanOffValue);
   }
};

TEST(FactoryMode, ShouldInitialize)
{
   ModuleIsInitialized();
}

TEST(FactoryMode, ShouldVoteOffForAllTheLoadsWhenFactoryModeEnableRequestInMinutesIsChangedFromZeroToNotZero)
{
   Given AllFactoryVoteErdsAreUninitialized();
   Given FactoryModeEnableRequestInMinutesIs(ZeroMinutes);
   Given ModuleIsInitialized();

   When FactoryModeEnableRequestInMinutesIs(OneMinute);
   AllFactoryVotesShouldBeOff();
}

TEST(FactoryMode, ShouldNotVoteToOffForAllTheLoadsWhenFactoryModeEnableRequestInMinutesDecreasese)
{
   Given ModuleIsInitialized();
   Given FactoryModeEnableRequestInMinutesIs(TwoMinutes);
   Given BooleanVoteStructErdHasValue(Erd_Boolean_FactoryVoteStruct, booleanOnValue, Vote_Care);
   Given U8VoteStructErdHasValue(Erd_U8_FactoryVoteStruct, U8ChangeValue, Vote_Care);
   Given U16VoteStructErdHasValue(Erd_U16_FactoryVoteStruct, U16ChangeValue, Vote_Care);
   Given U32VoteStructErdHasValue(Erd_U32_FactoryVoteStruct, U32ChangeValue, Vote_Care);
   Given U8VoteStructErdHasValue(Erd_U8_AnotherFactoryVoteStruct, U8ChangeValue, Vote_Care);
   Given U16VoteStructErdHasValue(Erd_U16_AnotherFactoryVoteStruct, U16ChangeValue, Vote_Care);
   Given U32VoteStructErdHasValue(Erd_U32_AnotherFactoryVoteStruct, U32ChangeValue, Vote_Care);

   After(OneMinute * MSEC_PER_MIN);
   TheBooleanVoteStructErdShouldBe(Erd_Boolean_FactoryVoteStruct, booleanOnValue, Vote_Care);
   TheU8VoteStructErdShouldBe(Erd_U8_FactoryVoteStruct, U8ChangeValue, Vote_Care);
   TheU16VoteStructErdShouldBe(Erd_U16_FactoryVoteStruct, U16ChangeValue, Vote_Care);
   TheU32VoteStructErdShouldBe(Erd_U32_FactoryVoteStruct, U32ChangeValue, Vote_Care);
   TheU8VoteStructErdShouldBe(Erd_U8_AnotherFactoryVoteStruct, U8ChangeValue, Vote_Care);
   TheU16VoteStructErdShouldBe(Erd_U16_AnotherFactoryVoteStruct, U16ChangeValue, Vote_Care);
   TheU32VoteStructErdShouldBe(Erd_U32_AnotherFactoryVoteStruct, U32ChangeValue, Vote_Care);
}

TEST(FactoryMode, ShouldAssertWhenOverSizeVoteErdIsIncludedInConfig)
{
   Given FactoryModeEnableRequestInMinutesIs(ZeroMinutes);
   Given ModuleIsInitializedWithU64Vote();

   ShouldFailAssertionWhen(FactoryModeEnableRequestInMinutesIs(OneMinute));
}

TEST(FactoryMode, ShouldBroadcastResetRequestWhenFactoryModeEnableRequestInMinutesIsSetToZero)
{
   Given ModuleIsInitialized();
   Given FactoryModeEnableRequestInMinutesIs(TwoMinutes);

   Signal_t currentBroadcastResetSignalValue;
   GetBroadcastResetSignalValue(&currentBroadcastResetSignalValue);

   When FactoryModeEnableRequestInMinutesIs(ZeroMinutes);
   BroadcastResetRequestShouldBe(currentBroadcastResetSignalValue + 1);
}

TEST(FactoryMode, ShouldBroadcastResetRequestWhenFactoryModeEnableRequestInMinutesReachesZero)
{
   Given ModuleIsInitialized();
   Given FactoryModeEnableRequestInMinutesIs(TwoMinutes);

   Signal_t currentBroadcastResetSignalValue;
   GetBroadcastResetSignalValue(&currentBroadcastResetSignalValue);

   After(OneMinute * MSEC_PER_MIN);
   BroadcastResetRequestShouldBe(currentBroadcastResetSignalValue);

   After(OneMinute * MSEC_PER_MIN - 1);
   BroadcastResetRequestShouldBe(currentBroadcastResetSignalValue);

   After(1);
   BroadcastResetRequestShouldBe(currentBroadcastResetSignalValue + 1);
}

TEST(FactoryMode, ShouldDecreaseFactoryModeEnableRequestInMinutesByOneEveryMinute)
{
   Given ModuleIsInitialized();
   Given FactoryModeEnableRequestInMinutesIs(TwoMinutes);

   After(OneMinute * MSEC_PER_MIN - 1);
   FactoryModeEnableRequestInMinutesShouldBe(TwoMinutes);

   After(1);
   FactoryModeEnableRequestInMinutesShouldBe(OneMinute);

   After(OneMinute * MSEC_PER_MIN - 1);
   FactoryModeEnableRequestInMinutesShouldBe(OneMinute);

   After(1);
   FactoryModeEnableRequestInMinutesShouldBe(ZeroMinutes);
}

TEST(FactoryMode, ShouldDecreaseFactoryModeEnableRequestInMinutesByOneEveryMinuteAfterHavingNewValue)
{
   Given ModuleIsInitialized();
   Given FactoryModeEnableRequestInMinutesIs(TwoMinutes);

   After(OneMinute * MSEC_PER_MIN);
   FactoryModeEnableRequestInMinutesShouldBe(OneMinute);

   When FactoryModeEnableRequestInMinutesIs(TwoMinutes);

   After(OneMinute * MSEC_PER_MIN - 1);
   FactoryModeEnableRequestInMinutesShouldBe(TwoMinutes);

   After(1);
   FactoryModeEnableRequestInMinutesShouldBe(OneMinute);

   After(OneMinute * MSEC_PER_MIN - 1);
   FactoryModeEnableRequestInMinutesShouldBe(OneMinute);

   After(1);
   FactoryModeEnableRequestInMinutesShouldBe(ZeroMinutes);
}

TEST(FactoryMode, ShouldWriteToErdValuePairsWhenFactoryModeChanges)
{
   GivenSealedSystemHomeRequestTo(false);
   Given ModuleIsInitialized();

   When FactoryModeEnableRequestInMinutesIs(OneMinute);
   SealedSystemHomeRequestShouldBe(true);
}

TEST(FactoryMode, ShouldHandleNullErdValuePairsWhenFactoryModeChanges)
{
   Given ModuleIsInitializedWithVotingOnlyConfig();

   When FactoryModeEnableRequestInMinutesIs(OneMinute);
   DoNothingWithErdValuePairs();
}

TEST(FactoryMode, ShouldHandleUint16InerdValuePairList)
{
   GivenU16StructSetTo(U16ChangeValue);

   Given ModuleIsInitialized();
   When FactoryModeEnableRequestInMinutesIs(OneMinute);
   GivenU16StructShouldBe(U16AnotherOffValue);
}

TEST(FactoryMode, ShouldHandleUint32InerdValuePairList)
{
   GivenU32StructSetTo(U32ChangeValue);
   Given ModuleIsInitialized();
   When FactoryModeEnableRequestInMinutesIs(OneMinute);
   GivenU32StructShouldBe(U32AnotherOffValue);
}

TEST(FactoryMode, ShouldAssertWhenOversizedNonVoteErdIsIncludedInConfig)
{
   Given FactoryModeEnableRequestInMinutesIs(ZeroMinutes);
   Given ModuleIsInitializedWithU64NonVote();

   ShouldFailAssertionWhen(FactoryModeEnableRequestInMinutesIs(OneMinute));
}

TEST(FactoryMode, ShouldWriteErdValuePairsBeforeWritingVotingErds)
{
   ModuleIsInitializedWithDataModelMock();

   ErdValuePairsShouldBeWrittenTo();
   VotingErdsShouldBeWrittenTo();
   When FactoryModeEnableRequestInMinutesIs(OneMinute);
}
