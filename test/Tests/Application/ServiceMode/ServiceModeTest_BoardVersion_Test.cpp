/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "ServiceModeTest_BoardVersion.h"
#include "Constants_Time.h"
#include "Gea2CommonCommands.h"
#include "Gea2RefAddresses.h"
#include "ServiceModeTestNumber.h"
#include "SystemErds.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "Gea2MessageEndpoint_TestDouble.h"

enum
{
   Dispenser_Version_Index,
   RFID_Version_Index,

   NumberOfGea2MessageRetries = 2,
   LongVersionResponsePayloadLength = 13,

   VersionRequestTimeoutInMsec = 1 * MSEC_PER_SEC
};

static const ServiceModeTestNumber_t testGroupItems[] = {
   ServiceModeTestNumber_DispenserBoardVersion,
   ServiceModeTestNumber_RfidBoardVersion
};

static const ServiceModeTest_TestNumbersMappingTable_t testGroupConfig = {
   .testNumberEntries = testGroupItems,
   .numberOfItems = NUM_ELEMENTS(testGroupItems)
};

static const uint8_t externalBoardsVersionsDataTestItems[] = {
   Gea2Address_Dispenser,
   Gea2Address_RfidBoard
};

static const ServiceModeTest_BoardVersionMappingConfig_t externalBoardsVersionsTestDataConfig = {
   .destinationAddresses = externalBoardsVersionsDataTestItems,
   .numberOfItems = NUM_ELEMENTS(externalBoardsVersionsDataTestItems)
};

STATIC_ALLOC_GEA2MESSAGE(expectedBoardRequestMessage, 0);
STATIC_ALLOC_GEA2MESSAGE(expectedBoardResponseMessage, 4);
STATIC_ALLOC_GEA2MESSAGE(expectedBoardIncorrectResponseMessage, 3);
STATIC_ALLOC_GEA2MESSAGE(expectedLongVersionResponseMessage, LongVersionResponsePayloadLength);

static void TestsResultCallback(void *context, const ServiceTestResultArgs_t *args)
{
   REINTERPRET(dataModel, context, I_DataModel_t *);

   DataModel_Write(dataModel, Erd_ServiceModeTestNumberStatus, &args->status);
}

