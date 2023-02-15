/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "PercentageDutyCycleVoteToPwmDutyCycleConverter.h"
#include "PercentageDutyCycleVote.h"
#include "PwmDutyCycle.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "DataModel_TestDouble.h"
#include "uassert_test.h"

enum
{
   Erd_PercentageDutyCycleVote,
   Erd_PwmDutyCycle,

   SomeDutyCycle = 25,
   SomeOtherDutyCycle = 60,
};

static const DataModel_TestDoubleConfigurationEntry_t erds[] = {
   { Erd_PercentageDutyCycleVote, sizeof(PercentageDutyCycleVote_t) },
   { Erd_PwmDutyCycle, sizeof(PwmDutyCycle_t) }
};

static const PercentageDutyCycleVoteToPwmDutyCycleConverterConfig_t config = {
   .inputPercentageDutyCycleVoteErd = Erd_PercentageDutyCycleVote,
   .outputPwmDutyCycleErd = Erd_PwmDutyCycle,
};

TEST_GROUP(PercentageDutyCycleVoteToPwmDutyCycleConverter)
{
   DataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;
   PercentageDutyCycleVoteToPwmDutyCycleConverter_t instance;

   void setup()
   {
      DataModel_TestDouble_Init(&dataModelTestDouble, erds, NUM_ELEMENTS(erds));
      dataModel = dataModelTestDouble.dataModel;
   }

   void GivenInitialization()
   {
      PercentageDutyCycleVoteToPwmDutyCycleConverter_Init(
         &instance,
         dataModel,
         &config);
   }

   void WhenThePercentageDutyCycleIs(PercentageDutyCycle_t percentage)
   {
      PercentageDutyCycleVote_t votedPercentageDutyCycle = {
         .percentageDutyCycle = percentage,
         .care = Vote_Care
      };

      DataModel_Write(dataModel, Erd_PercentageDutyCycleVote, &votedPercentageDutyCycle);
   }

   void WhenTheDontCarePercentageDutyCycleIs(PercentageDutyCycle_t percentage)
   {
      PercentageDutyCycleVote_t votedPercentageDutyCycle = {
         .percentageDutyCycle = percentage,
         .care = Vote_DontCare
      };

      DataModel_Write(dataModel, Erd_PercentageDutyCycleVote, &votedPercentageDutyCycle);
   }

   void TheConvertedPwmDutyCycleShouldBe(PwmDutyCycle_t expected)
   {
      PwmDutyCycle_t actual;
      DataModel_Read(dataModel, Erd_PwmDutyCycle, &actual);
      CHECK_EQUAL(expected, actual);
   }

   PwmDutyCycle_t PercentageDutyCycleAsPwmDutyCycle(PwmDutyCycle_t dutyCycle)
   {
      return (PwmDutyCycle_Max * dutyCycle) / 100;
   }
};

TEST(PercentageDutyCycleVoteToPwmDutyCycleConverter, ShouldConvertPercentageDutyCycleToPwmDutyCycleWhenPercentageChanges)
{
   GivenInitialization();

   for(uint8_t percentage = 0; percentage <= 100; percentage++)
   {
      WhenThePercentageDutyCycleIs(percentage);
      TheConvertedPwmDutyCycleShouldBe(PercentageDutyCycleAsPwmDutyCycle(percentage));
   }
}

TEST(PercentageDutyCycleVoteToPwmDutyCycleConverter, ShouldNotConvertPercentageDutyCycleToPwmDutyCycleWhenDutyCycleVoteDoesNotCare)
{
   GivenInitialization();

   WhenThePercentageDutyCycleIs(SomeDutyCycle);
   TheConvertedPwmDutyCycleShouldBe(PercentageDutyCycleAsPwmDutyCycle(SomeDutyCycle));

   WhenTheDontCarePercentageDutyCycleIs(SomeOtherDutyCycle);
   TheConvertedPwmDutyCycleShouldBe(PercentageDutyCycleAsPwmDutyCycle(SomeDutyCycle));

   WhenThePercentageDutyCycleIs(SomeOtherDutyCycle);
   TheConvertedPwmDutyCycleShouldBe(PercentageDutyCycleAsPwmDutyCycle(SomeOtherDutyCycle));
}
