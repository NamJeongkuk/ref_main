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
#include "Constants_Binary.h"
#include "PersonalityParametricData.h"
#include "Constants_Time.h"
#include "RfidFaultHandler.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

enum
{
   Open,
   Closed,

   FaultRequestMockOverride_Enabled = 1,
   FaultRequestMockOverride_Disabled = 0,

   SignalCount_1 = 1,
   SignalCount_2 = 2,
   SignalCount_3 = 3,
   SignalCount_4 = 4,

   SomeInvalidResponse = 0xFF
};

uint8_t Uid[RfidUidSizeInBytes] = "ABCDEF";
uint8_t NewUid[RfidUidSizeInBytes] = "ZYXWVU";

static const RfidCommunicationControllerConfig_t config = {
   .rfidCommunicationControllerStateErd = Erd_RfidCommunicationControllerState,
   .rfidFilterReadWriteResultErd = Erd_RfidFilterReadWriteResult_RfidBoard,
   .allFreshFoodDoorsAreClosedErd = Erd_AllFreshFoodDoorsAreClosed,
   .rfidFilterUidRfidBoardErd = Erd_RfidFilterUid_RfidBoard,
   .rfidFilterUidErd = Erd_RfidFilterUid,
   .rfidFilterStatusRfidBoardErd = Erd_RfidFilterStatus_RfidBoard,
   .rfidFilterIdentifierErd = Erd_RfidFilterIdentifier_RfidBoard,
   .demoModeEnableErd = Erd_EnableDemoModeStatus,
   .waterFilterTypeErd = Erd_WaterFilterType,
   .rfidFaultRequestErd = Erd_RfidFaultRequest,
   .rfidBoardTagAuthenticationFailedFault = Erd_RfidBoardTagAuthenticationFailedFault,
   .rfidBoardHardwareFailureFaultErd = Erd_RfidBoardHardwareFailureFault,
   .rfidBoardLeakDetectedFaultErd = Erd_RfidBoardLeakDetectedFault,
   .rfidBoardBlockedTagFaultErd = Erd_RfidBoardBlockedTagFault,
   .rfidFilterDataRequestErd = Erd_RfidFilterDataRequest,
   .newFilterInstalledSignalErd = Erd_NewFilterInstalledSignal,
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
   else if(args->erd == Erd_RfidFaultRequest)
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

   void ShouldSendFaultRequestWithLeakDetectedFault(bool status, uint8_t signal)
   {
      RfidFaultRequest_t rfidFaultRequest;
      DataModel_Read(dataModel, Erd_RfidFaultRequest, &rfidFaultRequest);

      ShouldSendFaultRequestWith(Erd_RfidBoardLeakDetectedFault, status, rfidFaultRequest.signal + signal);
   }

   void ShouldSendFaultRequestWithBlockedTagFault(bool status, uint8_t signal)
   {
      RfidFaultRequest_t rfidFaultRequest;
      DataModel_Read(dataModel, Erd_RfidFaultRequest, &rfidFaultRequest);

      ShouldSendFaultRequestWith(Erd_RfidBoardBlockedTagFault, status, rfidFaultRequest.signal + signal);
   }

   void ShouldSendFaultRequestWithTagAuthenticationFailedFault(bool status, uint8_t signal)
   {
      RfidFaultRequest_t rfidFaultRequest;
      DataModel_Read(dataModel, Erd_RfidFaultRequest, &rfidFaultRequest);

      ShouldSendFaultRequestWith(Erd_RfidBoardTagAuthenticationFailedFault, status, rfidFaultRequest.signal + signal);
   }

   void ShouldSendFaultRequestWithHardwareFailureFault(bool status, uint8_t signal)
   {
      RfidFaultRequest_t rfidFaultRequest;
      DataModel_Read(dataModel, Erd_RfidFaultRequest, &rfidFaultRequest);

      ShouldSendFaultRequestWith(Erd_RfidBoardHardwareFailureFault, status, rfidFaultRequest.signal + signal);
   }

   bool ReadWriteResultIsReadFailure(const ReadWriteResult_t result)
   {
      return (result == ReadWriteResult_ReadFailure ||
         result == ReadWriteResult_ReadBeforeWriteFailure ||
         result == ReadWriteResult_ReadAfterWriteFailure);
   }

   bool ReadWriteResultIsAWriteFailure(const ReadWriteResult_t result)
   {
      return (result == ReadWriteResult_EepromWriteFailure ||
         result == ReadWriteResult_UidMismatch ||
         result == ReadWriteResult_TagUidIsInvalid);
   }

   void ShouldClearAllFaults()
   {
      ShouldSendFaultRequestWithLeakDetectedFault(CLEAR, SignalCount_1);
      ShouldSendFaultRequestWithBlockedTagFault(CLEAR, SignalCount_2);
      ShouldSendFaultRequestWithTagAuthenticationFailedFault(CLEAR, SignalCount_3);
      ShouldSendFaultRequestWithHardwareFailureFault(CLEAR, SignalCount_4);
   }

   void WhenAnRfidMessageIsSentWithResult(ReadWriteResult_t result, bool faultRequestMockOverrideEnabled)
   {
      if(!faultRequestMockOverrideEnabled)
      {
         if(result == ReadWriteResult_Success)
         {
            if(RfidLeakDetectedIsSet())
            {
               ShouldSendFaultRequestWithLeakDetectedFault(SET, SignalCount_1);
            }
            else
            {
               ShouldSendFaultRequestWithLeakDetectedFault(CLEAR, SignalCount_1);
            }
            ShouldSendFaultRequestWithBlockedTagFault(CLEAR, SignalCount_2);
            ShouldSendFaultRequestWithTagAuthenticationFailedFault(CLEAR, SignalCount_3);
            ShouldSendFaultRequestWithHardwareFailureFault(CLEAR, SignalCount_4);
         }
         else if(ReadWriteResultIsReadFailure(result) || ReadWriteResultIsAWriteFailure(result))
         {
            ShouldSendFaultRequestWithTagAuthenticationFailedFault(SET, SignalCount_1);
         }
         else if(result == ReadWriteResult_HardwareFailure)
         {
            ShouldSendFaultRequestWithHardwareFailureFault(SET, SignalCount_1);
         }
      }
      RfidFilterReadWriteResult_t rfidFilterReadWriteResult;
      DataModel_Read(dataModel, Erd_RfidFilterReadWriteResult_RfidBoard, &rfidFilterReadWriteResult);
      rfidFilterReadWriteResult.signal++;
      rfidFilterReadWriteResult.result = result;
      DataModel_Write(dataModel, Erd_RfidFilterReadWriteResult_RfidBoard, &rfidFilterReadWriteResult);
   }

   void AndSentWithinAnRfidMessageWithResult(ReadWriteResult_t result, bool faultRequestMockOverride)
   {
      WhenAnRfidMessageIsSentWithResult(result, faultRequestMockOverride);
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

   bool RfidLeakDetectedIsSet()
   {
      FilterStatusRfidBoard_t rfidFilterStatus;
      DataModel_Read(dataModel, Erd_RfidFilterStatus_RfidBoard, &rfidFilterStatus);

      return BIT_STATE(rfidFilterStatus, RfidFilterStatusBits_LeakDetected);
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

   void WhenFilterIdentifierIsABypassPlug()
   {
      uint8_t filterId[FilterIdentifierSizeInBytes] = "BPXWFE";
      DataModel_Write(dataModel, Erd_RfidFilterIdentifier_RfidBoard, &filterId);
      DataModel_Write(dataModel, Erd_RfidFilterUid_RfidBoard, NewUid);
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

   void ShouldNotWriteToABypassPlug()
   {
      mock().expectNoCall("Rfid Filter Data Request Write");
   }

   void WhenANewRfidFilterIsInstalled()
   {
      DataModel_Write(dataModel, Erd_RfidFilterUid_RfidBoard, NewUid);
      WhenAnRfidMessageIsSentWithResult(ReadWriteResult_Success, FaultRequestMockOverride_Disabled);
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
      WhenAnRfidMessageIsSentWithResult(ReadWriteResult_Success, FaultRequestMockOverride_Disabled);
   }

   void GivenTheRfidCommunicationControllerIsInAllDoorsClosedReadState()
   {
      GivenTheRfidCommunicationControllerIsInAllFreshFoodDoorsJustClosedState();
      RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_AllFreshFoodDoorsJustClosed);

      ShouldSendReadRequestToRfidFilter();
      After(rfidFilterUpdateRateData->doorJustClosedFilterReadFrequencyInSeconds * MSEC_PER_SEC);
      WhenAnRfidMessageIsSentWithResult(ReadWriteResult_Success, FaultRequestMockOverride_Disabled);
      After(rfidFilterUpdateRateData->doorJustClosedFilterReadFrequencyInSeconds * MSEC_PER_SEC);
      RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_AllDoorsClosedRead);
   }

   void GivenTheRfidCommunicationControllerIsInAllDoorsClosedWriteState()
   {
      GivenTheRfidCommunicationControllerIsInAllDoorsClosedReadState();

      ShouldSendReadRequestToRfidFilter();
      After(rfidFilterUpdateRateData->doorClosedFilterUpdateTimeInMinutes * MSEC_PER_MIN);

      ShouldSendWriteRequestToRfidFilter();
      WhenAnRfidMessageIsSentWithResult(ReadWriteResult_Success, FaultRequestMockOverride_Disabled);
   }

   void NewFilterInstalledSignalShouldBe(Signal_t expected)
   {
      Signal_t actual;
      DataModel_Read(dataModel, Erd_NewFilterInstalledSignal, &actual);
      CHECK_EQUAL(expected, actual);
   }
};

TEST(RfidCommunicationController, ShouldUpdateTheWaterFilterTypeToXWFEUponInit)
{
   GivenInitialization();
   WaterFilterTypeShouldBe(WaterFilterType_XWFE);
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

TEST(RfidCommunicationController, ShouldRequestToSetTheLeakFaultWhenThereIsALeak)
{
   GivenInitialization();

   ShouldSendFaultRequestWithLeakDetectedFault(SET, SignalCount_1);
   ShouldSendFaultRequestWithBlockedTagFault(CLEAR, SignalCount_2);
   ShouldSendFaultRequestWithTagAuthenticationFailedFault(CLEAR, SignalCount_3);
   ShouldSendFaultRequestWithHardwareFailureFault(CLEAR, SignalCount_4);
   WhenRfidFilterLeakDetectedIs(SET);
   AndSentWithinAnRfidMessageWithResult(ReadWriteResult_Success, FaultRequestMockOverride_Enabled);
}

TEST(RfidCommunicationController, ShouldRequestToSetTheBlockedTagFaultWhenThereIsABlockedRfidFilter)
{
   GivenInitialization();

   ShouldSendFaultRequestWithLeakDetectedFault(CLEAR, SignalCount_1);
   ShouldSendFaultRequestWithBlockedTagFault(SET, SignalCount_2);
   ShouldSendFaultRequestWithTagAuthenticationFailedFault(CLEAR, SignalCount_3);
   ShouldSendFaultRequestWithHardwareFailureFault(CLEAR, SignalCount_4);
   WhenRfidBlockedIs(SET);
   AndSentWithinAnRfidMessageWithResult(ReadWriteResult_Success, FaultRequestMockOverride_Enabled);
}

TEST(RfidCommunicationController, ShouldRequestToClearTheLeakFaultWhenALeakIsNoLongerDetected)
{
   GivenInitialization();

   ShouldClearAllFaults();
   WhenRfidFilterLeakDetectedIs(CLEAR);
   AndSentWithinAnRfidMessageWithResult(ReadWriteResult_Success, FaultRequestMockOverride_Enabled);
}

TEST(RfidCommunicationController, ShouldRequestToClearTheBlockedTagFaultWhenABlockedRfidFilterIsNoLongerDetected)
{
   GivenInitialization();

   ShouldSendFaultRequestWithLeakDetectedFault(CLEAR, SignalCount_1);
   ShouldSendFaultRequestWithBlockedTagFault(SET, SignalCount_2);
   ShouldSendFaultRequestWithTagAuthenticationFailedFault(CLEAR, SignalCount_3);
   ShouldSendFaultRequestWithHardwareFailureFault(CLEAR, SignalCount_4);
   WhenRfidBlockedIs(SET);
   AndSentWithinAnRfidMessageWithResult(ReadWriteResult_Success, FaultRequestMockOverride_Enabled);

   ShouldClearAllFaults();
   WhenRfidBlockedIs(CLEAR);
   AndSentWithinAnRfidMessageWithResult(ReadWriteResult_Success, FaultRequestMockOverride_Enabled);
}

TEST(RfidCommunicationController, ShouldEnterDemoModeStateWhenDemoModeIsEnabled)
{
   GivenInitialization();
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_FreshFoodDoorOpen);

   WhenDemoModeIs(ENABLED);
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_DemoMode);
}

