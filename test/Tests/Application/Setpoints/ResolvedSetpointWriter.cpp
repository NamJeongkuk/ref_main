/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "ResolvedSetpointWriter.h"
#include "Setpoint.h"
#include "utils.h"
}
#include "DataModel_TestDouble.h"
#include "CppUTest/TestHarness.h"

enum
{
   Erd_SetpointResolvedVote,
   Erd_ResolvedSetpoint,
   Erd_NotInConfig,
   SomeValue = 10,
   SomeOtherValue = 20
};

static const DataModel_TestDoubleConfigurationEntry_t dataModelTestDoubleConfig[] = {
   { Erd_SetpointResolvedVote, sizeof(SetpointVotedTemperature_t) },
   { Erd_ResolvedSetpoint, sizeof(int16_t) },
   { Erd_NotInConfig, sizeof(SetpointVotedTemperature_t) }
};

static const ResolvedSetpointWriterConfiguration_t resolvedSetpointWriterConfiguration = {
   .resolvedSetpointVoteErd = Erd_SetpointResolvedVote,
   .resolvedSetpointErd = Erd_ResolvedSetpoint
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
};

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
