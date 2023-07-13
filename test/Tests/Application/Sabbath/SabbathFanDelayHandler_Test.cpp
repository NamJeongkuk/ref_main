/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "SabbathFanDelayHandler.h"
}

#include "CppUTest/TestHarness.h"
#include "ReferDataModel_TestDouble.h"

static Erd_t sabbathFanVoteErdList[] = {
   Erd_CondenserFanSpeed_SabbathVote,
   Erd_FreezerEvapFanSpeed_SabbathVote,
   Erd_FreshFoodEvapFanSpeed_SabbathVote
};

static const SabbathFanDelayHandlerConfiguration_t config = {
   .compressorStateErd = Erd_CompressorState,
   .sabbathFanVoteErdList = {
      .erds = sabbathFanVoteErdList,
      .numberOfErds = NUM_ELEMENTS(sabbathFanVoteErdList) }
};

TEST_GROUP(SabbathFanDelayHandler)
{
   ReferDataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;
   SabbathFanDelayHandler_t instance;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelTestDouble);
      dataModel = dataModelTestDouble.dataModel;
   }

   void GivenTheSabbathFanDelayHandlerIsInitialized()
   {
      SabbathFanDelayHandler_Init(
         &instance,
         dataModel,
         &config);
   }

   void GivenTheCompressorStateIs(CompressorState_t compressorState)
   {
      WhenTheCompressorStateIs(compressorState);
   }

   void WhenTheCompressorStateIs(CompressorState_t compressorState)
   {
      DataModel_Write(
         dataModel,
         Erd_CompressorState,
         &compressorState);
   }

   void GivenTheSabbathFanVoteIs(FanSpeed_t speed, Vote_t care)
   {
      FanVotedSpeed_t fanVotedSpeed = { .speed = speed, .care = care };
      ErdList_WriteAll(
         dataModel,
         &config.sabbathFanVoteErdList,
         &fanVotedSpeed);
   }

   void TheSabbathVoteShouldBe(FanSpeed_t expectedSpeed, Vote_t expectedCare)
   {
      FanVotedSpeed_t actualFanVotedSpeed;
      for(uint8_t sabbathVoteErdListIndex = 0; sabbathVoteErdListIndex < NUM_ELEMENTS(sabbathFanVoteErdList); sabbathVoteErdListIndex += 1)
      {
         DataModel_Read(
            dataModel,
            sabbathFanVoteErdList[sabbathVoteErdListIndex],
            &actualFanVotedSpeed);

         CHECK_EQUAL(expectedSpeed, actualFanVotedSpeed.speed);
         CHECK_EQUAL(expectedCare, actualFanVotedSpeed.care);
      }
   }
};

TEST(SabbathFanDelayHandler, ShouldVoteOffForSabbathVoteFansWhenCompressorStateIsSabbathDelay)
{
   GivenTheSabbathFanDelayHandlerIsInitialized();
   GivenTheCompressorStateIs(CompressorState_SabbathDelay);
   TheSabbathVoteShouldBe(FanSpeed_Off, Vote_Care);
}

TEST(SabbathFanDelayHandler, ShouldVoteDontCareForSabbathVoteFansWhenCompressorStateIsNotSabbathDelay)
{
   GivenTheSabbathFanDelayHandlerIsInitialized();
   GivenTheCompressorStateIs(CompressorState_SabbathDelay);
   TheSabbathVoteShouldBe(FanSpeed_Off, Vote_Care);

   WhenTheCompressorStateIs(CompressorState_On);
   TheSabbathVoteShouldBe(FanSpeed_Off, Vote_DontCare);
}

TEST(SabbathFanDelayHandler, ShouldToggleSabbathVoteFansWhenCompressorStateChangesToSabbathDelayFromOtherStates)
{
   GivenTheSabbathFanDelayHandlerIsInitialized();
   TheSabbathVoteShouldBe(FanSpeed_Off, Vote_DontCare);

   WhenTheCompressorStateIs(CompressorState_SabbathDelay);
   TheSabbathVoteShouldBe(FanSpeed_Off, Vote_Care);

   WhenTheCompressorStateIs(CompressorState_Max);
   TheSabbathVoteShouldBe(FanSpeed_Off, Vote_DontCare);

   WhenTheCompressorStateIs(CompressorState_SabbathDelay);
   TheSabbathVoteShouldBe(FanSpeed_Off, Vote_Care);

   WhenTheCompressorStateIs(CompressorState_Off);
   TheSabbathVoteShouldBe(FanSpeed_Off, Vote_DontCare);
}
