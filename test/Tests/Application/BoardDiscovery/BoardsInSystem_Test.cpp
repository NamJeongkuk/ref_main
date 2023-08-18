/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "BoardsInSystem.h"
#include "Constants_Binary.h"
#include "Gea2CommonCommands.h"
#include "Gea2Addresses.h"
#include "utils.h"
#include "Constants_Time.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "Gea2MessageEndpoint_TestDouble.h"
#include "uassert_test.h"

enum
{
   NumberRetryRequests = 3,
   NumberOfGea2MessageRetries = 2,
   ADifferentGea2Command = 3,
   AnUnknownGeaAddress = 0x15,
   ARetryPeriodInSeconds = 3,
   AnInitialDelayPeriodInSeconds = 4
};

#define FOR_EACH_GEA_ADDRESS_PAIR_INDEX_STARTING_AT(_indexName, _startIndex) \
   for(uint8_t _indexName = _startIndex; _indexName < config.addressToErdMapArrayNumberEntries; _indexName++)

static const GeaAddressToNvInSystemMap_t geaErdEntries[] = {
   { Gea2Address_Dispenser, Erd_DispenserUiInSystem },
   { Gea2Address_RfidBoard, Erd_RfidBoardInSystem },
   { Gea2Address_AndroidSbc, Erd_AndroidSbcBoardInSystem },
   { Gea2Address_DoorBoard, Erd_DoorBoardInSystem },
   { Gea2Address_EmbeddedWiFi, Erd_EmbeddedWifiInSystem }
};

static const BoardsInSystemConfig_t config = {
   .addressToErdMapArray = geaErdEntries,
   .addressToErdMapArrayNumberEntries = NUM_ELEMENTS(geaErdEntries),
   .retryPeriodInSec = ARetryPeriodInSeconds,
   .initialDelayTimeInSec = AnInitialDelayPeriodInSeconds,
   .numberOfRetryRequests = NumberRetryRequests,
   .geaMessageEndpointErd = Erd_Gea2MessageEndpoint,
   .TimerModuleErd = Erd_TimerModule,
};

STATIC_ALLOC_GEA2MESSAGE(expectedDispenserUiRequestMessage, 0);
STATIC_ALLOC_GEA2MESSAGE(expectedRfidBoardRequestMessage, 0);
STATIC_ALLOC_GEA2MESSAGE(expectedAndroidSbcRequestMessage, 0);
STATIC_ALLOC_GEA2MESSAGE(expectedDoorBoardRequestMessage, 0);
STATIC_ALLOC_GEA2MESSAGE(expectedMgmTempBoardRequestMessage, 0);
STATIC_ALLOC_GEA2MESSAGE(expectedDeliPanRequestMessage, 0);
STATIC_ALLOC_GEA2MESSAGE(expectedComRequestMessage, 0);
STATIC_ALLOC_GEA2MESSAGE(expectedEmbeddedWiFiRequestMessage, 0);

static Gea2Message_t *const expectedGea2RequestMessages[] = {
   expectedDispenserUiRequestMessage,
   expectedRfidBoardRequestMessage,
   expectedAndroidSbcRequestMessage,
   expectedDoorBoardRequestMessage,
   expectedMgmTempBoardRequestMessage,
   expectedDeliPanRequestMessage,
   expectedComRequestMessage,
   expectedEmbeddedWiFiRequestMessage,
};

STATIC_ALLOC_GEA2MESSAGE(expectedRfidBoardResponseMessage, 4);
STATIC_ALLOC_GEA2MESSAGE(expectedDispenserUiResponseMessage, 4);
STATIC_ALLOC_GEA2MESSAGE(expectedAndroidSbcResponseMessage, 4);
STATIC_ALLOC_GEA2MESSAGE(expectedDoorBoardResponseMessage, 4);
STATIC_ALLOC_GEA2MESSAGE(expectedMgmTempBoardResponseMessage, 4);
STATIC_ALLOC_GEA2MESSAGE(expectedDeliPanResponseMessage, 4);
STATIC_ALLOC_GEA2MESSAGE(expectedComResponseMessage, 4);
STATIC_ALLOC_GEA2MESSAGE(expectedEmbeddedWiFiResponseMessage, 4);

