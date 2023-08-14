/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "ColdestOfSetpointVotes.h"
#include "SystemErds.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

static const Erd_t setpointVoteErds[] = {
   Erd_FreezerSetpoint_TurboFreezeVote,
   Erd_FreezerSetpoint_IceInDoorVote,
   Erd_FreezerSetpoint_FreezerIceMakerVote,
   Erd_FreezerSetpoint_FreezerIceRateVote,
   Erd_FreezerSetpoint_UserVote
};

static const ErdList_t erdListOfSetpointVoteErds = {
   .erds = setpointVoteErds,
   .numberOfErds = NUM_ELEMENTS(setpointVoteErds)
};

static const ColdestOfSetpointVotesConfiguration_t config = {
   .listOfSetpointVoteErds = erdListOfSetpointVoteErds,
   .coldestSetpointVoteErd = Erd_FreezerSetpoint_ColdestSetpointVote,
   .coldestSetpointWinningVoteErd = Erd_FreezerSetpoint_ColdestSetpointWinningVote
};

TEST_GROUP(ColdestOfSetpointVotes)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   ColdestOfSetpointVotes_t instance;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
   }

   void GivenTheModuleIsInitialized()
   {
      ColdestOfSetpointVotes_Init(&instance, dataModel, &config);
   }

   void GivenVoteIs(Erd_t erd, SetpointVotedTemperature_t vote)
   {
      DataModel_Write(dataModel, erd, &vote);
   }

   void WhenVoteIs(Erd_t erd, SetpointVotedTemperature_t vote)
   {
      GivenVoteIs(erd, vote);
   }

   void ColdestSetpointVoteShouldBe(SetpointVotedTemperature_t expected)
   {
      SetpointVotedTemperature_t actual;
      DataModel_Read(dataModel, Erd_FreezerSetpoint_ColdestSetpointVote, &actual);

      CHECK_EQUAL(expected.temperatureInDegFx100, actual.temperatureInDegFx100);
      CHECK_EQUAL(expected.care, actual.care);
   }

   void ColdestSetpointWinningVoteErdShouldBe(Erd_t expected)
   {
      Erd_t actual;
      DataModel_Read(dataModel, Erd_FreezerSetpoint_ColdestSetpointWinningVote, &actual);

      CHECK_EQUAL(expected, actual);
   }
};

TEST(ColdestOfSetpointVotes, ShouldBe300AndCareIfAllVotesAreTheSameAndCareAndWinningVoteWillBeTheFirstOneInTheList)
{
   GivenVoteIs(Erd_FreezerSetpoint_TurboFreezeVote, { 300, Vote_Care });
   GivenVoteIs(Erd_FreezerSetpoint_IceInDoorVote, { 300, Vote_Care });
   GivenVoteIs(Erd_FreezerSetpoint_FreezerIceMakerVote, { 300, Vote_Care });
   GivenVoteIs(Erd_FreezerSetpoint_FreezerIceRateVote, { 300, Vote_Care });
   GivenVoteIs(Erd_FreezerSetpoint_UserVote, { 300, Vote_Care });

   GivenTheModuleIsInitialized();

   ColdestSetpointVoteShouldBe({ 300, Vote_Care });
   ColdestSetpointWinningVoteErdShouldBe(Erd_FreezerSetpoint_TurboFreezeVote);
}

TEST(ColdestOfSetpointVotes, ShouldBeTheColdestOfTheCaringVotesWhenAllVotesAreCare)
{
   GivenVoteIs(Erd_FreezerSetpoint_TurboFreezeVote, { 300, Vote_Care });
   GivenVoteIs(Erd_FreezerSetpoint_IceInDoorVote, { 200, Vote_Care });
   GivenVoteIs(Erd_FreezerSetpoint_FreezerIceMakerVote, { 300, Vote_Care });
   GivenVoteIs(Erd_FreezerSetpoint_FreezerIceRateVote, { -300, Vote_Care });
   GivenVoteIs(Erd_FreezerSetpoint_UserVote, { -150, Vote_Care });

   GivenTheModuleIsInitialized();

   ColdestSetpointVoteShouldBe({ -300, Vote_Care });
   ColdestSetpointWinningVoteErdShouldBe(Erd_FreezerSetpoint_FreezerIceRateVote);
}

