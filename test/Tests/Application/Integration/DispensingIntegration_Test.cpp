/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "Application.h"
#include "SystemErds.h"
#include "Rx2xxResetSource.h"
#include "Constants_Binary.h"
}

#include "CppUTest/TestHarness.h"
#include "ReferDataModel_TestDouble.h"

TEST_GROUP(DispensingIntegration)
{
   Application_t instance;
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   ResetReason_t resetReason;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
   }

   void GivenApplicationHasBeenInitialized()
   {
      Application_Init(
         &instance,
         dataModel,
         resetReason);
   }

   void TheDispenseSelectionRequestShouldBe(DispenseSelection_t expected)
   {
      DispenseSelection_t actual;
      DataModel_Read(dataModel, Erd_DispenseSelectionRequest, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void GivenDispensingIsEnabled()
   {
      DataModel_Write(dataModel, Erd_DispensingDisabled, clear);
   }

   void WhenAllDoorsAreClosed()
   {
      DataModel_Write(dataModel, Erd_LeftSideFreshFoodDoorIsOpen, clear);
      DataModel_Write(dataModel, Erd_RightSideFreshFoodDoorStatus, clear);
      DataModel_Write(dataModel, Erd_DoorInDoorIsOpen, clear);
      DataModel_Write(dataModel, Erd_LeftSideFreezerDoorStatus, clear);
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentDoorIsOpen, clear);
   }

   void GivenAllDoorsAreClosed()
   {
      WhenAllDoorsAreClosed();
   }

   void GivenDispensingNotInhibitedByRfid()
   {
      DataModel_Write(dataModel, Erd_DispensingInhibitedByRfid, clear);
   }

   void WaterDispensingInhibitedByDoorShouldBe(bool expectedState)
   {
      bool actualState;
      DataModel_Read(
         dataModel,
         Erd_WaterDispensingInhibitedByDoor,
         &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void IceDispensingInhibitedByDoorShouldBe(bool expectedState)
   {
      bool actualState;
      DataModel_Read(
         dataModel,
         Erd_IceDispensingInhibitedByDoor,
         &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void WhenTheDispensingRequestIs(DispensingAction_t action, DispensingRequestSelection_t selection)
   {
      DispensingRequest_t dispensingRequest;
      DataModel_Read(
         dataModel,
         Erd_DispensingRequest,
         &dispensingRequest);

      dispensingRequest.action = action;
      dispensingRequest.selection = selection;
      DataModel_Write(
         dataModel,
         Erd_DispensingRequest,
         &dispensingRequest);
   }

   void ThePrivateDispensingRequestShouldBe(DispensingAction_t expectedAction, DispensingRequestSelection_t expectedSelection)
   {
      DispensingRequest_t actual;
      DataModel_Read(
         dataModel,
         Erd_PrivateDispensingRequest,
         &actual);

      CHECK_EQUAL(expectedAction, actual.action);
      CHECK_EQUAL(expectedSelection, actual.selection);
   }

   void TheDispensingRequestStatusShouldBe(DispenseStatus_t expectedStatus)
   {
      DispensingRequestStatus_t actual;
      DataModel_Read(
         dataModel,
         Erd_DispensingRequestStatus,
         &actual);

      CHECK_EQUAL(expectedStatus, actual.status);
   }

   void TheDispensingValveDispensingVoteShouldBe(WaterValveState_t state, Vote_t vote)
   {
      WaterValveVotedState_t actual;
      DataModel_Read(dataModel, Erd_DispenserWaterValve_DispensingVote, &actual);

      CHECK_EQUAL(state, actual.state);
      CHECK_EQUAL(vote, actual.care);
   }

   void TheIsolationValveDispensingVoteShouldBe(WaterValveState_t state, Vote_t vote)
   {
      WaterValveVotedState_t actual;
      DataModel_Read(dataModel, Erd_IsolationWaterValve_DispensingVote, &actual);

      CHECK_EQUAL(state, actual.state);
      CHECK_EQUAL(vote, actual.care);
   }

   void TheAugerMotorDispensingVoteShouldBe(AugerMotorIceType_t iceType, Vote_t vote)
   {
      AugerMotorVotedIceType_t actual;
      DataModel_Read(dataModel, Erd_AugerMotor_DispensingVote, &actual);

      CHECK_EQUAL(iceType, actual.iceType);
      CHECK_EQUAL(vote, actual.care);
   }

   void WhenDispensingIsInhibitedByRfid()
   {
      DataModel_Write(dataModel, Erd_DispensingInhibitedByRfid, set);
   }

   void WhenWaterDispensingIsInhibitedByAnOpenDoor()
   {
      DataModel_Write(dataModel, Erd_LeftSideFreshFoodDoorIsOpen, set);
   }

   void WhenIceDispensingIsInhibitedByDoor()
   {
      DataModel_Write(dataModel, Erd_IceDispensingInhibitedByDoor, set);
   }

   void TheDispenseStatusShouldBe(DispenseStatus_t expected)
   {
      DispenseStatus_t actual;
      DataModel_Read(dataModel, Erd_PrivateDispensingResultStatus, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void AfterFirstDispenseIsCompletedSuccessfully()
   {
      GivenDispensingIsEnabled();
      GivenAllDoorsAreClosed();
      GivenDispensingNotInhibitedByRfid();

      WaterDispensingInhibitedByDoorShouldBe(false);
      IceDispensingInhibitedByDoorShouldBe(false);

      WhenTheDispensingRequestIs(DispensingAction_Start, DispensingRequestSelection_CrushedIce);
      ThePrivateDispensingRequestShouldBe(DispensingAction_Start, DispensingRequestSelection_CrushedIce);
      TheDispensingRequestStatusShouldBe(DispenseStatus_Dispensing);

      TheAugerMotorDispensingVoteShouldBe(AugerMotorIceType_Crushed, Vote_Care);
      TheDispensingValveDispensingVoteShouldBe(WaterValveState_Off, Vote_DontCare);
      TheIsolationValveDispensingVoteShouldBe(WaterValveState_Off, Vote_DontCare);

      WhenTheDispensingRequestIs(DispensingAction_Stop, DispensingRequestSelection_CrushedIce);
      ThePrivateDispensingRequestShouldBe(DispensingAction_Stop, DispensingRequestSelection_CrushedIce);
      TheDispensingRequestStatusShouldBe(DispenseStatus_CompletedSuccessfully);
   }

   void GivenApplicationHasBeenInitializedWithDispensingNotInhibited()
   {
      GivenDispensingIsEnabled();
      GivenAllDoorsAreClosed();
      GivenDispensingNotInhibitedByRfid();
      GivenApplicationHasBeenInitialized();

      WaterDispensingInhibitedByDoorShouldBe(false);
      IceDispensingInhibitedByDoorShouldBe(false);
   }

   void AfterAWaterDispenseHasStarted()
   {
      WhenTheDispensingRequestIs(DispensingAction_Start, DispensingRequestSelection_Water);
      ThePrivateDispensingRequestShouldBe(DispensingAction_Start, DispensingRequestSelection_Water);
      TheDispensingRequestStatusShouldBe(DispenseStatus_Dispensing);

      TheDispensingValveDispensingVoteShouldBe(WaterValveState_On, Vote_Care);
      TheIsolationValveDispensingVoteShouldBe(WaterValveState_On, Vote_Care);
      TheAugerMotorDispensingVoteShouldBe(AugerMotorIceType_Off, Vote_DontCare);
   }
};

TEST(DispensingIntegration, ShouldInitializeAndSetDispenseSelectionRequestToReset)
{
   GivenApplicationHasBeenInitialized();
   TheDispenseSelectionRequestShouldBe(DispenseSelection_Reset);
}

TEST(DispensingIntegration, ShouldVoteIsolationValveOnAndDispensingValveOnUponEnteringDispensingStateIfRequestIsWaterAndNotInhibitedByDoor)
{
   GivenDispensingIsEnabled();
   GivenAllDoorsAreClosed();
   GivenDispensingNotInhibitedByRfid();
   GivenApplicationHasBeenInitialized();

   WaterDispensingInhibitedByDoorShouldBe(false);
   IceDispensingInhibitedByDoorShouldBe(false);

   WhenTheDispensingRequestIs(DispensingAction_Start, DispensingRequestSelection_Water);
   ThePrivateDispensingRequestShouldBe(DispensingAction_Start, DispensingRequestSelection_Water);
   TheDispensingRequestStatusShouldBe(DispenseStatus_Dispensing);

   TheDispensingValveDispensingVoteShouldBe(WaterValveState_On, Vote_Care);
   TheIsolationValveDispensingVoteShouldBe(WaterValveState_On, Vote_Care);
   TheAugerMotorDispensingVoteShouldBe(AugerMotorIceType_Off, Vote_DontCare);
}

TEST(DispensingIntegration, ShouldVoteIsolationValveOnAndDispensingValveOnUponEnteringDispensingStateIfRequestIsAutofillAndNotInhibitedByDoor)
{
   GivenApplicationHasBeenInitializedWithDispensingNotInhibited();

   WhenTheDispensingRequestIs(DispensingAction_Start, DispensingRequestSelection_Autofill);
   ThePrivateDispensingRequestShouldBe(DispensingAction_Start, DispensingRequestSelection_Autofill);
   TheDispensingRequestStatusShouldBe(DispenseStatus_Dispensing);

   TheDispensingValveDispensingVoteShouldBe(WaterValveState_On, Vote_Care);
   TheIsolationValveDispensingVoteShouldBe(WaterValveState_On, Vote_Care);
   TheAugerMotorDispensingVoteShouldBe(AugerMotorIceType_Off, Vote_DontCare);
}

TEST(DispensingIntegration, ShouldVoteAugerMotorCubedUponEnteringDispensingStateIfRequestIsCubedIceAndNotInhibitedByDoor)
{
   GivenApplicationHasBeenInitializedWithDispensingNotInhibited();

   WhenTheDispensingRequestIs(DispensingAction_Start, DispensingRequestSelection_CubedIce);
   ThePrivateDispensingRequestShouldBe(DispensingAction_Start, DispensingRequestSelection_CubedIce);
   TheDispensingRequestStatusShouldBe(DispenseStatus_Dispensing);

   TheAugerMotorDispensingVoteShouldBe(AugerMotorIceType_Cubed, Vote_Care);
   TheDispensingValveDispensingVoteShouldBe(WaterValveState_Off, Vote_DontCare);
   TheIsolationValveDispensingVoteShouldBe(WaterValveState_Off, Vote_DontCare);
}

TEST(DispensingIntegration, ShouldVoteAugerMotorCrushedUponEnteringDispensingStateIfRequestIsCrushedIceAndNotInhibitedByDoor)
{
   GivenApplicationHasBeenInitializedWithDispensingNotInhibited();

   WhenTheDispensingRequestIs(DispensingAction_Start, DispensingRequestSelection_CrushedIce);
   ThePrivateDispensingRequestShouldBe(DispensingAction_Start, DispensingRequestSelection_CrushedIce);
   TheDispensingRequestStatusShouldBe(DispenseStatus_Dispensing);

   TheAugerMotorDispensingVoteShouldBe(AugerMotorIceType_Crushed, Vote_Care);
   TheDispensingValveDispensingVoteShouldBe(WaterValveState_Off, Vote_DontCare);
   TheIsolationValveDispensingVoteShouldBe(WaterValveState_Off, Vote_DontCare);
}

TEST(DispensingIntegration, ShouldNotDispenseWaterWhenWaterDispensingIsInhibitedByAnOpenDoor)
{
   GivenApplicationHasBeenInitialized();
   AfterFirstDispenseIsCompletedSuccessfully();

   WhenWaterDispensingIsInhibitedByAnOpenDoor();
   WaterDispensingInhibitedByDoorShouldBe(true);
   IceDispensingInhibitedByDoorShouldBe(false);

   WhenTheDispensingRequestIs(DispensingAction_Start, DispensingRequestSelection_Water);
   ThePrivateDispensingRequestShouldBe(DispensingAction_Stop, DispensingRequestSelection_Water);
   TheDispensingRequestStatusShouldBe(DispenseStatus_DispenseInhibitedDueToDoorOpen);

   TheDispensingValveDispensingVoteShouldBe(WaterValveState_Off, Vote_DontCare);
   TheIsolationValveDispensingVoteShouldBe(WaterValveState_Off, Vote_DontCare);
   TheAugerMotorDispensingVoteShouldBe(AugerMotorIceType_Off, Vote_DontCare);
}

TEST(DispensingIntegration, ShouldSetDispenceStatusToDispensingIsInhibitedByAnOpenDoorWhenDispensingIsInhibitedByAnOpenDoorAndByRfid)
{
   GivenApplicationHasBeenInitialized();
   AfterFirstDispenseIsCompletedSuccessfully();

   WhenWaterDispensingIsInhibitedByAnOpenDoor();
   WaterDispensingInhibitedByDoorShouldBe(true);
   IceDispensingInhibitedByDoorShouldBe(false);
   WhenDispensingIsInhibitedByRfid();

   WhenTheDispensingRequestIs(DispensingAction_Start, DispensingRequestSelection_Water);
   ThePrivateDispensingRequestShouldBe(DispensingAction_Stop, DispensingRequestSelection_Water);
   TheDispensingRequestStatusShouldBe(DispenseStatus_DispenseInhibitedDueToDoorOpen);

   TheDispensingValveDispensingVoteShouldBe(WaterValveState_Off, Vote_DontCare);
   TheIsolationValveDispensingVoteShouldBe(WaterValveState_Off, Vote_DontCare);
   TheAugerMotorDispensingVoteShouldBe(AugerMotorIceType_Off, Vote_DontCare);
}

TEST(DispensingIntegration, ShouldSetStatusToInhibitedDueToDoorOpenWhenInDispensingStateAndWaterDispensingIsInhibited)
{
   GivenApplicationHasBeenInitializedWithDispensingNotInhibited();

   WhenTheDispensingRequestIs(DispensingAction_Start, DispensingRequestSelection_Water);
   ThePrivateDispensingRequestShouldBe(DispensingAction_Start, DispensingRequestSelection_Water);
   TheDispensingRequestStatusShouldBe(DispenseStatus_Dispensing);

   TheDispensingValveDispensingVoteShouldBe(WaterValveState_On, Vote_Care);
   TheIsolationValveDispensingVoteShouldBe(WaterValveState_On, Vote_Care);
   TheAugerMotorDispensingVoteShouldBe(AugerMotorIceType_Off, Vote_DontCare);

   WhenWaterDispensingIsInhibitedByAnOpenDoor();
   WaterDispensingInhibitedByDoorShouldBe(true);
   TheDispenseStatusShouldBe(DispenseStatus_DispenseInhibitedDueToDoorOpen);

   TheDispensingValveDispensingVoteShouldBe(WaterValveState_Off, Vote_DontCare);
   TheIsolationValveDispensingVoteShouldBe(WaterValveState_Off, Vote_DontCare);
   TheAugerMotorDispensingVoteShouldBe(AugerMotorIceType_Off, Vote_DontCare);
}

TEST(DispensingIntegration, ShouldOnlyContinueDispensingOnceDoorInhibitingDispenseClosesAndANewStartIsRequested)
{
   GivenApplicationHasBeenInitializedWithDispensingNotInhibited();

   AfterAWaterDispenseHasStarted();
   WhenWaterDispensingIsInhibitedByAnOpenDoor();
   WaterDispensingInhibitedByDoorShouldBe(true);
   TheDispenseStatusShouldBe(DispenseStatus_DispenseInhibitedDueToDoorOpen);

   TheDispensingValveDispensingVoteShouldBe(WaterValveState_Off, Vote_DontCare);
   TheIsolationValveDispensingVoteShouldBe(WaterValveState_Off, Vote_DontCare);
   TheAugerMotorDispensingVoteShouldBe(AugerMotorIceType_Off, Vote_DontCare);

   WhenAllDoorsAreClosed();
   TheDispensingRequestStatusShouldBe(DispenseStatus_DispenseInhibitedDueToDoorOpen);

   WhenTheDispensingRequestIs(DispensingAction_Start, DispensingRequestSelection_Water);
   ThePrivateDispensingRequestShouldBe(DispensingAction_Start, DispensingRequestSelection_Water);
   TheDispensingRequestStatusShouldBe(DispenseStatus_Dispensing);

   TheDispensingValveDispensingVoteShouldBe(WaterValveState_On, Vote_Care);
   TheIsolationValveDispensingVoteShouldBe(WaterValveState_On, Vote_Care);
   TheAugerMotorDispensingVoteShouldBe(AugerMotorIceType_Off, Vote_DontCare);
}

TEST(DispensingIntegration, ShouldDispenseIceWhenIceIsRequestedAndWaterDispensingIsInhibitedByDoor)
{
   GivenApplicationHasBeenInitialized();
   GivenDispensingIsEnabled();
   WhenWaterDispensingIsInhibitedByAnOpenDoor();
   GivenDispensingNotInhibitedByRfid();

   WaterDispensingInhibitedByDoorShouldBe(true);
   IceDispensingInhibitedByDoorShouldBe(false);

   WhenTheDispensingRequestIs(DispensingAction_Start, DispensingRequestSelection_CubedIce);
   ThePrivateDispensingRequestShouldBe(DispensingAction_Start, DispensingRequestSelection_CubedIce);
   TheDispensingRequestStatusShouldBe(DispenseStatus_Dispensing);

   TheAugerMotorDispensingVoteShouldBe(AugerMotorIceType_Cubed, Vote_Care);
   TheDispensingValveDispensingVoteShouldBe(WaterValveState_Off, Vote_DontCare);
   TheIsolationValveDispensingVoteShouldBe(WaterValveState_Off, Vote_DontCare);
}

TEST(DispensingIntegration, ShouldAllowWaterDispenseWhenOnlyIceDispensingIsInhibitedByDoor)
{
   GivenApplicationHasBeenInitializedWithDispensingNotInhibited();

   WhenTheDispensingRequestIs(DispensingAction_Start, DispensingRequestSelection_Water);
   ThePrivateDispensingRequestShouldBe(DispensingAction_Start, DispensingRequestSelection_Water);
   TheDispensingRequestStatusShouldBe(DispenseStatus_Dispensing);

   WhenIceDispensingIsInhibitedByDoor();
   IceDispensingInhibitedByDoorShouldBe(true);
   TheDispensingRequestStatusShouldBe(DispenseStatus_Dispensing);

   TheDispensingValveDispensingVoteShouldBe(WaterValveState_On, Vote_Care);
   TheIsolationValveDispensingVoteShouldBe(WaterValveState_On, Vote_Care);
   TheAugerMotorDispensingVoteShouldBe(AugerMotorIceType_Off, Vote_DontCare);
}

TEST(DispensingIntegration, ShouldSetStatusToInhibitedDueToDoorOpenWhenInDispensingStateAndIceDispensingIsInhibited)
{
   GivenApplicationHasBeenInitializedWithDispensingNotInhibited();

   WhenTheDispensingRequestIs(DispensingAction_Start, DispensingRequestSelection_CubedIce);
   ThePrivateDispensingRequestShouldBe(DispensingAction_Start, DispensingRequestSelection_CubedIce);
   TheDispensingRequestStatusShouldBe(DispenseStatus_Dispensing);

   TheAugerMotorDispensingVoteShouldBe(AugerMotorIceType_Cubed, Vote_Care);
   TheDispensingValveDispensingVoteShouldBe(WaterValveState_Off, Vote_DontCare);
   TheIsolationValveDispensingVoteShouldBe(WaterValveState_Off, Vote_DontCare);

   WhenIceDispensingIsInhibitedByDoor();
   IceDispensingInhibitedByDoorShouldBe(true);
   TheDispenseStatusShouldBe(DispenseStatus_DispenseInhibitedDueToDoorOpen);

   TheAugerMotorDispensingVoteShouldBe(AugerMotorIceType_Off, Vote_DontCare);
   TheDispensingValveDispensingVoteShouldBe(WaterValveState_Off, Vote_DontCare);
   TheIsolationValveDispensingVoteShouldBe(WaterValveState_Off, Vote_DontCare);
}
