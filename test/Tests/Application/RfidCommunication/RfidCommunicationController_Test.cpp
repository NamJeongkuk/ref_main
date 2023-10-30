/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "RfidCommunicationController.h"
#include "RfidCommunicationControllerState.h"
#include "RfidTypes.h"
#include "WaterFilterType.h"
#include "Constants_Binary.h"
#include "PersonalityParametricData.h"
#include "Constants_Time.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

enum
{
   Open,
   Closed,
   SomeLeakDetectedCount = 10,
   SomeVolumeUsage = 1000,
   SomeCalendarUsage = 2000
};

uint8_t GivenMainboardUnitSerialNumber[UnitSerialNumberSizeInBytes] = { '1', '2', '3', '4', '5', '6', '7', '8' };
uint8_t Uid[RfidUidSizeInBytes] = "ABCDEF";
uint8_t NewUid[RfidUidSizeInBytes] = "ZYXWVU";
uint8_t BlankSerialNumber[UnitSerialNumberSizeInBytes] = "";
uint8_t SomePreviousUnitSerialNumber[UnitSerialNumberSizeInBytes] = "ABCDEFG";

static const RfidCommunicationControllerConfig_t config = {
   .rfidCommunicationControllerStateErd = Erd_RfidCommunicationControllerState,
   .rfidFilterReadWriteResultErd = Erd_RfidFilterReadWriteResult_RfidBoard,
   .allFreshFoodDoorsAreClosedErd = Erd_AllFreshFoodDoorsAreClosed,
   .rfidFilterUidRfidBoardErd = Erd_RfidFilterUid_RfidBoard,
   .rfidFilterUidErd = Erd_RfidFilterUid,
   .rfidFilterUnitSerialNumberRfidBoardErd = Erd_RfidFilterUnitSerialNumber_RfidBoard,
   .rfidFilterUnitSerialNumberErd = Erd_RfidFilterUnitSerialNumber,
   .rfidFilterCalendarUsageInSecondsRfidBoardErd = Erd_RfidFilterCalendarUsageInSeconds_RfidBoard,
   .rfidFilterCalendarUsageInSecondsErd = Erd_WaterFilterCalendarUsageInSeconds,
   .eepromWaterFilterCalendarUsageInSecondsErd = Erd_Eeprom_WaterFilterCalendarUsageInSeconds,
   .rfidFilterWaterVolumeUsageInOuncesX100RfidBoardErd = Erd_RfidFilterWaterVolumeUsageInOuncesX100_RfidBoard,
   .totalWaterVolumeUsageInOuncesX100Erd = Erd_TotalWaterVolumeUsageInOuncesX100,
   .eepromTotalWaterVolumeUsageInOuncesX100Erd = Erd_Eeprom_TotalWaterVolumeUsageInOuncesX100,
   .rfidFilterStatusRfidBoardErd = Erd_RfidFilterStatus_RfidBoard,
   .rfidFilterIdentifierErd = Erd_RfidFilterIdentifier_RfidBoard,
   .demoModeEnableErd = Erd_EnableDemoModeStatus,
   .waterFilterTypeErd = Erd_WaterFilterType,
   .bypassPlugInstalledErd = Erd_BypassPlugInstalled,
   .rfidFilterBadReadCountErd = Erd_RfidFilterBadReadCount,
   .rfidFilterBadWriteCountErd = Erd_RfidFilterBadWriteCount,
   .rfidFilterHardwareFailureCountErd = Erd_RfidFilterHardwareFailureCount,
   .rfidFilterLeakDetectedCountErd = Erd_RfidFilterLeakDetectCount,
   .rfidFilterBlockedCountErd = Erd_RfidFilterBlockedCount,
   .rfidFilterDataRequestErd = Erd_RfidFilterDataRequest,
   .newFilterInstalledSignalErd = Erd_NewFilterInstalledSignal,
   .rfidFilterNumberOfUnitsFilterHasBeenOnErd = Erd_RfidFilterNumberOfUnitsRfidFilterHasBeenOn,
   .rfidFilterPreviousUnitSerialNumberErd = Erd_RfidFilterPreviousUnitSerialNumber
};

static void OnDataModelChange(void *context, const void *_args)
{
   REINTERPRET(dataModel, context, I_DataModel_t *);
   REINTERPRET(args, _args, const DataModelOnDataChangeArgs_t *);

   if(args->erd == Erd_RfidFilterDataRequest)
   {
      REINTERPRET(data, args->data, const ReadWriteRequest_t *);

      if(*data == ReadWriteRequest_Read)
      {
         mock()
            .actualCall("Rfid Filter Data Request Read")
            .onObject(dataModel)
            .withParameter("Erd", args->erd)
            .withParameter("Data", *data);
      }
      else if(*data == ReadWriteRequest_Write)
      {
         mock()
            .actualCall("Rfid Filter Data Request Write")
            .onObject(dataModel)
            .withParameter("Erd", args->erd)
            .withParameter("Data", *data);
      }

      ReadWriteRequest_t request = ReadWriteRequest_Idle;
      DataModel_Write(dataModel, Erd_RfidFilterDataRequest, &request);
   }
}