TEST_GROUP(ServiceModeTest_BoardVersion)
{
   ServiceModeTest_BoardVersion_t instance;

   ReferDataModel_TestDouble_t referDataModelTestDouble;
   I_DataModel_t *dataModel;

   TimerModule_TestDouble_t *timerModuleDouble;

   Gea2MessageEndpoint_TestDouble_t messageEndpointTestDouble;
   I_Gea2MessageEndpoint_t *messageEndpoint;

   ServiceTestResources_t resources;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&referDataModelTestDouble);
      dataModel = referDataModelTestDouble.dataModel;

      timerModuleDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&referDataModelTestDouble);

      Gea2MessageEndpoint_TestDouble_Init(
         &messageEndpointTestDouble,
         Gea2Address_ReferMainBoard);

      messageEndpoint = &messageEndpointTestDouble.interface;

      DataModel_Write(
         dataModel,
         Erd_Gea2MessageEndpoint,
         &messageEndpoint);

      resources.dataModel = dataModel;
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleDouble, ticks);
   }

   void GivenTheModuleIsInitialized()
   {
      ServiceModeTest_BoardVersion_Init(
         &instance,
         &testGroupConfig,
         &externalBoardsVersionsTestDataConfig);
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

   void VersionRequestShouldBeSentForIndex(uint8_t itemIndex)
   {
      Gea2Message_SetDestination(expectedBoardRequestMessage, externalBoardsVersionsDataTestItems[itemIndex]);
      Gea2Message_SetCommand(expectedBoardRequestMessage, Gea2CommonCommand_Version);
      Gea2Message_SetSource(expectedBoardRequestMessage, Gea2Address_ReferMainBoard);

      mock()
         .expectOneCall("Send")
         .onObject(messageEndpoint)
         .withParameterOfType("Gea2Message_t", "message", expectedBoardRequestMessage)
         .withParameter("retries", NumberOfGea2MessageRetries);
   }

   void VersionRequestShouldNotBeSent()
   {
      mock().expectNoCall("Send");
   }

   void WhenVersionResponseIsReceived(uint8_t criticalMajor, uint8_t criticalMinor, uint8_t major, uint8_t minor)
   {
      Gea2Message_SetDestination(expectedBoardResponseMessage, Gea2Address_ReferMainBoard);
      Gea2Message_SetSource(expectedBoardResponseMessage, externalBoardsVersionsDataTestItems[resources.itemIndex]);
      Gea2Message_SetCommand(expectedBoardResponseMessage, Gea2CommonCommand_Version);
      uint8_t *payload = Gea2Message_GetPayload(expectedBoardResponseMessage);
      payload[0] = criticalMajor;
      payload[1] = criticalMinor;
      payload[2] = major;
      payload[3] = minor;

      Gea2MessageEndpoint_TestDouble_TriggerReceive(&messageEndpointTestDouble, expectedBoardResponseMessage);
   }

   void WhenIncorrectVersionResponseIsReceived()
   {
      Gea2Message_SetDestination(expectedBoardIncorrectResponseMessage, Gea2Address_ReferMainBoard);
      Gea2Message_SetSource(expectedBoardIncorrectResponseMessage, externalBoardsVersionsDataTestItems[resources.itemIndex]);
      Gea2Message_SetCommand(expectedBoardIncorrectResponseMessage, Gea2CommonCommand_Version);

      Gea2MessageEndpoint_TestDouble_TriggerReceive(&messageEndpointTestDouble, expectedBoardIncorrectResponseMessage);
   }

   void WhenLongVersionResponseIsReceived(const uint8_t *expectedResponsePayload)
   {
      Gea2Message_SetDestination(expectedLongVersionResponseMessage, Gea2Address_ReferMainBoard);
      Gea2Message_SetSource(expectedLongVersionResponseMessage, externalBoardsVersionsDataTestItems[resources.itemIndex]);
      Gea2Message_SetCommand(expectedLongVersionResponseMessage, Gea2CommonCommand_Version);
      uint8_t *payload = Gea2Message_GetPayload(expectedLongVersionResponseMessage);
      memcpy(payload, expectedResponsePayload, LongVersionResponsePayloadLength);

      Gea2MessageEndpoint_TestDouble_TriggerReceive(&messageEndpointTestDouble, expectedLongVersionResponseMessage);
   }

   void ServiceModeTestNumberStatusShouldBe(ServiceModeTestStatus_t expected)
   {
      ServiceModeTestStatus_t actual;
      DataModel_Read(dataModel, Erd_ServiceModeTestNumberStatus, &actual);

      MEMCMP_EQUAL(&expected, &actual, sizeof(ServiceModeTestStatus_t));
   }
};

TEST(ServiceModeTest_BoardVersion, ShouldStoreTestGroupInTheInterface)
{
   GivenTheModuleIsInitialized();

   MEMCMP_EQUAL(&testGroupConfig, instance.interface.testNumbersMappingTable, sizeof(ServiceModeTest_TestNumbersMappingTable_t));
}

TEST(ServiceModeTest_BoardVersion, ShouldSendVersionRequestPeriodicallyWhenTestIsStarted)
{
   GivenTheModuleIsInitialized();

   VersionRequestShouldBeSentForIndex(RFID_Version_Index);
   WhenTestIsStartedFor(ServiceModeTestNumber_RfidBoardVersion, RFID_Version_Index);

   VersionRequestShouldBeSentForIndex(RFID_Version_Index);
   After(VersionRequestTimeoutInMsec);

   VersionRequestShouldBeSentForIndex(RFID_Version_Index);
   After(VersionRequestTimeoutInMsec);

   VersionRequestShouldBeSentForIndex(RFID_Version_Index);
   After(VersionRequestTimeoutInMsec);
}

