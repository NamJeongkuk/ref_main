/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "UserSetpointRequestHandler.h"
#include "SystemErds.h"
#include "Vote.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "DataModel_TestDouble.h"
#include "uassert_test.h"

enum
{
   Erd_UserSetpointRangeData,
   Erd_UserSetpointRequest,
   Erd_UserSetpointVote,
   Erd_UserSetpointStatus,

   SomeHighTemperatureBound = 40,
   SomeLowTemperatureBound = -5,
   SomeDefaultTemperature = 0,

   SomeSetpointBetweenTheBounds = 21,
   SomeOtherSetpointBetweenTheBounds = 24,

   ResetValue = INT8_MAX,
   ScalingFactorDegFx100 = 100,
};

static const DataModel_TestDoubleConfigurationEntry_t erds[] = {
   { Erd_UserSetpointRangeData, sizeof(UserSetpointRangeData_t) },
   { Erd_UserSetpointRequest, sizeof(Setpoint_t) },
   { Erd_UserSetpointVote, sizeof(SetpointVotedTemperature_t) },
   { Erd_UserSetpointStatus, sizeof(Setpoint_t) },
};

static const UserSetpointRequestHandlerConfig_t config = {
   .userSetpointRequestErd = Erd_UserSetpointRequest,
   .userSetpointVoteErd = Erd_UserSetpointVote,
   .userSetpointRangeDataErd = Erd_UserSetpointRangeData,
   .userSetpointStatusErd = Erd_UserSetpointStatus,
};

static const UserSetpointRangeData_t rangeData = {
   .lowTemperatureSetpoint = SomeLowTemperatureBound,
   .highTemperatureSetpoint = SomeHighTemperatureBound,
   .defaultTemperatureSetpoint = SomeDefaultTemperature
};