TEST_GROUP(RfidCommunicationController)
{
   RfidCommunicationController_t instance;
   ReferDataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   EventSubscription_t dataModelTestDoubleOnChangeEventSubscription;
   const RfidFilterUpdateRateData_t *rfidFilterUpdateRateData;

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

      rfidFilterUpdateRateData = PersonalityParametricData_Get(dataModel)->filterData->rfidFilterData->rfidFilterUpdateRateData;

      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelTestDouble);

      SetRfidDataRequestErdToIdle();
      SetMainboardSerialNumberTo(GivenMainboardUnitSerialNumber);
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks);
   }

   void SetRfidDataRequestErdToIdle()
   {
      ReadWriteRequest_t request = ReadWriteRequest_Idle;
      DataModel_Write(dataModel, Erd_RfidFilterDataRequest, &request);
   }

   void SetMainboardSerialNumberTo(uint8_t * serialNumber)
   {
      DataModel_Write(dataModel, Erd_RfidFilterUnitSerialNumber, &serialNumber);
   }

   void GivenFilterUidIs(uint8_t * uid)
   {
      DataModel_Write(dataModel, Erd_RfidFilterUid_RfidBoard, uid);
      DataModel_Write(dataModel, Erd_RfidFilterUid, uid);
   }

   void GivenInitialization()
   {
      GivenFilterUidIs(Uid);

      mock().disable();
      RfidCommunicationController_Init(
         &instance,
         dataModel,
         &config,
         rfidFilterUpdateRateData);
      mock().enable();
   }

   void WhenInitialized()
   {
      GivenFilterUidIs(Uid);

      RfidCommunicationController_Init(
         &instance,
         dataModel,
         &config,
         rfidFilterUpdateRateData);
   }

   void GivenAllFreshFoodDoorsAre(bool state)
   {
      DataModel_Write(dataModel, Erd_AllFreshFoodDoorsAreClosed, &state);
   }

   void WhenAllFreshFoodDoorsAre(bool state)
   {
      GivenAllFreshFoodDoorsAre(state);
   }

   void WhenAFreshFoodDoorIsOpened()
   {
      GivenAllFreshFoodDoorsAre(Open);
   }

   void GivenAFreshFoodDoorIsOpened()
   {
      GivenAllFreshFoodDoorsAre(false);
   }

   void GivenTheBypassPlugIs(const bool state)
   {
      DataModel_Write(dataModel, Erd_BypassPlugInstalled, &state);
   }

   void RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerState_t expected)
   {
      RfidCommunicationControllerState_t actual;
      DataModel_Read(dataModel, Erd_RfidCommunicationControllerState, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void WaterFilterTypeShouldBe(WaterFilterType_t expected)
   {
      WaterFilterType_t actual;
      DataModel_Read(dataModel, Erd_WaterFilterType, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void BypassPlugShouldBe(const bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_BypassPlugInstalled, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void BypassPlugStateIs(bool state)
   {
      DataModel_Write(dataModel, Erd_BypassPlugInstalled, &state);
   }

   void WhenAnRfidMessageIsSentWithResult(ReadWriteResult_t result)
   {
      RfidFilterReadWriteResult_t rfidFilterReadWriteResult;
      DataModel_Read(dataModel, Erd_RfidFilterReadWriteResult_RfidBoard, &rfidFilterReadWriteResult);
      rfidFilterReadWriteResult.signal++;
      rfidFilterReadWriteResult.result = result;
      DataModel_Write(dataModel, Erd_RfidFilterReadWriteResult_RfidBoard, &rfidFilterReadWriteResult);
   }

   void AndSentWithinAnRfidMessageWithResult(ReadWriteResult_t result)
   {
      WhenAnRfidMessageIsSentWithResult(result);
   }

   void WhenRfidFilterLeakDetectedIs(bool given)
   {
      FilterStatusRfidBoard_t rfidFilterStatus;
      DataModel_Read(dataModel, Erd_RfidFilterStatus_RfidBoard, &rfidFilterStatus);

      if(given)
      {
         BIT_SET(rfidFilterStatus, RfidFilterStatusBits_LeakDetected);
      }
      else
      {
         BIT_CLEAR(rfidFilterStatus, RfidFilterStatusBits_LeakDetected);
      }

      DataModel_Write(dataModel, Erd_RfidFilterStatus_RfidBoard, &rfidFilterStatus);
   }

   void LeakDetectCountShouldBe(uint8_t expected)
   {
      uint8_t actual;
      DataModel_Read(dataModel, Erd_RfidFilterLeakDetectCount, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void GivenTheLeakDetectCountIs(uint8_t value)
   {
      DataModel_Write(dataModel, Erd_RfidFilterLeakDetectCount, &value);
   }

   void WhenRfidBlockedIs(bool state)
   {
      FilterStatusRfidBoard_t rfidFilterStatus;
      DataModel_Read(dataModel, Erd_RfidFilterStatus_RfidBoard, &rfidFilterStatus);

      if(state)
      {
         BIT_SET(rfidFilterStatus, RfidFilterStatusBits_Blocked_RfidBoard);
      }
      else
      {
         BIT_CLEAR(rfidFilterStatus, RfidFilterStatusBits_Blocked_RfidBoard);
      }

      DataModel_Write(dataModel, Erd_RfidFilterStatus_RfidBoard, &rfidFilterStatus);
   }

   void BlockedCountShouldBe(uint8_t expected)
   {
      uint8_t actual;
      DataModel_Read(dataModel, Erd_RfidFilterBlockedCount, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void WhenFilterIdentifierIsABypassPlug()
   {
      uint8_t filterId[FilterIdentifierSizeInBytes] = "BPXWFE";
      DataModel_Write(dataModel, Erd_RfidFilterIdentifier_RfidBoard, &filterId);
      DataModel_Write(dataModel, Erd_RfidFilterUid_RfidBoard, NewUid);
   }

   void RfidFilterBadReadCountShouldBe(uint8_t expected)
   {
      uint8_t actual;
      DataModel_Read(dataModel, Erd_RfidFilterBadReadCount, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void RfidFilterBadWriteCountShouldBe(uint8_t expected)
   {
      uint8_t actual;
      DataModel_Read(dataModel, Erd_RfidFilterBadWriteCount, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void RfidFilterHardwareFailureCountShouldBe(uint8_t expected)
   {
      uint8_t actual;
      DataModel_Read(dataModel, Erd_RfidFilterHardwareFailureCount, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void WhenDemoModeIs(const bool state)
   {
      DataModel_Write(dataModel, Erd_EnableDemoModeStatus, &state);
   }

   void GivenDemoModeIs(const bool state)
   {
      WhenDemoModeIs(state);
   }

   void ShouldSendReadRequestToRfidFilter()
   {
      mock()
         .expectOneCall("Rfid Filter Data Request Read")
         .onObject(dataModel)
         .withParameter("Erd", Erd_RfidFilterDataRequest)
         .withParameter("Data", ReadWriteRequest_Read);
   }

   void ShouldSendWriteRequestToRfidFilter()
   {
      mock()
         .expectOneCall("Rfid Filter Data Request Write")
         .onObject(dataModel)
         .withParameter("Erd", Erd_RfidFilterDataRequest)
         .withParameter("Data", ReadWriteRequest_Write);
   }

   void ShouldSendReadRequestToRfidFilterMultipleTimes(uint8_t numberOfCalls)
   {
      mock()
         .expectNCalls(numberOfCalls, "Rfid Filter Data Request Read")
         .onObject(dataModel)
         .withParameter("Erd", Erd_RfidFilterDataRequest)
         .withParameter("Data", ReadWriteRequest_Read);
   }

   void NothingShouldHappen()
   {
   }

   void WhenANewRfidFilterIsInstalled()
   {
      DataModel_Write(dataModel, Erd_RfidFilterUid_RfidBoard, NewUid);
      WhenAnRfidMessageIsSentWithResult(ReadWriteResult_Success);
   }

   void WhenANewRfidFilterIsInstalledWithADifferentSerialNumber(uint8_t * serialNumber)
   {
      DataModel_Write(dataModel, Erd_RfidFilterUid_RfidBoard, NewUid);
      DataModel_Write(dataModel, Erd_RfidFilterUnitSerialNumber_RfidBoard, serialNumber);

      WhenAnRfidMessageIsSentWithResult(ReadWriteResult_Success);
   }

   void MainboardRfidFilterUidShouldBe(uint8_t * expected)
   {
      RfidUid_t actual;
      DataModel_Read(dataModel, Erd_RfidFilterUid, &actual);
      MEMCMP_EQUAL(expected, &actual, sizeof(RfidUid_t));
   }

   void GivenRfidFilterUnitSerialNumberRfidBoardIs(uint8_t * unitSerialNumber)
   {
      DataModel_Write(dataModel, Erd_RfidFilterUnitSerialNumber_RfidBoard, unitSerialNumber);
   }

   void RfidFilterPreviousUnitSerialNumberShouldBe(uint8_t * expected)
   {
      UnitSerialNumber_t actual;
      DataModel_Read(dataModel, Erd_RfidFilterPreviousUnitSerialNumber, &actual);
      MEMCMP_EQUAL(expected, &actual, sizeof(UnitSerialNumber_t));
   }

   void RfidFilterUnitSerialNumberMainboardShouldBe(uint8_t * expected)
   {
      UnitSerialNumber_t actual;
      DataModel_Read(dataModel, Erd_RfidFilterUnitSerialNumber, &actual);
      MEMCMP_EQUAL(expected, &actual, sizeof(UnitSerialNumber_t));
   }

   void RfidFilterNumberOfUnitsFilterHasBeenOnShouldBe(RfidFilterNumberOfUnitsFilterHasBeenOn_t expected)
   {
      RfidFilterNumberOfUnitsFilterHasBeenOn_t actual;
      DataModel_Read(dataModel, Erd_RfidFilterNumberOfUnitsRfidFilterHasBeenOn, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void GivenTheRfidCommunicationControllerIsInAFreshFoodDoorOpenState()
   {
      GivenAllFreshFoodDoorsAre(Open);
      GivenInitialization();
   }

   void GivenTheRfidCommunicationControllerIsInAllFreshFoodDoorsJustClosedState()
   {
      GivenAllFreshFoodDoorsAre(Closed);
      GivenInitialization();
      WhenAnRfidMessageIsSentWithResult(ReadWriteResult_Success);
   }

   void GivenTheRfidCommunicationControllerIsInAllDoorsClosedReadState()
   {
      GivenTheRfidCommunicationControllerIsInAllFreshFoodDoorsJustClosedState();
      RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_AllFreshFoodDoorsJustClosed);

      ShouldSendReadRequestToRfidFilter();
      After(rfidFilterUpdateRateData->doorJustClosedFilterReadFrequencyInSeconds * MSEC_PER_SEC);
      WhenAnRfidMessageIsSentWithResult(ReadWriteResult_Success);
      After(rfidFilterUpdateRateData->doorJustClosedFilterReadFrequencyInSeconds * MSEC_PER_SEC);
      RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_AllDoorsClosedRead);
   }

   void GivenTheRfidCommunicationControllerIsInAllDoorsClosedWriteState()
   {
      GivenTheRfidCommunicationControllerIsInAllDoorsClosedReadState();

      ShouldSendReadRequestToRfidFilter();
      After(rfidFilterUpdateRateData->doorClosedFilterUpdateTimeInMinutes * MSEC_PER_MIN);

      ShouldSendWriteRequestToRfidFilter();
      WhenAnRfidMessageIsSentWithResult(ReadWriteResult_Success);
   }

   void NewFilterInstalledSignalShouldBe(Signal_t expected)
   {
      Signal_t actual;
      DataModel_Read(dataModel, Erd_NewFilterInstalledSignal, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void WhenCalendarUsageOnRfidBoardIs(CalendarUsageInSeconds_t calendarUsageInSeconds)
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

   void WhenVolumeUsageOnRfidBoardIs(VolumeInOuncesX100_t volumeUsageInOuncesX100)
   {
      DataModel_Write(dataModel, Erd_RfidFilterWaterVolumeUsageInOuncesX100_RfidBoard, &volumeUsageInOuncesX100);
   }

   void VolumeUsageOnMainboardShouldBe(VolumeInOuncesX100_t expected)
   {
      VolumeInOuncesX100_t actual;
      DataModel_Read(dataModel, Erd_TotalWaterVolumeUsageInOuncesX100, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void EepromVolumeUsageOnMainboardShouldBe(CalendarUsageInSeconds_t expected)
   {
      VolumeInOuncesX100_t actual;
      DataModel_Read(dataModel, Erd_Eeprom_TotalWaterVolumeUsageInOuncesX100, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void GivenTheABadWriteCountIs(uint8_t count)
   {
      DataModel_Write(dataModel, Erd_RfidFilterBadWriteCount, &count);
   }

   void NewFilterSignalAndVolumeUsageAndCalendarUsageShouldAllBeClear()
   {
      NewFilterInstalledSignalShouldBe(0);
      CalendarUsageOnMainboardShouldBe(0);
      VolumeUsageOnMainboardShouldBe(0);
      EepromCalendarUsageInSecondsOnMainboardShouldBe(0);
      EepromVolumeUsageOnMainboardShouldBe(0);
   }
};

TEST(RfidCommunicationController, ShouldUpdateTheWaterFilterTypeToXWFEUponInit)
{
   GivenInitialization();
   WaterFilterTypeShouldBe(WaterFilterType_XWFE);
}

TEST(RfidCommunicationController, ShouldClearBypassPlugStateUponInit)
{
   GivenTheBypassPlugIs(SET);
   GivenInitialization();
   BypassPlugShouldBe(CLEAR);
}

TEST(RfidCommunicationController, ShouldEnterTheFreshFoodDoorClosedStateWhenThereIsAFeshFoodDoorClosedUponInit)
{
   GivenTheRfidCommunicationControllerIsInAllFreshFoodDoorsJustClosedState();
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_AllFreshFoodDoorsJustClosed);
}

TEST(RfidCommunicationController, ShouldEnterTheFreshFoodDoorOpenStateWhenThereIsAFeshFoodDoorOpenUponInit)
{
   GivenTheRfidCommunicationControllerIsInAFreshFoodDoorOpenState();
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_FreshFoodDoorOpen);
}

TEST(RfidCommunicationController, ShouldEnterDemoModeStateIfDemoModeIsEnabledOnInit)
{
   GivenDemoModeIs(ENABLED);
   GivenInitialization();
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_DemoMode);
}

TEST(RfidCommunicationController, ShouldIncrementTheLeakCountWhenThereIsALeak)
{
   GivenInitialization();

   WhenRfidFilterLeakDetectedIs(SET);
   AndSentWithinAnRfidMessageWithResult(ReadWriteResult_Success);
   LeakDetectCountShouldBe(1);
}

TEST(RfidCommunicationController, ShouldIncrementTheBlockedRfidFilterCountWhenThereIsABlockedRfidFilter)
{
   GivenInitialization();

   WhenRfidBlockedIs(SET);
   AndSentWithinAnRfidMessageWithResult(ReadWriteResult_Success);
   BlockedCountShouldBe(1);
}

TEST(RfidCommunicationController, ShouldClearTheLeakDetectedCountWhenALeakIsNoLongerDetected)
{
   GivenInitialization();
   GivenTheLeakDetectCountIs(SomeLeakDetectedCount);

   WhenRfidFilterLeakDetectedIs(CLEAR);
   AndSentWithinAnRfidMessageWithResult(ReadWriteResult_Success);
   LeakDetectCountShouldBe(0);
}

TEST(RfidCommunicationController, ShouldClearTheBlockedCountWhenABlockedRfidFilterIsNoLongerDetected)
{
   GivenInitialization();
   WhenRfidBlockedIs(SET);
   AndSentWithinAnRfidMessageWithResult(ReadWriteResult_Success);
   BlockedCountShouldBe(1);

   WhenRfidBlockedIs(CLEAR);
   AndSentWithinAnRfidMessageWithResult(ReadWriteResult_Success);
   BlockedCountShouldBe(0);
}

TEST(RfidCommunicationController, ShouldSetBypassPlugWhenFilterIsABypassPlug)
{
   GivenInitialization();
   BypassPlugShouldBe(CLEAR);

   WhenFilterIdentifierIsABypassPlug();
   AndSentWithinAnRfidMessageWithResult(ReadWriteResult_Success);
   BypassPlugShouldBe(SET);
}

TEST(RfidCommunicationController, ShouldNotSetLeakDetectedOrBlockedWhenAFilterIsABypassPlug)
{
   GivenInitialization();
   BypassPlugShouldBe(CLEAR);

   WhenFilterIdentifierIsABypassPlug();
   WhenRfidFilterLeakDetectedIs(SET);
   WhenRfidBlockedIs(SET);
   AndSentWithinAnRfidMessageWithResult(ReadWriteResult_Success);

   BypassPlugShouldBe(SET);
   LeakDetectCountShouldBe(0);
   BlockedCountShouldBe(0);
}

TEST(RfidCommunicationController, ShouldClearLeakDetectedAndBlockedWhenAFilterIsABypassPlug)
{
   GivenInitialization();
   BypassPlugShouldBe(CLEAR);

   WhenRfidFilterLeakDetectedIs(SET);
   WhenRfidBlockedIs(SET);
   AndSentWithinAnRfidMessageWithResult(ReadWriteResult_Success);
   LeakDetectCountShouldBe(1);
   BlockedCountShouldBe(1);

   WhenFilterIdentifierIsABypassPlug();
   AndSentWithinAnRfidMessageWithResult(ReadWriteResult_Success);

   BypassPlugShouldBe(SET);
   LeakDetectCountShouldBe(0);
   BlockedCountShouldBe(0);
}

TEST(RfidCommunicationController, ShouldEnterDemoModeStateWhenDemoModeIsEnabled)
{
   GivenInitialization();
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_FreshFoodDoorOpen);

   WhenDemoModeIs(ENABLED);
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_DemoMode);
}

TEST(RfidCommunicationController, ShouldClearBypassPlugStateWhenEnteringDemoMode)
{
   GivenInitialization();
   GivenTheBypassPlugIs(SET);

   WhenDemoModeIs(ENABLED);
   BypassPlugShouldBe(CLEAR);
}

TEST(RfidCommunicationController, ShouldIgnoreDoorStatusWhenInDemoMode)
{
   GivenAllFreshFoodDoorsAre(Closed);
   GivenDemoModeIs(ENABLED);
   GivenInitialization();

   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_DemoMode);
   WhenAFreshFoodDoorIsOpened();
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_DemoMode);
}

TEST(RfidCommunicationController, ShouldReadRfidFilterAtEntryOfFreshFoodDoorIsOpen)
{
   GivenAFreshFoodDoorIsOpened();

   ShouldSendReadRequestToRfidFilter();
   WhenInitialized();
}

TEST(RfidCommunicationController, ShouldSendReadRequestWhenDoorOpenReadRequestTimerExpiresInFreshFoodDoorIsOpenState)
{
   GivenTheRfidCommunicationControllerIsInAFreshFoodDoorOpenState();

   NothingShouldHappen();
   After((rfidFilterUpdateRateData->doorOpenFilterReadFrequencyInSeconds * MSEC_PER_SEC) - 1);

   ShouldSendReadRequestToRfidFilter();
   After(1);
}

TEST(RfidCommunicationController, ShouldStopReadRequestTimerWhenTransitioningFromFreshFoodDoorIsOpenStateToDoorJustClosedState)
{
   GivenTheRfidCommunicationControllerIsInAFreshFoodDoorOpenState();

   ShouldSendReadRequestToRfidFilter();
   After(rfidFilterUpdateRateData->doorOpenFilterReadFrequencyInSeconds * MSEC_PER_SEC);

   ShouldSendReadRequestToRfidFilter();
   WhenAllFreshFoodDoorsAre(Closed);
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_AllFreshFoodDoorsJustClosed);

   NothingShouldHappen();
   After(rfidFilterUpdateRateData->doorOpenFilterReadFrequencyInSeconds * MSEC_PER_SEC);
}

TEST(RfidCommunicationController, ShouldResetBadReadCountAfterReceivingASuccessfulRfidFilterReadWhileInFreshFoodDoorIsOpenState)
{
   GivenTheRfidCommunicationControllerIsInAFreshFoodDoorOpenState();

   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_ReadFailure);
   RfidFilterBadReadCountShouldBe(1);

   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_Success);
   RfidFilterBadReadCountShouldBe(0);
}

TEST(RfidCommunicationController, ShouldResetHardwareFailureCountAfterReceivingASuccessfulRfidFilterReadWhileInFreshFoodDoorOpenState)
{
   GivenTheRfidCommunicationControllerIsInAFreshFoodDoorOpenState();

   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_HardwareFailure);
   RfidFilterHardwareFailureCountShouldBe(1);

   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_Success);
   RfidFilterHardwareFailureCountShouldBe(0);
}

TEST(RfidCommunicationController, ShouldUpdateMainboardRfidFilterUidWhenNewFilterIsDetectedWhileInFreshFoodDoorOpenState)
{
   GivenTheRfidCommunicationControllerIsInAFreshFoodDoorOpenState();
   MainboardRfidFilterUidShouldBe(Uid);

   WhenANewRfidFilterIsInstalled();
   MainboardRfidFilterUidShouldBe(NewUid);
}

TEST(RfidCommunicationController, ShouldNotUpdateMainboardWithNewFilterDataWhenThereIsASuccessfulReadOnTheCurrentFilterWhileInFreshFoodDoorOpenState)
{
   GivenTheRfidCommunicationControllerIsInAFreshFoodDoorOpenState();
   WhenCalendarUsageOnRfidBoardIs(SomeCalendarUsage);
   WhenVolumeUsageOnRfidBoardIs(SomeVolumeUsage);
   GivenTheABadWriteCountIs(1);
   NewFilterSignalAndVolumeUsageAndCalendarUsageShouldAllBeClear();

   RfidFilterNumberOfUnitsFilterHasBeenOnShouldBe(0);
   GivenRfidFilterUnitSerialNumberRfidBoardIs(SomePreviousUnitSerialNumber);

   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_Success);
   RfidFilterBadWriteCountShouldBe(1);
   NewFilterSignalAndVolumeUsageAndCalendarUsageShouldAllBeClear();
   RfidFilterNumberOfUnitsFilterHasBeenOnShouldBe(0);
   GivenRfidFilterUnitSerialNumberRfidBoardIs(SomePreviousUnitSerialNumber);
}

TEST(RfidCommunicationController, ShouldSetBypassPlugWhenABypassPlugIsInstalledWhileInFreshFoodDoorOpenState)
{
   GivenTheRfidCommunicationControllerIsInAFreshFoodDoorOpenState();
   BypassPlugShouldBe(CLEAR);

   WhenFilterIdentifierIsABypassPlug();
   AndSentWithinAnRfidMessageWithResult(ReadWriteResult_Success);
   BypassPlugShouldBe(SET);
}

TEST(RfidCommunicationController, ShouldClearBypassPlugWhenANonBypassPlugIsInstalledWhileInFreshFoodDoorOpenState)
{
   GivenTheRfidCommunicationControllerIsInAFreshFoodDoorOpenState();
   BypassPlugStateIs(SET);
   BypassPlugShouldBe(SET);

   WhenANewRfidFilterIsInstalled();
   BypassPlugShouldBe(CLEAR);
}

TEST(RfidCommunicationController, ShouldClearBadReadCountWhenANewFilterIsDetectedWhileInFreshFoodDoorOpenState)
{
   GivenTheRfidCommunicationControllerIsInAFreshFoodDoorOpenState();

   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_ReadFailure);
   RfidFilterBadReadCountShouldBe(1);

   WhenANewRfidFilterIsInstalled();
   RfidFilterBadReadCountShouldBe(0);
}

TEST(RfidCommunicationController, ShouldClearBadWriteCountWhenANewFilterIsDetectedWhileInFreshFoodDoorOpenState)
{
   GivenTheABadWriteCountIs(1);
   GivenTheRfidCommunicationControllerIsInAFreshFoodDoorOpenState();

   WhenANewRfidFilterIsInstalled();
   RfidFilterBadWriteCountShouldBe(0);
}

TEST(RfidCommunicationController, ShouldSendNewFilterInstalledSignalAndCopyCalendarAndVolumeUsageToMainboardWhenANewFilterIsDetectedInFreshFoodDoorOpenState)
{
   GivenTheRfidCommunicationControllerIsInAFreshFoodDoorOpenState();
   NewFilterSignalAndVolumeUsageAndCalendarUsageShouldAllBeClear();

   WhenCalendarUsageOnRfidBoardIs(SomeCalendarUsage);
   WhenVolumeUsageOnRfidBoardIs(SomeVolumeUsage);
   WhenANewRfidFilterIsInstalled();
   NewFilterInstalledSignalShouldBe(1);
   CalendarUsageOnMainboardShouldBe(SomeCalendarUsage);
   VolumeUsageOnMainboardShouldBe(SomeVolumeUsage);
   EepromCalendarUsageInSecondsOnMainboardShouldBe(SomeCalendarUsage);
   EepromVolumeUsageOnMainboardShouldBe(SomeVolumeUsage);
}

TEST(RfidCommunicationController, ShouldCopyRfidFilterUnitSerialNumberToPreviousSerialNumberWhenANewFilterIsDetectedAndTheSerialNumberIsDifferentThanTheUnitSerialNumberWhileInFreshFoodDoorOpenState)
{
   GivenRfidFilterUnitSerialNumberRfidBoardIs(SomePreviousUnitSerialNumber);
   GivenTheRfidCommunicationControllerIsInAFreshFoodDoorOpenState();
   RfidFilterPreviousUnitSerialNumberShouldBe(BlankSerialNumber);

   WhenANewRfidFilterIsInstalledWithADifferentSerialNumber(SomePreviousUnitSerialNumber);
   RfidFilterPreviousUnitSerialNumberShouldBe(SomePreviousUnitSerialNumber);
}

TEST(RfidCommunicationController, ShouldIncrementNumberOfUnitsFilterHasBeenOnWhenANewFilterIsDetectedAndTheSerialNumberIsDifferentThanTheUnitSerialNumberWhileInFreshFoodDoorOpenState)
{
   GivenRfidFilterUnitSerialNumberRfidBoardIs(SomePreviousUnitSerialNumber);
   GivenTheRfidCommunicationControllerIsInAFreshFoodDoorOpenState();
   RfidFilterNumberOfUnitsFilterHasBeenOnShouldBe(0);

   WhenANewRfidFilterIsInstalled();
   RfidFilterNumberOfUnitsFilterHasBeenOnShouldBe(1);
}

TEST(RfidCommunicationController, ShouldIncrementBadReadCountWhenReceivingAReadFailureWhileInFreshFoodDoorOpenState)
{
   GivenTheRfidCommunicationControllerIsInAFreshFoodDoorOpenState();
   RfidFilterBadReadCountShouldBe(0);

   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_ReadFailure);
   RfidFilterBadReadCountShouldBe(1);
}

TEST(RfidCommunicationController, ShouldIncrementBadReadCountWhenReceivingAReadBeforeWriteFailureWhileInFreshFoodDoorOpenState)
{
   GivenTheRfidCommunicationControllerIsInAFreshFoodDoorOpenState();
   RfidFilterBadReadCountShouldBe(0);

   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_ReadBeforeWriteFailure);
   RfidFilterBadReadCountShouldBe(1);
}

TEST(RfidCommunicationController, ShouldIncrementBadReadCountWhenReceivingAReadAfterWriteFailureWhileInFreshFoodDoorOpenState)
{
   GivenTheRfidCommunicationControllerIsInAFreshFoodDoorOpenState();
   RfidFilterBadReadCountShouldBe(0);

   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_ReadAfterWriteFailure);
   RfidFilterBadReadCountShouldBe(1);
}

TEST(RfidCommunicationController, ShouldIncrementHardwareFailureCountWhenReceivingAHardwareFailureWhileInFreshFoodDoorOpenState)
{
   GivenTheRfidCommunicationControllerIsInAFreshFoodDoorOpenState();
   RfidFilterHardwareFailureCountShouldBe(0);

   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_HardwareFailure);
   RfidFilterHardwareFailureCountShouldBe(1);
}

TEST(RfidCommunicationController, ShouldTransitionToAllFreshFoodDoorsJustClosedStateWhenAllDoorsBecomeClosedWhileInFreshFoodDoorOpenState)
{
   GivenTheRfidCommunicationControllerIsInAFreshFoodDoorOpenState();
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_FreshFoodDoorOpen);

   ShouldSendReadRequestToRfidFilter();
   WhenAllFreshFoodDoorsAre(Closed);
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_AllFreshFoodDoorsJustClosed);
}

