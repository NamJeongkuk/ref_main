/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "DispensingRequestHandler.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "uassert_test.h"
#include "ReferDataModel_TestDouble.h"

static const DispensingRequestHandlerConfig_t config = {
   .dispensingRequestErd = Erd_DispensingRequest,
   .dispensingRequestStatusErd = Erd_DispensingRequestStatus,
   .privateDispensingRequestErd = Erd_PrivateDispensingRequest,
   .privateDispensingRequestStatusErd = Erd_PrivateDispensingRequestStatus
};

TEST_GROUP(DispensingRequestHandler)
{
   DispensingRequestHandler_t instance;
   ReferDataModel_TestDouble_t referDataModelTestDouble;
   TimerModule_TestDouble_t *timerModuleTestDouble;

   I_DataModel_t *dataModel;
   EventSubscription_t dataModelOnChangeSubscription;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&referDataModelTestDouble);
      dataModel = referDataModelTestDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&referDataModelTestDouble);
   }

   void GivenTheModuleIsInitialized(void)
   {
      DispensingRequestHandler_Init(&instance, dataModel, &config);
   }

   void After(TimerTicks_t ticks, TimeSourceTickCount_t ticksToElapseAtATime = 1000)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks, ticksToElapseAtATime);
   }

   void WhenTheDispensingRequestActionIs(uint8_t action)
   {
      DispensingRequest_t dispensingRequest;
      DataModel_Read(
         dataModel,
         Erd_DispensingRequest,
         &dispensingRequest);

      dispensingRequest.action = action;
      DataModel_Write(
         dataModel,
         Erd_DispensingRequest,
         &dispensingRequest);
   }

   void GivenTheDispensingRequestIsInRequestingDispense(void)
   {
      GivenTheModuleIsInitialized();
      WhenTheDispensingRequestActionIs(DispensingAction_Start);
   }

   void WhenTheDispensingRequestSelectionIs(uint8_t selection)
   {
      DispensingRequest_t dispensingRequest;
      DataModel_Read(
         dataModel,
         Erd_DispensingRequest,
         &dispensingRequest);

      dispensingRequest.selection = selection;
      DataModel_Write(
         dataModel,
         Erd_DispensingRequest,
         &dispensingRequest);
   }

   void WhenThePrivateDispensingRequestStatusActionIs(uint8_t action)
   {
      DispensingRequestStatus_t privateDispensingRequestStatus;
      DataModel_Read(
         dataModel,
         Erd_PrivateDispensingRequestStatus,
         &privateDispensingRequestStatus);

      privateDispensingRequestStatus.action = action;
      DataModel_Write(
         dataModel,
         Erd_PrivateDispensingRequestStatus,
         &privateDispensingRequestStatus);
   }

   void WhenThePrivateDispensingRequestStatusSelectionIs(uint8_t selection)
   {
      DispensingRequestStatus_t privateDispensingRequestStatus;
      DataModel_Read(
         dataModel,
         Erd_PrivateDispensingRequestStatus,
         &privateDispensingRequestStatus);

      privateDispensingRequestStatus.selection = selection;
      DataModel_Write(
         dataModel,
         Erd_PrivateDispensingRequestStatus,
         &privateDispensingRequestStatus);
   }

   void WhenThePrivateDispensingRequestStatusSpecialOptionsIs(uint8_t option)
   {
      DispensingRequestStatus_t privateDispensingRequestStatus;
      DataModel_Read(
         dataModel,
         Erd_PrivateDispensingRequestStatus,
         &privateDispensingRequestStatus);

      privateDispensingRequestStatus.specialOptions = option;
      DataModel_Write(
         dataModel,
         Erd_PrivateDispensingRequestStatus,
         &privateDispensingRequestStatus);
   }

   void WhenThePrivateDispensingRequestStatusIs(uint8_t status)
   {
      DispensingRequestStatus_t dispensingRequestStatus;
      DataModel_Read(
         dataModel,
         Erd_PrivateDispensingRequestStatus,
         &dispensingRequestStatus);

      dispensingRequestStatus.status = status;
      DataModel_Write(
         dataModel,
         Erd_PrivateDispensingRequestStatus,
         &dispensingRequestStatus);
   }

   void WhenThePrivateDispensingRequestStatusPreciseFillOuncesx100Is(uint16_t ouncesx100)
   {
      DispensingRequestStatus_t priVateDispensingRequestStatus;
      DataModel_Read(
         dataModel,
         Erd_PrivateDispensingRequestStatus,
         &priVateDispensingRequestStatus);

      priVateDispensingRequestStatus.preciseFillOuncesx100 = ouncesx100;
      DataModel_Write(
         dataModel,
         Erd_PrivateDispensingRequestStatus,
         &priVateDispensingRequestStatus);
   }

   void GivenThePrivateDispensingRequestActionIs(uint8_t action)
   {
      DispensingRequest_t privateDispensingRequest;
      DataModel_Read(
         dataModel,
         Erd_PrivateDispensingRequest,
         &privateDispensingRequest);

      privateDispensingRequest.action = action;
      DataModel_Write(
         dataModel,
         Erd_PrivateDispensingRequest,
         &privateDispensingRequest);
   }

   void GivenTheDispensingRequestStatusIs(uint8_t status)
   {
      DispensingRequestStatus_t dispensingRequestStatus;
      DataModel_Read(
         dataModel,
         Erd_DispensingRequestStatus,
         &dispensingRequestStatus);

      dispensingRequestStatus.status = status;
      DataModel_Write(
         dataModel,
         Erd_DispensingRequestStatus,
         &dispensingRequestStatus);
   }

   void ThePrivateDispensingRequestActionShouldBe(uint8_t action)
   {
      DispensingRequest_t privateDispensingRequest;
      DataModel_Read(
         dataModel,
         Erd_PrivateDispensingRequest,
         &privateDispensingRequest);

      CHECK_EQUAL(privateDispensingRequest.action, action);
   }

   void TheDispensingRequestActionShouldBe(uint8_t action)
   {
      DispensingRequest_t dispensingRequest;
      DataModel_Read(
         dataModel,
         Erd_DispensingRequest,
         &dispensingRequest);

      CHECK_EQUAL(dispensingRequest.action, action);
   }

   void TheDispensingRequestStatusShouldBe(uint8_t status)
   {
      DispensingRequestStatus_t dispensingRequestStatus;
      DataModel_Read(
         dataModel,
         Erd_DispensingRequestStatus,
         &dispensingRequestStatus);

      CHECK_EQUAL(dispensingRequestStatus.status, status);
   }

   void TheDispensingRequestShouldBeClear(void)
   {
      DispensingRequest_t expectedDispensingRequest = {
         .action = DispensingAction_None,
         .selection = DispensingRequestSelection_None,
         .specialOptions = DispensingSpecialOptions_EmptyRequest,
         .padding = UINT8_MAX,
         .preciseFillOuncesx100 = UINT16_MAX
      };

      DispensingRequest_t dispensingRequest;
      DataModel_Read(
         dataModel,
         Erd_DispensingRequest,
         &dispensingRequest);

      CHECK_EQUAL(0, memcmp(&expectedDispensingRequest, &dispensingRequest, sizeof(DispensingRequest_t)));
   }

   void ThePrivateAndPublicDispensingRequestStatusShouldBeTheSameValue(void)
   {
      DispensingRequestStatus_t privateDispensingRequestStatus;
      DispensingRequestStatus_t publicDispensingRequestStatus;

      DataModel_Read(
         dataModel,
         Erd_PrivateDispensingRequestStatus,
         &privateDispensingRequestStatus);

      DataModel_Read(
         dataModel,
         Erd_DispensingRequestStatus,
         &publicDispensingRequestStatus);

      CHECK_EQUAL(0, memcmp(&privateDispensingRequestStatus, &publicDispensingRequestStatus, sizeof(DispensingRequestStatus_t)));
   }
};

