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
   TestIndex_0,
   TestIndex_1,

   SomeTestNumber = 23,
   SomeOtherTestNumber
};

static const ServiceModeTestNumber_t testGroupItems[] = {
   SomeTestNumber,
   SomeOtherTestNumber
};

static const ServiceModeTest_TestNumbersMappingTable_t testGroupConfig = {
   .testNumberEntries = testGroupItems,
   .numberOfItems = NUM_ELEMENTS(testGroupItems)
};

static const Erd_t configDataItems[] = {
   Erd_ApplicationVersion,
   Erd_ParametricVersion
};

static const ServiceModeTest_VersionMappingConfig_t config = {
   .versionErds = configDataItems,
   .numberOfItems = NUM_ELEMENTS(configDataItems)
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
      ServiceModeTest_Version_Init(&instance, &testGroupConfig, &config);
   }

   void ServiceModeTestNumberStatusShouldBe(ServiceModeTestStatus_t expected)
   {
      ServiceModeTestStatus_t actual;
      DataModel_Read(dataModel, Erd_ServiceModeTestNumberStatus, &actual);

      MEMCMP_EQUAL(&expected, &actual, sizeof(ServiceModeTestStatus_t));
   }

   void WhenTestIsStartedFor(ServiceModeTestNumber_t testNumber, uint8_t itemIndex)
   {
      resources.itemIndex = itemIndex;
      resources.testNumber = testNumber;

      ServiceTest_Start(&instance.interface, dataModel, &resources, TestsResultCallback);
   }

   void WhenTestIsStoppedFor(ServiceModeTestNumber_t testNumber, uint8_t itemIndex)
   {
      resources.itemIndex = itemIndex;
      resources.testNumber = testNumber;

      ServiceTest_Stop(&instance.interface, dataModel, &resources, TestsResultCallback);
   }

   void GivenVersionIs(uint8_t criticalMajor, uint8_t criticalMinor, uint8_t major, uint8_t minor)
   {
      Version_t version;
      version.criticalMajor = criticalMajor;
      version.criticalMinor = criticalMinor;
      version.major = major;
      version.minor = minor;
      DataModel_Write(dataModel, configDataItems[TestIndex_1], &version);
   }
};

TEST(ServiceModeTest_Version, ShouldStoreTestGroupInTheInterface)
{
   GivenTheModuleIsInitialized();

   MEMCMP_EQUAL(&testGroupConfig, instance.interface.testNumbersMappingTable, sizeof(ServiceModeTest_TestNumbersMappingTable_t));
}

TEST(ServiceModeTest_Version, ShouldSetVersionToTheResponseWhenTestIsStarted)
{
   GivenTheModuleIsInitialized();
   GivenVersionIs(1, 2, 3, 4);

   ServiceModeTestStatus_t testStatus;
   testStatus.testResponse = ServiceModeTestStatusResponse_Running;
   testStatus.dataFormat = ServiceModeTestStatusDataFormat_VersionInfo;
   testStatus.testNumber = SomeOtherTestNumber;
   testStatus.diagnosticData[0] = 3;
   testStatus.diagnosticData[1] = 4;
   testStatus.diagnosticData[2] = 0;
   testStatus.diagnosticData[3] = 0;

   WhenTestIsStartedFor(SomeOtherTestNumber, TestIndex_1);
   ServiceModeTestNumberStatusShouldBe(testStatus);
}

TEST(ServiceModeTest_Version, ShouldSetStopToTheResponseWhenTestIsStopped)
{
   GivenTheModuleIsInitialized();

   ServiceModeTestStatus_t testStatus;
   testStatus.testResponse = ServiceModeTestStatusResponse_Stopped;
   testStatus.dataFormat = ServiceModeTestStatusDataFormat_Unused;
   testStatus.testNumber = SomeOtherTestNumber;
   testStatus.diagnosticData[0] = 0;
   testStatus.diagnosticData[1] = 0;
   testStatus.diagnosticData[2] = 0;
   testStatus.diagnosticData[3] = 0;

   WhenTestIsStoppedFor(SomeOtherTestNumber, TestIndex_1);
   ServiceModeTestNumberStatusShouldBe(testStatus);
}
