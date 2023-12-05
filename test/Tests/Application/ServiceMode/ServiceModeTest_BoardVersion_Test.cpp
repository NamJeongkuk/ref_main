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
#include "Gea2RefAddresses.h"
#include "Gea2CommonCommands.h"
#include "SystemErds.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "Gea2MessageEndpoint_TestDouble.h"

enum
{
   SomeTestNumber = 23,
   NumberOfGea2MessageRetries = 2,
   LongVersionResponsePayloadLength = 13,

   VersionRequestTimeoutInMsec = 1 * MSEC_PER_SEC
};

static const ServiceModeTest_BoardVersionConfig_t config = {
   .destinationAddress = Gea2Address_Dispenser
};

STATIC_ALLOC_GEA2MESSAGE(expectedDispenserUiRequestMessage, 0);
STATIC_ALLOC_GEA2MESSAGE(expectedDispenserUiResponseMessage, 4);
STATIC_ALLOC_GEA2MESSAGE(expectedDispenserUiIncorrectResponseMessage, 3);
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
      ServiceModeTest_BoardVersion_Init(&instance, SomeTestNumber, &config);
   }

   void WhenTestIsStarted(void)
   {
      ServiceTest_Start(&instance.interface, dataModel, &resources, TestsResultCallback);
   }

   void WhenTestIsStopped(void)
   {
      ServiceTest_Stop(&instance.interface, dataModel, &resources, TestsResultCallback);
   }

   void VersionRequestShouldBeSent()
   {
      Gea2Message_SetDestination(expectedDispenserUiRequestMessage, config.destinationAddress);
      Gea2Message_SetCommand(expectedDispenserUiRequestMessage, Gea2CommonCommand_Version);
      Gea2Message_SetSource(expectedDispenserUiRequestMessage, Gea2Address_ReferMainBoard);

      mock()
         .expectOneCall("Send")
         .onObject(messageEndpoint)
         .withParameterOfType("Gea2Message_t", "message", expectedDispenserUiRequestMessage)
         .withParameter("retries", NumberOfGea2MessageRetries);
   }

   void VersionRequestShouldNotBeSent()
   {
      mock().expectNoCall("Send");
   }

   void WhenVersionResponseIsReceived(uint8_t criticalMajor, uint8_t criticalMinor, uint8_t major, uint8_t minor)
   {
      Gea2Message_SetDestination(expectedDispenserUiResponseMessage, Gea2Address_ReferMainBoard);
      Gea2Message_SetSource(expectedDispenserUiResponseMessage, config.destinationAddress);
      Gea2Message_SetCommand(expectedDispenserUiResponseMessage, Gea2CommonCommand_Version);
      uint8_t *payload = Gea2Message_GetPayload(expectedDispenserUiResponseMessage);
      payload[0] = criticalMajor;
      payload[1] = criticalMinor;
      payload[2] = major;
      payload[3] = minor;

      Gea2MessageEndpoint_TestDouble_TriggerReceive(&messageEndpointTestDouble, expectedDispenserUiResponseMessage);
   }

   void WhenIncorrectVersionResponseIsReceived()
   {
      Gea2Message_SetDestination(expectedDispenserUiIncorrectResponseMessage, Gea2Address_ReferMainBoard);
      Gea2Message_SetSource(expectedDispenserUiIncorrectResponseMessage, config.destinationAddress);
      Gea2Message_SetCommand(expectedDispenserUiIncorrectResponseMessage, Gea2CommonCommand_Version);

      Gea2MessageEndpoint_TestDouble_TriggerReceive(&messageEndpointTestDouble, expectedDispenserUiIncorrectResponseMessage);
   }

   void WhenLongVersionResponseIsReceived(const uint8_t *expectedResponsePayload)
   {
      Gea2Message_SetDestination(expectedLongVersionResponseMessage, Gea2Address_ReferMainBoard);
      Gea2Message_SetSource(expectedLongVersionResponseMessage, config.destinationAddress);
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

TEST(ServiceModeTest_BoardVersion, ShouldStoreTestNumberInTheInterface)
{
   GivenTheModuleIsInitialized();
   CHECK_EQUAL(SomeTestNumber, instance.interface.testNumber);
}

TEST(ServiceModeTest_BoardVersion, ShouldSendVersionRequestPeriodicallyWhenTestIsStarted)
{
   GivenTheModuleIsInitialized();

   VersionRequestShouldBeSent();
   WhenTestIsStarted();

   VersionRequestShouldBeSent();
   After(VersionRequestTimeoutInMsec);

   VersionRequestShouldBeSent();
   After(VersionRequestTimeoutInMsec);

   VersionRequestShouldBeSent();
   After(VersionRequestTimeoutInMsec);
}

TEST(ServiceModeTest_BoardVersion, ShouldSetVersionToTheResponseWhenVersionResponseIsReceived)
{
   GivenTheModuleIsInitialized();

   VersionRequestShouldBeSent();
   WhenTestIsStarted();

   VersionRequestShouldBeSent();
   WhenVersionResponseIsReceived(1, 2, 3, 4);

   ServiceModeTestStatus_t testStatus;
   testStatus.testResponse = ServiceModeTestStatusResponse_Running;
   testStatus.dataFormat = ServiceModeTestStatusDataFormat_VersionInfo;
   testStatus.testNumber = instance.interface.testNumber;
   testStatus.diagnosticData[0] = 3;
   testStatus.diagnosticData[1] = 4;
   testStatus.diagnosticData[2] = 0;
   testStatus.diagnosticData[3] = 0;
   ServiceModeTestNumberStatusShouldBe(testStatus);
}

TEST(ServiceModeTest_BoardVersion, ShouldSetNoVersionInfoToTheResponseWhenVersionResponseIsNotReceivedWithinTimeout)
{
   GivenTheModuleIsInitialized();

   VersionRequestShouldBeSent();
   WhenTestIsStarted();

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

   VersionRequestShouldBeSent();
   After(1);
   testStatus.testResponse = ServiceModeTestStatusResponse_Running;
   testStatus.dataFormat = ServiceModeTestStatusDataFormat_NoVersionInfo;
   testStatus.testNumber = instance.interface.testNumber;
   testStatus.diagnosticData[0] = 0;
   testStatus.diagnosticData[1] = 0;
   testStatus.diagnosticData[2] = 0;
   testStatus.diagnosticData[3] = 0;
   ServiceModeTestNumberStatusShouldBe(testStatus);
}

TEST(ServiceModeTest_BoardVersion, ShouldNotSetNoVersionInfoToTheResponseWhenVersionResponseIsReceivedWithinTimeout)
{
   GivenTheModuleIsInitialized();

   VersionRequestShouldBeSent();
   WhenTestIsStarted();

   After(VersionRequestTimeoutInMsec - 1);

   VersionRequestShouldBeSent();
   WhenVersionResponseIsReceived(1, 2, 3, 4);

   After(1);
   ServiceModeTestStatus_t testStatus;
   testStatus.testResponse = ServiceModeTestStatusResponse_Running;
   testStatus.dataFormat = ServiceModeTestStatusDataFormat_VersionInfo;
   testStatus.testNumber = instance.interface.testNumber;
   testStatus.diagnosticData[0] = 3;
   testStatus.diagnosticData[1] = 4;
   testStatus.diagnosticData[2] = 0;
   testStatus.diagnosticData[3] = 0;
   ServiceModeTestNumberStatusShouldBe(testStatus);
}

TEST(ServiceModeTest_BoardVersion, ShouldRestartTheTimerWhenVersionResponseIsReceived)
{
   GivenTheModuleIsInitialized();

   VersionRequestShouldBeSent();
   WhenTestIsStarted();

   After(VersionRequestTimeoutInMsec - 1);

   VersionRequestShouldBeSent();
   WhenVersionResponseIsReceived(1, 2, 3, 4);

   VersionRequestShouldBeSent();
   After(VersionRequestTimeoutInMsec);
}

TEST(ServiceModeTest_BoardVersion, ShouldNotUpdateTestResponseWhenIncorrectVersionResponseIsReceived)
{
   GivenTheModuleIsInitialized();

   VersionRequestShouldBeSent();
   WhenTestIsStarted();

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

   VersionRequestShouldBeSent();
   After(VersionRequestTimeoutInMsec);
   testStatus.testResponse = ServiceModeTestStatusResponse_Running;
   testStatus.dataFormat = ServiceModeTestStatusDataFormat_NoVersionInfo;
   testStatus.testNumber = instance.interface.testNumber;
   testStatus.diagnosticData[0] = 0;
   testStatus.diagnosticData[1] = 0;
   testStatus.diagnosticData[2] = 0;
   testStatus.diagnosticData[3] = 0;
   ServiceModeTestNumberStatusShouldBe(testStatus);
}

TEST(ServiceModeTest_BoardVersion, ShouldSetStopToTheResponseWhenTestIsStopped)
{
   GivenTheModuleIsInitialized();

   VersionRequestShouldBeSent();
   WhenTestIsStarted();

   WhenTestIsStopped();
   ServiceModeTestStatus_t testStatus;
   testStatus.testResponse = ServiceModeTestStatusResponse_Stopped;
   testStatus.dataFormat = ServiceModeTestStatusDataFormat_Unused;
   testStatus.testNumber = instance.interface.testNumber;
   testStatus.diagnosticData[0] = 0;
   testStatus.diagnosticData[1] = 0;
   testStatus.diagnosticData[2] = 0;
   testStatus.diagnosticData[3] = 0;
   ServiceModeTestNumberStatusShouldBe(testStatus);
}

TEST(ServiceModeTest_BoardVersion, ShouldStopTheTimerWhenTestIsStopped)
{
   GivenTheModuleIsInitialized();

   VersionRequestShouldBeSent();
   WhenTestIsStarted();

   WhenTestIsStopped();

   VersionRequestShouldNotBeSent();
   After(VersionRequestTimeoutInMsec);
}

TEST(ServiceModeTest_BoardVersion, ShouldNotSetVersionToTheResponseAfterTestIsStopped)
{
   GivenTheModuleIsInitialized();

   VersionRequestShouldBeSent();
   WhenTestIsStarted();

   WhenTestIsStopped();
   WhenVersionResponseIsReceived(1, 2, 3, 4);

   ServiceModeTestStatus_t testStatus;
   testStatus.testResponse = ServiceModeTestStatusResponse_Stopped;
   testStatus.dataFormat = ServiceModeTestStatusDataFormat_Unused;
   testStatus.testNumber = instance.interface.testNumber;
   testStatus.diagnosticData[0] = 0;
   testStatus.diagnosticData[1] = 0;
   testStatus.diagnosticData[2] = 0;
   testStatus.diagnosticData[3] = 0;
   ServiceModeTestNumberStatusShouldBe(testStatus);
}

TEST(ServiceModeTest_BoardVersion, ShouldNotSetNoVersionInfoToTheResponseAfterTestIsStopped)
{
   GivenTheModuleIsInitialized();

   VersionRequestShouldBeSent();
   WhenTestIsStarted();

   WhenTestIsStopped();

   After(VersionRequestTimeoutInMsec);
   ServiceModeTestStatus_t testStatus;
   testStatus.testResponse = ServiceModeTestStatusResponse_Stopped;
   testStatus.dataFormat = ServiceModeTestStatusDataFormat_Unused;
   testStatus.testNumber = instance.interface.testNumber;
   testStatus.diagnosticData[0] = 0;
   testStatus.diagnosticData[1] = 0;
   testStatus.diagnosticData[2] = 0;
   testStatus.diagnosticData[3] = 0;
   ServiceModeTestNumberStatusShouldBe(testStatus);
}

TEST(ServiceModeTest_BoardVersion, ShouldAcceptLongVersionResponse)
{
   GivenTheModuleIsInitialized();

   VersionRequestShouldBeSent();
   WhenTestIsStarted();

   uint8_t longVersionResponse[LongVersionResponsePayloadLength] = { 1, 2, 0xE, 0xF, 5, 6, 7, 8, 9, 0xA, 0xB, 0xC, 0xD };
   VersionRequestShouldBeSent();
   WhenLongVersionResponseIsReceived(longVersionResponse);

   ServiceModeTestStatus_t testStatus;
   testStatus.testResponse = ServiceModeTestStatusResponse_Running;
   testStatus.dataFormat = ServiceModeTestStatusDataFormat_VersionInfo;
   testStatus.testNumber = instance.interface.testNumber;
   testStatus.diagnosticData[0] = 0xE;
   testStatus.diagnosticData[1] = 0xF;
   testStatus.diagnosticData[2] = 0;
   testStatus.diagnosticData[3] = 0;
   ServiceModeTestNumberStatusShouldBe(testStatus);
}