TEST_GROUP(UserSetpointRequestHandler)
{
   DataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;

   UserSetpointRequestHandler_t instance;

   void setup()
   {
      DataModel_TestDouble_Init(&dataModelTestDouble, erds, NUM_ELEMENTS(erds));
      dataModel = dataModelTestDouble.dataModel;

      DataModel_Write(dataModel, Erd_UserSetpointRangeData, &rangeData);
   }

   void GivenInitialization()
   {
      UserSetpointRequestHandler_Init(
         &instance,
         &config,
         dataModel);
   }

   void TheRequestedSetpointShouldBe(Setpoint_t expected)
   {
      Setpoint_t actual;
      DataModel_Read(dataModel, Erd_UserSetpointRequest, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void TheSetpointStatusShouldBe(Setpoint_t expected)
   {
      Setpoint_t actual;
      DataModel_Read(dataModel, Erd_UserSetpointStatus, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void GivenTheSetpointStatusIs(Setpoint_t setpoint)
   {
      DataModel_Write(dataModel, Erd_UserSetpointStatus, &setpoint);
   }

   void WhenTheRequestedSetpointChangesTo(Setpoint_t requestSetpoint)
   {
      DataModel_Write(dataModel, Erd_UserSetpointRequest, &requestSetpoint);
   }

   void TheVotedSetpointShouldBe(TemperatureDegFx100_t expected, bool expectedCare)
   {
      SetpointVotedTemperature_t actual;
      DataModel_Read(dataModel, Erd_UserSetpointVote, &actual);

      CHECK_EQUAL(expected, actual.temperatureInDegFx100);
      CHECK_EQUAL(expectedCare, actual.care);
   }
};

TEST(UserSetpointRequestHandler, ShouldSetSetpointRequestToResetValueOnStartUp)
{
   GivenInitialization();
   TheRequestedSetpointShouldBe(ResetValue);
}

TEST(UserSetpointRequestHandler, ShouldVoteForUserSetpointBasedOffOfTheUserSetpointRequestWhenTheRequestIsInTheBounds)
{
   GivenInitialization();
   WhenTheRequestedSetpointChangesTo(SomeSetpointBetweenTheBounds);
   TheSetpointStatusShouldBe(SomeSetpointBetweenTheBounds);
   TheVotedSetpointShouldBe(SomeSetpointBetweenTheBounds * ScalingFactorDegFx100, Vote_Care);
}

TEST(UserSetpointRequestHandler, ShouldNotVoteForUserSetpointWhenTheUserSetpointRequestIsAboveTheUpperBound)
{
   GivenInitialization();
   TheVotedSetpointShouldBe(0, Vote_Care);

   WhenTheRequestedSetpointChangesTo(SomeHighTemperatureBound + 1);
   TheSetpointStatusShouldBe(0);
   TheVotedSetpointShouldBe(0, Vote_Care);
}

TEST(UserSetpointRequestHandler, ShouldNotVoteForUserSetpointWhenTheUserSetpointRequestIsBelowTheLowerBound)
{
   GivenInitialization();
   TheVotedSetpointShouldBe(0, Vote_Care);

   WhenTheRequestedSetpointChangesTo(SomeLowTemperatureBound - 1);
   TheSetpointStatusShouldBe(0);
   TheVotedSetpointShouldBe(0, Vote_Care);
}

TEST(UserSetpointRequestHandler, ShouldBeAbleToVoteMultipleTimes)
{
   GivenInitialization();
   WhenTheRequestedSetpointChangesTo(SomeSetpointBetweenTheBounds);
   TheSetpointStatusShouldBe(SomeSetpointBetweenTheBounds);
   TheVotedSetpointShouldBe(SomeSetpointBetweenTheBounds * ScalingFactorDegFx100, Vote_Care);

   WhenTheRequestedSetpointChangesTo(SomeHighTemperatureBound + 1);
   TheSetpointStatusShouldBe(SomeSetpointBetweenTheBounds);
   TheVotedSetpointShouldBe(SomeSetpointBetweenTheBounds * ScalingFactorDegFx100, Vote_Care);

   WhenTheRequestedSetpointChangesTo(SomeOtherSetpointBetweenTheBounds);
   TheSetpointStatusShouldBe(SomeOtherSetpointBetweenTheBounds);
   TheVotedSetpointShouldBe(SomeOtherSetpointBetweenTheBounds * ScalingFactorDegFx100, Vote_Care);

   WhenTheRequestedSetpointChangesTo(SomeLowTemperatureBound - 1);
   TheSetpointStatusShouldBe(SomeOtherSetpointBetweenTheBounds);
   TheVotedSetpointShouldBe(SomeOtherSetpointBetweenTheBounds * ScalingFactorDegFx100, Vote_Care);
}

TEST(UserSetpointRequestHandler, RequestedSetpointShouldChangeBackToItsResetValueAfterEachChange)
{
   GivenInitialization();
   TheRequestedSetpointShouldBe(ResetValue);

   WhenTheRequestedSetpointChangesTo(SomeSetpointBetweenTheBounds);
   TheRequestedSetpointShouldBe(ResetValue);

   WhenTheRequestedSetpointChangesTo(SomeOtherSetpointBetweenTheBounds);
   TheRequestedSetpointShouldBe(ResetValue);
}

TEST(UserSetpointRequestHandler, ShouldVoteOnInitialization)
{
   GivenTheSetpointStatusIs(SomeSetpointBetweenTheBounds);
   GivenInitialization();

   TheVotedSetpointShouldBe(SomeSetpointBetweenTheBounds * ScalingFactorDegFx100, Vote_Care);
}

TEST(UserSetpointRequestHandler, ShouldSetStatusToDefaultRangeDataIfStatusIsTheResetValueOnInitialization)
{
   GivenTheSetpointStatusIs(ResetValue);
   GivenInitialization();

   TheVotedSetpointShouldBe(SomeDefaultTemperature * ScalingFactorDegFx100, Vote_Care);
   TheSetpointStatusShouldBe(SomeDefaultTemperature);
}