static Gea2Message_t *const expectedGea2ResponseMessages[] = {
   expectedRfidBoardResponseMessage,
   expectedDispenserUiResponseMessage,
   expectedAndroidSbcResponseMessage,
   expectedDoorBoardResponseMessage,
   expectedMgmTempBoardResponseMessage,
   expectedDeliPanResponseMessage,
   expectedComResponseMessage,
   expectedEmbeddedWiFiResponseMessage,
};

STATIC_ALLOC_GEA2MESSAGE(messageWithUnkownGeaAddress, 4);

TEST_GROUP(BoardsInSystem)
{
   BoardsInSystem_t instance;

   ReferDataModel_TestDouble_t referDataModelTestDouble;
   I_DataModel_t *dataModel;
   Gea2MessageEndpoint_TestDouble_t messageEndpointTestDouble;
   I_Gea2MessageEndpoint_t *messageEndpoint;
   TimerModule_TestDouble_t *timerModuleDouble;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&referDataModelTestDouble);
      dataModel = referDataModelTestDouble.dataModel;

      timerModuleDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&referDataModelTestDouble);

      Gea2MessageEndpoint_TestDouble_Init(
         &messageEndpointTestDouble,
         Gea2Address_Mainboard);

      messageEndpoint = &messageEndpointTestDouble.interface;

      DataModel_Write(
         dataModel,
         Erd_Gea2MessageEndpoint,
         &messageEndpoint);
   }

   void GivenTheModuleIsInitialized()
   {
      BoardsInSystem_Init(
         &instance,
         dataModel,
         &config);
   }

   void WhenTheModuleIsInitializedAndInitialDelayPeriodExpires()
   {
      GivenTheModuleIsInitializedAndInitialDelayPeriodExpires();
   }

   void GivenTheModuleIsInitializedAndInitialDelayPeriodExpires()
   {
      GivenTheModuleIsInitialized();
      After(AnInitialDelayPeriodInSeconds * MSEC_PER_SEC);
   }

   void GeaMessageShouldBeRequestedForIndex(uint8_t command, uint8_t index)
   {
      Gea2Message_SetDestination(expectedGea2RequestMessages[index], config.addressToErdMapArray[index].geaAddress);
      Gea2Message_SetCommand(expectedGea2RequestMessages[index], command);
      Gea2Message_SetSource(expectedGea2RequestMessages[index], Gea2Address_Mainboard);

      mock()
         .expectOneCall("Send")
         .onObject(messageEndpoint)
         .withParameterOfType("Gea2Message_t", "message", expectedGea2RequestMessages[index])
         .withParameter("retries", NumberOfGea2MessageRetries);
   }

   void ShouldSendVersionRequestForEachBoardAddressInArray()
   {
      FOR_EACH_GEA_ADDRESS_PAIR_INDEX_STARTING_AT(i, 0)
      {
         GeaMessageShouldBeRequestedForIndex(Gea2CommonCommand_Version, i);
      }
   }

   void GivenNvErdIsSetForIndex(uint8_t index)
   {
      DataModel_Write(
         dataModel,
         config.addressToErdMapArray[index].nvErd,
         set);
   }

   void GivenNvErdIsSetForAllIndices()
   {
      FOR_EACH_GEA_ADDRESS_PAIR_INDEX_STARTING_AT(i, 0)
      {
         GivenNvErdIsSetForIndex(i);
      }
   }

   void NvErdForIndexShouldBe(uint8_t index, bool expected)
   {
      bool actual;
      DataModel_Read(
         dataModel,
         config.addressToErdMapArray[index].nvErd,
         &actual);

      CHECK_EQUAL(expected, actual);
   }

   void NothingShouldHappen()
   {
      mock().expectNoCall("none");
   }

   void WhenBoardWithGeaAddressIndexRespondsToVersionRequest(uint8_t geaAddressIndex)
   {
      Gea2Message_SetDestination(expectedGea2ResponseMessages[geaAddressIndex], Gea2Address_Mainboard);
      Gea2Message_SetSource(expectedGea2ResponseMessages[geaAddressIndex], config.addressToErdMapArray[geaAddressIndex].geaAddress);
      Gea2Message_SetCommand(expectedGea2ResponseMessages[geaAddressIndex], Gea2CommonCommand_Version);
      uint8_t *payload = Gea2Message_GetPayload(expectedGea2ResponseMessages[geaAddressIndex]);
      payload[0] = 0xDE;
      payload[1] = 0xAD;
      payload[2] = 0xBE;
      payload[3] = 0xEF;

      Gea2MessageEndpoint_TestDouble_TriggerReceive(&messageEndpointTestDouble, expectedGea2ResponseMessages[geaAddressIndex]);
   }

   void WhenBoardWithGeaAddressIndexRespondsWithNonVersionRequestGeaCommand(uint8_t geaAddressIndex)
   {
      Gea2Message_SetDestination(expectedGea2ResponseMessages[geaAddressIndex], Gea2Address_Mainboard);
      Gea2Message_SetSource(expectedGea2ResponseMessages[geaAddressIndex], config.addressToErdMapArray[geaAddressIndex].geaAddress);
      Gea2Message_SetCommand(expectedGea2ResponseMessages[geaAddressIndex], ADifferentGea2Command);
      uint8_t *payload = Gea2Message_GetPayload(expectedGea2ResponseMessages[geaAddressIndex]);
      payload[0] = 0xDE;
      payload[1] = 0xAD;
      payload[2] = 0xBE;
      payload[3] = 0xEF;

      Gea2MessageEndpoint_TestDouble_TriggerReceive(&messageEndpointTestDouble, expectedGea2ResponseMessages[geaAddressIndex]);
   }

   void WhenBoardWithUnknownGeaAddressSendsVersionRequestResponse()
   {
      Gea2Message_SetDestination(messageWithUnkownGeaAddress, Gea2Address_Mainboard);
      Gea2Message_SetSource(messageWithUnkownGeaAddress, AnUnknownGeaAddress);
      Gea2Message_SetCommand(messageWithUnkownGeaAddress, Gea2CommonCommand_Version);

      uint8_t *payload = Gea2Message_GetPayload(messageWithUnkownGeaAddress);
      payload[0] = 0xDE;
      payload[1] = 0xAD;
      payload[2] = 0xBE;
      payload[3] = 0xEF;

      Gea2MessageEndpoint_TestDouble_TriggerReceive(&messageEndpointTestDouble, messageWithUnkownGeaAddress);
   }

   void AllNvErdsInAddressMapShouldBe(bool expected)
   {
      for(uint8_t i = 0; i < config.addressToErdMapArrayNumberEntries; i++)
      {
         bool actual;
         DataModel_Read(
            dataModel,
            config.addressToErdMapArray[i].nvErd,
            &actual);
         CHECK_EQUAL(expected, actual);
      }
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleDouble, ticks);
   }
};