TEST(RfidCommunicationController, ShouldReadRfidFilterAtEntryOfFreshFoodDoorIsClosed)
{
   GivenAllFreshFoodDoorsAre(Closed);

   ShouldSendReadRequestToRfidFilter();
   WhenInitialized();
}

TEST(RfidCommunicationController, ShouldTransitionToFreshFoodDoorOpenStateAndRequestToReadFilterWhenADoorOpensWhileInAllDoorsClosedReadState)
{
   GivenTheRfidCommunicationControllerIsInAllDoorsClosedReadState();

   ShouldSendReadRequestToRfidFilter();
   WhenAFreshFoodDoorIsOpened();
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_FreshFoodDoorOpen);
}

TEST(RfidCommunicationController, ShouldSendReadRequestWhenDoorClosedReadRequestTimerExpires)
{
   GivenTheRfidCommunicationControllerIsInAllFreshFoodDoorsJustClosedState();

   NothingShouldHappen();
   After((rfidFilterUpdateRateData->doorJustClosedFilterReadFrequencyInSeconds * MSEC_PER_SEC) - 1);

   ShouldSendReadRequestToRfidFilter();
   After(1);
}

TEST(RfidCommunicationController, ShouldTransitionToAllDoorsClosedReadStateAndStopReadRequestTimerWhenOverallTimeoutTimerExpiresWhileInAllFreshFoodDoorsJustClosedState)
{
   GivenTheRfidCommunicationControllerIsInAllFreshFoodDoorsJustClosedState();
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_AllFreshFoodDoorsJustClosed);

   ShouldSendReadRequestToRfidFilter();
   After((rfidFilterUpdateRateData->doorJustClosedFilterReadTimeoutInSeconds * MSEC_PER_SEC));

   NothingShouldHappen();
   After(rfidFilterUpdateRateData->doorJustClosedFilterReadFrequencyInSeconds * MSEC_PER_SEC);

   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_AllDoorsClosedRead);
}

