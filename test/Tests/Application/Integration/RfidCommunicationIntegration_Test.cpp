/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "Application.h"
#include "Gea2Addresses.h"
#include "SystemErds.h"
#include "Constants_Binary.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "uassert_test.h"
#include "ReferDataModel_TestDouble.h"
#include "Gea2MessageEndpoint_TestDouble.h"

enum
{
   RfidMessageCommand = 0xEA,
   WaitForEepromWritesToCompleteTimeInMsec = 1000,
   VolumeInOuncesBigEndian = 0xE02E0000,
   CalendarUsageInSecondsBigEndian = 0x74A40000,
   LeakDetectedTimeInSecondsBigEndian = 0x1E000000,
   NumberOfWriteOperationsBigEndian = 0x90010000,
   FilterStatusRfidBoardBigEndian = 0x3412,
   FilterStatusMainboard = 0x12,
   InitErrorBitfield = 0b00000001,
   Lock = 1,
   NumberOfUnitsFilterHasBeenOn = 1,
   NumberOfRetries = 3,
};

const uint8_t Uid[RfidUidSizeInBytes] = "ABCDEF";
const uint8_t GivenUnitSerialNumber[32] = "123456789ABCDEFG";
const uint8_t GivenUnitSerialNumberFirstEightBytes[UnitSerialNumberSizeInBytes] = { '1', '2', '3', '4', '5', '6', '7', '8' };
const uint8_t UnitSerialNumber[UnitSerialNumberSizeInBytes] = "ABCDEF";
const uint8_t BlankSerialNumber[UnitSerialNumberSizeInBytes] = "";
const uint8_t PreviousUnitSerialNumber[UnitSerialNumberSizeInBytes] = "ZYXWVUT";
const uint8_t FilterIdentifier[FilterIdentifierSizeInBytes] = "ABCDEFGHIJKLMNO";
const uint8_t LastTwelveMonthsOfWaterUsageInGallons[MonthlyGallonUsageSizeInBytes] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B };
const uint8_t LastTwelveMonthsOfWaterUsageInGallonsEmpty[MonthlyGallonUsageSizeInBytes] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

