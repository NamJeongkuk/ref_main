/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "RfidCommunicator.h"
#include "Gea2Addresses.h"
#include "SystemErds.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "uassert_test.h"
#include "Gea2MessageEndpoint_TestDouble.h"
#include "DataSource_NonMockTestDouble.h"
#include "ReferDataModel_TestDouble.h"

enum
{
   RfidMessageCommand = 0xEA,
   RfidBadMessageCommand = 0xBB,

   VolumeInOunces = 0x00002EE0,
   VolumeInOuncesBigEndian = 0xE02E0000,
   CalendarUsageInSeconds = 0x0000A474,
   CalendarUsageInSecondsBigEndian = 0x74A40000,
   LeakDetectedTimeInSeconds = 0x0000001E,
   LeakDetectedTimeInSecondsBigEndian = 0x1E000000,
   NumberOfWriteOperations = 0x00000190,
   NumberOfWriteOperationsBigEndian = 0x90010000,
   FilterStatusRfidBoard = 0x1234,
   FilterStatusRfidBoardBigEndian = 0x3412,
   FilterStatusMainboard = 0x12,
   InitErrorBitfield = 0b00000001,
   Lock = 1,
   NumberOfUnitsFilterHasBeenOn = 1,
   NumberOfRetries = 3,
   InvalidRequest = 3
};

const uint8_t Uid[RfidUidSizeInBytes] = "ABCDEF";
const uint8_t UnitSerialNumber[UnitSerialNumberSizeInBytes] = "ABCDEF";
const uint8_t BlankSerialNumber[UnitSerialNumberSizeInBytes] = "";
const uint8_t PreviousUnitSerialNumber[UnitSerialNumberSizeInBytes] = "ZYXWVUT";
const uint8_t FilterIdentifier[FilterIdentifierSizeInBytes] = "ABCDEFGHIJKLMNO";
const uint8_t LastTwelveMonthsOfWaterUsageInGallons[MonthlyGallonUsageSizeInBytes] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B };
const uint8_t LastTwelveMonthsOfWaterUsageInGallonsEmpty[MonthlyGallonUsageSizeInBytes] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

typedef struct
{
   ReadWriteResult_t readWriteResult;
   RfidUid_t uid;
   RfidFilterIdentifier_t filterIdentifier;
   VolumeInOunces_t volumeUsageInOunces;
   CalendarUsageInSeconds_t calendarUsageInSeconds;
   LeakDetectedInSeconds_t leakDetectedTimeInSeconds;
   NumberOfWriteOperations_t numberOfWriteOperations;
   FilterStatusRfidBoard_t filterStatus;
   RfidFilterInitializationErrorBitfield_t initErrorBitfield;
   uint8_t unused;
} LegacyReceivedPayload_t;