TEST(BoardsInSystem, ShouldRequestVersionFromEachBoardOnInitWhenNvErdsAreClear)
{
   ShouldSendVersionRequestForEachBoardAddressInArray();
   WhenTheModuleIsInitializedAndInitialDelayPeriodExpires();
}

TEST(BoardsInSystem, ShouldSkipRequestingVersionFromBoardsWhoseNvErdsAreSet)
{
   GivenNvErdIsSetForIndex(0);

   FOR_EACH_GEA_ADDRESS_PAIR_INDEX_STARTING_AT(i, 1)
   {
      GeaMessageShouldBeRequestedForIndex(Gea2CommonCommand_Version, i);
   }

   WhenTheModuleIsInitializedAndInitialDelayPeriodExpires();
}

TEST(BoardsInSystem, ShouldNotIssueAnyVersionRequestsWhenAllNvErdsAreSet)
{
   for(uint8_t i = 0; i < config.addressToErdMapArrayNumberEntries; i++)
   {
      GivenNvErdIsSetForIndex(i);
   }

   NothingShouldHappen();
   WhenTheModuleIsInitializedAndInitialDelayPeriodExpires();
}

TEST(BoardsInSystem, ShouldSetNvErdWhenVersionResponseMessageIsSentFromBoardAddress)
{
   ShouldSendVersionRequestForEachBoardAddressInArray();
   WhenTheModuleIsInitializedAndInitialDelayPeriodExpires();

   WhenBoardWithGeaAddressIndexRespondsToVersionRequest(0);
   NvErdForIndexShouldBe(0, SET);

   FOR_EACH_GEA_ADDRESS_PAIR_INDEX_STARTING_AT(i, 1)
   {
      NvErdForIndexShouldBe(i, CLEAR);
   }

   WhenBoardWithGeaAddressIndexRespondsToVersionRequest(1);
   NvErdForIndexShouldBe(0, SET);
   NvErdForIndexShouldBe(1, SET);

   FOR_EACH_GEA_ADDRESS_PAIR_INDEX_STARTING_AT(i, 2)
   {
      NvErdForIndexShouldBe(i, CLEAR);
   }
}