TEST(ServiceModeTest_BoardVersion, ShouldSetVersionToTheResponseWhenVersionResponseIsReceived)
{
   GivenTheModuleIsInitialized();

   VersionRequestShouldBeSentForIndex(RFID_Version_Index);
   WhenTestIsStartedFor(ServiceModeTestNumber_RfidBoardVersion, RFID_Version_Index);

   VersionRequestShouldBeSentForIndex(RFID_Version_Index);
   WhenVersionResponseIsReceived(1, 2, 3, 4);

   ServiceModeTestStatus_t testStatus;
   testStatus.testResponse = ServiceModeTestStatusResponse_Running;
   testStatus.dataFormat = ServiceModeTestStatusDataFormat_VersionInfo;
   testStatus.testNumber = ServiceModeTestNumber_RfidBoardVersion;
   testStatus.diagnosticData[0] = 3;
   testStatus.diagnosticData[1] = 4;
   testStatus.diagnosticData[2] = 0;
   testStatus.diagnosticData[3] = 0;

   ServiceModeTestNumberStatusShouldBe(testStatus);
}

TEST(ServiceModeTest_BoardVersion, ShouldSetNoVersionInfoToTheResponseWhenVersionResponseIsNotReceivedWithinTimeout)
{
   GivenTheModuleIsInitialized();

   VersionRequestShouldBeSentForIndex(RFID_Version_Index);
   WhenTestIsStartedFor(ServiceModeTestNumber_RfidBoardVersion, RFID_Version_Index);

   After(VersionRequestTimeoutInMsec - 1);
   ServiceModeTestStatus_t testStatus;
   testStatus.testResponse = ServiceModeTestStatusResponse_Inactive;
   testStatus.dataFormat = ServiceModeTestStatusDataFormat_Unused;
   testStatus.testNumber = 0;
   testStatus.diagnosticData[0] = 0;
   testStatus.diagnosticData[1] = 0;
   testStatus.diagnosticData[2] = 0;
   testStatus.diagnosticData[3] = 0;

   ServiceModeTestNumberStatusShouldBe(testStatus);

   VersionRequestShouldBeSentForIndex(RFID_Version_Index);
   After(1);
   testStatus.testResponse = ServiceModeTestStatusResponse_Running;
   testStatus.dataFormat = ServiceModeTestStatusDataFormat_NoVersionInfo;
   testStatus.testNumber = ServiceModeTestNumber_RfidBoardVersion;
   testStatus.diagnosticData[0] = 0;
   testStatus.diagnosticData[1] = 0;
   testStatus.diagnosticData[2] = 0;
   testStatus.diagnosticData[3] = 0;

   ServiceModeTestNumberStatusShouldBe(testStatus);
}

TEST(ServiceModeTest_BoardVersion, ShouldNotSetNoVersionInfoToTheResponseWhenVersionResponseIsReceivedWithinTimeout)
{
   GivenTheModuleIsInitialized();

   VersionRequestShouldBeSentForIndex(Dispenser_Version_Index);
   WhenTestIsStartedFor(ServiceModeTestNumber_DispenserBoardVersion, Dispenser_Version_Index);

   After(VersionRequestTimeoutInMsec - 1);

   VersionRequestShouldBeSentForIndex(Dispenser_Version_Index);
   WhenVersionResponseIsReceived(1, 2, 3, 4);

   After(1);
   ServiceModeTestStatus_t testStatus;
   testStatus.testResponse = ServiceModeTestStatusResponse_Running;
   testStatus.dataFormat = ServiceModeTestStatusDataFormat_VersionInfo;
   testStatus.testNumber = ServiceModeTestNumber_DispenserBoardVersion;
   testStatus.diagnosticData[0] = 3;
   testStatus.diagnosticData[1] = 4;
   testStatus.diagnosticData[2] = 0;
   testStatus.diagnosticData[3] = 0;

   ServiceModeTestNumberStatusShouldBe(testStatus);
}

