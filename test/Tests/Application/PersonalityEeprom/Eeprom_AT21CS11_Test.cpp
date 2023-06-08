/*!
 * @file
 * @brief Test code for AT21CS11 Eeprom adapter
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "Eeprom_AT21CS11.h"
#include "utils.h"
#include "SystemErds.h"
#include "Constants_Binary.h"
#include "SystemErds.h"
#include "DataModelErdPointerAccess.h"
}

#include <string.h>
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "uassert_test.h"
#include "OneWire_TestDouble.h"
#include "TimerModule_TestDouble.h"

enum
{
   TargetAddress = 0x23,
   TimeToWrite8Bytes = 5,
   TimeToRead8Bytes = 5,
   ExpectedEepromSize = 128,
   ReadBufferSize = ExpectedEepromSize,
   EmptyByteMarker = 0xA5,
   LocationOfUniqueId = 0x80
};

static const uint8_t testData[] = {
   0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F
};

static const uint64_t testUniqueId = 0x1234567812345678;

TEST_GROUP(Eeprom_AT21CS11)
{
   Eeprom_AT21CS11_t eeprom;

   TimerModule_TestDouble_t timeModuleTestDouble;
   TimerModule_t *timerModule;
   OneWire_TestDouble_t oneWireTestDouble;
   I_HardwareEeprom_t *instance;
   EventSubscription_t writeSubscription;
   EventSubscription_t readSubscription;
   EventSubscription_t eraseSubscription;
   uint8_t *deviceData;
   uint8_t readBuffer[ReadBufferSize];

   void setup()
   {
      TimerModule_TestDouble_Init(&timeModuleTestDouble);
      timerModule = &timeModuleTestDouble.timerModule;

      OneWire_TestDouble_Init(&oneWireTestDouble);
      deviceData = OneWire_TestDouble_GetDataLocation(&oneWireTestDouble);

      memcpy(&deviceData[LocationOfUniqueId], &testUniqueId, sizeof(testUniqueId));

      EventSubscription_Init(&writeSubscription, NULL, OnWriteHandler);
      EventSubscription_Init(&readSubscription, NULL, OnReadHandler);
      EventSubscription_Init(&eraseSubscription, NULL, OnEraseHandler);

      memset(readBuffer, EmptyByteMarker, ReadBufferSize);
   }

   void After(TimerTicks_t delay)
   {
      TimerModule_TestDouble_ElapseTime(&timeModuleTestDouble, delay);
   }

   void NothingHappens(void)
   {
   }

   void GivenEventHandlersAreConnected(void)
   {
      Event_Subscribe(HardwareEeprom_GetOnWrittenEvent(instance), &writeSubscription);
      Event_Subscribe(HardwareEeprom_GetOnReadEvent(instance), &readSubscription);
      Event_Subscribe(HardwareEeprom_GetOnErasedEvent(instance), &eraseSubscription);
   }

   static void OnWriteHandler(void *context, const void *args)
   {
      (void)context;
      (void)args;
      mock().actualCall("OnWriteCallback");
   }

   void WriteEventCompleteShouldHappen()
   {
      mock().expectOneCall("OnWriteCallback");
   }

   static void OnReadHandler(void *context, const void *args)
   {
      (void)context;
      (void)args;
      mock().actualCall("OnReadCallback");
   }

   void ReadEventCompleteShouldHappen()
   {
      mock().expectOneCall("OnReadCallback");
   }

   static void OnEraseHandler(void *context, const void *args)
   {
      (void)context;
      (void)args;
      mock().actualCall("OnEraseCallback");
   }

   void EraseCompleteEventShouldHappen()
   {
      mock().expectOneCall("OnEraseCallback");
   }

   void GivenDeviceIsNotPresent(void)
   {
      OneWire_TestDouble_SetDevicePresent(&oneWireTestDouble, false);
   }

   void GivenTheEepromAdapterIsInitialized(void)
   {
      Eeprom_AT21CS11_Init(&eeprom, timerModule, &oneWireTestDouble.interface);
      instance = Eeprom_AT21CS11_GetInterface(&eeprom);
   }
};

TEST(Eeprom_AT21CS11, ShouldReturnEventsImmediatelyIfDeviceIsntPresent)
{
   GivenDeviceIsNotPresent();
   GivenTheEepromAdapterIsInitialized();
   GivenEventHandlersAreConnected();

   WriteEventCompleteShouldHappen();
   HardwareEeprom_Write(instance, TargetAddress, sizeof(testData), testData);

   ReadEventCompleteShouldHappen();
   HardwareEeprom_Read(instance, TargetAddress, sizeof(testData), readBuffer);

   EraseCompleteEventShouldHappen();
   HardwareEeprom_Erase(instance);
}

TEST(Eeprom_AT21CS11, ShouldStoreLargeDataAtSpecifiedLocationWithEventOnCompletionWhenWritten)
{
   GivenTheEepromAdapterIsInitialized();
   GivenEventHandlersAreConnected();

   NothingHappens();
   HardwareEeprom_Write(instance, TargetAddress, sizeof(testData), testData);

   NothingHappens();
   After(TimeToWrite8Bytes);

   WriteEventCompleteShouldHappen();
   After(TimeToWrite8Bytes);

   CHECK_EQUAL(testData[0], deviceData[TargetAddress + 0]);
   CHECK_EQUAL(testData[1], deviceData[TargetAddress + 1]);
   CHECK_EQUAL(testData[2], deviceData[TargetAddress + 2]);
   CHECK_EQUAL(testData[3], deviceData[TargetAddress + 3]);
   CHECK_EQUAL(testData[4], deviceData[TargetAddress + 4]);
   CHECK_EQUAL(testData[5], deviceData[TargetAddress + 5]);
   CHECK_EQUAL(testData[6], deviceData[TargetAddress + 6]);
   CHECK_EQUAL(testData[7], deviceData[TargetAddress + 7]);
   CHECK_EQUAL(testData[8], deviceData[TargetAddress + 8]);
   CHECK_EQUAL(testData[9], deviceData[TargetAddress + 9]);
   CHECK_EQUAL(testData[10], deviceData[TargetAddress + 10]);
   CHECK_EQUAL(testData[11], deviceData[TargetAddress + 11]);
   CHECK_EQUAL(testData[12], deviceData[TargetAddress + 12]);
   CHECK_EQUAL(testData[13], deviceData[TargetAddress + 13]);
   CHECK_EQUAL(testData[14], deviceData[TargetAddress + 14]);
   CHECK_EQUAL(testData[15], deviceData[TargetAddress + 15]);
   CHECK_EQUAL(0, deviceData[TargetAddress + 16]);
}

TEST(Eeprom_AT21CS11, ShouldStoreShortDataAtSpecifiedLocationWithEventOnCompletionWhenWritten)
{
   GivenTheEepromAdapterIsInitialized();
   GivenEventHandlersAreConnected();

   NothingHappens();
   HardwareEeprom_Write(instance, TargetAddress, 1, testData);

   WriteEventCompleteShouldHappen();
   After(TimeToWrite8Bytes);

   CHECK_EQUAL(testData[0], deviceData[TargetAddress]);
   CHECK_EQUAL(0, deviceData[TargetAddress + 1]);
}

TEST(Eeprom_AT21CS11, ShouldBeAbleToReadBackLongWrittenData)
{
   GivenTheEepromAdapterIsInitialized();
   GivenEventHandlersAreConnected();

   NothingHappens();
   HardwareEeprom_Write(instance, TargetAddress, sizeof(testData), testData);

   WriteEventCompleteShouldHappen();
   After(TimeToWrite8Bytes * 2);

   ReadEventCompleteShouldHappen();
   HardwareEeprom_Read(instance, TargetAddress, sizeof(testData), readBuffer);

   CHECK_EQUAL(readBuffer[0], testData[0]);
   CHECK_EQUAL(readBuffer[1], testData[1]);
   CHECK_EQUAL(readBuffer[2], testData[2]);
   CHECK_EQUAL(readBuffer[3], testData[3]);
   CHECK_EQUAL(readBuffer[4], testData[4]);
   CHECK_EQUAL(readBuffer[5], testData[5]);
   CHECK_EQUAL(readBuffer[6], testData[6]);
   CHECK_EQUAL(readBuffer[7], testData[7]);
   CHECK_EQUAL(readBuffer[8], testData[8]);
   CHECK_EQUAL(readBuffer[9], testData[9]);
   CHECK_EQUAL(readBuffer[10], testData[10]);
   CHECK_EQUAL(readBuffer[11], testData[11]);
   CHECK_EQUAL(readBuffer[12], testData[12]);
   CHECK_EQUAL(readBuffer[13], testData[13]);
   CHECK_EQUAL(readBuffer[14], testData[14]);
   CHECK_EQUAL(readBuffer[15], testData[15]);
   CHECK_EQUAL(readBuffer[16], EmptyByteMarker);
}

TEST(Eeprom_AT21CS11, ShouldBeAbleToReadBackShortWrittenData)
{
   GivenTheEepromAdapterIsInitialized();
   GivenEventHandlersAreConnected();

   NothingHappens();
   HardwareEeprom_Write(instance, TargetAddress, 4, testData);

   WriteEventCompleteShouldHappen();
   After(TimeToWrite8Bytes);

   ReadEventCompleteShouldHappen();
   HardwareEeprom_Read(instance, TargetAddress, 4, readBuffer);

   CHECK_EQUAL(readBuffer[0], testData[0]);
   CHECK_EQUAL(readBuffer[1], testData[1]);
   CHECK_EQUAL(readBuffer[2], testData[2]);
   CHECK_EQUAL(readBuffer[3], testData[3]);
   CHECK_EQUAL(readBuffer[4], EmptyByteMarker);
}

TEST(Eeprom_AT21CS11, ShouldClearEntireEepromToZeroWhenErased)
{
   GivenTheEepromAdapterIsInitialized();
   GivenEventHandlersAreConnected();

   NothingHappens();
   HardwareEeprom_Write(instance, 0, 1, testData);
   WriteEventCompleteShouldHappen();
   After(TimeToWrite8Bytes);

   NothingHappens();
   HardwareEeprom_Write(instance, 63, 1, testData);
   WriteEventCompleteShouldHappen();
   After(TimeToWrite8Bytes);

   NothingHappens();
   HardwareEeprom_Write(instance, 95, 1, testData);
   WriteEventCompleteShouldHappen();
   After(TimeToWrite8Bytes);

   CHECK_EQUAL(deviceData[0], testData[0]);
   CHECK_EQUAL(deviceData[63], testData[0]);
   CHECK_EQUAL(deviceData[95], testData[0]);

   HardwareEeprom_Erase(instance);
   EraseCompleteEventShouldHappen();
   After(TimeToRead8Bytes * ExpectedEepromSize);

   CHECK_EQUAL(deviceData[0], 0);
   CHECK_EQUAL(deviceData[63], 0);
   CHECK_EQUAL(deviceData[95], 0);
}

TEST(Eeprom_AT21CS11, ShouldReturnExpectedEepromSize)
{
   GivenTheEepromAdapterIsInitialized();
   CHECK_EQUAL(ExpectedEepromSize, HardwareEeprom_GetSize(instance));
}

TEST(Eeprom_AT21CS11, ShouldReturnExpectedByteAlignment)
{
   GivenTheEepromAdapterIsInitialized();
   CHECK_EQUAL(1, HardwareEeprom_GetByteAlignment(instance));
}
