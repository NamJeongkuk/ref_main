/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "ResolvedSetpointWriter.h"
#include "Constants_Binary.h"
#include "SetpointVotedTemperature.h"
#include "utils.h"
}
#include "DataModel_TestDouble.h"
#include "CppUTest/TestHarness.h"
#include "uassert_test.h"

enum
{
   SomeValue = 10,
   SomeOtherValue = 20
};

enum
{
   Erd_SetpointResolvedVote,
   Erd_ResolvedSetpoint,
   Erd_NotInConfig,
   Erd_UserSetpointPluginReady
};

static const DataModel_TestDoubleConfigurationEntry_t dataModelTestDoubleConfig[] = {
   { Erd_SetpointResolvedVote, sizeof(SetpointVotedTemperature_t) },
   { Erd_ResolvedSetpoint, sizeof(int16_t) },
   { Erd_NotInConfig, sizeof(SetpointVotedTemperature_t) },
   { Erd_UserSetpointPluginReady, sizeof(bool) }
};

static const ResolvedSetpointWriterConfiguration_t resolvedSetpointWriterConfiguration = {
   .resolvedSetpointVoteErd = Erd_SetpointResolvedVote,
   .resolvedSetpointErd = Erd_ResolvedSetpoint,
   .userSetpointPluginReadyErd = Erd_UserSetpointPluginReady
};

TEST_GROUP(ResolvedSetpointWriter)
{
   ResolvedSetpointWriter_t instance;
   DataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;

   void setup()
   {
      DataModel_TestDouble_Init(&dataModelTestDouble, dataModelTestDoubleConfig, NUM_ELEMENTS(dataModelTestDoubleConfig));
      dataModel = dataModelTestDouble.dataModel;

      DataModel_Write(dataModel, Erd_UserSetpointPluginReady, set);
   }

   void GivenModuleIsInitialized()
   {
      ResolvedSetpointWriter_Init(&instance, dataModel, &resolvedSetpointWriterConfiguration);
   }

   void GivenResolvedVoteIs(int16_t value)
   {
      SetpointVotedTemperature_t vote = {
         .temperatureInDegFx100 = value,
         .care = true
      };
      DataModel_Write(dataModel, Erd_SetpointResolvedVote, &vote);
   }

   void WhenAnotherErdIs(int16_t value)
   {
      SetpointVotedTemperature_t vote = {
         .temperatureInDegFx100 = value,
         .care = true
      };
      DataModel_Write(dataModel, Erd_NotInConfig, &vote);
   }

   void TheResolvedSetpointShouldBe(int16_t expected)
   {
      int16_t actual;
      DataModel_Read(dataModel, Erd_ResolvedSetpoint, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void GivenUserSetpointPluginReadyIs(bool state)
   {
      DataModel_Write(dataModel, Erd_UserSetpointPluginReady, &state);
   }
};

TEST(ResolvedSetpointWriter, ShouldAssertWhenUserSetpointPluginReadyIsClearOnInit)
{
   GivenUserSetpointPluginReadyIs(CLEAR);
   ShouldFailAssertion(GivenModuleIsInitialized());
}

TEST(ResolvedSetpointWriter, ShouldWriteToResolvedSetpointOnInit)
{
   GivenResolvedVoteIs(SomeValue);
   GivenModuleIsInitialized();
   TheResolvedSetpointShouldBe(SomeValue);
}

TEST(ResolvedSetpointWriter, ShouldWriteToResolvedSetpointWhenVoteChanges)
{
   GivenModuleIsInitialized();
   GivenResolvedVoteIs(SomeValue);
   TheResolvedSetpointShouldBe(SomeValue);
}

TEST(ResolvedSetpointWriter, ShouldNotWriteToResolvedSetpointWhenAnotherErdChanges)
{
   GivenResolvedVoteIs(SomeValue);
   GivenModuleIsInitialized();

   WhenAnotherErdIs(SomeOtherValue);
   TheResolvedSetpointShouldBe(SomeValue);
}
