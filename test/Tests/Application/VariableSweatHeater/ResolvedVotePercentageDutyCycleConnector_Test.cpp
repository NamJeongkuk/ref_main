/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "ResolvedVotePercentageDutyCycleConnector.h"
#include "SystemErds.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"
#include "Vote.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "DataModel_TestDouble.h"
#include "uassert_test.h"

enum
{
   SomePercentageDutyCycle = 85
};

enum
{
   Erd_SomePercentageDutyCycle_ResolvedVote,
   Erd_SomePercentageDutyCycle_MappedErd
};

static const ResolvedVotePercentageDutyCycleConnectorConfiguration_t config = {
   .resolvedPercentageDutyCycleVoteErd = Erd_SomePercentageDutyCycle_ResolvedVote,
   .percentageDutyCycleOutputErd = Erd_SomePercentageDutyCycle_MappedErd
};

static const DataModel_TestDoubleConfigurationEntry_t dataModelTestDoubleConfigurationTable[] = {
   { Erd_SomePercentageDutyCycle_ResolvedVote, sizeof(PercentageDutyCycleVote_t) },
   { Erd_SomePercentageDutyCycle_MappedErd, sizeof(PercentageDutyCycle_t) }
};

TEST_GROUP(ResolvedVotePercentageDutyCycleConnector)
{
   DataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;
   ResolvedVotePercentageDutyCycleConnector_t instance;

   void setup()
   {
      DataModel_TestDouble_Init(
         &dataModelTestDouble,
         dataModelTestDoubleConfigurationTable,
         NUM_ELEMENTS(dataModelTestDoubleConfigurationTable));

      dataModel = dataModelTestDouble.dataModel;
   }

   void GivenResolvedVotePercentageDutyCycleConnectorIsInitialized()
   {
      ResolvedVotePercentageDutyCycleConnector_Init(&instance, dataModel, &config);
   }

   void WhenThePercentageDutyCycleResolvedVoteIs(PercentageDutyCycle_t dutyCycle)
   {
      PercentageDutyCycleVote_t percentageDutyCycleVote;
      percentageDutyCycleVote.percentageDutyCycle = dutyCycle;
      percentageDutyCycleVote.care = true;

      DataModel_Write(
         dataModel,
         Erd_SomePercentageDutyCycle_ResolvedVote,
         &percentageDutyCycleVote);
   }

   void GivenThePercentageDutyCycleResolvedVoteIs(PercentageDutyCycle_t dutyCycle)
   {
      WhenThePercentageDutyCycleResolvedVoteIs(dutyCycle);
   }

   void TheMappedPercentageDutyCycleShouldBe(PercentageDutyCycle_t expectedDutyCycle)
   {
      PercentageDutyCycle_t actualDutyCycle;
      DataModel_Read(dataModel, Erd_SomePercentageDutyCycle_MappedErd, &actualDutyCycle);

      CHECK_EQUAL(expectedDutyCycle, actualDutyCycle);
   }
};

TEST(ResolvedVotePercentageDutyCycleConnector, ShouldInitialize)
{
   GivenResolvedVotePercentageDutyCycleConnectorIsInitialized();
}

TEST(ResolvedVotePercentageDutyCycleConnector, ShouldTurnOnAtSomePercentageDutyCycleWhenResolvedVoteIsSomePercentageDutyCycle)
{
   GivenResolvedVotePercentageDutyCycleConnectorIsInitialized();

   WhenThePercentageDutyCycleResolvedVoteIs(SomePercentageDutyCycle);
   TheMappedPercentageDutyCycleShouldBe(SomePercentageDutyCycle);
}

TEST(ResolvedVotePercentageDutyCycleConnector, ShouldTurnOffAtMinPercentageDutyCycleWhenResolvedVoteIsMinPercentageDutyCycle)
{
   GivenResolvedVotePercentageDutyCycleConnectorIsInitialized();

   WhenThePercentageDutyCycleResolvedVoteIs(PercentageDutyCycle_Min);
   TheMappedPercentageDutyCycleShouldBe(PercentageDutyCycle_Min);
}

TEST(ResolvedVotePercentageDutyCycleConnector, ShouldTurnOffAtMinPercentageDutyCycleWhenResolvedVoteIsMinPercentageDutyCycleAfterBeingOn)
{
   GivenResolvedVotePercentageDutyCycleConnectorIsInitialized();

   WhenThePercentageDutyCycleResolvedVoteIs(SomePercentageDutyCycle);
   TheMappedPercentageDutyCycleShouldBe(SomePercentageDutyCycle);

   WhenThePercentageDutyCycleResolvedVoteIs(PercentageDutyCycle_Min);
   TheMappedPercentageDutyCycleShouldBe(PercentageDutyCycle_Min);
}

TEST(ResolvedVotePercentageDutyCycleConnector, ShouldTurnOnAtSomePercentageDutyCycleWhenResolvedVoteIsSomePercentageDutyCycleAfterBeingOff)
{
   GivenResolvedVotePercentageDutyCycleConnectorIsInitialized();

   WhenThePercentageDutyCycleResolvedVoteIs(PercentageDutyCycle_Min);
   TheMappedPercentageDutyCycleShouldBe(PercentageDutyCycle_Min);

   WhenThePercentageDutyCycleResolvedVoteIs(SomePercentageDutyCycle);
   TheMappedPercentageDutyCycleShouldBe(SomePercentageDutyCycle);
}

TEST(ResolvedVotePercentageDutyCycleConnector, ShouldTurnOffAtMinPercentageDutyCycleDuringInitializationIfPercentageDutyCycleVoteIsMinPercentageDutyCycle)
{
   GivenThePercentageDutyCycleResolvedVoteIs(PercentageDutyCycle_Min);
   GivenResolvedVotePercentageDutyCycleConnectorIsInitialized();

   TheMappedPercentageDutyCycleShouldBe(PercentageDutyCycle_Min);
}

TEST(ResolvedVotePercentageDutyCycleConnector, ShouldTurnOnAtSomePercentageDutyCycleDuringInitializationIfPercentageDutyCycleVoteIsSomePercentageDutyCycle)
{
   GivenThePercentageDutyCycleResolvedVoteIs(SomePercentageDutyCycle);
   GivenResolvedVotePercentageDutyCycleConnectorIsInitialized();

   TheMappedPercentageDutyCycleShouldBe(SomePercentageDutyCycle);
}