TEST(RfidCommunicationController, ShouldResetBadReadCountAfterReceivingASuccessfulRfidFilterReadWhileInAllFreshFoodDoorsJustClosedState)
{
   GivenTheRfidCommunicationControllerIsInAllFreshFoodDoorsJustClosedState();

   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_ReadFailure);
   RfidFilterBadReadCountShouldBe(1);

   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_Success);
   RfidFilterBadReadCountShouldBe(0);
}

TEST(RfidCommunicationController, ShouldResetHardwareFailureCountAfterReceivingASuccessfulRfidFilterReadWhileInAllFreshFoodDoorsJustClosedState)
{
   GivenTheRfidCommunicationControllerIsInAllFreshFoodDoorsJustClosedState();

   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_HardwareFailure);
   RfidFilterHardwareFailureCountShouldBe(1);

   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_Success);
   RfidFilterHardwareFailureCountShouldBe(0);
}

TEST(RfidCommunicationController, ShouldUpdateMainboardRfidFilterUidWhenNewFilterIsDetectedWhileInAllFreshFoodDoorsJustClosedState)
{
   GivenTheRfidCommunicationControllerIsInAllFreshFoodDoorsJustClosedState();
   MainboardRfidFilterUidShouldBe(Uid);

   WhenANewRfidFilterIsInstalled();
   MainboardRfidFilterUidShouldBe(NewUid);
}