TEST(DispensingRequestHandler, ShouldClearDispensingRequestOnInit)
{
   GivenTheModuleIsInitialized();
   TheDispensingRequestShouldBeClear();
}

TEST(DispensingRequestHandler, ShouldSetDispensingRequestStatusToBeCompletedSuccessfullyWhenTheDispensingRequestActionIsStopInIdle)
{
   GivenTheModuleIsInitialized();

   WhenTheDispensingRequestActionIs(DispensingAction_Stop);
   TheDispensingRequestStatusShouldBe(DispenseStatus_CompletedSuccessfully);
}

TEST(DispensingRequestHandler, ShouldClearDispensingRequestWhenTheDispensingRequestActionIsStopInIdle)
{
   GivenTheModuleIsInitialized();

   WhenTheDispensingRequestActionIs(DispensingAction_Stop);
   TheDispensingRequestShouldBeClear();
}

TEST(DispensingRequestHandler, ShouldUpdateDispensingRequestStatusActionToStartWhenTheDispensingRequestActionIsStartInIdle)
{
   GivenTheModuleIsInitialized();

   WhenTheDispensingRequestActionIs(DispensingAction_Start);
   ThePrivateDispensingRequestActionShouldBe(DispensingAction_Start);
}

TEST(DispensingRequestHandler, ShouldUpdateDispensingRequestStatusToBadCommandWhenTheDispensingRequestActionIsInvalidInIdle)
{
   GivenTheModuleIsInitialized();

   WhenTheDispensingRequestActionIs(DispensingAction_Continue + 1);
   TheDispensingRequestStatusShouldBe(DispenseStatus_BadCommand);
}

