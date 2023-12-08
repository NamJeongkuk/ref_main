/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "NewFilterInstalledHandler.h"
#include "Signal.h"
#include "Constants_Binary.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

enum
{
   SomeVolumeUsage = 1000,
   SomeCalendarUsage = 2000,
   SomeNumberOfUnits = 10,
   SomeValveOnTimeInSeconds = 30000,
   MaxNumberOfUnitsFiltersCanBeInstalledOn = UINT8_MAX
};

static uint8_t RfidMainboardUnitSerialNumber[UnitSerialNumberSizeInBytes] = { '1', '2', '3', '4', '5', '6', '7', '8' };
static uint8_t MainboardUnitSerialNumber[32] = "123456789ABCDEF";
static uint8_t Uid[RfidUidSizeInBytes] = "ABCDEF";
static uint8_t NewUid[RfidUidSizeInBytes] = "ZYXWVU";
static const uint8_t LastTwelveMonthsOfWaterUsageInGallons[MonthlyGallonUsageSizeInBytes] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B };
static const uint8_t LastTwelveMonthsOfWaterUsageInGallonsEmpty[MonthlyGallonUsageSizeInBytes] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static uint8_t BlankSerialNumber[UnitSerialNumberSizeInBytes] = "";
static uint8_t SomePreviousUnitSerialNumber[UnitSerialNumberSizeInBytes] = "ABCDEFG";

const NewFilterInstalledHandlerReadErds_t readErdsConfig = {
   .rfidFilterUidRfidBoardErd = Erd_RfidFilterUid_RfidBoard,
   .rfidFilterWaterVolumeUsageInOuncesX100RfidBoardErd = Erd_RfidFilterWaterVolumeUsageInOuncesX100_RfidBoard,
   .rfidFilterCalendarUsageInSecondsRfidBoardErd = Erd_RfidFilterCalendarUsageInSeconds_RfidBoard,
   .rfidFilterLastTwelveMonthsOfWaterUsageInGallonsRfidBoardErd = Erd_RfidFilterLastTwelveMonthsOfWaterUsageInGallons_RfidBoard,
   .rfidFilterNumberOfUnitsRfidFilterHasBeenOnRfidBoardErd = Erd_RfidFilterNumberOfUnitsRfidFilterHasBeenOn_RfidBoard,
   .rfidFilterUnitSerialNumberRfidBoardErd = Erd_RfidFilterUnitSerialNumber_RfidBoard
};

const NewFilterInstalledHandlerWriteErds_t writeErdsConfig = {
   .rfidFilterUidErd = Erd_RfidFilterUid,
   .waterFilterVolumeUsageInOuncesX100Erd = Erd_WaterFilterVolumeUsageInOuncesX100,
   .eepromwaterFilterVolumeUsageInOuncesX100Erd = Erd_Eeprom_WaterFilterVolumeUsageInOuncesX100,
   .rfidFilterCalendarUsageInSecondsErd = Erd_WaterFilterCalendarUsageInSeconds,
   .eepromWaterFilterCalendarUsageInSecondsErd = Erd_Eeprom_WaterFilterCalendarUsageInSeconds,
   .rfidFilterLastTwelveMonthsOfWaterUsageInGallonsErd = Erd_RfidFilterLastTwelveMonthsOfWaterUsageInGallons,
   .rfidFilterNumberOfUnitsFilterHasBeenOnErd = Erd_RfidFilterNumberOfUnitsRfidFilterHasBeenOn,
   .rfidFilterPreviousUnitSerialNumberErd = Erd_RfidFilterPreviousUnitSerialNumber,
   .totalValveOnTimeInSecondsErd = Erd_WaterFilterTotalValveOnTimeInSeconds
};

const NewFilterInstalledHandlerConfig_t config = {
   .readErds = &readErdsConfig,
   .writeErds = &writeErdsConfig,
   .newFilterInstalledSignalErd = Erd_NewFilterInstalledSignal,
   .rfidBoardInSystemErd = Erd_RfidBoardInSystem,
   .rfidFilterIdentifierRfidBoardErd = Erd_RfidFilterIdentifier_RfidBoard,
   .rfidFaultRequestErd = Erd_RfidFaultRequest,
   .rfidFilterLeakDetectedFaultErd = Erd_RfidBoardLeakDetectedFault,
   .rfidFilterBlockedTagFaultErd = Erd_RfidBoardBlockedTagFault,
   .bypassPlugInstalledErd = Erd_BypassPlugInstalled,
   .rfidFilterUnitSerialNumberErd = Erd_RfidFilterUnitSerialNumber
};