TEST_GROUP(RfidCommunicationIntegration)
{
   Application_t instance;
   ReferDataModel_TestDouble_t dataModelDouble;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   RfidReceivedPayload_t receivedPayload;
   I_Gea2MessageEndpoint_t *messageEndpoint;
   Gea2MessageEndpoint_TestDouble_t messageEndpointTestDouble;
   I_DataModel_t *dataModel;
   ResetReason_t resetReason;
   RfidMessagePayload_t readMessagePayload;

   void InitReceivedPayload(RfidReceivedPayload_t * initPayload)
   {
      initPayload->readWriteResult = ReadWriteResult_ReadFailure;
      memcpy(&initPayload->uid, &Uid, sizeof(RfidUid_t));
      memcpy(&initPayload->filterIdentifier, &FilterIdentifier, sizeof(RfidFilterIdentifier_t));
      initPayload->volumeUsageInOuncesX100 = VolumeInOuncesBigEndian;
      initPayload->calendarUsageInSeconds = CalendarUsageInSecondsBigEndian;
      initPayload->leakDetectedTimeInSeconds = LeakDetectedTimeInSecondsBigEndian;
      initPayload->numberOfWriteOperations = NumberOfWriteOperationsBigEndian;
      initPayload->filterStatus = FilterStatusRfidBoardBigEndian;
      initPayload->initErrorBitfield = InitErrorBitfield;
      initPayload->lock = Lock;
      initPayload->numberOfUnitsFilterHasBeenOn = NumberOfUnitsFilterHasBeenOn;
      memcpy(&initPayload->unitSerialNumber, &UnitSerialNumber, sizeof(UnitSerialNumber_t));
      memcpy(&initPayload->previousUnitSerialNumber, &PreviousUnitSerialNumber, sizeof(UnitSerialNumber_t));
      memcpy(&initPayload->lastTwelveMonthsOfWaterUsageInGallons, &LastTwelveMonthsOfWaterUsageInGallons, sizeof(RfidTwelveMonthsGallonUsage_t));
   }

   void InitMessagePayloadWith(RfidMessagePayload_t * initPayload, ReadWriteRequest_t readWriteRequest)
   {
      memset(initPayload, 0, sizeof(RfidMessagePayload_t));
      initPayload->readWriteRequest = readWriteRequest;
      if(readWriteRequest == ReadWriteRequest_Write)
      {
         memcpy(&initPayload->uid, &Uid, sizeof(RfidUid_t));
         initPayload->volumeUsageInOuncesX100 = VolumeInOuncesBigEndian;
         initPayload->calendarUsageInSeconds = CalendarUsageInSecondsBigEndian;
         initPayload->filterStatus = FilterStatusMainboard;
         initPayload->lock = Lock;
         initPayload->numberOfUnitsFilterHasBeenOn = NumberOfUnitsFilterHasBeenOn;
         memcpy(&initPayload->unitSerialNumber, &UnitSerialNumber, sizeof(UnitSerialNumber_t));
         memcpy(&initPayload->previousUnitSerialNumber, &PreviousUnitSerialNumber, sizeof(UnitSerialNumber_t));
         memcpy(&initPayload->lastTwelveMonthsOfWaterUsageInGallons, &LastTwelveMonthsOfWaterUsageInGallons, sizeof(RfidTwelveMonthsGallonUsage_t));
      }
   }

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      Gea2MessageEndpoint_TestDouble_Init(
         &messageEndpointTestDouble,
         Gea2Address_Mainboard);
      messageEndpoint = &messageEndpointTestDouble.interface;
      DataModel_Write(dataModel, Erd_Gea2MessageEndpoint, &messageEndpoint);
      InitReceivedPayload(&receivedPayload);
      InitMessagePayloadWith(&readMessagePayload, ReadWriteRequest_Read);
   }

   void GivenApplicationHasBeenInitialized()
   {
      Application_Init(&instance, dataModel, resetReason);
   }

   void WhenAnRfidMessageIsReceived()
   {
      STACK_ALLOC_GEA2MESSAGE(message, sizeof(RfidReceivedPayload_t));

      Gea2Message_SetDestination(message, Gea2Address_Mainboard);
      Gea2Message_SetSource(message, Gea2Address_RfidBoard);
      Gea2Message_SetCommand(message, RfidMessageCommand);
      Gea2Message_SetPayloadLength(message, sizeof(RfidReceivedPayload_t));

      REINTERPRET(payload, Gea2Message_GetPayload(message), RfidReceivedPayload_t *);
      memcpy(payload, &receivedPayload, sizeof(RfidReceivedPayload_t));

      Gea2MessageEndpoint_TestDouble_TriggerReceive(&messageEndpointTestDouble, message);
   }

   void RfidFilterReadWriteResultShouldBe(RfidFilterReadWriteResult_t expected)
   {
      RfidFilterReadWriteResult_t actual;
      DataModel_Read(dataModel, Erd_RfidFilterReadWriteResult_RfidBoard, &actual);
      MEMCMP_EQUAL(&expected, &actual, sizeof(RfidFilterReadWriteResult_t));
   }

   void GivenTheRfidBoardIsInTheSystem()
   {
      DataModel_Write(dataModel, Erd_RfidBoardInSystem, set);
   }

   void WhenTheRfidBoardIsInTheSystem()
   {
      GivenTheRfidBoardIsInTheSystem();
   }

   void GivenTheRfidBoardIsNotInTheSystem()
   {
      DataModel_Write(dataModel, Erd_RfidBoardInSystem, clear);
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks);
   }

   void AMessageShouldBeSent(RfidMessagePayload_t messagePayload)
   {
      STATIC_ALLOC_GEA2MESSAGE(message, sizeof(RfidMessagePayload_t));

      Gea2Message_SetDestination(message, Gea2Address_RfidBoard);
      Gea2Message_SetSource(message, Gea2Address_Mainboard);
      Gea2Message_SetCommand(message, RfidMessageCommand);
      Gea2Message_SetPayloadLength(message, sizeof(RfidMessagePayload_t));

      REINTERPRET(payload, Gea2Message_GetPayload(message), RfidMessagePayload_t *);
      memcpy(payload, &messagePayload, sizeof(RfidMessagePayload_t));

      mock()
         .expectOneCall("Send")
         .onObject(messageEndpoint)
         .withParameterOfType("Gea2Message_t", "message", message)
         .withParameter("retries", NumberOfRetries);
   }

   void GivenUnitSerialNumberIs(const uint8_t *unitSerialNumber)
   {
      DataModel_Write(dataModel, Erd_SerialNumber, unitSerialNumber);
   }

   void RfidFilterUnitSerialNumberMainboardShouldBe(const uint8_t *expected)
   {
      UnitSerialNumber_t actual;
      DataModel_Read(dataModel, Erd_RfidFilterUnitSerialNumber, &actual);
      MEMCMP_EQUAL(expected, &actual, sizeof(UnitSerialNumber_t));
   }
};

