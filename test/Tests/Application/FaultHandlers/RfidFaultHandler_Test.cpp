/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "RfidFaultHandler.h"
#include "Constants_Binary.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "uassert_test.h"
#include "DataModel_TestDouble.h"

enum
{
   Erd_RfidFaultRequest,
   Erd_Fault1,
   Erd_Fault2,
   Erd_Fault3,
   Erd_Fault4,

   Fault1Count = 5,
   Fault2Count = 10,
   Fault3Count = 1,
   ALotOfFaultRequests = 3000
};

static const DataModel_TestDoubleConfigurationEntry_t erdTable[] = {
   { Erd_RfidFaultRequest, sizeof(RfidFaultRequest_t) },
   { Erd_Fault1, sizeof(bool) },
   { Erd_Fault2, sizeof(bool) },
   { Erd_Fault3, sizeof(bool) },
   { Erd_Fault4, sizeof(bool) },
};

static const RfidFaultConfig_t faultConfigs[] = {
   {
      .faultErd = Erd_Fault1,
      .minimumConsecutiveRequestsToSetFault = Fault1Count,
   },
   {
      .faultErd = Erd_Fault2,
      .minimumConsecutiveRequestsToSetFault = Fault2Count,
   },
   {
      .faultErd = Erd_Fault3,
      .minimumConsecutiveRequestsToSetFault = Fault3Count,
   }
};

static uint8_t buffer[NUM_ELEMENTS(faultConfigs)] = { 0 };

static const RfidFaultHandlerConfig_t config = {
   .faultConfigurations = faultConfigs,
   .numberOfFaults = NUM_ELEMENTS(faultConfigs),
   .rfidFaultRequestErd = Erd_RfidFaultRequest
};

TEST_GROUP(RfidFaultHandler)
{
   RfidFaultHandler_t instance;
   DataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;

   void setup()
   {
      DataModel_TestDouble_Init(
         &dataModelTestDouble,
         erdTable,
         NUM_ELEMENTS(erdTable));
      dataModel = dataModelTestDouble.dataModel;
   }

   void GivenInitialization()
   {
      memset(&buffer, 0, NUM_ELEMENTS(faultConfigs));

      RfidFaultHandler_Init(
         &instance,
         dataModel,
         &config,
         buffer);
   }

   void FaultRequestIs(Erd_t erd, bool status)
   {
      RfidFaultRequest_t request;
      DataModel_Read(dataModel, Erd_RfidFaultRequest, &request);
      request.faultErd = erd;
      request.requestStatus = status;
      request.signal++;
      DataModel_Write(dataModel, Erd_RfidFaultRequest, &request);
   }

   void FaultShouldBe(Erd_t erd, bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, erd, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void GivenFaultIs(Erd_t erd, bool status)
   {
      DataModel_Write(dataModel, erd, &status);
   }

   void FaultErdShouldOnlyBeSetAfterXFaultRequests(Erd_t erd, uint8_t numberOfFaultRequests)
   {
      for(uint8_t i = 0; i < numberOfFaultRequests - 1; i++)
      {
         FaultRequestIs(erd, SET);
         FaultShouldBe(erd, CLEAR);
      }

      FaultRequestIs(erd, SET);
      FaultShouldBe(erd, SET);
   }
};

TEST(RfidFaultHandler, ShouldSetAFaultWhenItHasBeenRequestedForAsManyLimitsAsItHas)
{
   GivenInitialization();
   FaultErdShouldOnlyBeSetAfterXFaultRequests(Erd_Fault1, Fault1Count);
}

TEST(RfidFaultHandler, ShouldClearAFaultWhenItHasBeenRequestedToClear)
{
   GivenInitialization();
   GivenFaultIs(Erd_Fault1, SET);

   FaultRequestIs(Erd_Fault1, CLEAR);
   FaultShouldBe(Erd_Fault1, CLEAR);
}

TEST(RfidFaultHandler, ShouldRestartTheCountOnAFaultThatWasCleared)
{
   GivenInitialization();
   FaultErdShouldOnlyBeSetAfterXFaultRequests(Erd_Fault1, Fault1Count);

   FaultRequestIs(Erd_Fault1, CLEAR);
   FaultShouldBe(Erd_Fault1, CLEAR);

   FaultErdShouldOnlyBeSetAfterXFaultRequests(Erd_Fault1, Fault1Count);
}

TEST(RfidFaultHandler, ShouldIgnoreFaultsThatAreNotInConfiguration)
{
   GivenInitialization();
   FaultRequestIs(Erd_Fault4, SET);
   FaultShouldBe(Erd_Fault4, CLEAR);
}

TEST(RfidFaultHandler, ShouldOnlyIncrementTheRequestedFaultWhenAFaultIsRequested)
{
   GivenInitialization();
   FaultShouldBe(Erd_Fault1, CLEAR);
   FaultShouldBe(Erd_Fault2, CLEAR);
   FaultShouldBe(Erd_Fault3, CLEAR);

   FaultErdShouldOnlyBeSetAfterXFaultRequests(Erd_Fault1, Fault1Count);
   FaultShouldBe(Erd_Fault1, SET);
   FaultShouldBe(Erd_Fault2, CLEAR);
   FaultShouldBe(Erd_Fault3, CLEAR);

   FaultErdShouldOnlyBeSetAfterXFaultRequests(Erd_Fault2, Fault2Count);
   FaultShouldBe(Erd_Fault1, SET);
   FaultShouldBe(Erd_Fault2, SET);
   FaultShouldBe(Erd_Fault3, CLEAR);

   FaultRequestIs(Erd_Fault3, SET);
   FaultRequestIs(Erd_Fault3, SET);
   FaultShouldBe(Erd_Fault1, SET);
   FaultShouldBe(Erd_Fault2, SET);
   FaultShouldBe(Erd_Fault3, SET);
}

TEST(RfidFaultHandler, ShouldNotOverflowCountIfContinuousSetFaultRequestsAreSent)
{
   GivenInitialization();

   FaultErdShouldOnlyBeSetAfterXFaultRequests(Erd_Fault1, Fault1Count);

   for(uint16_t i = 0; i < ALotOfFaultRequests; i++)
   {
      FaultRequestIs(Erd_Fault1, SET);
      FaultShouldBe(Erd_Fault1, SET);
   }
}