TEST_GROUP(RfidCommunicator)
{
   RfidCommunicator_t instance;
   ReferDataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;
   I_DataSource_t *externalDataSource;
   Gea2MessageEndpoint_TestDouble_t messageEndpointTestDouble;
   I_Gea2MessageEndpoint_t *messageEndpoint;
   RfidReceivedPayload_t receivedPayload;
   LegacyReceivedPayload_t legacyReceivedPayload;
   RfidMessagePayload_t writeMessagePayload;
   RfidMessagePayload_t readMessagePayload;

   const RfidCommunicatorRfidBoardErds_t rfidBoardErds = {
      .readWriteResult = PublicErd_RfidFilterReadWriteResult_RfidBoard,
      .filterUid = PublicErd_RfidFilterUid_RfidBoard,
      .filterIdentifier = PublicErd_RfidFilterIdentifier_RfidBoard,
      .volumeWaterUsageInOunces = PublicErd_RfidFilterWaterVolumeUsageInOunces_RfidBoard,
      .calendarUsageInSeconds = PublicErd_RfidFilterCalendarUsageInSeconds_RfidBoard,
      .leakDetectedTimeInSeconds = PublicErd_RfidFilterLeakDetectedTimeInSeconds_RfidBoard,
      .numberOfWriteOperations = PublicErd_RfidFilterNumberOfWriteOperations_RfidBoard,
      .filterStatus = PublicErd_RfidFilterStatus_RfidBoard,
      .initializationErrorBitfield = PublicErd_RfidFilterInitializationErrorBitfield_RfidBoard,
      .lockByte = PublicErd_RfidFilterLockByte_RfidBoard,
      .numberOfUnitsFilterHasBeenOn = PublicErd_RfidFilterNumberOfUnitsRfidFilterHasBeenOn_RfidBoard,
      .unitSerialNumber = PublicErd_RfidFilterUnitSerialNumber_RfidBoard,
      .previousUnitSerialNumber = PublicErd_RfidFilterPreviousUnitSerialNumber_RfidBoard,
      .filterWaterUsageInGallons = PublicErd_RfidFilterLastTwelveMonthsOfWaterUsageInGallons_RfidBoard,
   };

   const RfidCommunicatorMainboardErds_t mainboardErds = {
      .uid = PublicErd_RfidFilterUid_Mainboard,
      .volumeWaterUsageInOunces = PublicErd_RfidFilterWaterVolumeUsageInOunces_Mainboard,
      .calendarUsageInSeconds = PublicErd_RfidFilterCalendarUsageInSeconds_Mainboard,
      .filterStatus = PublicErd_RfidFilterStatus_Mainboard,
      .lockByte = PublicErd_RfidFilterLockByte_Mainboard,
      .numberOfUnitsFilterHasBeenOn = PublicErd_RfidFilterNumberOfUnitsRfidFilterHasBeenOn_Mainboard,
      .unitSerialNumber = PublicErd_RfidFilterUnitSerialNumber_Mainboard,
      .previousUnitSerialNumber = PublicErd_RfidFilterPreviousUnitSerialNumber_Mainboard,
      .filterWaterUsageInGallons = PublicErd_RfidFilterLastTwelveMonthsOfWaterUsageInGallons_Mainboard,
   };

   const RfidCommunicatorConfiguration_t config = {
      .rfidBoardErds = &rfidBoardErds,
      .mainboardErds = &mainboardErds,
      .dataRequestErd = PublicErd_RfidFilterDataRequest,
      .geaMessageEndpointErd = PublicErd_Gea2MessageEndpoint
   };

   void InitReceivedPayload(RfidReceivedPayload_t * initPayload)
   {
      initPayload->readWriteResult = ReadWriteResult_ReadFailure;
      memcpy(&initPayload->uid, &Uid, sizeof(RfidUid_t));
      memcpy(&initPayload->filterIdentifier, &FilterIdentifier, sizeof(RfidFilterIdentifier_t));
      initPayload->volumeUsageInOunces = VolumeInOuncesBigEndian;
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

   void InitLegacyReceivedPayload(LegacyReceivedPayload_t * initPayload)
   {
      initPayload->readWriteResult = ReadWriteResult_ReadFailure;
      memcpy(&initPayload->uid, &Uid, sizeof(RfidUid_t));
      memcpy(&initPayload->filterIdentifier, &FilterIdentifier, sizeof(RfidFilterIdentifier_t));
      initPayload->volumeUsageInOunces = VolumeInOuncesBigEndian;
      initPayload->calendarUsageInSeconds = CalendarUsageInSecondsBigEndian;
      initPayload->leakDetectedTimeInSeconds = LeakDetectedTimeInSecondsBigEndian;
      initPayload->numberOfWriteOperations = NumberOfWriteOperationsBigEndian;
      initPayload->filterStatus = FilterStatusRfidBoardBigEndian;
      initPayload->initErrorBitfield = InitErrorBitfield;
      initPayload->unused = 0;
   }

   void InitMessagePayloadWith(RfidMessagePayload_t * initPayload, ReadWriteRequest_t readWriteRequest)
   {
      memset(initPayload, 0, sizeof(RfidMessagePayload_t));
      initPayload->readWriteRequest = readWriteRequest;
      if(readWriteRequest == ReadWriteRequest_Write)
      {
         memcpy(&initPayload->uid, &Uid, sizeof(RfidUid_t));
         initPayload->volumeUsageInOunces = VolumeInOuncesBigEndian;
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
      ReferDataModel_TestDouble_Init(&dataModelTestDouble);
      dataModel = dataModelTestDouble.dataModel;
      DataModel_Read(dataModel, Erd_ExternalDataSource, &externalDataSource);

      Gea2MessageEndpoint_TestDouble_Init(
         &messageEndpointTestDouble,
         Gea2Address_Mainboard);
      messageEndpoint = &messageEndpointTestDouble.interface;
      DataModel_Write(dataModel, Erd_Gea2MessageEndpoint, &messageEndpoint);

      InitReceivedPayload(&receivedPayload);
      InitLegacyReceivedPayload(&legacyReceivedPayload);
      InitMessagePayloadWith(&writeMessagePayload, ReadWriteRequest_Write);
      InitMessagePayloadWith(&readMessagePayload, ReadWriteRequest_Read);
   }

   void GivenInitialization()
   {
      RfidCommunicator_Init(
         &instance,
         externalDataSource,
         &config);
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

   void WhenALegacyRfidMessageIsReceived()
   {
      STACK_ALLOC_GEA2MESSAGE(message, sizeof(LegacyReceivedPayload_t));

      Gea2Message_SetDestination(message, Gea2Address_Mainboard);
      Gea2Message_SetSource(message, Gea2Address_RfidBoard);
      Gea2Message_SetCommand(message, RfidMessageCommand);
      Gea2Message_SetPayloadLength(message, sizeof(LegacyReceivedPayload_t));

      REINTERPRET(payload, Gea2Message_GetPayload(message), LegacyReceivedPayload_t *);
      memcpy(payload, &legacyReceivedPayload, sizeof(LegacyReceivedPayload_t));

      Gea2MessageEndpoint_TestDouble_TriggerReceive(&messageEndpointTestDouble, message);
   }

   void WhenAMessageIsReceivedFromANonRfidBoard()
   {
      STATIC_ALLOC_GEA2MESSAGE(message, sizeof(RfidReceivedPayload_t));

      Gea2Message_SetDestination(message, Gea2Address_Mainboard);
      Gea2Message_SetSource(message, Gea2Address_Dispenser);
      Gea2Message_SetCommand(message, RfidMessageCommand);
      Gea2Message_SetPayloadLength(message, sizeof(RfidReceivedPayload_t));

      REINTERPRET(payload, Gea2Message_GetPayload(message), RfidReceivedPayload_t *);
      memcpy(payload, &receivedPayload, sizeof(RfidReceivedPayload_t));

      Gea2MessageEndpoint_TestDouble_TriggerReceive(&messageEndpointTestDouble, message);
   }

   void WhenAMessageIsReceivedFromARfidBoardThatIsNotTheMessageCommand()
   {
      STATIC_ALLOC_GEA2MESSAGE(message, sizeof(RfidReceivedPayload_t));

      Gea2Message_SetDestination(message, Gea2Address_Mainboard);
      Gea2Message_SetSource(message, Gea2Address_RfidBoard);
      Gea2Message_SetCommand(message, RfidBadMessageCommand);
      Gea2Message_SetPayloadLength(message, sizeof(RfidReceivedPayload_t));

      REINTERPRET(payload, Gea2Message_GetPayload(message), RfidReceivedPayload_t *);
      memcpy(payload, &receivedPayload, sizeof(RfidReceivedPayload_t));

      Gea2MessageEndpoint_TestDouble_TriggerReceive(&messageEndpointTestDouble, message);
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

   void NothingShouldHappen()
   {
   }

   void WhenADataRequestMessageHasBeenSentWith(ReadWriteRequest_t readWriteRequest)
   {
      DataModel_Write(dataModel, Erd_RfidFilterDataRequest, &readWriteRequest);
   }

   void GivenMainboardRfidErdsAreInitialized()
   {
      VolumeInOunces_t volumeOunces = VolumeInOunces;
      CalendarUsageInSeconds_t calendarUsage = CalendarUsageInSeconds;
      RfidFilterStatusMainboard_t filterStatus = FilterStatusMainboard;
      RfidFilterNumberOfUnitsFilterHasBeenOn_t numberOfUnitsFilterHasBeenOn = NumberOfUnitsFilterHasBeenOn;

      DataModel_Write(dataModel, Erd_RfidFilterUid_Mainboard, &Uid);
      DataModel_Write(dataModel, Erd_RfidFilterWaterVolumeUsageInOunces_Mainboard, &volumeOunces);
      DataModel_Write(dataModel, Erd_RfidFilterCalendarUsageInSeconds_Mainboard, &calendarUsage);
      DataModel_Write(dataModel, Erd_RfidFilterStatus_Mainboard, &filterStatus);
      uint8_t lockByte = Lock;
      DataModel_Write(dataModel, Erd_RfidFilterLockByte_Mainboard, &lockByte);
      DataModel_Write(dataModel, Erd_RfidFilterNumberOfUnitsRfidFilterHasBeenOn_Mainboard, &numberOfUnitsFilterHasBeenOn);
      DataModel_Write(dataModel, Erd_RfidFilterUnitSerialNumber_Mainboard, &UnitSerialNumber);
      DataModel_Write(dataModel, Erd_RfidFilterPreviousUnitSerialNumber_Mainboard, &PreviousUnitSerialNumber);
      DataModel_Write(dataModel, Erd_RfidFilterLastTwelveMonthsOfWaterUsageInGallons_Mainboard, &LastTwelveMonthsOfWaterUsageInGallons);
   }

   void RfidFilterReadWriteResultShouldBe(RfidFilterReadWriteResult_t expected)
   {
      RfidFilterReadWriteResult_t actual;
      DataModel_Read(dataModel, Erd_RfidFilterReadWriteResult_RfidBoard, &actual);
      MEMCMP_EQUAL(&expected, &actual, sizeof(RfidFilterReadWriteResult_t));
   }

   void RfidFilterUidShouldBe(RfidUid_t expected)
   {
      RfidUid_t actual;
      DataModel_Read(dataModel, Erd_RfidFilterUid_RfidBoard, &actual);
      MEMCMP_EQUAL(&expected, &actual, sizeof(RfidUid_t));
   }

   void RfidFilterIdentifierShouldBe(RfidFilterIdentifier_t expected)
   {
      RfidFilterIdentifier_t actual;
      DataModel_Read(dataModel, Erd_RfidFilterIdentifier_RfidBoard, &actual);
      MEMCMP_EQUAL(&expected, &actual, sizeof(RfidFilterIdentifier_t));
   }

   void RfidFilterVolumeOuncesShouldBe(VolumeInOunces_t expected)
   {
      VolumeInOunces_t actual;
      DataModel_Read(dataModel, Erd_RfidFilterWaterVolumeUsageInOunces_RfidBoard, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void RfidFilterCalendarUsageShouldBe(CalendarUsageInSeconds_t expected)
   {
      CalendarUsageInSeconds_t actual;
      DataModel_Read(dataModel, Erd_RfidFilterCalendarUsageInSeconds_RfidBoard, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void RfidFilterLeakDetectedTimeShouldBe(LeakDetectedInSeconds_t expected)
   {
      LeakDetectedInSeconds_t actual;
      DataModel_Read(dataModel, Erd_RfidFilterLeakDetectedTimeInSeconds_RfidBoard, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void RfidFilterNumberOfWriteOperationsShouldBe(NumberOfWriteOperations_t expected)
   {
      NumberOfWriteOperations_t actual;
      DataModel_Read(dataModel, Erd_RfidFilterNumberOfWriteOperations_RfidBoard, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void RfidFilterStatusShouldBe(FilterStatusRfidBoard_t expected)
   {
      FilterStatusRfidBoard_t actual;
      DataModel_Read(dataModel, Erd_RfidFilterStatus_RfidBoard, &actual);
      MEMCMP_EQUAL(&expected, &actual, sizeof(FilterStatusRfidBoard_t));
   }

   void RfidFilterInitializationErrorBitfieldShouldBe(RfidFilterInitializationErrorBitfield_t expected)
   {
      RfidFilterInitializationErrorBitfield_t actual;
      DataModel_Read(dataModel, Erd_RfidFilterInitializationErrorBitfield_RfidBoard, &actual);
      MEMCMP_EQUAL(&expected, &actual, sizeof(RfidFilterInitializationErrorBitfield_t));
   }

   void RfidFilterLockByteShouldBe(uint8_t expected)
   {
      uint8_t actual;
      DataModel_Read(dataModel, Erd_RfidFilterLockByte_RfidBoard, &actual);
      MEMCMP_EQUAL(&expected, &actual, sizeof(uint8_t));
   }

   void RfidFilterNumberOfUnitsFilterHasBeenOnShouldBe(RfidFilterNumberOfUnitsFilterHasBeenOn_t expected)
   {
      RfidFilterNumberOfUnitsFilterHasBeenOn_t actual;
      DataModel_Read(dataModel, Erd_RfidFilterNumberOfUnitsRfidFilterHasBeenOn_RfidBoard, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void RfidFilterUnitSerialNumberShouldBe(UnitSerialNumber_t expected)
   {
      UnitSerialNumber_t actual;
      DataModel_Read(dataModel, Erd_RfidFilterUnitSerialNumber_RfidBoard, &actual);
      MEMCMP_EQUAL(&expected, &actual, sizeof(UnitSerialNumber_t));
   }

   void RfidFilterPreviousUnitSerialNumberShouldBe(UnitSerialNumber_t expected)
   {
      UnitSerialNumber_t actual;
      DataModel_Read(dataModel, Erd_RfidFilterPreviousUnitSerialNumber_RfidBoard, &actual);
      MEMCMP_EQUAL(&expected, &actual, sizeof(UnitSerialNumber_t));
   }

   void RfidFilterWaterUsageInGallonsShouldBe(RfidTwelveMonthsGallonUsage_t expected)
   {
      RfidTwelveMonthsGallonUsage_t actual;
      DataModel_Read(dataModel, Erd_RfidFilterLastTwelveMonthsOfWaterUsageInGallons_RfidBoard, &actual);
      MEMCMP_EQUAL(&expected, &actual, sizeof(RfidTwelveMonthsGallonUsage_t));
   }
};

TEST(RfidCommunicator, ShouldStoreRfidFilterReadWriteResultDataOnMainboardWhenAMessageIsReceived)
{
   GivenInitialization();
   WhenAnRfidMessageIsReceived();

   RfidFilterReadWriteResult_t readWriteResultExpected = {
      .result = ReadWriteResult_ReadFailure,
      .signal = 1
   };
   RfidFilterReadWriteResultShouldBe(readWriteResultExpected);
}

TEST(RfidCommunicator, ShouldStoreRfidFilterUidDataOnMainboardWhenAMessageIsReceived)
{
   GivenInitialization();
   WhenAnRfidMessageIsReceived();

   RfidUid_t uidExpected;
   memcpy(&uidExpected, &Uid, sizeof(RfidUid_t));

   RfidFilterUidShouldBe(uidExpected);
}

TEST(RfidCommunicator, ShouldStoreRfidFilterIdentifierDataOnMainboardWhenAMessageIsReceived)
{
   GivenInitialization();
   WhenAnRfidMessageIsReceived();

   RfidFilterIdentifier_t filterIdentifierExpected;
   memcpy(&filterIdentifierExpected, &FilterIdentifier, sizeof(RfidFilterIdentifier_t));

   RfidFilterIdentifierShouldBe(filterIdentifierExpected);
}

TEST(RfidCommunicator, ShouldStoreRfidFilterVolumeUsageDataOnMainboardWhenAMessageIsReceived)
{
   GivenInitialization();
   WhenAnRfidMessageIsReceived();

   RfidFilterVolumeOuncesShouldBe(VolumeInOunces);
}

TEST(RfidCommunicator, ShouldStoreRfidFilterCalendarUsageDataOnMainboardWhenAMessageIsReceived)
{
   GivenInitialization();
   WhenAnRfidMessageIsReceived();

   RfidFilterCalendarUsageShouldBe(CalendarUsageInSeconds);
}

TEST(RfidCommunicator, ShouldStoreRfidFilterLeakDetectedTimeDataOnMainboardWhenAMessageIsReceived)
{
   GivenInitialization();
   WhenAnRfidMessageIsReceived();

   RfidFilterLeakDetectedTimeShouldBe(LeakDetectedTimeInSeconds);
}

TEST(RfidCommunicator, ShouldStoreRfidFilterNumberOfWriteOperationsDataOnMainboardWhenAMessageIsReceived)
{
   GivenInitialization();
   WhenAnRfidMessageIsReceived();

   RfidFilterNumberOfWriteOperationsShouldBe(NumberOfWriteOperations);
}

TEST(RfidCommunicator, ShouldStoreRfidFilterStatusDataOnMainboardWhenAMessageIsReceived)
{
   GivenInitialization();
   WhenAnRfidMessageIsReceived();

   RfidFilterStatusShouldBe(FilterStatusRfidBoard);
}

TEST(RfidCommunicator, ShouldStoreRfidFilterInitializationErrorBitfieldDataOnMainboardWhenAMessageIsReceived)
{
   GivenInitialization();
   WhenAnRfidMessageIsReceived();

   RfidFilterInitializationErrorBitfieldShouldBe(InitErrorBitfield);
}

TEST(RfidCommunicator, ShouldStoreRfidFilterLockByteDataOnMainboardWhenAMessageIsReceived)
{
   GivenInitialization();
   WhenAnRfidMessageIsReceived();

   RfidFilterLockByteShouldBe(Lock);
}

TEST(RfidCommunicator, ShouldStoreRfidFilterNumberOfUnitsFilterHasBeenOnDataOnMainboardWhenAMessageIsReceived)
{
   GivenInitialization();
   WhenAnRfidMessageIsReceived();

   RfidFilterNumberOfUnitsFilterHasBeenOnShouldBe(NumberOfUnitsFilterHasBeenOn);
}

TEST(RfidCommunicator, ShouldStoreRfidFilterUnitSerialNumberDataOnMainboardWhenAMessageIsReceived)
{
   GivenInitialization();
   WhenAnRfidMessageIsReceived();

   UnitSerialNumber_t unitSerialNumberExpected;
   memcpy(&unitSerialNumberExpected, &UnitSerialNumber, sizeof(UnitSerialNumber_t));

   RfidFilterUnitSerialNumberShouldBe(unitSerialNumberExpected);
}

TEST(RfidCommunicator, ShouldStoreRfidFilterPreviousUnitSerialNumberDataOnMainboardWhenAMessageIsReceived)
{
   GivenInitialization();
   WhenAnRfidMessageIsReceived();

   UnitSerialNumber_t previousUnitSerialNumberExpected;
   memcpy(&previousUnitSerialNumberExpected, &PreviousUnitSerialNumber, sizeof(UnitSerialNumber_t));

   RfidFilterPreviousUnitSerialNumberShouldBe(previousUnitSerialNumberExpected);
}

TEST(RfidCommunicator, ShouldStoreRfidFilterWaterUsageInGallonsDataOnMainboardWhenAMessageIsReceived)
{
   GivenInitialization();
   WhenAnRfidMessageIsReceived();

   RfidTwelveMonthsGallonUsage_t waterUsageInGallonsExpected;
   memcpy(&waterUsageInGallonsExpected, &LastTwelveMonthsOfWaterUsageInGallons, sizeof(RfidTwelveMonthsGallonUsage_t));

   RfidFilterWaterUsageInGallonsShouldBe(waterUsageInGallonsExpected);
}

TEST(RfidCommunicator, ShouldSendWriteRequestMessageToRfidBoardWhenRequested)
{
   GivenMainboardRfidErdsAreInitialized();
   GivenInitialization();
   AMessageShouldBeSent(writeMessagePayload);
   WhenADataRequestMessageHasBeenSentWith(ReadWriteRequest_Write);
}

TEST(RfidCommunicator, ShouldSendReadRequestMessageToRfidBoardWhenRequested)
{
   GivenMainboardRfidErdsAreInitialized();
   GivenInitialization();
   AMessageShouldBeSent(readMessagePayload);
   WhenADataRequestMessageHasBeenSentWith(ReadWriteRequest_Read);
}

TEST(RfidCommunicator, ShouldNotSendAMessageWhenAnInvalidDataRequestIsSent)
{
   GivenMainboardRfidErdsAreInitialized();
   GivenInitialization();
   NothingShouldHappen();
   WhenADataRequestMessageHasBeenSentWith(InvalidRequest);
}

TEST(RfidCommunicator, ShouldBeAbleToSendMultipleReadRequests)
{
   GivenMainboardRfidErdsAreInitialized();
   GivenInitialization();
   AMessageShouldBeSent(readMessagePayload);
   WhenADataRequestMessageHasBeenSentWith(ReadWriteRequest_Read);
   AMessageShouldBeSent(readMessagePayload);
   WhenADataRequestMessageHasBeenSentWith(ReadWriteRequest_Read);
}

TEST(RfidCommunicator, ShouldOnlyAcceptMessagesFromTheRfidBoard)
{
   GivenInitialization();

   RfidFilterReadWriteResult_t readWriteResultExpected = {
      .result = ReadWriteResult_Success,
      .signal = 0
   };
   RfidFilterReadWriteResultShouldBe(readWriteResultExpected);

   WhenAMessageIsReceivedFromANonRfidBoard();
   RfidFilterReadWriteResultShouldBe(readWriteResultExpected);
}

TEST(RfidCommunicator, ShouldOnlyAcceptMessagesThatAreOfRfidMessageCommand)
{
   GivenInitialization();

   RfidFilterReadWriteResult_t readWriteResultExpected = {
      .result = ReadWriteResult_Success,
      .signal = 0
   };
   RfidFilterReadWriteResultShouldBe(readWriteResultExpected);

   WhenAMessageIsReceivedFromARfidBoardThatIsNotTheMessageCommand();
   RfidFilterReadWriteResultShouldBe(readWriteResultExpected);
}

TEST(RfidCommunicator, ShouldAcceptLegacyMessages)
{
   GivenInitialization();
   WhenALegacyRfidMessageIsReceived();

   RfidFilterIdentifier_t filterIdentifierExpected;
   memcpy(&filterIdentifierExpected, &FilterIdentifier, sizeof(RfidFilterIdentifier_t));

   RfidFilterIdentifierShouldBe(filterIdentifierExpected);
   RfidFilterVolumeOuncesShouldBe(VolumeInOunces);
   RfidFilterCalendarUsageShouldBe(CalendarUsageInSeconds);
   RfidFilterLeakDetectedTimeShouldBe(LeakDetectedTimeInSeconds);
   RfidFilterNumberOfWriteOperationsShouldBe(NumberOfWriteOperations);
   RfidFilterStatusShouldBe(FilterStatusRfidBoard);
   RfidFilterInitializationErrorBitfieldShouldBe(InitErrorBitfield);
   RfidFilterNumberOfUnitsFilterHasBeenOnShouldBe(0);

   UnitSerialNumber_t unitSerialNumberExpected;
   memcpy(&unitSerialNumberExpected, &BlankSerialNumber, sizeof(UnitSerialNumber_t));
   RfidFilterUnitSerialNumberShouldBe(unitSerialNumberExpected);

   UnitSerialNumber_t previousUnitSerialNumberExpected;
   memcpy(&previousUnitSerialNumberExpected, &BlankSerialNumber, sizeof(UnitSerialNumber_t));
   RfidFilterPreviousUnitSerialNumberShouldBe(previousUnitSerialNumberExpected);

   RfidTwelveMonthsGallonUsage_t waterUsageInGallonsExpected;
   memcpy(&waterUsageInGallonsExpected, &LastTwelveMonthsOfWaterUsageInGallonsEmpty, sizeof(RfidTwelveMonthsGallonUsage_t));
   RfidFilterWaterUsageInGallonsShouldBe(waterUsageInGallonsExpected);
}