TEST(RfidCommunicationIntegration, ShouldStoreRfidFilterReadWriteResultDataOnMainboardWhenAMessageIsReceivedWhenRfidBoardIsInTheSystem)
{
   GivenTheRfidBoardIsInTheSystem();
   After(WaitForEepromWritesToCompleteTimeInMsec);

   AMessageShouldBeSent(readMessagePayload);
   GivenApplicationHasBeenInitialized();
   WhenAnRfidMessageIsReceived();

   RfidFilterReadWriteResult_t readWriteResultExpected = {
      .result = ReadWriteResult_ReadFailure,
      .signal = 1
   };
   RfidFilterReadWriteResultShouldBe(readWriteResultExpected);
}

TEST(RfidCommunicationIntegration, ShouldNotStoreRfidFilterReadWriteResultDataOnMainboardWhenAMessageIsReceivedWhenRfidBoardIsNotInTheSystem)
{
   GivenTheRfidBoardIsNotInTheSystem();
   After(WaitForEepromWritesToCompleteTimeInMsec);

   GivenApplicationHasBeenInitialized();
   WhenAnRfidMessageIsReceived();

   RfidFilterReadWriteResult_t readWriteResultExpected = {
      .result = 0,
      .signal = 0
   };
   RfidFilterReadWriteResultShouldBe(readWriteResultExpected);
}

TEST(RfidCommunicationIntegration, ShouldStoreRfidFilterReadWriteResultDataOnMainboardWhenAMessageIsReceivedAfterRfidBoardIsDiscoveredInTheSystem)
{
   GivenTheRfidBoardIsNotInTheSystem();
   After(WaitForEepromWritesToCompleteTimeInMsec);

   AMessageShouldBeSent(readMessagePayload);
   GivenApplicationHasBeenInitialized();

   WhenTheRfidBoardIsInTheSystem();
   WhenAnRfidMessageIsReceived();

   RfidFilterReadWriteResult_t readWriteResultExpected = {
      .result = ReadWriteResult_ReadFailure,
      .signal = 1
   };
   RfidFilterReadWriteResultShouldBe(readWriteResultExpected);
}

TEST(RfidCommunicationIntegration, ShouldCopyTheFirstEightBytesOfTheUnitSerialNumberToMainboardSerialNumberUponInit)
{
   GivenTheRfidBoardIsInTheSystem();
   GivenUnitSerialNumberIs(GivenUnitSerialNumber);

   AMessageShouldBeSent(readMessagePayload);
   GivenApplicationHasBeenInitialized();

   RfidFilterUnitSerialNumberMainboardShouldBe(GivenUnitSerialNumberFirstEightBytes);
}