TEST(DispensingRequestHandler, ShouldClearDispensingRequestWhenTheDispensingRequestActionIsInvalidInIdle)
{
   GivenTheModuleIsInitialized();

   WhenTheDispensingRequestActionIs(DispensingAction_Continue + 1);
   TheDispensingRequestShouldBeClear();
}

TEST(DispensingRequestHandler, ShouldUpdateDispensingRequestStatusToBadCommandWhenTheDispensingRequestSelectionIsInvalidInIdle)
{
   GivenTheModuleIsInitialized();

   WhenTheDispensingRequestSelectionIs(DispensingRequestSelection_Autofill + 1);
   TheDispensingRequestStatusShouldBe(DispenseStatus_BadCommand);
}

TEST(DispensingRequestHandler, ShouldClearDispensingRequestWhenTheDispensingRequestSelectionIsInvalidInIdle)
{
   GivenTheModuleIsInitialized();

   WhenTheDispensingRequestSelectionIs(DispensingRequestSelection_Autofill + 1);
   TheDispensingRequestShouldBeClear();
}

TEST(DispensingRequestHandler, ShouldClearDispensingRequestWhenTheDispensingRequestActionIsContinueInRequestingDispense)
{
   GivenTheDispensingRequestIsInRequestingDispense();

   WhenTheDispensingRequestActionIs(DispensingAction_Continue);
   TheDispensingRequestShouldBeClear();
}

TEST(DispensingRequestHandler, ShouldUpdateDispensingRequestStatusToDidNotReceiveDispenseRequestAfterHeartbeatWhenTheDispensingContinueRequestIsNotReceivedWithinTimeout)
{
   GivenTheDispensingRequestIsInRequestingDispense();
   GivenTheDispensingRequestStatusIs(DispenseStatus_Dispensing);
   WhenTheDispensingRequestActionIs(DispensingAction_Continue);

   After(DispensingRequestContinueWaitMaxTimeInMsec - 1);
   TheDispensingRequestStatusShouldBe(DispenseStatus_Dispensing);

   After(1);
   TheDispensingRequestStatusShouldBe(DispenseStatus_DidNotReceiveDispenseRequestAfterHeartbeat);
}

TEST(DispensingRequestHandler, ShouldClearDispensingRequestWhenTheDispensingRequestActionIsStopInRequestingDispense)
{
   GivenTheDispensingRequestIsInRequestingDispense();

   WhenTheDispensingRequestActionIs(DispensingAction_Stop);
   TheDispensingRequestShouldBeClear();
}

TEST(DispensingRequestHandler, ShouldUpdateDispensingRequestStatusToCompletedSuccessfullyWhenTheDispensingRequestActionIsStopInRequestingDispense)
{
   GivenTheDispensingRequestIsInRequestingDispense();

   WhenTheDispensingRequestActionIs(DispensingAction_Stop);
   TheDispensingRequestStatusShouldBe(DispenseStatus_CompletedSuccessfully);
}

TEST(DispensingRequestHandler, ShouldUpdatePrivateDispensingRequestActionToStopWhenTheDispensingRequestActionIsStopInRequestingDispense)
{
   GivenTheDispensingRequestIsInRequestingDispense();
   ThePrivateDispensingRequestActionShouldBe(DispensingAction_Start);

   WhenTheDispensingRequestActionIs(DispensingAction_Stop);
   ThePrivateDispensingRequestActionShouldBe(DispensingAction_Stop);
}

TEST(DispensingRequestHandler, ShouldUpdateDispensingRequestStatusToDidNotReceiveDispenseRequestAfterHeartbeatWhenDispensingContinueRequestIsNotReceivedWithinTimeoutInRequestingDispense)
{
   GivenTheDispensingRequestIsInRequestingDispense();
   GivenTheDispensingRequestStatusIs(DispenseStatus_Dispensing);

   After(DispensingRequestContinueWaitMaxTimeInMsec - 1);
   TheDispensingRequestStatusShouldBe(DispenseStatus_Dispensing);

   After(1);
   TheDispensingRequestStatusShouldBe(DispenseStatus_DidNotReceiveDispenseRequestAfterHeartbeat);
}

TEST(DispensingRequestHandler, ShouldUpdatePrivateDispensingRequestActionToStopWhenDispensingContinueRequestIsNotReceivedWithinTimeoutInRequestingDispense)
{
   GivenTheDispensingRequestIsInRequestingDispense();
   GivenThePrivateDispensingRequestActionIs(DispensingAction_Start);

   After(DispensingRequestContinueWaitMaxTimeInMsec - 1);
   ThePrivateDispensingRequestActionShouldBe(DispensingAction_Start);
   After(1);
   ThePrivateDispensingRequestActionShouldBe(DispensingAction_Stop);
}