TEST(RfidCommunicationController, ShouldNotUpdateMainboardWithNewFilterDataWhenThereIsASuccessfulReadOnTheCurrentFilterWhileInAllFreshFoodDoorsJustClosedState)
{
   GivenTheRfidCommunicationControllerIsInAllFreshFoodDoorsJustClosedState();
   WhenCalendarUsageOnRfidBoardIs(SomeCalendarUsage);
   WhenVolumeUsageOnRfidBoardIs(SomeVolumeUsage);
   GivenTheABadWriteCountIs(1);
   NewFilterSignalAndVolumeUsageAndCalendarUsageShouldAllBeClear();

   RfidFilterNumberOfUnitsFilterHasBeenOnShouldBe(0);
   GivenRfidFilterUnitSerialNumberRfidBoardIs(SomePreviousUnitSerialNumber);

   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_Success);
   RfidFilterBadWriteCountShouldBe(1);
   NewFilterSignalAndVolumeUsageAndCalendarUsageShouldAllBeClear();
   RfidFilterNumberOfUnitsFilterHasBeenOnShouldBe(0);
   GivenRfidFilterUnitSerialNumberRfidBoardIs(SomePreviousUnitSerialNumber);
}

TEST(RfidCommunicationController, ShouldSetBypassPlugWhenABypassPlugIsInstalledWhileInAllFreshFoodDoorsJustClosedState)
{
   GivenTheRfidCommunicationControllerIsInAllFreshFoodDoorsJustClosedState();
   BypassPlugShouldBe(CLEAR);

   WhenFilterIdentifierIsABypassPlug();
   AndSentWithinAnRfidMessageWithResult(ReadWriteResult_Success);
   BypassPlugShouldBe(SET);
}

TEST(RfidCommunicationController, ShouldClearBypassPlugWhenANonBypassPlugIsInstalledWhileInAllFreshFoodDoorsJustClosedState)
{
   GivenTheRfidCommunicationControllerIsInAllFreshFoodDoorsJustClosedState();
   BypassPlugStateIs(SET);
   BypassPlugShouldBe(SET);

   WhenANewRfidFilterIsInstalled();
   BypassPlugShouldBe(CLEAR);
}

TEST(RfidCommunicationController, ShouldClearBadWriteCountWhenANewFilterIsDetectedWhileInAllFreshFoodDoorsJustClosedState)
{
   GivenTheABadWriteCountIs(1);
   GivenTheRfidCommunicationControllerIsInAllFreshFoodDoorsJustClosedState();

   WhenANewRfidFilterIsInstalled();
   RfidFilterBadWriteCountShouldBe(0);
}

TEST(RfidCommunicationController, ShouldTransitionToAllFreshFoodDoorsClosedWhenANewFilterIsDetectedInAllFreshFoodDoorsJustClosedState)
{
   GivenTheRfidCommunicationControllerIsInAllFreshFoodDoorsJustClosedState();
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_AllFreshFoodDoorsJustClosed);

   WhenANewRfidFilterIsInstalled();
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_AllDoorsClosedRead);
}

TEST(RfidCommunicationController, ShouldCopyRfidFilterUnitSerialNumberToPreviousSerialNumberWhenANewFilterIsDetectedAndTheSerialNumberIsDifferentThanTheUnitSerialNumberWhileInAllFreshFoodDoorsJustClosedState)
{
   GivenRfidFilterUnitSerialNumberRfidBoardIs(SomePreviousUnitSerialNumber);
   GivenTheRfidCommunicationControllerIsInAllFreshFoodDoorsJustClosedState();

   RfidFilterPreviousUnitSerialNumberShouldBe(BlankSerialNumber);

   WhenANewRfidFilterIsInstalled();
   RfidFilterPreviousUnitSerialNumberShouldBe(SomePreviousUnitSerialNumber);
}