TEST(ColdestOfSetpointVotes, ShouldBeTheColdestOfTheCaringVotesAndAllVotesWhenSomeVotesAreDontCare)
{
   GivenVoteIs(Erd_FreezerSetpoint_TurboFreezeVote, { 300, Vote_DontCare });
   GivenVoteIs(Erd_FreezerSetpoint_IceInDoorVote, { 200, Vote_Care });
   GivenVoteIs(Erd_FreezerSetpoint_FreezerIceMakerVote, { 300, Vote_Care });
   GivenVoteIs(Erd_FreezerSetpoint_FreezerIceRateVote, { -300, Vote_DontCare });
   GivenVoteIs(Erd_FreezerSetpoint_UserVote, { -150, Vote_Care });

   GivenTheModuleIsInitialized();

   ColdestSetpointVoteShouldBe({ -150, Vote_Care });
   ColdestSetpointWinningVoteErdShouldBe(Erd_FreezerSetpoint_UserVote);
}

TEST(ColdestOfSetpointVotes, ShouldBeInt16MaxandDontCareWhenAllAreDontCareAndWinningVoteShouldBeInvalid)
{
   GivenVoteIs(Erd_FreezerSetpoint_TurboFreezeVote, { 300, Vote_DontCare });
   GivenVoteIs(Erd_FreezerSetpoint_IceInDoorVote, { 200, Vote_DontCare });
   GivenVoteIs(Erd_FreezerSetpoint_FreezerIceMakerVote, { 300, Vote_DontCare });
   GivenVoteIs(Erd_FreezerSetpoint_FreezerIceRateVote, { -300, Vote_DontCare });
   GivenVoteIs(Erd_FreezerSetpoint_UserVote, { -150, Vote_DontCare });

   GivenTheModuleIsInitialized();

   ColdestSetpointVoteShouldBe({ INT16_MAX, Vote_DontCare });
   ColdestSetpointWinningVoteErdShouldBe(Erd_Invalid);
}

TEST(ColdestOfSetpointVotes, ShouldUpdateColdestSetpointVoteWhenTheColdestVoteChangesEvenColder)
{
   GivenVoteIs(Erd_FreezerSetpoint_TurboFreezeVote, { 300, Vote_DontCare });
   GivenVoteIs(Erd_FreezerSetpoint_IceInDoorVote, { 200, Vote_Care });
   GivenVoteIs(Erd_FreezerSetpoint_FreezerIceMakerVote, { 300, Vote_Care });
   GivenVoteIs(Erd_FreezerSetpoint_FreezerIceRateVote, { -300, Vote_DontCare });
   GivenVoteIs(Erd_FreezerSetpoint_UserVote, { -150, Vote_Care });

   GivenTheModuleIsInitialized();

   ColdestSetpointVoteShouldBe({ -150, Vote_Care });
   ColdestSetpointWinningVoteErdShouldBe(Erd_FreezerSetpoint_UserVote);

   WhenVoteIs(Erd_FreezerSetpoint_UserVote, { -200, Vote_Care });
   ColdestSetpointVoteShouldBe({ -200, Vote_Care });
   ColdestSetpointWinningVoteErdShouldBe(Erd_FreezerSetpoint_UserVote);
}

TEST(ColdestOfSetpointVotes, ShouldUpdateColdestSetpointVoteWhenTheColdestVoteChangesAndIsNoLongerTheColdest)
{
   GivenVoteIs(Erd_FreezerSetpoint_TurboFreezeVote, { 300, Vote_DontCare });
   GivenVoteIs(Erd_FreezerSetpoint_IceInDoorVote, { 200, Vote_Care });
   GivenVoteIs(Erd_FreezerSetpoint_FreezerIceMakerVote, { 300, Vote_Care });
   GivenVoteIs(Erd_FreezerSetpoint_FreezerIceRateVote, { -300, Vote_DontCare });
   GivenVoteIs(Erd_FreezerSetpoint_UserVote, { -150, Vote_Care });

   GivenTheModuleIsInitialized();

   ColdestSetpointVoteShouldBe({ -150, Vote_Care });
   ColdestSetpointWinningVoteErdShouldBe(Erd_FreezerSetpoint_UserVote);

   WhenVoteIs(Erd_FreezerSetpoint_UserVote, { 500, Vote_Care });
   ColdestSetpointVoteShouldBe({ 200, Vote_Care });
   ColdestSetpointWinningVoteErdShouldBe(Erd_FreezerSetpoint_IceInDoorVote);
}

