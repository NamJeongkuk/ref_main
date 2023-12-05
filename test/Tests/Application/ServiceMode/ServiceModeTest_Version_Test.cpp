/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "ServiceModeTest_Version.h"
#include "utils.h"
}

#include "CppUTest/TestHarness.h"
#include "ReferDataModel_TestDouble.h"

enum
{
   SomeTestNumber = 23
};

static const ServiceModeTest_VersionConfig_t config = {
   .versionErd = Erd_ApplicationVersion
};

static void TestsResultCallback(void *context, const ServiceTestResultArgs_t *args)
{
   REINTERPRET(dataModel, context, I_DataModel_t *);
   DataModel_Write(dataModel, Erd_ServiceModeTestNumberStatus, &args->status);
}

TEST_GROUP(ServiceModeTest_Version)
{
   ServiceModeTest_Version_t instance;

   ReferDataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;
   ServiceTestResources_t resources;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelTestDouble);
      dataModel = dataModelTestDouble.dataModel;

      resources.dataModel = dataModel;
   }

   void GivenTheModuleIsInitialized()
   {
      ServiceModeTest_Version_Init(&instance, SomeTestNumber, &config);
   }

   void ServiceModeTestNumberStatusShouldBe(ServiceModeTestStatus_t expected)
   {
      ServiceModeTestStatus_t actual;
      DataModel_Read(dataModel, Erd_ServiceModeTestNumberStatus, &actual);

      MEMCMP_EQUAL(&expected, &actual, sizeof(ServiceModeTestStatus_t));
   }

   void WhenTestIsStarted(void)
   {
      ServiceTest_Start(&instance.interface, dataModel, &resources, TestsResultCallback);
   }

   void WhenTestIsStopped(void)
   {
      ServiceTest_Stop(&instance.interface, dataModel, &resources, TestsResultCallback);
   }

   void GivenVersionIs(uint8_t criticalMajor, uint8_t criticalMinor, uint8_t major, uint8_t minor)
   {
      Version_t version;
      version.criticalMajor = criticalMajor;
      version.criticalMinor = criticalMinor;
      version.major = major;
      version.minor = minor;
      DataModel_Write(dataModel, config.versionErd, &version);
   }
};

TEST(ServiceModeTest_Version, ShouldStoreTestNumberInTheInterface)
{
   GivenTheModuleIsInitialized();
   CHECK_EQUAL(SomeTestNumber, instance.interface.testNumber);
}

TEST(ServiceModeTest_Version, ShouldSetVersionToTheResponseWhenTestIsStarted)
{
   GivenTheModuleIsInitialized();
   GivenVersionIs(1, 2, 3, 4);

   ServiceModeTestStatus_t testStatus;
   testStatus.testResponse = ServiceModeTestStatusResponse_Running;
   testStatus.dataFormat = ServiceModeTestStatusDataFormat_VersionInfo;
   testStatus.testNumber = instance.interface.testNumber;
   testStatus.diagnosticData[0] = 3;
   testStatus.diagnosticData[1] = 4;
   testStatus.diagnosticData[2] = 0;
   testStatus.diagnosticData[3] = 0;

   WhenTestIsStarted();
   ServiceModeTestNumberStatusShouldBe(testStatus);
}

TEST(ServiceModeTest_Version, ShouldSetStopToTheResponseWhenTestIsStopped)
{
   GivenTheModuleIsInitialized();

   ServiceModeTestStatus_t testStatus;
   testStatus.testResponse = ServiceModeTestStatusResponse_Stopped;
   testStatus.dataFormat = ServiceModeTestStatusDataFormat_Unused;
   testStatus.testNumber = instance.interface.testNumber;
   testStatus.diagnosticData[0] = 0;
   testStatus.diagnosticData[1] = 0;
   testStatus.diagnosticData[2] = 0;
   testStatus.diagnosticData[3] = 0;

   WhenTestIsStopped();
   ServiceModeTestNumberStatusShouldBe(testStatus);
}
