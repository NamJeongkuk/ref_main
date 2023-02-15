/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "ResolvedVoteRelayConnector.h"
#include "SystemErds.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"
#include "Vote.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "DataModel_TestDouble.h"
#include "uassert_test.h"

#define Given
#define When
#define Then
#define And
#define The

enum
{
   Erd_SomeRelay_ResolvedVote,
   Erd_SomeRelay_MappedErd
};

static const ResolvedVoteRelayConnectorConfiguration_t config = {
   .resolvedRelayVoteErd = Erd_SomeRelay_ResolvedVote,
   .relayOutputErd = Erd_SomeRelay_MappedErd
};

static const DataModel_TestDoubleConfigurationEntry_t dataModelTestDoubleConfigurationTable[] = {
   { Erd_SomeRelay_ResolvedVote, sizeof(RelayVotedState_t) },
   { Erd_SomeRelay_MappedErd, sizeof(bool) }
};

TEST_GROUP(ResolvedVoteRelayConnector)
{
   DataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;
   ResolvedVoteRelayConnector_t instance;

   void setup()
   {
      DataModel_TestDouble_Init(
         &dataModelTestDouble,
         dataModelTestDoubleConfigurationTable,
         NUM_ELEMENTS(dataModelTestDoubleConfigurationTable));

      dataModel = dataModelTestDouble.dataModel;
   }

   void ResolvedVoteRelayConnectorIsInitialized()
   {
      ResolvedVoteRelayConnector_Init(&instance, dataModel, &config);
   }

   void RelayResolvedVoteStateIs(RelayState_t state)
   {
      RelayVotedState_t relayVote;
      relayVote.state = state;
      relayVote.care = true;

      DataModel_Write(
         dataModel,
         Erd_SomeRelay_ResolvedVote,
         &relayVote);
   }

   void MappedRelayShouldBe(bool expectedState)
   {
      bool actualState;
      DataModel_Read(dataModel, Erd_SomeRelay_MappedErd, &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }
};

TEST(ResolvedVoteRelayConnector, ShouldInitialize)
{
   Given The ResolvedVoteRelayConnectorIsInitialized();
}

TEST(ResolvedVoteRelayConnector, ShouldTurnOnRelayWhenResolvedVoteIsOn)
{
   Given The ResolvedVoteRelayConnectorIsInitialized();

   When The RelayResolvedVoteStateIs(HeaterState_On);
   The MappedRelayShouldBe(ON);
}

TEST(ResolvedVoteRelayConnector, ShouldTurnOffRelayWhenResolvedVoteIsOff)
{
   Given The ResolvedVoteRelayConnectorIsInitialized();

   When The RelayResolvedVoteStateIs(HeaterState_Off);
   The MappedRelayShouldBe(OFF);
}

TEST(ResolvedVoteRelayConnector, ShouldTurnRelayOffWhenResolvedVoteIsOffAfterBeingOn)
{
   Given The ResolvedVoteRelayConnectorIsInitialized();

   When The RelayResolvedVoteStateIs(HeaterState_On);
   The MappedRelayShouldBe(ON);

   When The RelayResolvedVoteStateIs(HeaterState_Off);
   The MappedRelayShouldBe(OFF);
}

TEST(ResolvedVoteRelayConnector, ShouldTurnRelayOnWhenResolvedVoteIsOnAfterBeingOff)
{
   Given The ResolvedVoteRelayConnectorIsInitialized();

   When The RelayResolvedVoteStateIs(HeaterState_Off);
   The MappedRelayShouldBe(OFF);

   When The RelayResolvedVoteStateIs(HeaterState_On);
   The MappedRelayShouldBe(ON);
}

TEST(ResolvedVoteRelayConnector, ShouldTurnRelayOffDuringInitializationIfRelayVoteIsOff)
{
   Given The RelayResolvedVoteStateIs(HeaterState_Off);

   When The ResolvedVoteRelayConnectorIsInitialized();
   The MappedRelayShouldBe(OFF);
}

TEST(ResolvedVoteRelayConnector, ShouldTurnRelayOnDuringInitializationIfRelayVoteIsOn)
{
   Given The RelayResolvedVoteStateIs(HeaterState_On);

   When The ResolvedVoteRelayConnectorIsInitialized();
   The MappedRelayShouldBe(ON);
}