TEST(RfidCommunicationController, ShouldRemainInDemoModeWhenDemoModeIsDisabled)
{
   GivenInitialization();
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_FreshFoodDoorOpen);

   WhenDemoModeIs(ENABLED);
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_DemoMode);

   WhenDemoModeIs(DISABLED);
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_DemoMode);
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

TEST(RfidCommunicationController, ShouldNotRequestToSetTagAuthenticationFailedFaultAfterReceivingReadFailureWhileInDoorOpenState)
{
   GivenInitialization();

   NothingShouldHappen();
   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_ReadFailure, FaultRequestMockOverride_Enabled);
}

TEST(RfidCommunicationController, ShouldRequestToSetTagAuthenticationFailedFaultAfterReceivingReadFailureWhileInDoorsJustClosedState)
{
   GivenTheRfidCommunicationControllerIsInAllFreshFoodDoorsJustClosedState();

   ShouldSendFaultRequestWithTagAuthenticationFailedFault(SET, SignalCount_1);
   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_ReadFailure, FaultRequestMockOverride_Enabled);
}

TEST(RfidCommunicationController, ShouldRequestToSetTagAuthenticationFailedFaultWhenReceivingAReadBeforeWriteFailureWhileInDoorsJustClosedState)
{
   GivenTheRfidCommunicationControllerIsInAllFreshFoodDoorsJustClosedState();

   ShouldSendFaultRequestWithTagAuthenticationFailedFault(SET, SignalCount_1);
   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_ReadBeforeWriteFailure, FaultRequestMockOverride_Enabled);
}

