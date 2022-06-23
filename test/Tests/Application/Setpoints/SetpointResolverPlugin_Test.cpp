/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "SetpointResolverPlugin.h"
#include "SystemErds.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "uassert_test.h"
#include "ReferDataModel_TestDouble.h"

#define Given
#define When

static const SetpointVotedTemperature_t careData = {
   .temperature = 100,
   .care = true
};

static const SetpointVotedTemperature_t dontCareData = {
   .temperature = 100,
   .care = false
};

TEST_GROUP(ErdResolverPlugin)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
   }

   void SetErdToCare(Erd_t erd)
   {
      DataModel_Write(
         dataModel,
         erd,
         &careData);
   }

   void SetErdToNotCare(Erd_t erd)
   {
      DataModel_Write(
         dataModel,
         erd,
         &dontCareData);
   }

   void WinningVoteShouldBe(Erd_t expected)
   {
      Erd_t actual;
      DataModel_Read(
         dataModel,
         Erd_FreshFoodSetpoint_WinningVoteErd,
         &actual);

      CHECK_EQUAL(actual, expected);
   }

   void ResolvedVoteCareShouldBe(bool expected)
   {
      SetpointVotedTemperature_t actual;
      DataModel_Read(
         dataModel,
         Erd_FreshFoodSetpoint_ResolvedVote,
         &actual);

      CHECK_EQUAL(actual.care, expected);
   }

   void ResolvedVoteTemperatureShouldBe(TemperatureDegFx100_t expected)
   {
      SetpointVotedTemperature_t actual;
      DataModel_Read(
         dataModel,
         Erd_FreshFoodSetpoint_ResolvedVote,
         &actual);

      CHECK_EQUAL(actual.temperature, expected);
   }

   void TheModuleIsInitialized()
   {
      SetpointResolverPlugin_Init(dataModel);
   }
};

TEST(ErdResolverPlugin, ShouldInit)
{
   Given TheModuleIsInitialized();
}

TEST(ErdResolverPlugin, ShouldChangeWinningVoteToErdFreshFoodSetpointTurboCoolWhenErdFreshFoodSetpointTurboCoolIsSetToCare)
{
   Given TheModuleIsInitialized();

   When SetErdToCare(Erd_FreshFoodSetpoint_TurboCool);
   WinningVoteShouldBe(Erd_FreshFoodSetpoint_TurboCool);
}

TEST(ErdResolverPlugin, ShouldChangeTheWinningVoteWhenAHigherPriorityErdIsSetToCare)
{
   Given TheModuleIsInitialized();

   SetErdToCare(Erd_FreshFoodSetpoint_TurboCool);

   When SetErdToCare(Erd_FreshFoodSetpoint_FactoryVote);
   WinningVoteShouldBe(Erd_FreshFoodSetpoint_FactoryVote);
}

TEST(ErdResolverPlugin, ShouldNotChangeTheWinningVoteWhenALowerPriorityErdIsSetToCare)
{
   Given TheModuleIsInitialized();

   SetErdToCare(Erd_FreshFoodSetpoint_TurboCool);

   When SetErdToCare(Erd_FreshFoodSetpoint_UserSetpoint);
   WinningVoteShouldBe(Erd_FreshFoodSetpoint_TurboCool);
}

TEST(ErdResolverPlugin, ShouldSetResolvedVoteToNotCareOnInit)
{
   Given TheModuleIsInitialized();

   ResolvedVoteCareShouldBe(false);
}

TEST(ErdResolverPlugin, ShouldSetResolvedVoteToNotCareWhenThereAreNoVotesThatCare)
{
   Given TheModuleIsInitialized();

   SetErdToNotCare(Erd_FreshFoodSetpoint_TurboCool);

   ResolvedVoteCareShouldBe(false);
}

TEST(ErdResolverPlugin, ShouldSetResolvedVoteTemperatureToTheValueOfErdFreshFoodSetpointTurboCoolWhenItIsSetToCare)
{
   Given TheModuleIsInitialized();

   SetErdToCare(Erd_FreshFoodSetpoint_TurboCool);

   ResolvedVoteCareShouldBe(careData.temperature);
}