TEST(DispensingRequestHandler, ShouldUpdateThePublicDispensingRequestStatustToThePriviteStatusWhenThePrivateStatusActionChangesToStartInRequestingDispense)
{
   GivenTheDispensingRequestIsInRequestingDispense();

   WhenThePrivateDispensingRequestStatusActionIs(DispensingAction_Start);
   ThePrivateAndPublicDispensingRequestStatusShouldBeTheSameValue();
}

TEST(DispensingRequestHandler, ShouldUpdateThePublicDispensingRequestStatusToThePrivateStatusWhenThePrivateDispensingRequestStatusSelectionIsChangesToCrushedIceInRequestingDispense)
{
   GivenTheDispensingRequestIsInRequestingDispense();

   WhenThePrivateDispensingRequestStatusSpecialOptionsIs(DispensingSpecialOptions_SwitchPressedOnSecondaryBoardRequired);
   ThePrivateAndPublicDispensingRequestStatusShouldBeTheSameValue();
}

TEST(DispensingRequestHandler, ShouldUpdateThePublicDispensingRequestStatusToThePrivateStatusWhenThePrivateDispensingRequestStatusOptionsChangesToSwitchPressedOnSecondaryBoardRequiredInRequestingDispense)
{
   GivenTheDispensingRequestIsInRequestingDispense();

   WhenThePrivateDispensingRequestStatusIs(DispenseStatus_HitMaxVolume);
   ThePrivateAndPublicDispensingRequestStatusShouldBeTheSameValue();
}

TEST(DispensingRequestHandler, ShouldUpdateThePublicDispensingRequestStatusToThePrivateStatusWhenThePrivateDispensingRequestStatusChangesToHitMaxVolumeInRequestingDispense)
{
   GivenTheDispensingRequestIsInRequestingDispense();

   WhenThePrivateDispensingRequestStatusIs(DispenseStatus_HitMaxVolume);
   ThePrivateAndPublicDispensingRequestStatusShouldBeTheSameValue();
}

TEST(DispensingRequestHandler, ShouldUpdateThePublicDispensingStatusToThePrivateStatusWhenThePrivateDispensingRequestStatusPreciseFillIsChangeInRequestingDispense)
{
   GivenTheDispensingRequestIsInRequestingDispense();

   WhenThePrivateDispensingRequestStatusPreciseFillOuncesx100Is(100);
   ThePrivateAndPublicDispensingRequestStatusShouldBeTheSameValue();
}

TEST(DispensingRequestHandler, ShouldUpdateThePrivateDispensingRequestActionToStopWhenThePrivateDispensingRequestStatusActionIChangesToStartInRequestingDispense)
{
   GivenTheDispensingRequestIsInRequestingDispense();
   GivenThePrivateDispensingRequestActionIs(DispensingAction_Start);

   WhenThePrivateDispensingRequestStatusActionIs(DispensingAction_Start);
   ThePrivateDispensingRequestActionShouldBe(DispensingAction_Stop);
}

TEST(DispensingRequestHandler, ShouldUpdateThePublicDispensingRequestStatusToBadCommandWhenTheDispensingRequestActionIsInvalidInRequestingDispense)
{
   GivenTheDispensingRequestIsInRequestingDispense();

   WhenTheDispensingRequestActionIs(DispensingAction_Continue + 1);
   TheDispensingRequestStatusShouldBe(DispenseStatus_BadCommand);
}

TEST(DispensingRequestHandler, ShouldClearDispensingRequestWhenTheDispensingRequestActionIsInvalidInRequestingDispense)
{
   GivenTheDispensingRequestIsInRequestingDispense();

   WhenTheDispensingRequestActionIs(DispensingAction_Continue + 1);
   TheDispensingRequestShouldBeClear();
}

TEST(DispensingRequestHandler, ShouldUpdateThePublicDispensingRequestStatusToBadCommandWhenTheDispensingRequestSelectionIsInvalidInRequestingDispense)
{
   GivenTheDispensingRequestIsInRequestingDispense();

   WhenTheDispensingRequestSelectionIs(DispensingRequestSelection_Autofill + 1);
   TheDispensingRequestStatusShouldBe(DispenseStatus_BadCommand);
}

TEST(DispensingRequestHandler, ShouldClearDispensingRequestWhenTheDispensingRequestSelectionIsInvalidInRequestingDispense)
{
   GivenTheDispensingRequestIsInRequestingDispense();

   WhenTheDispensingRequestSelectionIs(DispensingRequestSelection_Autofill + 1);
   TheDispensingRequestShouldBeClear();
}