TEST(RfidCommunicationController, ShouldRequestToSetTagAuthenticationFailedFaultWhenReceivingAReadAfterWriteFailureWhileInDoorsJustClosedState)
{
   GivenTheRfidCommunicationControllerIsInAllFreshFoodDoorsJustClosedState();

   ShouldSendFaultRequestWithTagAuthenticationFailedFault(SET, SignalCount_1);
   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_ReadAfterWriteFailure, FaultRequestMockOverride_Enabled);
}

TEST(RfidCommunicationController, ShouldRequestToSetTagAuthenticationFailedFaultAfterReceivingEepromWriteFailureWhileInDoorsJustClosedState)
{
   GivenTheRfidCommunicationControllerIsInAllFreshFoodDoorsJustClosedState();

   ShouldSendFaultRequestWithTagAuthenticationFailedFault(SET, SignalCount_1);
   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_EepromWriteFailure, FaultRequestMockOverride_Enabled);
}

TEST(RfidCommunicationController, ShouldRequestToSetTagAuthenticationFailedFaultWhenReceivingAUidMismatchWhileInDoorsJustClosedState)
{
   GivenTheRfidCommunicationControllerIsInAllFreshFoodDoorsJustClosedState();

   ShouldSendFaultRequestWithTagAuthenticationFailedFault(SET, SignalCount_1);
   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_UidMismatch, FaultRequestMockOverride_Enabled);
}