TEST(BoardsInSystem, ShouldNotSetNvErdWhenMessageFromBoardAddressIsNotAVersionResponseMessage)
{
   ShouldSendVersionRequestForEachBoardAddressInArray();
   WhenTheModuleIsInitializedAndInitialDelayPeriodExpires();

   WhenBoardWithGeaAddressIndexRespondsWithNonVersionRequestGeaCommand(0);
   AllNvErdsInAddressMapShouldBe(CLEAR);

   WhenBoardWithGeaAddressIndexRespondsWithNonVersionRequestGeaCommand(1);
   AllNvErdsInAddressMapShouldBe(CLEAR);
}

TEST(BoardsInSystem, ShouldNotSetAnyNvErdsWhenMessageFromBoardIsNotInAddressTable)
{
   ShouldSendVersionRequestForEachBoardAddressInArray();
   WhenTheModuleIsInitializedAndInitialDelayPeriodExpires();

   WhenBoardWithUnknownGeaAddressSendsVersionRequestResponse();
   AllNvErdsInAddressMapShouldBe(CLEAR);
}

TEST(BoardsInSystem, ShouldIssueRetryRequestIfThereIsNoResponseAfterRetryPeriod)
{
   ShouldSendVersionRequestForEachBoardAddressInArray();
   WhenTheModuleIsInitializedAndInitialDelayPeriodExpires();

   for(uint8_t i = 1; i < config.numberOfRetryRequests; i++)
   {
      ShouldSendVersionRequestForEachBoardAddressInArray();
      After(ARetryPeriodInSeconds * MSEC_PER_SEC);
   }
   AllNvErdsInAddressMapShouldBe(CLEAR);

   NothingShouldHappen();
   After(ARetryPeriodInSeconds * MSEC_PER_SEC);

   FOR_EACH_GEA_ADDRESS_PAIR_INDEX_STARTING_AT(i, 0)
   {
      WhenBoardWithGeaAddressIndexRespondsToVersionRequest(i);
      NvErdForIndexShouldBe(i, CLEAR);
   }
}

TEST(BoardsInSystem, ShouldDoNothingIfAllNvErdsAreSetBeforeInit)
{
   GivenNvErdIsSetForAllIndices();
   NothingShouldHappen();
   WhenTheModuleIsInitializedAndInitialDelayPeriodExpires();
   After(ARetryPeriodInSeconds * MSEC_PER_SEC);
}