TEST(ColdestOfSetpointVotes, ShouldUpdateColdestSetpointVoteWhenADifferentVoteChangesToColderThanTheColdest)
{
   GivenVoteIs(Erd_FreezerSetpoint_TurboFreezeVote, { 300, Vote_DontCare });
   GivenVoteIs(Erd_FreezerSetpoint_IceInDoorVote, { 200, Vote_Care });
   GivenVoteIs(Erd_FreezerSetpoint_FreezerIceMakerVote, { 300, Vote_Care });
   GivenVoteIs(Erd_FreezerSetpoint_FreezerIceRateVote, { -300, Vote_DontCare });
   GivenVoteIs(Erd_FreezerSetpoint_UserVote, { -150, Vote_Care });

   GivenTheModuleIsInitialized();

   ColdestSetpointVoteShouldBe({ -150, Vote_Care });
   ColdestSetpointWinningVoteErdShouldBe(Erd_FreezerSetpoint_UserVote);

   WhenVoteIs(Erd_FreezerSetpoint_IceInDoorVote, { -200, Vote_Care });
   ColdestSetpointVoteShouldBe({ -200, Vote_Care });
   ColdestSetpointWinningVoteErdShouldBe(Erd_FreezerSetpoint_IceInDoorVote);
}

TEST(ColdestOfSetpointVotes, ShouldUpdateColdestSetpointVoteWhenOneOfTheVotesChangesFromDontCareToCareAndIsNowTheColdest)
{
   GivenVoteIs(Erd_FreezerSetpoint_TurboFreezeVote, { 300, Vote_DontCare });
   GivenVoteIs(Erd_FreezerSetpoint_IceInDoorVote, { 200, Vote_Care });
   GivenVoteIs(Erd_FreezerSetpoint_FreezerIceMakerVote, { 300, Vote_Care });
   GivenVoteIs(Erd_FreezerSetpoint_FreezerIceRateVote, { -300, Vote_DontCare });
   GivenVoteIs(Erd_FreezerSetpoint_UserVote, { -150, Vote_Care });

   GivenTheModuleIsInitialized();

   ColdestSetpointVoteShouldBe({ -150, Vote_Care });
   ColdestSetpointWinningVoteErdShouldBe(Erd_FreezerSetpoint_UserVote);

   WhenVoteIs(Erd_FreezerSetpoint_FreezerIceRateVote, { -300, Vote_Care });
   ColdestSetpointVoteShouldBe({ -300, Vote_Care });
   ColdestSetpointWinningVoteErdShouldBe(Erd_FreezerSetpoint_FreezerIceRateVote);
}

TEST(ColdestOfSetpointVotes, ShouldUpdateColdestSetpointVoteWhenOneOfTheVotesChangesFromCareToDontCareAndIsNoLongerTheColdest)
{
   GivenVoteIs(Erd_FreezerSetpoint_TurboFreezeVote, { 300, Vote_DontCare });
   GivenVoteIs(Erd_FreezerSetpoint_IceInDoorVote, { 200, Vote_Care });
   GivenVoteIs(Erd_FreezerSetpoint_FreezerIceMakerVote, { 300, Vote_Care });
   GivenVoteIs(Erd_FreezerSetpoint_FreezerIceRateVote, { 450, Vote_Care });
   GivenVoteIs(Erd_FreezerSetpoint_UserVote, { 250, Vote_Care });

   GivenTheModuleIsInitialized();

   ColdestSetpointVoteShouldBe({ 200, Vote_Care });
   ColdestSetpointWinningVoteErdShouldBe(Erd_FreezerSetpoint_IceInDoorVote);

   WhenVoteIs(Erd_FreezerSetpoint_IceInDoorVote, { 200, Vote_DontCare });
   ColdestSetpointVoteShouldBe({ 250, Vote_Care });
   ColdestSetpointWinningVoteErdShouldBe(Erd_FreezerSetpoint_UserVote);
}