TEST(RfidCommunicationController, ShouldRequestToSetTagAuthenticationFailedFaultWhenReceivingATagUidIsInvalidWhileInDoorsJustClosedState)
{
   GivenTheRfidCommunicationControllerIsInAllFreshFoodDoorsJustClosedState();

   ShouldSendFaultRequestWithTagAuthenticationFailedFault(SET, SignalCount_1);
   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_TagUidIsInvalid, FaultRequestMockOverride_Enabled);
}

TEST(RfidCommunicationController, ShouldRequestToClearTagAuthenticationFailedFaultAfterReceivingASuccessfulRfidFilterReadWhileInDoorsJustClosedState)
{
   GivenTheRfidCommunicationControllerIsInAllFreshFoodDoorsJustClosedState();

   ShouldSendFaultRequestWithTagAuthenticationFailedFault(SET, SignalCount_1);
   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_ReadFailure, FaultRequestMockOverride_Enabled);

   ShouldClearAllFaults();
   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_Success, FaultRequestMockOverride_Enabled);
}

TEST(RfidCommunicationController, ShouldRequestToSetTheHardwareFailureFaultWhenReceivingAHardwareFailure)
{
   GivenInitialization();

   ShouldSendFaultRequestWithHardwareFailureFault(SET, SignalCount_1);
   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_HardwareFailure, FaultRequestMockOverride_Enabled);
}

