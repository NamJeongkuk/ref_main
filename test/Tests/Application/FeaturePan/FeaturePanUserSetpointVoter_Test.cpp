/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "FeaturePanUserSetpointVoter.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"

static const Erd_t resolvedModeSetpointErds[] = {
   Erd_FeaturePanMode1_Status,
   Erd_FeaturePanMode2_Status,
   Erd_FeaturePanMode3_Status,
   Erd_FeaturePanMode4_Status,
   Erd_FeaturePanMode5_Status,
   Erd_FeaturePanMode6_Status,
   Erd_FeaturePanMode7_Status
};

static const FeaturePanUserSetpointVoterConfiguration_t config = {
   .featurePanCurrentModeErd = Erd_FeaturePanCurrentMode,
   .userSetpointVoteErd = Erd_FeaturePanSetpoint_UserVote,
   .resolvedModeSetpointErds = resolvedModeSetpointErds,
   .numberOfErds = NUM_ELEMENTS(resolvedModeSetpointErds)
};

TEST_GROUP(FeaturePanUserSetpointVoter)
{
   ReferDataModel_TestDouble_t referDataModelTestDouble;
   I_DataModel_t *dataModel;

   FeaturePanUserSetpointVoter_t instance;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&referDataModelTestDouble);
      dataModel = referDataModelTestDouble.dataModel;
   }

   void TheModuleIsInitialized()
   {
      FeaturePanUserSetpointVoter_Init(&instance, dataModel, &config);
   }

   void GivenAllFeaturePanModeSetpointStatusErdsAreWrittenWithSomeData(void)
   {
      for(uint8_t i = 0; i < NUM_ELEMENTS(resolvedModeSetpointErds); i++)
      {
         TemperatureDegFx100_t someTemp = (TemperatureDegFx100_t)i;
         DataModel_Write(
            dataModel,
            resolvedModeSetpointErds[i],
            &someTemp);
      }
   }

   void WhenTheFeaturePanModeErdChangesTo(FeaturePanCurrentMode_t mode)
   {
      DataModel_Write(dataModel, Erd_FeaturePanCurrentMode, &mode);
   }

   void TheUserSetpointVoteErdShouldBe(TemperatureDegFx100_t temperature)
   {
      SetpointVotedTemperature_t expected = { .temperatureInDegFx100 = temperature, .care = Vote_Care };
      SetpointVotedTemperature_t actual;

      DataModel_Read(
         dataModel,
         Erd_FeaturePanSetpoint_UserVote,
         &actual);

      CHECK_EQUAL(expected.temperatureInDegFx100, actual.temperatureInDegFx100);
      CHECK_EQUAL(expected.care, actual.care);
   }
};

TEST(FeaturePanUserSetpointVoter, ShouldVoteForTheCurrentModeUserSetpointWhenModeChangesToValidMode)
{
   GivenAllFeaturePanModeSetpointStatusErdsAreWrittenWithSomeData();
   WhenTheFeaturePanModeErdChangesTo(FeaturePanCurrentMode_Mode5);
   TheModuleIsInitialized();
   TheUserSetpointVoteErdShouldBe(4);

   WhenTheFeaturePanModeErdChangesTo(FeaturePanCurrentMode_Mode1);
   TheUserSetpointVoteErdShouldBe(0);

   WhenTheFeaturePanModeErdChangesTo(FeaturePanCurrentMode_Mode2);
   TheUserSetpointVoteErdShouldBe(1);

   WhenTheFeaturePanModeErdChangesTo(FeaturePanCurrentMode_Mode3);
   TheUserSetpointVoteErdShouldBe(2);

   WhenTheFeaturePanModeErdChangesTo(FeaturePanCurrentMode_Mode4);
   TheUserSetpointVoteErdShouldBe(3);

   WhenTheFeaturePanModeErdChangesTo(FeaturePanCurrentMode_Mode5);
   TheUserSetpointVoteErdShouldBe(4);

   WhenTheFeaturePanModeErdChangesTo(FeaturePanCurrentMode_Mode6);
   TheUserSetpointVoteErdShouldBe(5);

   WhenTheFeaturePanModeErdChangesTo(FeaturePanCurrentMode_Mode7);
   TheUserSetpointVoteErdShouldBe(6);

   WhenTheFeaturePanModeErdChangesTo(FeaturePanCurrentMode_Mode1 - 1);
   TheUserSetpointVoteErdShouldBe(0);

   WhenTheFeaturePanModeErdChangesTo(FeaturePanCurrentMode_Mode7 + 1);
   TheUserSetpointVoteErdShouldBe(6);
}
