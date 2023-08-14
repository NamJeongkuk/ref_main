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
#include "Constants_Binary.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "uassert_test.h"
#include "ReferDataModel_TestDouble.h"
#include "TddPersonality.h"

TEST_GROUP(SetpointIntegration)
{
   Application_t instance;
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   ResetReason_t resetReason;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble, TddPersonality_DevelopmentSingleSpeedCompressorGridIntegration);
      dataModel = dataModelDouble.dataModel;
   }

   void GivenApplicationHasBeenInitialized()
   {
      Application_Init(&instance, dataModel, resetReason);
   }

   void GivenVoteIs(Erd_t erd, SetpointVotedTemperature_t vote)
   {
      DataModel_Write(dataModel, erd, &vote);
   }

   void WhenVoteIs(Erd_t erd, SetpointVotedTemperature_t vote)
   {
      GivenVoteIs(erd, vote);
   }

   void ColdestFreezerSetpointVoteShouldBe(SetpointVotedTemperature_t expected)
   {
      SetpointVotedTemperature_t actual;
      DataModel_Read(dataModel, Erd_FreezerSetpoint_ColdestSetpointVote, &actual);

      CHECK_EQUAL(expected.temperatureInDegFx100, actual.temperatureInDegFx100);
      CHECK_EQUAL(expected.care, actual.care);
   }

   void ColdestFreezerSetpointWinningVoteErdShouldBe(Erd_t expected)
   {
      Erd_t actual;
      DataModel_Read(dataModel, Erd_FreezerSetpoint_ColdestSetpointWinningVote, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void FreezerSetpointResolvedVoteShouldBe(SetpointVotedTemperature_t expected)
   {
      SetpointVotedTemperature_t actual;
      DataModel_Read(dataModel, Erd_FreezerSetpoint_ResolvedVote, &actual);

      CHECK_EQUAL(expected.temperatureInDegFx100, actual.temperatureInDegFx100);
      CHECK_EQUAL(expected.care, actual.care);
   }

   void FreezerSetpointWinningVoteErdShouldBe(Erd_t expected)
   {
      Erd_t actual;
      DataModel_Read(dataModel, Erd_FreezerSetpoint_WinningVoteErd, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void ColdestFreshFoodSetpointVoteShouldBe(SetpointVotedTemperature_t expected)
   {
      SetpointVotedTemperature_t actual;
      DataModel_Read(dataModel, Erd_FreshFoodSetpoint_ColdestSetpointVote, &actual);

      CHECK_EQUAL(expected.temperatureInDegFx100, actual.temperatureInDegFx100);
      CHECK_EQUAL(expected.care, actual.care);
   }

   void ColdestFreshFoodSetpointWinningVoteErdShouldBe(Erd_t expected)
   {
      Erd_t actual;
      DataModel_Read(dataModel, Erd_FreshFoodSetpoint_ColdestSetpointWinningVote, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void FreshFoodSetpointResolvedVoteShouldBe(SetpointVotedTemperature_t expected)
   {
      SetpointVotedTemperature_t actual;
      DataModel_Read(dataModel, Erd_FreshFoodSetpoint_ResolvedVote, &actual);

      CHECK_EQUAL(expected.temperatureInDegFx100, actual.temperatureInDegFx100);
      CHECK_EQUAL(expected.care, actual.care);
   }

   void FreshFoodSetpointWinningVoteErdShouldBe(Erd_t expected)
   {
      Erd_t actual;
      DataModel_Read(dataModel, Erd_FreshFoodSetpoint_WinningVoteErd, &actual);

      CHECK_EQUAL(expected, actual);
   }
};

TEST(SetpointIntegration, ShouldSetFreezerResolvedVoteToEnhancedSabbathVoteIfEnhancedSabbathVotesCare)
{
   GivenApplicationHasBeenInitialized();
   GivenVoteIs(Erd_FreezerSetpoint_TurboFreezeVote, { 300, Vote_Care });
   GivenVoteIs(Erd_FreezerSetpoint_IceInDoorVote, { 300, Vote_Care });
   GivenVoteIs(Erd_FreezerSetpoint_FreezerIceMakerVote, { 300, Vote_Care });
   GivenVoteIs(Erd_FreezerSetpoint_FreezerIceRateVote, { 300, Vote_Care });
   GivenVoteIs(Erd_FreezerSetpoint_UserVote, { 300, Vote_Care });
   GivenVoteIs(Erd_FreezerSetpoint_EnhancedSabbathVote, { 800, Vote_Care });
   GivenVoteIs(Erd_FreezerSetpoint_FactoryVote, { -250, Vote_DontCare });

   ColdestFreezerSetpointVoteShouldBe({ 300, Vote_Care });
   ColdestFreezerSetpointWinningVoteErdShouldBe(Erd_FreezerSetpoint_TurboFreezeVote);

   FreezerSetpointResolvedVoteShouldBe({ 800, Vote_Care });
   FreezerSetpointWinningVoteErdShouldBe(Erd_FreezerSetpoint_EnhancedSabbathVote);
}

TEST(SetpointIntegration, ShouldSetFreezerResolvedVoteToColdestSetpointVoteIfEnhancedSabbathVotesDontCare)
{
   GivenApplicationHasBeenInitialized();
   GivenVoteIs(Erd_FreezerSetpoint_TurboFreezeVote, { 250, Vote_Care });
   GivenVoteIs(Erd_FreezerSetpoint_IceInDoorVote, { 200, Vote_Care });
   GivenVoteIs(Erd_FreezerSetpoint_FreezerIceMakerVote, { 100, Vote_Care });
   GivenVoteIs(Erd_FreezerSetpoint_FreezerIceRateVote, { -50, Vote_Care });
   GivenVoteIs(Erd_FreezerSetpoint_UserVote, { -100, Vote_Care });
   GivenVoteIs(Erd_FreezerSetpoint_EnhancedSabbathVote, { -250, Vote_DontCare });
   GivenVoteIs(Erd_FreezerSetpoint_FactoryVote, { -250, Vote_DontCare });

   ColdestFreezerSetpointVoteShouldBe({ -100, Vote_Care });
   ColdestFreezerSetpointWinningVoteErdShouldBe(Erd_FreezerSetpoint_UserVote);

   FreezerSetpointResolvedVoteShouldBe({ -100, Vote_Care });
   FreezerSetpointWinningVoteErdShouldBe(Erd_FreezerSetpoint_ColdestSetpointVote);
}

TEST(SetpointIntegration, ShouldSetFreezerResolvedVoteToEnhancedSabbathVoteWhenEnhancedSabbathVotesCare)
{
   GivenApplicationHasBeenInitialized();
   GivenVoteIs(Erd_FreezerSetpoint_TurboFreezeVote, { 250, Vote_Care });
   GivenVoteIs(Erd_FreezerSetpoint_IceInDoorVote, { 200, Vote_Care });
   GivenVoteIs(Erd_FreezerSetpoint_FreezerIceMakerVote, { 100, Vote_Care });
   GivenVoteIs(Erd_FreezerSetpoint_FreezerIceRateVote, { -50, Vote_Care });
   GivenVoteIs(Erd_FreezerSetpoint_UserVote, { -100, Vote_Care });
   GivenVoteIs(Erd_FreezerSetpoint_EnhancedSabbathVote, { -250, Vote_DontCare });
   GivenVoteIs(Erd_FreezerSetpoint_FactoryVote, { -250, Vote_DontCare });

   FreezerSetpointResolvedVoteShouldBe({ -100, Vote_Care });
   FreezerSetpointWinningVoteErdShouldBe(Erd_FreezerSetpoint_ColdestSetpointVote);

   WhenVoteIs(Erd_FreezerSetpoint_EnhancedSabbathVote, { -250, Vote_Care });
   FreezerSetpointResolvedVoteShouldBe({ -250, Vote_Care });
   FreezerSetpointWinningVoteErdShouldBe(Erd_FreezerSetpoint_EnhancedSabbathVote);
}

TEST(SetpointIntegration, ShouldSetFreezerResolvedVoteToColdestSetpointVoteWhenEnhancedSabbathVotesDontCare)
{
   GivenApplicationHasBeenInitialized();
   GivenVoteIs(Erd_FreezerSetpoint_TurboFreezeVote, { 250, Vote_Care });
   GivenVoteIs(Erd_FreezerSetpoint_IceInDoorVote, { 200, Vote_Care });
   GivenVoteIs(Erd_FreezerSetpoint_FreezerIceMakerVote, { 100, Vote_Care });
   GivenVoteIs(Erd_FreezerSetpoint_FreezerIceRateVote, { -50, Vote_Care });
   GivenVoteIs(Erd_FreezerSetpoint_UserVote, { -100, Vote_Care });
   GivenVoteIs(Erd_FreezerSetpoint_EnhancedSabbathVote, { -250, Vote_Care });
   GivenVoteIs(Erd_FreezerSetpoint_FactoryVote, { -250, Vote_DontCare });

   FreezerSetpointResolvedVoteShouldBe({ -250, Vote_Care });
   FreezerSetpointWinningVoteErdShouldBe(Erd_FreezerSetpoint_EnhancedSabbathVote);

   WhenVoteIs(Erd_FreezerSetpoint_EnhancedSabbathVote, { -250, Vote_DontCare });
   FreezerSetpointResolvedVoteShouldBe({ -100, Vote_Care });
   FreezerSetpointWinningVoteErdShouldBe(Erd_FreezerSetpoint_ColdestSetpointVote);
}

TEST(SetpointIntegration, ShouldSetFreezerResolvedVoteToFactoryVoteIfItVotesCare)
{
   GivenApplicationHasBeenInitialized();
   GivenVoteIs(Erd_FreezerSetpoint_TurboFreezeVote, { 250, Vote_Care });
   GivenVoteIs(Erd_FreezerSetpoint_IceInDoorVote, { 200, Vote_Care });
   GivenVoteIs(Erd_FreezerSetpoint_FreezerIceMakerVote, { 100, Vote_Care });
   GivenVoteIs(Erd_FreezerSetpoint_FreezerIceRateVote, { -50, Vote_Care });
   GivenVoteIs(Erd_FreezerSetpoint_UserVote, { -100, Vote_Care });
   GivenVoteIs(Erd_FreezerSetpoint_EnhancedSabbathVote, { -275, Vote_Care });
   GivenVoteIs(Erd_FreezerSetpoint_FactoryVote, { -250, Vote_Care });

   FreezerSetpointResolvedVoteShouldBe({ -250, Vote_Care });
   FreezerSetpointWinningVoteErdShouldBe(Erd_FreezerSetpoint_FactoryVote);
}

TEST(SetpointIntegration, ShouldSetFreshFoodResolvedVoteToEnhancedSabbathVoteIfEnhancedSabbathVotesCare)
{
   GivenApplicationHasBeenInitialized();
   GivenVoteIs(Erd_FreshFoodSetpoint_TurboCoolVote, { 300, Vote_Care });
   GivenVoteIs(Erd_FreshFoodSetpoint_UserVote, { 300, Vote_Care });
   GivenVoteIs(Erd_FreshFoodSetpoint_EnhancedSabbathVote, { 800, Vote_Care });
   GivenVoteIs(Erd_FreshFoodSetpoint_FactoryVote, { -250, Vote_DontCare });

   ColdestFreshFoodSetpointVoteShouldBe({ 300, Vote_Care });
   ColdestFreshFoodSetpointWinningVoteErdShouldBe(Erd_FreshFoodSetpoint_TurboCoolVote);

   FreshFoodSetpointResolvedVoteShouldBe({ 800, Vote_Care });
   FreshFoodSetpointWinningVoteErdShouldBe(Erd_FreshFoodSetpoint_EnhancedSabbathVote);
}

TEST(SetpointIntegration, ShouldSetFreshFoodResolvedVoteToColdestSetpointVoteIfEnhancedSabbathVotesDontCare)
{
   GivenApplicationHasBeenInitialized();
   GivenVoteIs(Erd_FreshFoodSetpoint_TurboCoolVote, { 250, Vote_Care });
   GivenVoteIs(Erd_FreshFoodSetpoint_UserVote, { -100, Vote_Care });
   GivenVoteIs(Erd_FreshFoodSetpoint_EnhancedSabbathVote, { -250, Vote_DontCare });
   GivenVoteIs(Erd_FreshFoodSetpoint_FactoryVote, { -250, Vote_DontCare });

   ColdestFreshFoodSetpointVoteShouldBe({ -100, Vote_Care });
   ColdestFreshFoodSetpointWinningVoteErdShouldBe(Erd_FreshFoodSetpoint_UserVote);

   FreshFoodSetpointResolvedVoteShouldBe({ -100, Vote_Care });
   FreshFoodSetpointWinningVoteErdShouldBe(Erd_FreshFoodSetpoint_ColdestSetpointVote);
}

TEST(SetpointIntegration, ShouldSetFreshFoodResolvedVoteToColdestSetpointVoteWhenEnhancedSabbathVotesCare)
{
   GivenApplicationHasBeenInitialized();
   GivenVoteIs(Erd_FreshFoodSetpoint_TurboCoolVote, { 250, Vote_Care });
   GivenVoteIs(Erd_FreshFoodSetpoint_UserVote, { -100, Vote_Care });
   GivenVoteIs(Erd_FreshFoodSetpoint_EnhancedSabbathVote, { -250, Vote_DontCare });
   GivenVoteIs(Erd_FreshFoodSetpoint_FactoryVote, { -250, Vote_DontCare });

   FreshFoodSetpointResolvedVoteShouldBe({ -100, Vote_Care });
   FreshFoodSetpointWinningVoteErdShouldBe(Erd_FreshFoodSetpoint_ColdestSetpointVote);

   WhenVoteIs(Erd_FreshFoodSetpoint_EnhancedSabbathVote, { -250, Vote_Care });
   FreshFoodSetpointResolvedVoteShouldBe({ -250, Vote_Care });
   FreshFoodSetpointWinningVoteErdShouldBe(Erd_FreshFoodSetpoint_EnhancedSabbathVote);
}

TEST(SetpointIntegration, ShouldSetFreshFoodResolvedVoteToColdestSetpointVoteWhenEnhancedSabbathVotesDontCare)
{
   GivenApplicationHasBeenInitialized();
   GivenVoteIs(Erd_FreshFoodSetpoint_TurboCoolVote, { 250, Vote_Care });
   GivenVoteIs(Erd_FreshFoodSetpoint_UserVote, { -100, Vote_Care });
   GivenVoteIs(Erd_FreshFoodSetpoint_EnhancedSabbathVote, { -250, Vote_Care });
   GivenVoteIs(Erd_FreshFoodSetpoint_FactoryVote, { -250, Vote_DontCare });

   FreshFoodSetpointResolvedVoteShouldBe({ -250, Vote_Care });
   FreshFoodSetpointWinningVoteErdShouldBe(Erd_FreshFoodSetpoint_EnhancedSabbathVote);

   WhenVoteIs(Erd_FreshFoodSetpoint_EnhancedSabbathVote, { -250, Vote_DontCare });
   FreshFoodSetpointResolvedVoteShouldBe({ -100, Vote_Care });
   FreshFoodSetpointWinningVoteErdShouldBe(Erd_FreshFoodSetpoint_ColdestSetpointVote);
}

TEST(SetpointIntegration, ShouldSetFreshFoodResolvedVoteToFactoryVoteIfItVotesCare)
{
   GivenApplicationHasBeenInitialized();
   GivenVoteIs(Erd_FreshFoodSetpoint_TurboCoolVote, { 250, Vote_Care });
   GivenVoteIs(Erd_FreshFoodSetpoint_UserVote, { -100, Vote_Care });
   GivenVoteIs(Erd_FreshFoodSetpoint_EnhancedSabbathVote, { -275, Vote_Care });
   GivenVoteIs(Erd_FreshFoodSetpoint_FactoryVote, { -250, Vote_Care });

   FreshFoodSetpointResolvedVoteShouldBe({ -250, Vote_Care });
   FreshFoodSetpointWinningVoteErdShouldBe(Erd_FreshFoodSetpoint_FactoryVote);
}