TEST(RfidCommunicationController, ShouldRequestToClearHardwareFailureFaultAfterReceivingASuccessfulRfidFilterRead)
{
   GivenInitialization();

   ShouldSendFaultRequestWithHardwareFailureFault(SET, SignalCount_1);
   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_HardwareFailure, FaultRequestMockOverride_Enabled);

   ShouldClearAllFaults();
   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_Success, FaultRequestMockOverride_Enabled);
}

TEST(RfidCommunicationController, ShouldNotSendNewFilterInstallSignalWhenThereIsASuccessfulReadOnTheCurrentFilterWhileInFreshFoodDoorOpenState)
{
   GivenTheRfidCommunicationControllerIsInAFreshFoodDoorOpenState();
   NewFilterInstalledSignalShouldBe(0);

   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_Success, FaultRequestMockOverride_Disabled);
   NewFilterInstalledSignalShouldBe(0);
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

TEST(RfidCommunicationController, ShouldNotSendNewFilterInstallSignalWhenThereIsASuccessfulReadOnTheCurrentFilterWhileInAllFreshFoodDoorsJustClosedState)
{
   GivenTheRfidCommunicationControllerIsInAllFreshFoodDoorsJustClosedState();
   NewFilterInstalledSignalShouldBe(0);

   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_Success, FaultRequestMockOverride_Disabled);
   NewFilterInstalledSignalShouldBe(0);
}

TEST(RfidCommunicationController, ShouldTransitionToAllFreshFoodDoorsClosedWhenANewFilterIsDetectedInAllFreshFoodDoorsJustClosedState)
{
   GivenTheRfidCommunicationControllerIsInAllFreshFoodDoorsJustClosedState();
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_AllFreshFoodDoorsJustClosed);

   WhenANewRfidFilterIsInstalled();
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_AllDoorsClosedRead);
}

TEST(RfidCommunicationController, ShouldTransitionToFreshFoodDoorOpenStateWhenADoorIsOpenedInAllFreshFoodDoorsJustClosedState)
{
   GivenTheRfidCommunicationControllerIsInAllFreshFoodDoorsJustClosedState();

   ShouldSendReadRequestToRfidFilter();
   WhenAFreshFoodDoorIsOpened();
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_FreshFoodDoorOpen);
}

TEST(RfidCommunicationController, ShouldNotSendNewFilterInstallSignalWhenThereIsASuccessfulReadOnTheCurrentFilterWhileInInAllDoorsClosedReadState)
{
   GivenTheRfidCommunicationControllerIsInAllDoorsClosedReadState();
   NewFilterInstalledSignalShouldBe(0);

   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_Success, FaultRequestMockOverride_Disabled);
   NewFilterInstalledSignalShouldBe(0);
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
   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_Success, FaultRequestMockOverride_Disabled);

   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_AllDoorsClosedWrite);
}

TEST(RfidCommunicationController, ShouldNotWriteToABypassPlug)
{
   GivenTheRfidCommunicationControllerIsInAllDoorsClosedReadState();
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_AllDoorsClosedRead);

   ShouldSendReadRequestToRfidFilter();
   After(rfidFilterUpdateRateData->doorClosedFilterUpdateTimeInMinutes * MSEC_PER_MIN);

   WhenFilterIdentifierIsABypassPlug();

   ShouldNotWriteToABypassPlug();
   ShouldSendFaultRequestWithTagAuthenticationFailedFault(CLEAR, SignalCount_1);
   ShouldSendFaultRequestWithHardwareFailureFault(CLEAR, SignalCount_2);
   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_Success, FaultRequestMockOverride_Enabled);
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_AllDoorsClosedRead);
}

