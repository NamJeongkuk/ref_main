/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "DutyCycleVotePercentageToPwmDutyCycleConverter.h"
#include "DutyCyclePercentageVote.h"
#include "PwmDutyCycle.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "DataModel_TestDouble.h"
#include "uassert_test.h"

enum
{
   Erd_DutyCyclePercentageVote,
   Erd_PwmDutyCycle,

   SomeDutyCycle = 25,
   SomeOtherDutyCycle = 60,
};

static const DataModel_TestDoubleConfigurationEntry_t erds[] = {
   { Erd_DutyCyclePercentageVote, sizeof(DutyCyclePercentageVote_t) },
   { Erd_PwmDutyCycle, sizeof(PwmDutyCycle_t) }
};

static const DutyCycleVotePercentageToPwmDutyCycleConverterConfig_t config = {
   .inputDutyCyclePercentageVoteErd = Erd_DutyCyclePercentageVote,
   .outputPwmDutyCycleErd = Erd_PwmDutyCycle,
};

TEST_GROUP(DutyCycleVotePercentageToPwmDutyCycleConverter)
{
   DataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;
   DutyCycleVotePercentageToPwmDutyCycleConverter_t instance;

   void setup()
   {
      DataModel_TestDouble_Init(&dataModelTestDouble, erds, NUM_ELEMENTS(erds));
      dataModel = dataModelTestDouble.dataModel;
   }

   void GivenInitialization()
   {
      DutyCycleVotePercentageToPwmDutyCycleConverter_Init(
         &instance,
         dataModel,
         &config);
   }

   void WhenTheDutyCyclePercentageIs(DutyCyclePercentage_t percentage)
   {
      DutyCyclePercentageVote_t votedDutyCyclePercentage = {
         .dutyCyclePercentage = percentage,
         .care = Vote_Care
      };

      DataModel_Write(dataModel, Erd_DutyCyclePercentageVote, &votedDutyCyclePercentage);
   }

   void WhenTheDontCareDutyCyclePercentageIs(DutyCyclePercentage_t percentage)
   {
      DutyCyclePercentageVote_t votedDutyCyclePercentage = {
         .dutyCyclePercentage = percentage,
         .care = Vote_DontCare
      };

      DataModel_Write(dataModel, Erd_DutyCyclePercentageVote, &votedDutyCyclePercentage);
   }

   void TheConvertedPwmDutyCycleShouldBe(PwmDutyCycle_t expected)
   {
      PwmDutyCycle_t actual;
      DataModel_Read(dataModel, Erd_PwmDutyCycle, &actual);
      CHECK_EQUAL(expected, actual);
   }

   PwmDutyCycle_t DutyCyclePercentageAsPwmDutyCycle(PwmDutyCycle_t dutyCycle)
   {
      return (PwmDutyCycle_Max * dutyCycle) / 100;
   }
};

TEST(DutyCycleVotePercentageToPwmDutyCycleConverter, ShouldConvertDutyCyclePercentageToPwmDutyCycleWhenPercentageChanges)
{
   GivenInitialization();

   for(uint8_t percentage = 0; percentage <= 100; percentage++)
   {
      WhenTheDutyCyclePercentageIs(percentage);
      TheConvertedPwmDutyCycleShouldBe(DutyCyclePercentageAsPwmDutyCycle(percentage));
   }
}

TEST(DutyCycleVotePercentageToPwmDutyCycleConverter, ShouldNotConvertDutyCyclePercentageToPwmDutyCycleWhenDutyCycleVoteDoesNotCare)
{
   GivenInitialization();

   WhenTheDutyCyclePercentageIs(SomeDutyCycle);
   TheConvertedPwmDutyCycleShouldBe(DutyCyclePercentageAsPwmDutyCycle(SomeDutyCycle));

   WhenTheDontCareDutyCyclePercentageIs(SomeOtherDutyCycle);
   TheConvertedPwmDutyCycleShouldBe(DutyCyclePercentageAsPwmDutyCycle(SomeDutyCycle));

   WhenTheDutyCyclePercentageIs(SomeOtherDutyCycle);
   TheConvertedPwmDutyCycleShouldBe(DutyCyclePercentageAsPwmDutyCycle(SomeOtherDutyCycle));
}