TEST(RfidCommunicationController, ShouldIncrementNumberOfUnitsFilterHasBeenOnWhenANewFilterIsDetectedAndTheSerialNumberIsDifferentThanTheUnitSerialNumberWhileInAllFreshFoodDoorsJustClosedState)
{
   GivenRfidFilterUnitSerialNumberRfidBoardIs(SomePreviousUnitSerialNumber);
   GivenTheRfidCommunicationControllerIsInAllFreshFoodDoorsJustClosedState();
   RfidFilterNumberOfUnitsFilterHasBeenOnShouldBe(0);

   WhenANewRfidFilterIsInstalled();
   RfidFilterNumberOfUnitsFilterHasBeenOnShouldBe(1);
}

TEST(RfidCommunicationController, ShouldSendNewFilterInstalledSignalAndCopyCalendarAndVolumeUsageToMainboardWhenANewFilterIsDetectedInAllFreshFoodDoorsJustClosedState)
{
   GivenTheRfidCommunicationControllerIsInAllFreshFoodDoorsJustClosedState();
   NewFilterSignalAndVolumeUsageAndCalendarUsageShouldAllBeClear();

   WhenCalendarUsageOnRfidBoardIs(SomeCalendarUsage);
   WhenVolumeUsageOnRfidBoardIs(SomeVolumeUsage);
   WhenANewRfidFilterIsInstalled();
   NewFilterInstalledSignalShouldBe(1);
   CalendarUsageOnMainboardShouldBe(SomeCalendarUsage);
   VolumeUsageOnMainboardShouldBe(SomeVolumeUsage);
   EepromCalendarUsageInSecondsOnMainboardShouldBe(SomeCalendarUsage);
   EepromVolumeUsageOnMainboardShouldBe(SomeVolumeUsage);
}

TEST(RfidCommunicationController, ShouldIncrementBadReadCountWhenReceivingAReadFailureInFreshFoodDoorOpenStateWhileInAllFreshFoodDoorsJustClosedState)
{
   GivenTheRfidCommunicationControllerIsInAllFreshFoodDoorsJustClosedState();
   RfidFilterBadReadCountShouldBe(0);

   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_ReadFailure);
   RfidFilterBadReadCountShouldBe(1);
}

TEST(RfidCommunicationController, ShouldIncrementBadReadCountWhenReceivingAReadBeforeWriteFailureWhileInAllFreshFoodDoorsJustClosedState)
{
   GivenTheRfidCommunicationControllerIsInAllFreshFoodDoorsJustClosedState();
   RfidFilterBadReadCountShouldBe(0);

   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_ReadBeforeWriteFailure);
   RfidFilterBadReadCountShouldBe(1);
}

TEST(RfidCommunicationController, ShouldIncrementBadReadCountWhenReceivingAReadAfterWriteFailureWhileInAllFreshFoodDoorsJustClosedState)
{
   GivenTheRfidCommunicationControllerIsInAllFreshFoodDoorsJustClosedState();
   RfidFilterBadReadCountShouldBe(0);

   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_ReadAfterWriteFailure);
   RfidFilterBadReadCountShouldBe(1);
}

TEST(RfidCommunicationController, ShouldIncrementHardwareFailureCountWhenReceivingAHardwareFailureWhileInAllFreshFoodDoorsJustClosedState)
{
   GivenTheRfidCommunicationControllerIsInAllFreshFoodDoorsJustClosedState();
   RfidFilterHardwareFailureCountShouldBe(0);

   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_HardwareFailure);
   RfidFilterHardwareFailureCountShouldBe(1);
}

TEST(RfidCommunicationController, ShouldTransitionToFreshFoodDoorOpenStateWhenADoorIsOpenedInAllFreshFoodDoorsJustClosedState)
{
   GivenTheRfidCommunicationControllerIsInAllFreshFoodDoorsJustClosedState();

   ShouldSendReadRequestToRfidFilter();
   WhenAFreshFoodDoorIsOpened();
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_FreshFoodDoorOpen);
}

TEST(RfidCommunicationController, ShouldResetBadReadCountAfterReceivingASuccessfulRfidFilterReadWhileInAllDoorsClosedReadState)
{
   GivenTheRfidCommunicationControllerIsInAllDoorsClosedReadState();

   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_ReadFailure);
   RfidFilterBadReadCountShouldBe(1);

   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_Success);
   RfidFilterBadReadCountShouldBe(0);
}

TEST(RfidCommunicationController, ShouldResetHardwareFailureCountAfterReceivingASuccessfulRfidFilterReadWhileInAllDoorsClosedReadState)
{
   GivenTheRfidCommunicationControllerIsInAllDoorsClosedReadState();

   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_HardwareFailure);
   RfidFilterHardwareFailureCountShouldBe(1);

   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_Success);
   RfidFilterHardwareFailureCountShouldBe(0);
}

TEST(RfidCommunicationController, ShouldUpdateMainboardRfidFilterUidWhenNewFilterIsDetectedWhileInInAllDoorsClosedReadState)
{
   GivenTheRfidCommunicationControllerIsInAllDoorsClosedReadState();
   MainboardRfidFilterUidShouldBe(Uid);

   WhenANewRfidFilterIsInstalled();
   MainboardRfidFilterUidShouldBe(NewUid);
}

TEST(RfidCommunicationController, ShouldNotUpdateMainboardWithNewFilterDataWhenThereIsASuccessfulReadOnTheCurrentFilterWhileInInAllDoorsClosedReadState)
{
   GivenTheRfidCommunicationControllerIsInAllDoorsClosedReadState();
   WhenCalendarUsageOnRfidBoardIs(SomeCalendarUsage);
   WhenVolumeUsageOnRfidBoardIs(SomeVolumeUsage);
   GivenTheABadWriteCountIs(1);
   NewFilterSignalAndVolumeUsageAndCalendarUsageShouldAllBeClear();

   RfidFilterNumberOfUnitsFilterHasBeenOnShouldBe(0);
   GivenRfidFilterUnitSerialNumberRfidBoardIs(SomePreviousUnitSerialNumber);

   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_Success);
   RfidFilterBadWriteCountShouldBe(1);
   NewFilterSignalAndVolumeUsageAndCalendarUsageShouldAllBeClear();
   RfidFilterNumberOfUnitsFilterHasBeenOnShouldBe(0);
   GivenRfidFilterUnitSerialNumberRfidBoardIs(SomePreviousUnitSerialNumber);
}

TEST(RfidCommunicationController, ShouldSetBypassPlugWhenABypassPlugIsInstalledWhileInAllDoorsClosedReadState)
{
   GivenTheRfidCommunicationControllerIsInAllDoorsClosedReadState();
   BypassPlugShouldBe(CLEAR);

   WhenFilterIdentifierIsABypassPlug();
   AndSentWithinAnRfidMessageWithResult(ReadWriteResult_Success);
   BypassPlugShouldBe(SET);
}

TEST(RfidCommunicationController, ShouldClearBypassPlugWhenANonBypassPlugIsInstalledWhileInAllDoorsClosedReadState)
{
   GivenTheRfidCommunicationControllerIsInAllDoorsClosedReadState();
   BypassPlugStateIs(SET);
   BypassPlugShouldBe(SET);

   WhenANewRfidFilterIsInstalled();
   BypassPlugShouldBe(CLEAR);
}

TEST(RfidCommunicationController, ShouldClearBadWriteCountWhenANewFilterIsInstalledWhileInAllDoorsClosedReadState)
{
   GivenTheABadWriteCountIs(1);
   GivenTheRfidCommunicationControllerIsInAllDoorsClosedReadState();

   WhenANewRfidFilterIsInstalled();
   RfidFilterBadWriteCountShouldBe(0);
}