TEST(RfidCommunicationController, ShouldRestartReadWriteTimerWhenReadWriteTimerExpiresOnAFilterIsBypassPlug)
{
   GivenTheRfidCommunicationControllerIsInAllDoorsClosedReadState();
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_AllDoorsClosedRead);

   ShouldSendReadRequestToRfidFilter();
   After(rfidFilterUpdateRateData->doorClosedFilterUpdateTimeInMinutes * MSEC_PER_MIN);

   WhenFilterIdentifierIsABypassPlug();
   ShouldNotWriteToABypassPlug();
   ShouldSendFaultRequestWithTagAuthenticationFailedFault(CLEAR, SignalCount_1);
   ShouldSendFaultRequestWithHardwareFailureFault(CLEAR, SignalCount_2);
   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_Success, FaultRequestMockOverride_Enabled);

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
   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_Success, FaultRequestMockOverride_Disabled);
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_AllDoorsClosedRead);
}

TEST(RfidCommunicationController, ShouldSendReadRequestButNotTransitionToAllDoorsClosedWriteWhenDoorClosedErrorDetectedReadTimerExpiresAndTheLastRfidReadWasAReadFailure)
{
   GivenTheRfidCommunicationControllerIsInAllFreshFoodDoorsJustClosedState();
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_AllFreshFoodDoorsJustClosed);

   ShouldSendReadRequestToRfidFilter();
   After(rfidFilterUpdateRateData->doorJustClosedFilterReadFrequencyInSeconds * MSEC_PER_SEC);
   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_ReadFailure, FaultRequestMockOverride_Disabled);
   After(rfidFilterUpdateRateData->doorJustClosedFilterReadFrequencyInSeconds * MSEC_PER_SEC);
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_AllDoorsClosedRead);

   NothingShouldHappen();
   After((rfidFilterUpdateRateData->doorClosedRfidErrorDetectedReadFrequencySeconds * MSEC_PER_SEC) - 1);

   ShouldSendReadRequestToRfidFilter();
   After(1);

   NothingShouldHappen();
   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_Success, FaultRequestMockOverride_Disabled);
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_AllDoorsClosedRead);
}

TEST(RfidCommunicationController, ShouldSendReadRequestButNotTransitionToAllDoorsClosedWriteWhenDoorClosedErrorDetectedReadTimerExpiresAndTheLastRfidReadWasAReadBeforeWriteFailure)
{
   GivenTheRfidCommunicationControllerIsInAllFreshFoodDoorsJustClosedState();
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_AllFreshFoodDoorsJustClosed);

   ShouldSendReadRequestToRfidFilter();
   After(rfidFilterUpdateRateData->doorJustClosedFilterReadFrequencyInSeconds * MSEC_PER_SEC);
   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_ReadBeforeWriteFailure, FaultRequestMockOverride_Disabled);
   After(rfidFilterUpdateRateData->doorJustClosedFilterReadFrequencyInSeconds * MSEC_PER_SEC);
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_AllDoorsClosedRead);

   NothingShouldHappen();
   After((rfidFilterUpdateRateData->doorClosedRfidErrorDetectedReadFrequencySeconds * MSEC_PER_SEC) - 1);

   ShouldSendReadRequestToRfidFilter();
   After(1);

   NothingShouldHappen();
   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_Success, FaultRequestMockOverride_Disabled);
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_AllDoorsClosedRead);
}