TEST(ServiceModeTest_BoardVersion, ShouldRestartTheTimerWhenVersionResponseIsReceived)
{
   GivenTheModuleIsInitialized();

   VersionRequestShouldBeSentForIndex(Dispenser_Version_Index);
   WhenTestIsStartedFor(ServiceModeTestNumber_DispenserBoardVersion, Dispenser_Version_Index);

   After(VersionRequestTimeoutInMsec - 1);

   VersionRequestShouldBeSentForIndex(Dispenser_Version_Index);
   WhenVersionResponseIsReceived(1, 2, 3, 4);

   VersionRequestShouldBeSentForIndex(Dispenser_Version_Index);
   After(VersionRequestTimeoutInMsec);
}

TEST(ServiceModeTest_BoardVersion, ShouldNotUpdateTestResponseWhenIncorrectVersionResponseIsReceived)
{
   GivenTheModuleIsInitialized();

   VersionRequestShouldBeSentForIndex(Dispenser_Version_Index);
   WhenTestIsStartedFor(ServiceModeTestNumber_DispenserBoardVersion, Dispenser_Version_Index);

   WhenIncorrectVersionResponseIsReceived();
   ServiceModeTestStatus_t testStatus;
   testStatus.testResponse = ServiceModeTestStatusResponse_Inactive;
   testStatus.dataFormat = ServiceModeTestStatusDataFormat_Unused;
   testStatus.testNumber = 0;
   testStatus.diagnosticData[0] = 0;
   testStatus.diagnosticData[1] = 0;
   testStatus.diagnosticData[2] = 0;
   testStatus.diagnosticData[3] = 0;
   ServiceModeTestNumberStatusShouldBe(testStatus);

   VersionRequestShouldBeSentForIndex(Dispenser_Version_Index);
   After(VersionRequestTimeoutInMsec);
   testStatus.testResponse = ServiceModeTestStatusResponse_Running;
   testStatus.dataFormat = ServiceModeTestStatusDataFormat_NoVersionInfo;
   testStatus.testNumber = ServiceModeTestNumber_DispenserBoardVersion;
   testStatus.diagnosticData[0] = 0;
   testStatus.diagnosticData[1] = 0;
   testStatus.diagnosticData[2] = 0;
   testStatus.diagnosticData[3] = 0;

   ServiceModeTestNumberStatusShouldBe(testStatus);
}

TEST(ServiceModeTest_BoardVersion, ShouldSetStopToTheResponseWhenTestIsStopped)
{
   GivenTheModuleIsInitialized();

   VersionRequestShouldBeSentForIndex(Dispenser_Version_Index);
   WhenTestIsStartedFor(ServiceModeTestNumber_DispenserBoardVersion, Dispenser_Version_Index);

   WhenTestIsStoppedFor(ServiceModeTestNumber_DispenserBoardVersion, Dispenser_Version_Index);
   ServiceModeTestStatus_t testStatus;
   testStatus.testResponse = ServiceModeTestStatusResponse_Stopped;
   testStatus.dataFormat = ServiceModeTestStatusDataFormat_Unused;
   testStatus.testNumber = ServiceModeTestNumber_DispenserBoardVersion;
   testStatus.diagnosticData[0] = 0;
   testStatus.diagnosticData[1] = 0;
   testStatus.diagnosticData[2] = 0;
   testStatus.diagnosticData[3] = 0;

   ServiceModeTestNumberStatusShouldBe(testStatus);
}