TEST(RfidCommunicationController, ShouldSendNewFilterInstalledSignalAndCopyCalendarAndVolumeUsageToMainboardWhenANewFilterIsDetectedWhileInAllDoorsClosedReadState)
{
   GivenTheRfidCommunicationControllerIsInAllDoorsClosedReadState();
   NewFilterInstalledSignalShouldBe(0);
   CalendarUsageOnMainboardShouldBe(0);
   VolumeUsageOnMainboardShouldBe(0);

   WhenCalendarUsageOnRfidBoardIs(SomeCalendarUsage);
   WhenVolumeUsageOnRfidBoardIs(SomeVolumeUsage);
   WhenANewRfidFilterIsInstalled();
   NewFilterInstalledSignalShouldBe(1);
   CalendarUsageOnMainboardShouldBe(SomeCalendarUsage);
   VolumeUsageOnMainboardShouldBe(SomeVolumeUsage);
}

TEST(RfidCommunicationController, ShouldCopyRfidFilterUnitSerialNumberToPreviousSerialNumberWhenANewFilterIsDetectedAndTheSerialNumberIsDifferentThanTheUnitSerialNumberWhileInAllDoorsClosedReadState)
{
   GivenRfidFilterUnitSerialNumberRfidBoardIs(SomePreviousUnitSerialNumber);
   GivenTheRfidCommunicationControllerIsInAllDoorsClosedReadState();

   RfidFilterPreviousUnitSerialNumberShouldBe(BlankSerialNumber);

   WhenANewRfidFilterIsInstalled();
   RfidFilterPreviousUnitSerialNumberShouldBe(SomePreviousUnitSerialNumber);
}

TEST(RfidCommunicationController, ShouldIncrementNumberOfUnitsFilterHasBeenOnWhenANewFilterIsDetectedAndTheSerialNumberIsDifferentThanTheUnitSerialNumberWhileInAllDoorsClosedReadState)
{
   GivenRfidFilterUnitSerialNumberRfidBoardIs(SomePreviousUnitSerialNumber);
   GivenTheRfidCommunicationControllerIsInAllDoorsClosedReadState();
   RfidFilterNumberOfUnitsFilterHasBeenOnShouldBe(0);

   WhenANewRfidFilterIsInstalled();
   RfidFilterNumberOfUnitsFilterHasBeenOnShouldBe(1);
}

TEST(RfidCommunicationController, ShouldTransitionToFreshFoodDoorOpenStateWhenDoorIsOpenedInAllDoorsClosedWriteState)
{
   GivenTheRfidCommunicationControllerIsInAllDoorsClosedWriteState();
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_AllDoorsClosedWrite);

   ShouldSendReadRequestToRfidFilter();
   WhenAFreshFoodDoorIsOpened();
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_FreshFoodDoorOpen);
}

TEST(RfidCommunicationController, ShouldTransisitionToAllDoorsClosedWriteStateAndSendWriteRequestWhenDoorClosedFilterUpdateTimerExpires)
{
   GivenTheRfidCommunicationControllerIsInAllDoorsClosedReadState();
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_AllDoorsClosedRead);

   ShouldSendReadRequestToRfidFilter();
   After(rfidFilterUpdateRateData->doorClosedFilterUpdateTimeInMinutes * MSEC_PER_MIN);

   ShouldSendWriteRequestToRfidFilter();
   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_Success);

   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_AllDoorsClosedWrite);
}

TEST(RfidCommunicationController, ShouldNotWriteToABypassPlug)
{
   GivenTheRfidCommunicationControllerIsInAllDoorsClosedReadState();
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_AllDoorsClosedRead);

   ShouldSendReadRequestToRfidFilter();
   After(rfidFilterUpdateRateData->doorClosedFilterUpdateTimeInMinutes * MSEC_PER_MIN);

   WhenFilterIdentifierIsABypassPlug();

   NothingShouldHappen();
   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_Success);
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_AllDoorsClosedRead);
}

TEST(RfidCommunicationController, ShouldRestartReadWriteTimerWhenReadWriteTimerExpiresOnAFilterIsBypassPlug)
{
   GivenTheRfidCommunicationControllerIsInAllDoorsClosedReadState();
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_AllDoorsClosedRead);

   ShouldSendReadRequestToRfidFilter();
   After(rfidFilterUpdateRateData->doorClosedFilterUpdateTimeInMinutes * MSEC_PER_MIN);

   WhenFilterIdentifierIsABypassPlug();
   NothingShouldHappen();
   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_Success);

   ShouldSendReadRequestToRfidFilter();
   After(rfidFilterUpdateRateData->doorClosedFilterUpdateTimeInMinutes * MSEC_PER_MIN);
}

TEST(RfidCommunicationController, ShouldSendReadRequestWhenDoorClosedFilterUpdateTimerExpiresAndThereAreNoLeaksPresent)
{
   WhenRfidFilterLeakDetectedIs(CLEAR);
   GivenTheRfidCommunicationControllerIsInAllDoorsClosedReadState();

   NothingShouldHappen();
   After((rfidFilterUpdateRateData->doorClosedFilterUpdateTimeInMinutes * MSEC_PER_MIN) - 1);

   ShouldSendReadRequestToRfidFilter();
   After(1);
}

TEST(RfidCommunicationController, ShouldSendReadRequestButNotTransitionToAllDoorsClosedWriteWhenDoorClosedErrorDetectedReadTimerExpiresAndThereIsALeakPresent)
{
   WhenRfidFilterLeakDetectedIs(SET);
   GivenTheRfidCommunicationControllerIsInAllDoorsClosedReadState();
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_AllDoorsClosedRead);

   NothingShouldHappen();
   After((rfidFilterUpdateRateData->doorClosedRfidErrorDetectedReadFrequencySeconds * MSEC_PER_SEC) - 1);

   ShouldSendReadRequestToRfidFilter();
   After(1);

   NothingShouldHappen();
   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_Success);
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_AllDoorsClosedRead);
}

TEST(RfidCommunicationController, ShouldSendReadRequestButNotTransitionToAllDoorsClosedWriteWhenDoorClosedErrorDetectedReadTimerExpiresAndTheLastRfidReadWasAReadFailure)
{
   GivenTheRfidCommunicationControllerIsInAllFreshFoodDoorsJustClosedState();
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_AllFreshFoodDoorsJustClosed);

   ShouldSendReadRequestToRfidFilter();
   After(rfidFilterUpdateRateData->doorJustClosedFilterReadFrequencyInSeconds * MSEC_PER_SEC);
   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_ReadFailure);
   After(rfidFilterUpdateRateData->doorJustClosedFilterReadFrequencyInSeconds * MSEC_PER_SEC);
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_AllDoorsClosedRead);

   NothingShouldHappen();
   After((rfidFilterUpdateRateData->doorClosedRfidErrorDetectedReadFrequencySeconds * MSEC_PER_SEC) - 1);

   ShouldSendReadRequestToRfidFilter();
   After(1);

   NothingShouldHappen();
   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_Success);
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_AllDoorsClosedRead);
}

TEST(RfidCommunicationController, ShouldSendReadRequestButNotTransitionToAllDoorsClosedWriteWhenDoorClosedErrorDetectedReadTimerExpiresAndTheLastRfidReadWasAReadBeforeWriteFailure)
{
   GivenTheRfidCommunicationControllerIsInAllFreshFoodDoorsJustClosedState();
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_AllFreshFoodDoorsJustClosed);

   ShouldSendReadRequestToRfidFilter();
   After(rfidFilterUpdateRateData->doorJustClosedFilterReadFrequencyInSeconds * MSEC_PER_SEC);
   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_ReadBeforeWriteFailure);
   After(rfidFilterUpdateRateData->doorJustClosedFilterReadFrequencyInSeconds * MSEC_PER_SEC);
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_AllDoorsClosedRead);

   NothingShouldHappen();
   After((rfidFilterUpdateRateData->doorClosedRfidErrorDetectedReadFrequencySeconds * MSEC_PER_SEC) - 1);

   ShouldSendReadRequestToRfidFilter();
   After(1);

   NothingShouldHappen();
   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_Success);
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_AllDoorsClosedRead);
}

