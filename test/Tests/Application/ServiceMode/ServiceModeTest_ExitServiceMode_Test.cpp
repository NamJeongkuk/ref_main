/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "ServiceModeTest_ExitServiceMode.h"
#include "SystemErds.h"
#include "I_ServiceTest.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "uassert_test.h"
#include "ReferDataModel_TestDouble.h"

enum
{
   SomeTestNumber = 16
};

TEST_GROUP(ServiceModeTest_ExitServiceMode)
{
   ServiceModeTest_ExitServiceMode_t instance;
   ReferDataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;
   ServiceTestResources_t resources;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelTestDouble);
      dataModel = dataModelTestDouble.dataModel;
      resources.dataModel = dataModel;
   }

   void GivenModuleIsInitialized()
   {
      ServiceModeTest_ExitServiceMode_Init(&instance, SomeTestNumber);
   }

   void BroadcastResetSignalErdShouldBe(Signal_t expected)
   {
      Signal_t actual;
      DataModel_Read(dataModel, Erd_BroadcastResetRequestSignal, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void WhenTestIsStarted(void)
   {
      ServiceTest_Start(&instance.interface, NULL, &resources, NULL);
   }
};

TEST(ServiceModeTest_ExitServiceMode, ShouldIncrementBroadcastSignalWhenStarted)
{
   BroadcastResetSignalErdShouldBe(0);
   GivenModuleIsInitialized();

   WhenTestIsStarted();
   BroadcastResetSignalErdShouldBe(1);
}

TEST(ServiceModeTest_ExitServiceMode, ShouldStoreTestNumberInTheInterface)
{
   GivenModuleIsInitialized();
   CHECK_EQUAL(SomeTestNumber, instance.interface.testNumber);
}
