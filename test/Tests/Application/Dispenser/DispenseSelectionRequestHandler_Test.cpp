/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "DispenseSelectionRequestHandler.h"
#include "DispenseSelection.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "uassert_test.h"
#include "DataModel_TestDouble.h"

static const DataModel_TestDoubleConfigurationEntry_t erds[] = {
   { Erd_DispenseSelectionRequest, sizeof(DispenseSelection_t) },
   { Erd_DispenseSelectionStatus, sizeof(DispenseSelection_t) },
};

static const DispenseSelectionRequestHandlerConfig_t config = {
   .dispenseSelectionRequestErd = Erd_DispenseSelectionRequest,
   .dispenseSelectionStatusErd = Erd_DispenseSelectionStatus,
};

TEST_GROUP(DispenseSelectionRequestHandler)
{
   DataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;

   DispenseSelectionRequestHandler_t instance;

   void setup()
   {
      DataModel_TestDouble_Init(&dataModelTestDouble, erds, NUM_ELEMENTS(erds));
      dataModel = dataModelTestDouble.dataModel;
   }

   void GivenInitialization()
   {
      DispenseSelectionRequestHandler_Init(
         &instance,
         &config,
         dataModel);
   }

   void WhenTheRequestedDispenseSelectionChangesTo(DispenseSelection_t requestDispense)
   {
      DataModel_Write(dataModel, Erd_DispenseSelectionRequest, &requestDispense);
   }

   void TheRequestedDispenseSelectionShouldBe(DispenseSelection_t expected)
   {
      DispenseSelection_t actual;
      DataModel_Read(dataModel, Erd_DispenseSelectionRequest, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void TheDispenseSelectionStatusShouldBe(DispenseSelection_t expected)
   {
      DispenseSelection_t actual;
      DataModel_Read(dataModel, Erd_DispenseSelectionStatus, &actual);

      CHECK_EQUAL(expected, actual);
   }
};

TEST(DispenseSelectionRequestHandler, ShouldSetDispenseSelectionRequestToResetValueOnStartUp)
{
   GivenInitialization();
   TheRequestedDispenseSelectionShouldBe(DispenseSelection_Reset);
}

TEST(DispenseSelectionRequestHandler, ShouldTakeDispenseSelectionRequestThenResetDispenseRequestToDefault)
{
   GivenInitialization();
   WhenTheRequestedDispenseSelectionChangesTo(DispenseSelection_ColdWater);
   TheRequestedDispenseSelectionShouldBe(DispenseSelection_Reset);
   TheDispenseSelectionStatusShouldBe(DispenseSelection_ColdWater);

   WhenTheRequestedDispenseSelectionChangesTo(DispenseSelection_ColdWater);
   TheRequestedDispenseSelectionShouldBe(DispenseSelection_Reset);
   TheDispenseSelectionStatusShouldBe(DispenseSelection_ColdWater);
}

TEST(DispenseSelectionRequestHandler, ShouldSetDispenseSelectionStatusToDispenseCubesWhenDispenseCubedIsRequested)
{
   GivenInitialization();
   WhenTheRequestedDispenseSelectionChangesTo(DispenseSelection_CubedIce);
   TheDispenseSelectionStatusShouldBe(DispenseSelection_CubedIce);
}

TEST(DispenseSelectionRequestHandler, ShouldSetDispenseSelectionStatusToDispenseCrushedWhenDispenseCrushedIsRequested)
{
   GivenInitialization();
   WhenTheRequestedDispenseSelectionChangesTo(DispenseSelection_CrushedIce);
   TheDispenseSelectionStatusShouldBe(DispenseSelection_CrushedIce);
}