TEST(RfidCommunicationController, ShouldSendReadRequestButNotTransitionToAllDoorsClosedWriteWhenDoorClosedErrorDetectedReadTimerExpiresAndTheLastRfidReadWasAReadAfterWriteFailure)
{
   GivenTheRfidCommunicationControllerIsInAllFreshFoodDoorsJustClosedState();
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_AllFreshFoodDoorsJustClosed);

   ShouldSendReadRequestToRfidFilter();
   After(rfidFilterUpdateRateData->doorJustClosedFilterReadFrequencyInSeconds * MSEC_PER_SEC);
   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_ReadAfterWriteFailure);
   After(rfidFilterUpdateRateData->doorJustClosedFilterReadFrequencyInSeconds * MSEC_PER_SEC);
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_AllDoorsClosedRead);

   NothingShouldHappen();
   After((rfidFilterUpdateRateData->doorClosedRfidErrorDetectedReadFrequencySeconds * MSEC_PER_SEC) - 1);

   ShouldSendReadRequestToRfidFilter();
   After(1);

   NothingShouldHappen();
   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_Success);
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_AllDoorsClosedRead);
}

TEST(RfidCommunicationController, ShouldSendReadRequestButNotTransitionToAllDoorsClosedWriteWhenDoorClosedErrorDetectedReadTimerExpiresAndTheLastRfidReadWasAHardwareFailure)
{
   GivenTheRfidCommunicationControllerIsInAllFreshFoodDoorsJustClosedState();
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_AllFreshFoodDoorsJustClosed);

   ShouldSendReadRequestToRfidFilter();
   After(rfidFilterUpdateRateData->doorJustClosedFilterReadFrequencyInSeconds * MSEC_PER_SEC);
   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_HardwareFailure);
   After(rfidFilterUpdateRateData->doorJustClosedFilterReadFrequencyInSeconds * MSEC_PER_SEC);
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_AllDoorsClosedRead);

   NothingShouldHappen();
   After((rfidFilterUpdateRateData->doorClosedRfidErrorDetectedReadFrequencySeconds * MSEC_PER_SEC) - 1);

   ShouldSendReadRequestToRfidFilter();
   After(1);

   NothingShouldHappen();
   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_Success);
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_AllDoorsClosedRead);
}

TEST(RfidCommunicationController, ShouldStopReadRequestTimerWhenExitingAllDoorsClosedReadState)
{
   WhenRfidFilterLeakDetectedIs(SET);
   GivenTheRfidCommunicationControllerIsInAllDoorsClosedReadState();

   ShouldSendReadRequestToRfidFilterMultipleTimes(181);
   After(rfidFilterUpdateRateData->doorClosedFilterUpdateTimeInMinutes * MSEC_PER_MIN);

   ShouldSendWriteRequestToRfidFilter();
   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_Success);

   NothingShouldHappen();
   After(rfidFilterUpdateRateData->doorClosedRfidErrorDetectedReadFrequencySeconds * MSEC_PER_SEC);
}

TEST(RfidCommunicationController, ShouldIncrementBadReadCountAndStartErrorDetectedReadTimerWhenReceivingAReadFailureWhileInAllDoorsClosedReadState)
{
   GivenTheRfidCommunicationControllerIsInAllDoorsClosedReadState();
   RfidFilterBadReadCountShouldBe(0);

   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_ReadFailure);
   RfidFilterBadReadCountShouldBe(1);

   NothingShouldHappen();
   After((rfidFilterUpdateRateData->doorClosedRfidErrorDetectedReadFrequencySeconds * MSEC_PER_SEC) - 1);

   ShouldSendReadRequestToRfidFilter();
   After(1);
}

TEST(RfidCommunicationController, ShouldIncrementBadReadCountAndStartErrorDetectedReadTimerWhenReceivingAReadBeforeWriteFailureWhileInAllDoorsClosedReadState)
{
   GivenTheRfidCommunicationControllerIsInAllDoorsClosedReadState();
   RfidFilterBadReadCountShouldBe(0);

   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_ReadBeforeWriteFailure);
   RfidFilterBadReadCountShouldBe(1);

   NothingShouldHappen();
   After((rfidFilterUpdateRateData->doorClosedRfidErrorDetectedReadFrequencySeconds * MSEC_PER_SEC) - 1);

   ShouldSendReadRequestToRfidFilter();
   After(1);
}

TEST(RfidCommunicationController, ShouldIncrementBadReadCountAndStartErrorDetectedReadTimerWhenReceivingAReadAfterWriteFailureWhileInAllDoorsClosedReadState)
{
   GivenTheRfidCommunicationControllerIsInAllDoorsClosedReadState();
   RfidFilterBadReadCountShouldBe(0);

   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_ReadAfterWriteFailure);
   RfidFilterBadReadCountShouldBe(1);

   NothingShouldHappen();
   After((rfidFilterUpdateRateData->doorClosedRfidErrorDetectedReadFrequencySeconds * MSEC_PER_SEC) - 1);

   ShouldSendReadRequestToRfidFilter();
   After(1);
}

TEST(RfidCommunicationController, ShouldIncrementHardwareFailureCountAndStartErrorDetectedReadTimerWhenReceivingAHardwareFailureWhileInAllDoorsClosedReadState)
{
   GivenTheRfidCommunicationControllerIsInAllDoorsClosedReadState();
   RfidFilterHardwareFailureCountShouldBe(0);

   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_HardwareFailure);
   RfidFilterHardwareFailureCountShouldBe(1);

   NothingShouldHappen();
   After((rfidFilterUpdateRateData->doorClosedRfidErrorDetectedReadFrequencySeconds * MSEC_PER_SEC) - 1);

   ShouldSendReadRequestToRfidFilter();
   After(1);
}

TEST(RfidCommunicationController, ShouldTransitionToAllDoorsClosedReadStateFromAllDoorsClosedWriteStateWhenFilterWriteTimeoutExpires)
{
   GivenTheRfidCommunicationControllerIsInAllDoorsClosedWriteState();

   NothingShouldHappen();
   After((rfidFilterUpdateRateData->rfidFilterWriteTimeoutInMinutes * MSEC_PER_MIN) - 1);

   After(1);
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_AllDoorsClosedRead);
}

TEST(RfidCommunicationController, ShouldIncrementBadWriteCountAndTransitionToAllDoorsClosedReadStateWhenAnRfidFilterReadWriteRequestIsEepromWriteFailure)
{
   GivenTheRfidCommunicationControllerIsInAllDoorsClosedWriteState();
   RfidFilterBadWriteCountShouldBe(0);

   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_EepromWriteFailure);
   RfidFilterBadWriteCountShouldBe(1);

   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_AllDoorsClosedRead);
}

TEST(RfidCommunicationController, ShouldIncrementBadWriteCountAndTransitionToAllDoorsClosedReadStateWhenAnRfidFilterReadWriteRequestIsUidMismatch)
{
   GivenTheRfidCommunicationControllerIsInAllDoorsClosedWriteState();
   RfidFilterBadWriteCountShouldBe(0);

   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_UidMismatch);
   RfidFilterBadWriteCountShouldBe(1);

   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_AllDoorsClosedRead);
}

TEST(RfidCommunicationController, ShouldIncrementBadWriteCountAndTransitionToAllDoorsClosedReadStateWhenAnRfidFilterReadWriteRequestIsTagUidIsInvalid)
{
   GivenTheRfidCommunicationControllerIsInAllDoorsClosedWriteState();
   RfidFilterBadWriteCountShouldBe(0);

   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_TagUidIsInvalid);
   RfidFilterBadWriteCountShouldBe(1);

   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_AllDoorsClosedRead);
}

TEST(RfidCommunicationController, ShouldTransitionToReadWhenReceivingASuccessfulWriteMessage)
{
   GivenTheRfidCommunicationControllerIsInAllDoorsClosedWriteState();

   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_Success);
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_AllDoorsClosedRead);
}

TEST(RfidCommunicationController, ShouldStopReadRequestTimerWhenTransitioningToDemoStateWhileInFreshFoodDoorOpenState)
{
   GivenTheRfidCommunicationControllerIsInAFreshFoodDoorOpenState();

   NothingShouldHappen();
   After((rfidFilterUpdateRateData->doorOpenFilterReadFrequencyInSeconds * MSEC_PER_SEC) - 1);

   WhenDemoModeIs(ENABLED);

   NothingShouldHappen();
   After(1);
}