TEST(ServiceModeTest_BoardVersion, ShouldStopTheTimerWhenTestIsStopped)
{
   GivenTheModuleIsInitialized();

   VersionRequestShouldBeSentForIndex(Dispenser_Version_Index);
   WhenTestIsStartedFor(ServiceModeTestNumber_DispenserBoardVersion, Dispenser_Version_Index);

   WhenTestIsStoppedFor(ServiceModeTestNumber_DispenserBoardVersion, Dispenser_Version_Index);

   VersionRequestShouldNotBeSent();
   After(VersionRequestTimeoutInMsec);
}

TEST(ServiceModeTest_BoardVersion, ShouldNotSetVersionToTheResponseAfterTestIsStopped)
{
   GivenTheModuleIsInitialized();

   VersionRequestShouldBeSentForIndex(Dispenser_Version_Index);
   WhenTestIsStartedFor(ServiceModeTestNumber_DispenserBoardVersion, Dispenser_Version_Index);

   WhenTestIsStoppedFor(ServiceModeTestNumber_DispenserBoardVersion, Dispenser_Version_Index);
   WhenVersionResponseIsReceived(1, 2, 3, 4);

   ServiceModeTestStatus_t testStatus;
   testStatus.testResponse = ServiceModeTestStatusResponse_Stopped;
   testStatus.dataFormat = ServiceModeTestStatusDataFormat_Unused;
   testStatus.testNumber = ServiceModeTestNumber_DispenserBoardVersion;
   testStatus.diagnosticData[0] = 0;
   testStatus.diagnosticData[1] = 0;
   testStatus.diagnosticData[2] = 0;
   testStatus.diagnosticData[3] = 0;

   ServiceModeTestNumberStatusShouldBe(testStatus);
}

TEST(ServiceModeTest_BoardVersion, ShouldNotSetNoVersionInfoToTheResponseAfterTestIsStopped)
{
   GivenTheModuleIsInitialized();

   VersionRequestShouldBeSentForIndex(Dispenser_Version_Index);
   WhenTestIsStartedFor(ServiceModeTestNumber_DispenserBoardVersion, Dispenser_Version_Index);

   WhenTestIsStoppedFor(ServiceModeTestNumber_DispenserBoardVersion, Dispenser_Version_Index);

   After(VersionRequestTimeoutInMsec);
   ServiceModeTestStatus_t testStatus;
   testStatus.testResponse = ServiceModeTestStatusResponse_Stopped;
   testStatus.dataFormat = ServiceModeTestStatusDataFormat_Unused;
   testStatus.testNumber = ServiceModeTestNumber_DispenserBoardVersion;
   testStatus.diagnosticData[0] = 0;
   testStatus.diagnosticData[1] = 0;
   testStatus.diagnosticData[2] = 0;
   testStatus.diagnosticData[3] = 0;

   ServiceModeTestNumberStatusShouldBe(testStatus);
}

TEST(ServiceModeTest_BoardVersion, ShouldAcceptLongVersionResponse)
{
   GivenTheModuleIsInitialized();

   VersionRequestShouldBeSentForIndex(Dispenser_Version_Index);
   WhenTestIsStartedFor(ServiceModeTestNumber_DispenserBoardVersion, Dispenser_Version_Index);

   uint8_t longVersionResponse[LongVersionResponsePayloadLength] = { 1, 2, 0xE, 0xF, 5, 6, 7, 8, 9, 0xA, 0xB, 0xC, 0xD };
   VersionRequestShouldBeSentForIndex(Dispenser_Version_Index);
   WhenLongVersionResponseIsReceived(longVersionResponse);

   ServiceModeTestStatus_t testStatus;
   testStatus.testResponse = ServiceModeTestStatusResponse_Running;
   testStatus.dataFormat = ServiceModeTestStatusDataFormat_VersionInfo;
   testStatus.testNumber = ServiceModeTestNumber_DispenserBoardVersion;
   testStatus.diagnosticData[0] = 0xE;
   testStatus.diagnosticData[1] = 0xF;
   testStatus.diagnosticData[2] = 0;
   testStatus.diagnosticData[3] = 0;

   ServiceModeTestNumberStatusShouldBe(testStatus);
}