static void OnDataModelChange(void *context, const void *_args)
{
   REINTERPRET(dataModel, context, I_DataModel_t *);
   REINTERPRET(args, _args, const DataModelOnDataChangeArgs_t *);

   if(args->erd == Erd_RfidFaultRequest)
   {
      REINTERPRET(data, args->data, const RfidFaultRequest_t *);

      mock()
         .actualCall("Rfid Fault Request")
         .onObject(dataModel)
         .withParameter("Erd", args->erd)
         .withParameter("Fault Erd", data->faultErd)
         .withParameter("Request Status", data->requestStatus)
         .withParameter("Signal", data->signal);
   }
}

TEST_GROUP(NewFilterInstalledHandler)
{
   NewFilterInstalledHandler_t instance;
   ReferDataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;
   EventSubscription_t dataModelTestDoubleOnChangeEventSubscription;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelTestDouble);
      dataModel = dataModelTestDouble.dataModel;

      EventSubscription_Init(
         &dataModelTestDoubleOnChangeEventSubscription,
         dataModel,
         OnDataModelChange);
      Event_Subscribe(
         dataModel->OnDataChange,
         &dataModelTestDoubleOnChangeEventSubscription);

      SetMainboardSerialNumberTo(MainboardUnitSerialNumber);
   }

   void SetMainboardSerialNumberTo(uint8_t * serialNumber)
   {
      DataModel_Write(dataModel, Erd_SerialNumber, serialNumber);
   }

   void GivenInitialization()
   {
      NewFilterInstalledHandler_Init(
         &instance,
         dataModel,
         &config);
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

   void WhenANewRfidFilterIsInstalled()
   {
      DataModel_Write(dataModel, Erd_RfidFilterUid_RfidBoard, NewUid);
      Signal_SendViaErd(DataModel_AsDataSource(dataModel), Erd_NewFilterInstalledSignal);
   }

   void WhenANewFilterIsInstalled()
   {
      Signal_SendViaErd(DataModel_AsDataSource(dataModel), Erd_NewFilterInstalledSignal);
   }

   void GivenCurrentFilterUidIs(uint8_t * uid)
   {
      DataModel_Write(dataModel, Erd_RfidFilterUid_RfidBoard, uid);
      DataModel_Write(dataModel, Erd_RfidFilterUid, uid);
   }

   void MainboardRfidFilterUidShouldBe(uint8_t * expected)
   {
      RfidUid_t actual;
      DataModel_Read(dataModel, Erd_RfidFilterUid, &actual);
      MEMCMP_EQUAL(expected, &actual, sizeof(RfidUid_t));
   }

   void WhenVolumeUsageOnNewRfidBoardIs(VolumeInOuncesX100_t volumeUsageInOuncesX100)
   {
      DataModel_Write(dataModel, Erd_RfidFilterWaterVolumeUsageInOuncesX100_RfidBoard, &volumeUsageInOuncesX100);
   }

   void GivenTheVolumeUsageOnMainboardIs(VolumeInOuncesX100_t volumeUsage)
   {
      DataModel_Write(dataModel, Erd_WaterFilterVolumeUsageInOuncesX100, &volumeUsage);
   }

   void GivenTheEepromVolumeUsageOnMainboardIs(VolumeInOuncesX100_t volumeUsage)
   {
      DataModel_Write(dataModel, Erd_Eeprom_WaterFilterVolumeUsageInOuncesX100, &volumeUsage);
   }

   void VolumeUsageOnMainboardShouldBe(VolumeInOuncesX100_t expected)
   {
      VolumeInOuncesX100_t actual;
      DataModel_Read(dataModel, Erd_WaterFilterVolumeUsageInOuncesX100, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void EepromVolumeUsageOnMainboardShouldBe(CalendarUsageInSeconds_t expected)
   {
      VolumeInOuncesX100_t actual;
      DataModel_Read(dataModel, Erd_Eeprom_WaterFilterVolumeUsageInOuncesX100, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void GivenTheCalendarUsageOnMainboardIs(CalendarUsageInSeconds_t calendarUsageInSeconds)
   {
      DataModel_Write(dataModel, Erd_WaterFilterCalendarUsageInSeconds, &calendarUsageInSeconds);
   }

   void GivenTheEepromCalendarUsageOnMainboardIs(VolumeInOuncesX100_t volumeUsage)
   {
      DataModel_Write(dataModel, Erd_Eeprom_WaterFilterCalendarUsageInSeconds, &volumeUsage);
   }

   void WhenCalendarUsageOnNewRfidBoardIs(CalendarUsageInSeconds_t calendarUsageInSeconds)
   {
      DataModel_Write(dataModel, Erd_RfidFilterCalendarUsageInSeconds_RfidBoard, &calendarUsageInSeconds);
   }

   void CalendarUsageOnMainboardShouldBe(CalendarUsageInSeconds_t expected)
   {
      CalendarUsageInSeconds_t actual;
      DataModel_Read(dataModel, Erd_WaterFilterCalendarUsageInSeconds, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void EepromCalendarUsageInSecondsOnMainboardShouldBe(CalendarUsageInSeconds_t expected)
   {
      CalendarUsageInSeconds_t actual;
      DataModel_Read(dataModel, Erd_Eeprom_WaterFilterCalendarUsageInSeconds, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void WhenTheLastTwelveMonthsOfWaterUsageInGallonsOnNewRfidFiliterIs(const uint8_t *lastTwelveMonths)
   {
      DataModel_Write(dataModel, Erd_RfidFilterLastTwelveMonthsOfWaterUsageInGallons_RfidBoard, lastTwelveMonths);
   }

   void LastTwelveMonthsOfWaterUsageShouldBe(const uint8_t *expected)
   {
      RfidTwelveMonthsGallonUsage_t actual;
      DataModel_Read(dataModel, Erd_RfidFilterLastTwelveMonthsOfWaterUsageInGallons, &actual);
      MEMCMP_EQUAL(expected, &actual, sizeof(RfidTwelveMonthsGallonUsage_t));
   }

   void WhenTheNumberOfUnitsRfidFilterHasBeenOnNewRfidFilterIs(RfidFilterNumberOfUnitsFilterHasBeenOn_t numberOfUnits)
   {
      DataModel_Write(dataModel, Erd_RfidFilterNumberOfUnitsRfidFilterHasBeenOn_RfidBoard, &numberOfUnits);
   }

   void NumberOfUnitsRfidFilterHasBeenOnShouldBe(RfidFilterNumberOfUnitsFilterHasBeenOn_t expected)
   {
      RfidFilterNumberOfUnitsFilterHasBeenOn_t actual;
      DataModel_Read(dataModel, Erd_RfidFilterNumberOfUnitsRfidFilterHasBeenOn, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void PreviousUnitSerialNumberShouldBe(uint8_t * expected)
   {
      UnitSerialNumber_t actual;
      DataModel_Read(dataModel, Erd_RfidFilterPreviousUnitSerialNumber, &actual);
      MEMCMP_EQUAL(expected, &actual, sizeof(UnitSerialNumber_t));
   }

   void WhenTheSerialNumberOnANewRfidFilterIs(uint8_t * unitSerialNumber)
   {
      DataModel_Write(dataModel, Erd_RfidFilterUnitSerialNumber_RfidBoard, unitSerialNumber);
   }

   void BypassPlugInstalledShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_BypassPlugInstalled, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void GivenTheBypassPlugInstalledIs(bool status)
   {
      DataModel_Write(dataModel, Erd_BypassPlugInstalled, &status);
   }

   void WhenTheNewRfidFilterIsABypassPlug()
   {
      uint8_t filterId[FilterIdentifierSizeInBytes] = "BPXWFE";
      DataModel_Write(dataModel, Erd_RfidFilterIdentifier_RfidBoard, &filterId);
   }

   void ShouldSendFaultRequestWith(Erd_t erd, bool status, uint8_t signal)
   {
      RfidFaultRequest_t rfidFaultRequest = {
         .faultErd = erd,
         .requestStatus = status,
         .signal = signal
      };

      mock()
         .expectOneCall("Rfid Fault Request")
         .onObject(dataModel)
         .withParameter("Erd", Erd_RfidFaultRequest)
         .withParameter("Fault Erd", rfidFaultRequest.faultErd)
         .withParameter("Request Status", rfidFaultRequest.requestStatus)
         .withParameter("Signal", rfidFaultRequest.signal);
   }

   void GivenWaterValveOnTimeIs(uint32_t valveTimeOn)
   {
      DataModel_Write(dataModel, Erd_WaterFilterTotalValveOnTimeInSeconds, &valveTimeOn);
   }

   void WaterFilterValveOnTimeShouldBe(uint32_t expected)
   {
      uint32_t actual;
      DataModel_Read(dataModel, Erd_WaterFilterTotalValveOnTimeInSeconds, &actual);
      CHECK_EQUAL(expected, actual);
   }
};

TEST(NewFilterInstalledHandler, ShouldCopyNewUidToMainboardWhenANewFilterIsInstalled)
{
   GivenCurrentFilterUidIs(Uid);
   GivenTheRfidBoardIsInTheSystem();
   GivenInitialization();
   MainboardRfidFilterUidShouldBe(Uid);

   WhenANewRfidFilterIsInstalled();
   MainboardRfidFilterUidShouldBe(NewUid);
}

TEST(NewFilterInstalledHandler, ShouldCopyWaterVolumeUsageToMainboardWhenANewFilterIsInstalled)
{
   GivenTheRfidBoardIsInTheSystem();
   GivenInitialization();
   VolumeUsageOnMainboardShouldBe(0);

   WhenVolumeUsageOnNewRfidBoardIs(SomeVolumeUsage);
   WhenANewRfidFilterIsInstalled();
   VolumeUsageOnMainboardShouldBe(SomeVolumeUsage);
}

TEST(NewFilterInstalledHandler, ShouldCopyEepromWaterVolumeUsageToMainboardWhenANewFilterIsInstalled)
{
   GivenTheRfidBoardIsInTheSystem();
   GivenInitialization();
   EepromVolumeUsageOnMainboardShouldBe(0);

   WhenVolumeUsageOnNewRfidBoardIs(SomeVolumeUsage);
   WhenANewRfidFilterIsInstalled();
   EepromVolumeUsageOnMainboardShouldBe(SomeVolumeUsage);
}

TEST(NewFilterInstalledHandler, ShouldCopyCalendarUsageToMainboardWhenANewFilterIsInstalled)
{
   GivenTheRfidBoardIsInTheSystem();
   GivenInitialization();
   CalendarUsageOnMainboardShouldBe(0);

   WhenCalendarUsageOnNewRfidBoardIs(SomeCalendarUsage);
   WhenANewRfidFilterIsInstalled();
   CalendarUsageOnMainboardShouldBe(SomeCalendarUsage);
}

TEST(NewFilterInstalledHandler, ShouldCopyEepromCalendarUsageToMainboardWhenANewFilterIsInstalled)
{
   GivenTheRfidBoardIsInTheSystem();
   GivenInitialization();
   EepromCalendarUsageInSecondsOnMainboardShouldBe(0);

   WhenCalendarUsageOnNewRfidBoardIs(SomeCalendarUsage);
   WhenANewRfidFilterIsInstalled();
   EepromCalendarUsageInSecondsOnMainboardShouldBe(SomeCalendarUsage);
}

TEST(NewFilterInstalledHandler, ShouldCopyLastTwelveMonthsOfWaterUsageToMainboardWhenANewFilterIsInstalled)
{
   GivenTheRfidBoardIsInTheSystem();
   GivenInitialization();
   LastTwelveMonthsOfWaterUsageShouldBe(LastTwelveMonthsOfWaterUsageInGallonsEmpty);

   WhenTheLastTwelveMonthsOfWaterUsageInGallonsOnNewRfidFiliterIs(LastTwelveMonthsOfWaterUsageInGallons);
   WhenANewRfidFilterIsInstalled();
   LastTwelveMonthsOfWaterUsageShouldBe(LastTwelveMonthsOfWaterUsageInGallons);
}

TEST(NewFilterInstalledHandler, ShouldCopyButNotIncrementTheNumberOfUnitsRfidFilterHasBeenOnToTheMainboardWhenANewFilterIsInstalledWithTheSameSerialNumber)
{
   GivenTheRfidBoardIsInTheSystem();
   GivenInitialization();
   NumberOfUnitsRfidFilterHasBeenOnShouldBe(0);

   WhenTheSerialNumberOnANewRfidFilterIs(RfidMainboardUnitSerialNumber);
   WhenTheNumberOfUnitsRfidFilterHasBeenOnNewRfidFilterIs(SomeNumberOfUnits);
   WhenANewRfidFilterIsInstalled();
   NumberOfUnitsRfidFilterHasBeenOnShouldBe(SomeNumberOfUnits);
}

TEST(NewFilterInstalledHandler, ShouldCopyAndIncrementNumberOfUnitsFilterHasBeenOnWhenANewFilterIsInstalledWithANewSerialNumber)
{
   GivenTheRfidBoardIsInTheSystem();
   GivenInitialization();
   NumberOfUnitsRfidFilterHasBeenOnShouldBe(0);

   WhenTheSerialNumberOnANewRfidFilterIs(SomePreviousUnitSerialNumber);
   WhenTheNumberOfUnitsRfidFilterHasBeenOnNewRfidFilterIs(SomeNumberOfUnits);
   WhenANewRfidFilterIsInstalled();

   NumberOfUnitsRfidFilterHasBeenOnShouldBe(SomeNumberOfUnits + 1);
}

TEST(NewFilterInstalledHandler, ShouldCopyTheUnitSerialNumberOnTheRfidBoardToThePreviousSerialNumberOnTheMainboardWhenANewFilterIsInstalledWithANewSerialNumber)
{
   GivenTheRfidBoardIsInTheSystem();
   GivenInitialization();
   PreviousUnitSerialNumberShouldBe(BlankSerialNumber);

   WhenTheSerialNumberOnANewRfidFilterIs(SomePreviousUnitSerialNumber);
   WhenANewRfidFilterIsInstalled();
   PreviousUnitSerialNumberShouldBe(SomePreviousUnitSerialNumber);
}

TEST(NewFilterInstalledHandler, ShouldNotCopyTheUnitSerialNumberOnTheRfidBoardToThePreviousSerialNumberOnTheMainboardWhenANewFilterIsInstalledWithTheSameSerialNumber)
{
   GivenTheRfidBoardIsInTheSystem();
   GivenInitialization();
   PreviousUnitSerialNumberShouldBe(BlankSerialNumber);

   WhenTheSerialNumberOnANewRfidFilterIs(RfidMainboardUnitSerialNumber);
   WhenANewRfidFilterIsInstalled();
   PreviousUnitSerialNumberShouldBe(BlankSerialNumber);
}

TEST(NewFilterInstalledHandler, ShouldClearTotalVolumeUsageIfTheRfidBoardIsNotInTheSystemWhenANewFilterIsInstalled)
{
   GivenTheRfidBoardIsNotInTheSystem();
   GivenInitialization();
   GivenTheVolumeUsageOnMainboardIs(SomeVolumeUsage);

   WhenANewFilterIsInstalled();
   VolumeUsageOnMainboardShouldBe(0);
}

TEST(NewFilterInstalledHandler, ShouldClearTotalVolumeUsageEepromIfTheRfidBoardIsNotInTheSystemWhenANewFilterIsInstalled)
{
   GivenTheRfidBoardIsNotInTheSystem();
   GivenInitialization();
   GivenTheEepromVolumeUsageOnMainboardIs(SomeVolumeUsage);

   WhenANewFilterIsInstalled();
   EepromVolumeUsageOnMainboardShouldBe(0);
}

TEST(NewFilterInstalledHandler, ShouldClearCalendarUsageIfTheRfidBoardIsNotInTheSystemWhenANewFilterIsInstalled)
{
   GivenTheRfidBoardIsNotInTheSystem();
   GivenInitialization();
   GivenTheCalendarUsageOnMainboardIs(SomeCalendarUsage);

   WhenANewFilterIsInstalled();
   CalendarUsageOnMainboardShouldBe(0);
}

TEST(NewFilterInstalledHandler, ShouldClearCalendarUsageEepromIfTheRfidBoardIsNotInTheSystemWhenANewFilterIsInstalled)
{
   GivenTheRfidBoardIsNotInTheSystem();
   GivenInitialization();
   GivenTheEepromCalendarUsageOnMainboardIs(SomeCalendarUsage);

   WhenANewFilterIsInstalled();
   EepromCalendarUsageInSecondsOnMainboardShouldBe(0);
}

TEST(NewFilterInstalledHandler, ShouldSetBypassPlugInstalledWhenTheNewFilterIsABypassPlug)
{
   GivenTheRfidBoardIsInTheSystem();
   GivenInitialization();
   BypassPlugInstalledShouldBe(CLEAR);

   ShouldSendFaultRequestWith(Erd_RfidBoardLeakDetectedFault, CLEAR, 1);
   ShouldSendFaultRequestWith(Erd_RfidBoardBlockedTagFault, CLEAR, 2);
   WhenTheNewRfidFilterIsABypassPlug();
   WhenANewRfidFilterIsInstalled();
   BypassPlugInstalledShouldBe(SET);
}

TEST(NewFilterInstalledHandler, ShouldClearBypassPlugInstalledWhenTheNewFilterIsNotABypassPlug)
{
   GivenTheRfidBoardIsInTheSystem();
   GivenInitialization();
   GivenTheBypassPlugInstalledIs(SET);

   WhenANewRfidFilterIsInstalled();
   BypassPlugInstalledShouldBe(CLEAR);
}

TEST(NewFilterInstalledHandler, ShouldRequestToClearLeakDetectedAndBlockedTagFaultWhenABypassFilterIsInstalled)
{
   GivenTheRfidBoardIsInTheSystem();
   GivenInitialization();

   ShouldSendFaultRequestWith(Erd_RfidBoardLeakDetectedFault, CLEAR, 1);
   ShouldSendFaultRequestWith(Erd_RfidBoardBlockedTagFault, CLEAR, 2);
   WhenTheNewRfidFilterIsABypassPlug();
   WhenANewRfidFilterIsInstalled();
}

TEST(NewFilterInstalledHandler, ShouldClampNumberOfUnitsFilterHasBeenOnWhenTheNumberIsAtTheMaxAndIsInstalledOnANewUnit)
{
   GivenTheRfidBoardIsInTheSystem();
   GivenInitialization();
   NumberOfUnitsRfidFilterHasBeenOnShouldBe(0);

   WhenTheSerialNumberOnANewRfidFilterIs(SomePreviousUnitSerialNumber);
   WhenTheNumberOfUnitsRfidFilterHasBeenOnNewRfidFilterIs(MaxNumberOfUnitsFiltersCanBeInstalledOn);
   WhenANewRfidFilterIsInstalled();

   NumberOfUnitsRfidFilterHasBeenOnShouldBe(MaxNumberOfUnitsFiltersCanBeInstalledOn);
}

TEST(NewFilterInstalledHandler, ShouldClearWaterValveOnTimeErdOnRfidUnitWhenANewRfidFilterIsInstalled)
{
   GivenTheRfidBoardIsInTheSystem();
   GivenInitialization();
   GivenWaterValveOnTimeIs(SomeValveOnTimeInSeconds);

   WhenANewRfidFilterIsInstalled();
   WaterFilterValveOnTimeShouldBe(0);
}

TEST(NewFilterInstalledHandler, ShouldClearWaterValveOnTimeErdOnNonRfidUnitWhenANewFilterIsInstalled)
{
   GivenTheRfidBoardIsNotInTheSystem();
   GivenInitialization();
   GivenWaterValveOnTimeIs(SomeValveOnTimeInSeconds);

   WhenANewFilterIsInstalled();
   WaterFilterValveOnTimeShouldBe(0);
}