TEST(RfidCommunicationController, ShouldSendReadRequestButNotTransitionToAllDoorsClosedWriteWhenDoorClosedErrorDetectedReadTimerExpiresAndTheLastRfidReadWasAReadAfterWriteFailure)
{
   GivenTheRfidCommunicationControllerIsInAllFreshFoodDoorsJustClosedState();
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_AllFreshFoodDoorsJustClosed);

   ShouldSendReadRequestToRfidFilter();
   After(rfidFilterUpdateRateData->doorJustClosedFilterReadFrequencyInSeconds * MSEC_PER_SEC);
   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_ReadAfterWriteFailure, FaultRequestMockOverride_Disabled);
   After(rfidFilterUpdateRateData->doorJustClosedFilterReadFrequencyInSeconds * MSEC_PER_SEC);
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_AllDoorsClosedRead);

   NothingShouldHappen();
   After((rfidFilterUpdateRateData->doorClosedRfidErrorDetectedReadFrequencySeconds * MSEC_PER_SEC) - 1);

   ShouldSendReadRequestToRfidFilter();
   After(1);

   NothingShouldHappen();
   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_Success, FaultRequestMockOverride_Disabled);
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_AllDoorsClosedRead);
}

TEST(RfidCommunicationController, ShouldSendReadRequestButNotTransitionToAllDoorsClosedWriteWhenDoorClosedErrorDetectedReadTimerExpiresAndTheLastRfidReadWasAHardwareFailure)
{
   GivenTheRfidCommunicationControllerIsInAllFreshFoodDoorsJustClosedState();
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_AllFreshFoodDoorsJustClosed);

   ShouldSendReadRequestToRfidFilter();
   After(rfidFilterUpdateRateData->doorJustClosedFilterReadFrequencyInSeconds * MSEC_PER_SEC);
   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_HardwareFailure, FaultRequestMockOverride_Disabled);
   After(rfidFilterUpdateRateData->doorJustClosedFilterReadFrequencyInSeconds * MSEC_PER_SEC);
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_AllDoorsClosedRead);

   NothingShouldHappen();
   After((rfidFilterUpdateRateData->doorClosedRfidErrorDetectedReadFrequencySeconds * MSEC_PER_SEC) - 1);

   ShouldSendReadRequestToRfidFilter();
   After(1);

   NothingShouldHappen();
   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_Success, FaultRequestMockOverride_Disabled);
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_AllDoorsClosedRead);
}

TEST(RfidCommunicationController, ShouldStopReadRequestTimerWhenExitingAllDoorsClosedReadState)
{
   WhenRfidFilterLeakDetectedIs(SET);
   GivenTheRfidCommunicationControllerIsInAllDoorsClosedReadState();

   ShouldSendReadRequestToRfidFilterMultipleTimes(181);
   After(rfidFilterUpdateRateData->doorClosedFilterUpdateTimeInMinutes * MSEC_PER_MIN);

   ShouldSendWriteRequestToRfidFilter();
   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_Success, FaultRequestMockOverride_Disabled);

   NothingShouldHappen();
   After(rfidFilterUpdateRateData->doorClosedRfidErrorDetectedReadFrequencySeconds * MSEC_PER_SEC);
}

TEST(RfidCommunicationController, ShouldStartErrorDetectedReadTimerWhenReceivingAReadFailureWhileInAllDoorsClosedReadState)
{
   GivenTheRfidCommunicationControllerIsInAllDoorsClosedReadState();

   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_ReadFailure, FaultRequestMockOverride_Disabled);

   NothingShouldHappen();
   After((rfidFilterUpdateRateData->doorClosedRfidErrorDetectedReadFrequencySeconds * MSEC_PER_SEC) - 1);

   ShouldSendReadRequestToRfidFilter();
   After(1);
}

TEST(RfidCommunicationController, ShouldStartErrorDetectedReadTimerWhenReceivingAReadBeforeWriteFailureWhileInAllDoorsClosedReadState)
{
   GivenTheRfidCommunicationControllerIsInAllDoorsClosedReadState();

   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_ReadBeforeWriteFailure, FaultRequestMockOverride_Disabled);

   NothingShouldHappen();
   After((rfidFilterUpdateRateData->doorClosedRfidErrorDetectedReadFrequencySeconds * MSEC_PER_SEC) - 1);

   ShouldSendReadRequestToRfidFilter();
   After(1);
}

TEST(RfidCommunicationController, ShouldStartErrorDetectedReadTimerWhenReceivingAReadAfterWriteFailureWhileInAllDoorsClosedReadState)
{
   GivenTheRfidCommunicationControllerIsInAllDoorsClosedReadState();

   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_ReadAfterWriteFailure, FaultRequestMockOverride_Disabled);

   NothingShouldHappen();
   After((rfidFilterUpdateRateData->doorClosedRfidErrorDetectedReadFrequencySeconds * MSEC_PER_SEC) - 1);

   ShouldSendReadRequestToRfidFilter();
   After(1);
}

TEST(RfidCommunicationController, ShouldStartErrorDetectedReadTimerWhenReceivingAHardwareFailureWhileInAllDoorsClosedReadState)
{
   GivenTheRfidCommunicationControllerIsInAllDoorsClosedReadState();

   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_HardwareFailure, FaultRequestMockOverride_Disabled);

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

TEST(RfidCommunicationController, ShouldTransitionToAllDoorsClosedReadStateWhenAnRfidFilterReadWriteRequestIsEepromWriteFailure)
{
   GivenTheRfidCommunicationControllerIsInAllDoorsClosedWriteState();

   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_EepromWriteFailure, FaultRequestMockOverride_Disabled);
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_AllDoorsClosedRead);
}

TEST(RfidCommunicationController, ShouldTransitionToAllDoorsClosedReadStateWhenAnRfidFilterReadWriteRequestIsUidMismatch)
{
   GivenTheRfidCommunicationControllerIsInAllDoorsClosedWriteState();

   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_UidMismatch, FaultRequestMockOverride_Disabled);
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_AllDoorsClosedRead);
}

TEST(RfidCommunicationController, ShouldTransitionToAllDoorsClosedReadStateWhenAnRfidFilterReadWriteRequestIsTagUidIsInvalid)
{
   GivenTheRfidCommunicationControllerIsInAllDoorsClosedWriteState();

   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_TagUidIsInvalid, FaultRequestMockOverride_Disabled);
   RfidCommunicationControllerStateShouldBe(RfidCommunicationControllerHsmState_AllDoorsClosedRead);
}

TEST(RfidCommunicationController, ShouldTransitionToReadWhenReceivingASuccessfulWriteMessage)
{
   GivenTheRfidCommunicationControllerIsInAllDoorsClosedWriteState();

   ShouldSendFaultRequestWithLeakDetectedFault(CLEAR, SignalCount_1);
   ShouldSendFaultRequestWithBlockedTagFault(CLEAR, SignalCount_2);
   WhenAnRfidMessageIsSentWithResult(ReadWriteResult_Success, FaultRequestMockOverride_Enabled);
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

TEST(RfidCommunicationController, ShouldIncrementNewFilterInstalledSignalWhenANewFilterIsInstalledWhileInFreshFoodDoorOpenState)
{
   GivenTheRfidCommunicationControllerIsInAFreshFoodDoorOpenState();
   NewFilterInstalledSignalShouldBe(0);

   WhenANewRfidFilterIsInstalled();
   NewFilterInstalledSignalShouldBe(1);
}

TEST(RfidCommunicationController, ShouldIncrementNewFilterInstalledSignalWhenANewFilterIsInstalledWhileInAllFreshFoodDoorsJustClosedState)
{
   GivenTheRfidCommunicationControllerIsInAllFreshFoodDoorsJustClosedState();
   NewFilterInstalledSignalShouldBe(0);

   WhenANewRfidFilterIsInstalled();
   NewFilterInstalledSignalShouldBe(1);
}

TEST(RfidCommunicationController, ShouldIncrementNewFilterInstalledSignalWhenANewFilterIsInstalledWhileInAllDoorsClosedReadState)
{
   GivenTheRfidCommunicationControllerIsInAllDoorsClosedReadState();
   NewFilterInstalledSignalShouldBe(0);

   WhenANewRfidFilterIsInstalled();
   NewFilterInstalledSignalShouldBe(1);
}

TEST(RfidCommunicationController, ShouldIgnoreInvalidReadWriteResult)
{
   GivenInitialization();

   NothingShouldHappen();
   WhenAnRfidMessageIsSentWithResult(SomeInvalidResponse, FaultRequestMockOverride_Disabled);
}
