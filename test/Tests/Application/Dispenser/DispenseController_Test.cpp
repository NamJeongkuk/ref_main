/*!
 * @file
 * @brief Dispense Controller tests
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "DispenseController.h"
#include "DispensingRequest.h"
#include "DispenseStatus.h"
#include "PersonalityParametricData.h"
#include "TddPersonality.h"
#include "Constants_Time.h"
#include "Constants_Binary.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "SystemErds.h"

static const DispenseControllerConfig_t config = {
   .privateDispensingRequestErd = Erd_PrivateDispensingRequest,
   .privateDispensingResultStatusErd = Erd_PrivateDispensingResultStatus,
   .dispensingInhibitedByRfidErd = Erd_DispensingInhibitedByRfid,
   .waterDispensingInhibitedByDoorErd = Erd_WaterDispensingInhibitedByDoor,
   .iceDispensingInhibitedByDoorErd = Erd_IceDispensingInhibitedByDoor,
   .autofillSensorErrorErd = Erd_AutofillSensorError,
   .dispensingDisabledErd = Erd_DispensingDisabled,
   .augerMotorDispensingVoteErd = Erd_AugerMotor_DispensingVote,
   .isolationValveDispensingVoteErd = Erd_IsolationWaterValve_DispensingVote,
   .dispensingValveDispensingVoteErd = Erd_DispenserWaterValve_DispensingVote,
   .timerModuleErd = Erd_TimerModule,
   .dispensingRequestStatusErd = Erd_DispensingRequestStatus
};

TEST_GROUP(DispenseController)
{
   DispenseController_t instance;
   ReferDataModel_TestDouble_t referDataModelTestDouble;
   TimerModule_TestDouble_t *timerModuleTestDouble;

   I_DataModel_t *dataModel;
   const DispenserData_t *dispenserData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&referDataModelTestDouble);
      dataModel = referDataModelTestDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&referDataModelTestDouble);

      dispenserData = PersonalityParametricData_Get(dataModel)->dispenserData;
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks);
   }

   void GivenTheModuleIsInitialized()
   {
      DispenseController_Init(
         &instance,
         dataModel,
         dispenserData,
         &config);
   }

   void WhenDispensingRequestActionIsStopThenStart()
   {
      WhenTheDispensingRequestActionIs(DispensingAction_Stop);
      WhenTheDispensingRequestActionIs(DispensingAction_Start);
   }

   void WhenDispensingIsEnabled()
   {
      DataModel_Write(dataModel, Erd_DispensingDisabled, clear);
   }

   void GivenDispensingIsDisabled()
   {
      DataModel_Write(dataModel, Erd_DispensingDisabled, set);
   }

   void WhenDispensingIsDisabled()
   {
      GivenDispensingIsDisabled();
   }

   void GivenDispensingInhibitedByRfid()
   {
      DataModel_Write(dataModel, Erd_DispensingInhibitedByRfid, set);
   }

   void WhenDispensingNotInhibitedByRfid()
   {
      DataModel_Write(dataModel, Erd_DispensingInhibitedByRfid, clear);
   }

   void WhenDispensingIsInhibitedByRfid()
   {
      GivenDispensingInhibitedByRfid();
   }

   void GivenIceDispensingInhibitedByDoor()
   {
      DataModel_Write(dataModel, Erd_IceDispensingInhibitedByDoor, set);
   }

   void WhenIceDispensingInhibitedByDoor()
   {
      GivenIceDispensingInhibitedByDoor();
   }

   void GivenWaterDispensingInhibitedByDoor()
   {
      DataModel_Write(dataModel, Erd_WaterDispensingInhibitedByDoor, set);
   }

   void WhenWaterDispensingInhibitedByDoor()
   {
      GivenWaterDispensingInhibitedByDoor();
   }

   void GivenIceDispensingNotInhibitedByDoor()
   {
      DataModel_Write(dataModel, Erd_IceDispensingInhibitedByDoor, clear);
   }

   void WhenIceDispensingNotInhibitedByDoor()
   {
      GivenIceDispensingNotInhibitedByDoor();
   }

   void WhenWaterDispensingNotInhibitedByDoor()
   {
      DataModel_Write(dataModel, Erd_WaterDispensingInhibitedByDoor, clear);
   }

   void GivenWaterDispensingNotInhibitedByDoor()
   {
      WhenWaterDispensingNotInhibitedByDoor();
   }

   void GivenDispensingInhibitedByAutofillSensorError()
   {
      DataModel_Write(dataModel, Erd_AutofillSensorError, set);
   }

   void WhenDispensingNotInhibitedByAutofillSensorError()
   {
      DataModel_Write(dataModel, Erd_AutofillSensorError, clear);
   }

   void WhenDispensingIsInhibitedByAutofillSensorError()
   {
      GivenDispensingInhibitedByAutofillSensorError();
   }

   void TheAugerMotorDispensingVoteShouldBe(AugerMotorIceType_t iceType, Vote_t vote)
   {
      AugerMotorVotedIceType_t actual;
      DataModel_Read(dataModel, Erd_AugerMotor_DispensingVote, &actual);

      CHECK_EQUAL(iceType, actual.iceType);
      CHECK_EQUAL(vote, actual.care);
   }

   void TheIsolationValveDispensingVoteShouldBe(WaterValveState_t state, Vote_t vote)
   {
      WaterValveVotedState_t actual;
      DataModel_Read(dataModel, Erd_IsolationWaterValve_DispensingVote, &actual);

      CHECK_EQUAL(state, actual.state);
      CHECK_EQUAL(vote, actual.care);
   }

   void TheDispensingValveDispensingVoteShouldBe(WaterValveState_t state, Vote_t vote)
   {
      WaterValveVotedState_t actual;
      DataModel_Read(dataModel, Erd_DispenserWaterValve_DispensingVote, &actual);

      CHECK_EQUAL(state, actual.state);
      CHECK_EQUAL(vote, actual.care);
   }

   void TheDispenseStatusShouldBe(DispenseStatus_t expected)
   {
      DispenseStatus_t actual;
      DataModel_Read(dataModel, Erd_PrivateDispensingResultStatus, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void TheDispensingRequestStatusAndSelectionAndActionShouldBe(DispenseStatus_t expectedStatus, DispenseStatus_t expectedSelection, DispensingAction_t expectedAction)
   {
      DispensingRequestStatus_t actual;
      DataModel_Read(dataModel, Erd_DispensingRequestStatus, &actual);

      CHECK_EQUAL(expectedStatus, actual.status);
      CHECK_EQUAL(expectedSelection, actual.selection);
      CHECK_EQUAL(expectedAction, actual.action);
   }

   void TheDispensingRequestStatusShouldBe(DispenseStatus_t expected)
   {
      DispensingRequestStatus_t actual;
      DataModel_Read(dataModel, Erd_DispensingRequestStatus, &actual);

      CHECK_EQUAL(expected, actual.status);
   }

   void WhenTheDispensingRequestActionIs(DispensingAction_t action)
   {
      DispensingRequest_t request;
      DataModel_Read(dataModel, Erd_PrivateDispensingRequest, &request);

      request.action = action;
      DataModel_Write(dataModel, Erd_PrivateDispensingRequest, &request);
   }

   void GivenTheDispensingRequestIs(DispensingRequestSelection_t selection, DispensingAction_t action)
   {
      DispensingRequest_t request;
      DataModel_Read(dataModel, Erd_PrivateDispensingRequest, &request);

      request.selection = selection;
      request.action = action;
      DataModel_Write(dataModel, Erd_PrivateDispensingRequest, &request);
   }

   void WhenTheDispensingRequestIs(DispensingRequestSelection_t selection, DispensingAction_t action)
   {
      GivenTheDispensingRequestIs(selection, action);
   }

   void ShouldNotBeDispensing()
   {
      TheAugerMotorDispensingVoteShouldBe(AugerMotorIceType_Off, Vote_DontCare);
      TheDispensingValveDispensingVoteShouldBe(WaterValveState_Off, Vote_DontCare);
      TheIsolationValveDispensingVoteShouldBe(WaterValveState_Off, Vote_DontCare);
   }

   void ShouldBeDispensing()
   {
      bool aLoadIsOn;

      WaterValveVotedState_t dispensingValve;
      DataModel_Read(dataModel, Erd_DispenserWaterValve_DispensingVote, &dispensingValve);

      WaterValveVotedState_t isolationValve;
      DataModel_Read(dataModel, Erd_IsolationWaterValve_DispensingVote, &isolationValve);

      AugerMotorVotedIceType_t augerMotor;
      DataModel_Read(dataModel, Erd_AugerMotor_DispensingVote, &augerMotor);

      aLoadIsOn = dispensingValve.care || isolationValve.care || augerMotor.care;
      CHECK_EQUAL(true, aLoadIsOn);
   }

   void TheMaxDispenseTimerShouldNotRun()
   {
      Timer_t *timer = &instance._private.maxDispenseTimer;
      CHECK_EQUAL(false, TimerModule_IsRunning(&timerModuleTestDouble->timerModule, timer));
   }

   void GivenTheFsmIsInitializedAndInDispensingStateWithSelection(DispensingRequestSelection_t selection)
   {
      GivenTheModuleIsInitialized();
      WhenDispensingIsEnabled();
      WhenWaterDispensingNotInhibitedByDoor();
      WhenIceDispensingNotInhibitedByDoor();
      WhenDispensingNotInhibitedByRfid();
      GivenTheDispensingRequestIs(selection, DispensingAction_Start);
   }

   void GivenTheFsmIsInitializedIntoNotDispensingState()
   {
      WhenTheDispensingRequestActionIs(DispensingAction_Stop);
      GivenTheModuleIsInitialized();
      TheDispenseStatusShouldBe(DispenseStatus_CompletedSuccessfully);
   }
};

TEST(DispenseController, ShouldSetDispensingStatusToDispensingAndVoteIsolationValveOnAndDispensingValveOnUponEnteringDispensingStateIfRequestIsWater)
{
   GivenTheFsmIsInitializedAndInDispensingStateWithSelection(DispensingRequestSelection_Water);

   TheDispensingValveDispensingVoteShouldBe(WaterValveState_On, Vote_Care);
   TheIsolationValveDispensingVoteShouldBe(WaterValveState_On, Vote_Care);
   TheAugerMotorDispensingVoteShouldBe(AugerMotorIceType_Off, Vote_DontCare);
   TheDispensingRequestStatusShouldBe(DispenseStatus_Dispensing);
}

TEST(DispenseController, ShouldVoteIsolationValveOnAndDispensingValveOnUponEnteringDispensingStateIfRequestIsAutofill)
{
   GivenTheFsmIsInitializedAndInDispensingStateWithSelection(DispensingRequestSelection_Autofill);

   TheDispensingValveDispensingVoteShouldBe(WaterValveState_On, Vote_Care);
   TheIsolationValveDispensingVoteShouldBe(WaterValveState_On, Vote_Care);
   TheAugerMotorDispensingVoteShouldBe(AugerMotorIceType_Off, Vote_DontCare);
}

TEST(DispenseController, ShouldVoteAugerMotorCubedUponEnteringDispensingStateIfRequestIsCubedIce)
{
   GivenTheFsmIsInitializedAndInDispensingStateWithSelection(DispensingRequestSelection_CubedIce);

   TheAugerMotorDispensingVoteShouldBe(AugerMotorIceType_Cubed, Vote_Care);
   TheDispensingValveDispensingVoteShouldBe(WaterValveState_Off, Vote_DontCare);
   TheIsolationValveDispensingVoteShouldBe(WaterValveState_Off, Vote_DontCare);
}

TEST(DispenseController, ShouldVoteAugerMotorCrushedUponEnteringDispensingStateIfRequestIsCrushedIce)
{
   GivenTheFsmIsInitializedAndInDispensingStateWithSelection(DispensingRequestSelection_CrushedIce);

   TheAugerMotorDispensingVoteShouldBe(AugerMotorIceType_Crushed, Vote_Care);
   TheDispensingValveDispensingVoteShouldBe(WaterValveState_Off, Vote_DontCare);
   TheIsolationValveDispensingVoteShouldBe(WaterValveState_Off, Vote_DontCare);
}

TEST(DispenseController, ShouldStopMaxTimerWhenDispensingRequestActionIsStop)
{
   GivenTheFsmIsInitializedAndInDispensingStateWithSelection(DispensingRequestSelection_Water);

   After(dispenserData->maximumDispenseTimeInSeconds * MSEC_PER_SEC - 10);
   ShouldBeDispensing();

   WhenTheDispensingRequestActionIs(DispensingAction_Stop);
   TheMaxDispenseTimerShouldNotRun();
}

TEST(DispenseController, ShouldSetStatusToHitMaxTimeWhenTimerExpires)
{
   GivenTheFsmIsInitializedAndInDispensingStateWithSelection(DispensingRequestSelection_Water);

   After(dispenserData->maximumDispenseTimeInSeconds * MSEC_PER_SEC - 1);
   ShouldBeDispensing();

   After(1);
   TheDispenseStatusShouldBe(DispenseStatus_HitMaxTime);
   TheMaxDispenseTimerShouldNotRun();
}

TEST(DispenseController, ShouldSetStatusToDisabledOrBlockedWhenInDispensingStateAndDispensingIsDisabled)
{
   GivenTheFsmIsInitializedAndInDispensingStateWithSelection(DispensingRequestSelection_Autofill);
   GivenDispensingIsDisabled();

   TheDispenseStatusShouldBe(DispenseStatus_DisabledOrBlocked);
}

TEST(DispenseController, ShouldSetStatusToSensorErrorWhenInDispensingStateAndAutofillSensorErrorErdIsTrue)
{
   GivenTheFsmIsInitializedAndInDispensingStateWithSelection(DispensingRequestSelection_Autofill);
   GivenDispensingInhibitedByAutofillSensorError();

   TheDispenseStatusShouldBe(DispenseStatus_SensorError);
}

TEST(DispenseController, ShouldSetStatusToInhibitedDueToDoorOpenWhenInDispensingStateAndWaterDispensingIsInhibited)
{
   GivenTheFsmIsInitializedAndInDispensingStateWithSelection(DispensingRequestSelection_Water);
   GivenWaterDispensingInhibitedByDoor();

   TheDispenseStatusShouldBe(DispenseStatus_DispenseInhibitedDueToDoorOpen);
}

TEST(DispenseController, ShouldSetStatusToInhibitedDueToDoorOpenWhenInDispensingStateAndIceDispensingIsInhibited)
{
   GivenTheFsmIsInitializedAndInDispensingStateWithSelection(DispensingRequestSelection_CubedIce);
   GivenIceDispensingInhibitedByDoor();

   TheDispenseStatusShouldBe(DispenseStatus_DispenseInhibitedDueToDoorOpen);
}

TEST(DispenseController, ShouldSetStatusToInhibitedDueToRfidWhenInDispensingStateAndRfidisInvalidErdIsTrue)
{
   GivenTheFsmIsInitializedAndInDispensingStateWithSelection(DispensingRequestSelection_CubedIce);
   GivenDispensingInhibitedByRfid();

   TheDispenseStatusShouldBe(DispenseStatus_DispenseInhibitedDueToRfidErrorOrLeak);
}

TEST(DispenseController, ShouldTransitionToNotDispensingStateIfSelectionIsAutofillAndDispenseRequestIsStop)
{
   GivenTheFsmIsInitializedAndInDispensingStateWithSelection(DispensingRequestSelection_Autofill);

   WhenTheDispensingRequestActionIs(DispensingAction_Stop);
   ShouldNotBeDispensing();
}

TEST(DispenseController, ShouldTransitionToNotDispensingStateIfSelectionIsWaterAndDispenseRequestIsStop)
{
   GivenTheFsmIsInitializedAndInDispensingStateWithSelection(DispensingRequestSelection_Water);

   WhenTheDispensingRequestActionIs(DispensingAction_Stop);
   ShouldNotBeDispensing();
}

TEST(DispenseController, ShouldTransitionToNotDispensingStateIfSelectionIsCubedIceAndDispenseRequestIsStop)
{
   GivenTheFsmIsInitializedAndInDispensingStateWithSelection(DispensingRequestSelection_CubedIce);

   WhenTheDispensingRequestActionIs(DispensingAction_Stop);
   ShouldNotBeDispensing();
}

TEST(DispenseController, ShouldTransitionToNotDispensingStateIfSelectionIsCrushedIceAndDispenseRequestIsStop)
{
   GivenTheFsmIsInitializedAndInDispensingStateWithSelection(DispensingRequestSelection_CrushedIce);

   WhenTheDispensingRequestActionIs(DispensingAction_Stop);
   ShouldNotBeDispensing();
}

TEST(DispenseController, ShouldVoteOffAndDontCareForAllLoadsUponEnteringNotDispensingState)
{
   GivenTheFsmIsInitializedIntoNotDispensingState();

   TheAugerMotorDispensingVoteShouldBe(AugerMotorIceType_Off, Vote_DontCare);
   TheDispensingValveDispensingVoteShouldBe(WaterValveState_Off, Vote_DontCare);
   TheIsolationValveDispensingVoteShouldBe(WaterValveState_Off, Vote_DontCare);
}

TEST(DispenseController, ShouldSetStatusToDisabledOrBlockedWhenInNotDispensingStateAndDispensingIsDisabled)
{
   GivenTheFsmIsInitializedIntoNotDispensingState();
   GivenDispensingIsDisabled();

   TheDispenseStatusShouldBe(DispenseStatus_DisabledOrBlocked);
}

TEST(DispenseController, ShouldSetStatusToSensorErrorWhenInNotDispensingStateAndAutofillSensorErrorErdIsTrue)
{
   GivenTheFsmIsInitializedIntoNotDispensingState();
   GivenDispensingInhibitedByAutofillSensorError();

   TheDispenseStatusShouldBe(DispenseStatus_SensorError);
}

TEST(DispenseController, ShouldSetStatusToInhibitedDueToDoorOpenWhenInNotDispensingStateAndWaterDispensingIsInhibited)
{
   GivenTheFsmIsInitializedIntoNotDispensingState();
   GivenWaterDispensingInhibitedByDoor();

   TheDispenseStatusShouldBe(DispenseStatus_DispenseInhibitedDueToDoorOpen);
}

TEST(DispenseController, ShouldSetStatusToInhibitedDueToDoorOpenWhenInNotDispensingStateAndIceDispensingIsInhibited)
{
   GivenTheFsmIsInitializedIntoNotDispensingState();
   GivenTheDispensingRequestIs(DispensingRequestSelection_CrushedIce, DispensingAction_Stop);
   GivenIceDispensingInhibitedByDoor();

   TheDispenseStatusShouldBe(DispenseStatus_DispenseInhibitedDueToDoorOpen);
}

TEST(DispenseController, ShouldSetStatusToInhibitedDueToRFIDWhenInNotDispensingStateAndRFIDisInvalidErdIsTrue)
{
   GivenTheFsmIsInitializedIntoNotDispensingState();
   GivenDispensingInhibitedByRfid();

   TheDispenseStatusShouldBe(DispenseStatus_DispenseInhibitedDueToRfidErrorOrLeak);
}

TEST(DispenseController, ShouldNotSetStatusToInhibitedDueToDoorOpenWhenInNotDispensingStateAndIceDispensingIsInhibitedButSelectionIsWater)
{
   GivenIceDispensingInhibitedByDoor();
   GivenTheFsmIsInitializedIntoNotDispensingState();

   WhenTheDispensingRequestIs(DispensingRequestSelection_Water, DispensingAction_Start);
   TheDispenseStatusShouldBe(DispenseStatus_CompletedSuccessfully);
}

TEST(DispenseController, ShouldSetStatusToInhibitedDueToDoorOpenWhenInNotDispensingStateAndWaterDispensingIsInhibitedAndSelectionIsWater)
{
   GivenWaterDispensingInhibitedByDoor();
   GivenTheFsmIsInitializedIntoNotDispensingState();

   WhenTheDispensingRequestIs(DispensingRequestSelection_Water, DispensingAction_Start);
   TheDispenseStatusShouldBe(DispenseStatus_DispenseInhibitedDueToDoorOpen);
}

TEST(DispenseController, ShouldNotSetStatusToInhibitedDueToDoorOpenWhenInNotDispensingStateAndIceDispensingIsInhibitedButSelectionIsAutofill)
{
   GivenIceDispensingInhibitedByDoor();
   GivenTheFsmIsInitializedIntoNotDispensingState();

   WhenTheDispensingRequestIs(DispensingRequestSelection_Autofill, DispensingAction_Start);
   TheDispenseStatusShouldBe(DispenseStatus_CompletedSuccessfully);
}

TEST(DispenseController, ShouldSetStatusToInhibitedDueToDoorOpenWhenInNotDispensingStateAndWaterDispensingIsInhibitedAndSelectionIsAutofill)
{
   GivenWaterDispensingInhibitedByDoor();
   GivenTheFsmIsInitializedIntoNotDispensingState();

   WhenTheDispensingRequestIs(DispensingRequestSelection_Autofill, DispensingAction_Start);
   TheDispenseStatusShouldBe(DispenseStatus_DispenseInhibitedDueToDoorOpen);
}

TEST(DispenseController, ShouldNotSetStatusToInhibitedDueToDoorOpenWhenInNotDispensingStateAndWaterDispensingIsInhibitedButSelectionIsCubedIce)
{
   GivenWaterDispensingInhibitedByDoor();
   GivenTheFsmIsInitializedIntoNotDispensingState();

   WhenTheDispensingRequestIs(DispensingRequestSelection_CubedIce, DispensingAction_Start);
   TheDispenseStatusShouldBe(DispenseStatus_CompletedSuccessfully);
}

TEST(DispenseController, ShouldSetStatusToInhibitedDueToDoorOpenWhenInNotDispensingStateAndWaterDispensingIsInhibitedAndSelectionIsCubedIce)
{
   GivenTheFsmIsInitializedIntoNotDispensingState();
   GivenIceDispensingInhibitedByDoor();

   WhenTheDispensingRequestIs(DispensingRequestSelection_CubedIce, DispensingAction_Start);
   TheDispenseStatusShouldBe(DispenseStatus_DispenseInhibitedDueToDoorOpen);
}

TEST(DispenseController, ShouldNotSetStatusToInhibitedDueToDoorOpenWhenInNotDispensingStateAndWaterDispensingIsInhibitedButSelectionIsCrushedIce)
{
   GivenWaterDispensingInhibitedByDoor();
   GivenTheFsmIsInitializedIntoNotDispensingState();

   WhenTheDispensingRequestIs(DispensingRequestSelection_CrushedIce, DispensingAction_Start);
   TheDispenseStatusShouldBe(DispenseStatus_CompletedSuccessfully);
}

TEST(DispenseController, ShouldSetStatusToInhibitedDueToDoorOpenWhenInNotDispensingStateAndWaterDispensingIsInhibitedAndSelectionIsCrushedIce)
{
   GivenIceDispensingInhibitedByDoor();
   GivenTheFsmIsInitializedIntoNotDispensingState();

   WhenTheDispensingRequestIs(DispensingRequestSelection_CrushedIce, DispensingAction_Start);
   TheDispenseStatusShouldBe(DispenseStatus_DispenseInhibitedDueToDoorOpen);
}

TEST(DispenseController, ShouldSetStatusToInhibitedDueToRfidWhenInNotDispensingStateAndDispensingIsInhibitedByRfidAndSelectionIsAutofill)
{
   GivenDispensingInhibitedByRfid();
   GivenWaterDispensingNotInhibitedByDoor();
   GivenTheFsmIsInitializedIntoNotDispensingState();

   WhenTheDispensingRequestIs(DispensingRequestSelection_Autofill, DispensingAction_Start);
   TheDispenseStatusShouldBe(DispenseStatus_DispenseInhibitedDueToRfidErrorOrLeak);
}

TEST(DispenseController, ShouldSetStatusToSensorErrorWhenInNotDispensingStateAndAutofillSensorErrorIsSetAndSelectionIsAutofill)
{
   GivenDispensingInhibitedByAutofillSensorError();
   GivenWaterDispensingNotInhibitedByDoor();
   GivenTheFsmIsInitializedIntoNotDispensingState();

   WhenTheDispensingRequestIs(DispensingRequestSelection_Autofill, DispensingAction_Start);
   TheDispenseStatusShouldBe(DispenseStatus_SensorError);
}

TEST(DispenseController, ShouldSetStatusToDisabledOrBlockedWhenInNotDispensingStateAndDispenseIsDisabledAndSelectionIsAutofill)
{
   GivenDispensingIsDisabled();
   GivenWaterDispensingNotInhibitedByDoor();
   GivenTheFsmIsInitializedIntoNotDispensingState();

   WhenTheDispensingRequestIs(DispensingRequestSelection_Autofill, DispensingAction_Start);
   TheDispenseStatusShouldBe(DispenseStatus_DisabledOrBlocked);
}

TEST(DispenseController, ShouldSetStatusToInhibitedDueToDoorOpenWhenInNotDispensingStateAndSelectionIsWaterAndWaterDispensingBecomesInhibited)
{
   GivenTheFsmIsInitializedIntoNotDispensingState();

   WhenTheDispensingRequestIs(DispensingRequestSelection_Water, DispensingAction_Start);
   WhenWaterDispensingInhibitedByDoor();
   TheDispenseStatusShouldBe(DispenseStatus_DispenseInhibitedDueToDoorOpen);
}

TEST(DispenseController, ShouldSetStatusToInhibitedDueToDoorOpenWhenInNotDispensingStateAndSelectionIsAutofillAndWaterDispensingBecomesInhibited)
{
   GivenTheFsmIsInitializedIntoNotDispensingState();

   WhenTheDispensingRequestIs(DispensingRequestSelection_Autofill, DispensingAction_Start);
   WhenWaterDispensingInhibitedByDoor();
   TheDispenseStatusShouldBe(DispenseStatus_DispenseInhibitedDueToDoorOpen);
}

TEST(DispenseController, ShouldSetStatusToInhibitedDueToDoorOpenWhenInNotDispensingStateAndSelectionIsCubedIceAndIceDispensingBecomesInhibited)
{
   GivenTheFsmIsInitializedIntoNotDispensingState();

   WhenTheDispensingRequestIs(DispensingRequestSelection_CubedIce, DispensingAction_Start);
   WhenIceDispensingInhibitedByDoor();
   TheDispenseStatusShouldBe(DispenseStatus_DispenseInhibitedDueToDoorOpen);
}

TEST(DispenseController, ShouldSetStatusToInhibitedDueToDoorOpenWhenInNotDispensingStateAndSelectionIsCrushedIceAndIceDispensingBecomesInhibited)
{
   GivenTheFsmIsInitializedIntoNotDispensingState();

   WhenTheDispensingRequestIs(DispensingRequestSelection_CrushedIce, DispensingAction_Start);
   WhenIceDispensingInhibitedByDoor();
   TheDispenseStatusShouldBe(DispenseStatus_DispenseInhibitedDueToDoorOpen);
}

TEST(DispenseController, ShouldSetStatusToDisabledOrBlockedWhenInNotDispensingStateAndDispenseIsDisabledAndSelectionIsCubedIce)
{
   GivenDispensingIsDisabled();
   GivenIceDispensingNotInhibitedByDoor();
   GivenTheFsmIsInitializedIntoNotDispensingState();

   WhenTheDispensingRequestIs(DispensingRequestSelection_CubedIce, DispensingAction_Start);
   TheDispenseStatusShouldBe(DispenseStatus_DisabledOrBlocked);
}

TEST(DispenseController, ShouldDispenseWhenWaterDispensingIsInhibitedByDoorDuringACrushedIceDispense)
{
   GivenTheFsmIsInitializedIntoNotDispensingState();

   WhenTheDispensingRequestIs(DispensingRequestSelection_CrushedIce, DispensingAction_Start);
   WhenWaterDispensingInhibitedByDoor();

   TheDispensingRequestStatusShouldBe(DispenseStatus_Dispensing);
}

TEST(DispenseController, ShouldDispenseWhenIceDispensingIsInhibitedByDoorDuringAWaterDispense)
{
   GivenTheFsmIsInitializedIntoNotDispensingState();

   WhenTheDispensingRequestIs(DispensingRequestSelection_Water, DispensingAction_Start);
   WhenIceDispensingInhibitedByDoor();

   TheDispensingRequestStatusShouldBe(DispenseStatus_Dispensing);
}

TEST(DispenseController, ShouldTransitionToDispensingStateIfSelectionIsAutofillAndNothingInhibitsDispense)
{
   GivenTheFsmIsInitializedIntoNotDispensingState();
   GivenDispensingIsDisabled();
   GivenWaterDispensingInhibitedByDoor();
   GivenDispensingInhibitedByRfid();
   GivenDispensingInhibitedByAutofillSensorError();
   GivenTheDispensingRequestIs(DispensingRequestSelection_Autofill, DispensingAction_Start);

   WhenDispensingRequestActionIsStopThenStart();
   ShouldNotBeDispensing();

   WhenDispensingIsEnabled();
   WhenDispensingRequestActionIsStopThenStart();
   ShouldNotBeDispensing();

   WhenWaterDispensingNotInhibitedByDoor();
   WhenDispensingRequestActionIsStopThenStart();
   ShouldNotBeDispensing();

   WhenDispensingNotInhibitedByRfid();
   WhenDispensingRequestActionIsStopThenStart();
   ShouldNotBeDispensing();

   WhenDispensingNotInhibitedByAutofillSensorError();
   WhenDispensingRequestActionIsStopThenStart();
   ShouldBeDispensing();
}

TEST(DispenseController, ShouldTransitionToDispensingStateIfSelectionIsWaterAndNothingInhibitsDispense)
{
   GivenTheFsmIsInitializedIntoNotDispensingState();
   GivenDispensingIsDisabled();
   GivenWaterDispensingInhibitedByDoor();
   GivenDispensingInhibitedByRfid();
   GivenDispensingInhibitedByAutofillSensorError();
   GivenTheDispensingRequestIs(DispensingRequestSelection_Water, DispensingAction_Start);

   WhenDispensingRequestActionIsStopThenStart();
   ShouldNotBeDispensing();

   WhenDispensingIsEnabled();
   WhenDispensingRequestActionIsStopThenStart();
   ShouldNotBeDispensing();

   WhenWaterDispensingNotInhibitedByDoor();
   WhenDispensingRequestActionIsStopThenStart();
   ShouldNotBeDispensing();

   WhenDispensingNotInhibitedByRfid();
   WhenDispensingRequestActionIsStopThenStart();
   ShouldBeDispensing();
}

TEST(DispenseController, ShouldTransitionToDispensingStateIfSelectionIsCubedIceAndNothingInhibitsDispense)
{
   GivenTheFsmIsInitializedIntoNotDispensingState();
   GivenDispensingIsDisabled();
   GivenIceDispensingInhibitedByDoor();
   GivenDispensingInhibitedByRfid();
   GivenDispensingInhibitedByAutofillSensorError();
   GivenTheDispensingRequestIs(DispensingRequestSelection_CubedIce, DispensingAction_Start);

   WhenDispensingRequestActionIsStopThenStart();
   ShouldNotBeDispensing();

   WhenDispensingIsEnabled();
   WhenDispensingRequestActionIsStopThenStart();
   ShouldNotBeDispensing();

   WhenIceDispensingNotInhibitedByDoor();
   WhenDispensingRequestActionIsStopThenStart();
   ShouldBeDispensing();
}

TEST(DispenseController, ShouldTransitionToDispensingStateIfSelectionIsCrushedIceAndNothingInhibitsDispense)
{
   GivenTheFsmIsInitializedIntoNotDispensingState();
   GivenDispensingIsDisabled();
   GivenIceDispensingInhibitedByDoor();
   GivenDispensingInhibitedByRfid();
   GivenDispensingInhibitedByAutofillSensorError();
   GivenTheDispensingRequestIs(DispensingRequestSelection_CrushedIce, DispensingAction_Start);

   WhenDispensingRequestActionIsStopThenStart();
   ShouldNotBeDispensing();

   WhenDispensingIsEnabled();
   WhenDispensingRequestActionIsStopThenStart();
   ShouldNotBeDispensing();

   WhenIceDispensingNotInhibitedByDoor();
   WhenDispensingRequestActionIsStopThenStart();
   ShouldBeDispensing();
}

TEST(DispenseController, ShouldNotStopDispensingIfANonStopActionIsRequested)
{
   GivenTheFsmIsInitializedAndInDispensingStateWithSelection(DispensingRequestSelection_CrushedIce);

   WhenTheDispensingRequestActionIs(DispensingAction_Continue);
   ShouldBeDispensing();
}

TEST(DispenseController, ShouldUpdateDispensingRequestStatusBasedOnPrivateDispensingRequest)
{
   GivenTheFsmIsInitializedIntoNotDispensingState();

   WhenTheDispensingRequestIs(DispensingRequestSelection_CrushedIce, DispensingAction_Start);
   TheDispensingRequestStatusAndSelectionAndActionShouldBe(
      DispenseStatus_Dispensing,
      DispensingRequestSelection_CrushedIce,
      DispensingAction_Start);
   ShouldBeDispensing();

   WhenTheDispensingRequestActionIs(DispensingAction_Stop);
   ShouldNotBeDispensing();

   WhenTheDispensingRequestIs(DispensingRequestSelection_CubedIce, DispensingAction_Start);
   TheDispensingRequestStatusAndSelectionAndActionShouldBe(
      DispenseStatus_Dispensing,
      DispensingRequestSelection_CubedIce,
      DispensingAction_Start);
   ShouldBeDispensing();

   WhenTheDispensingRequestActionIs(DispensingAction_Stop);
   ShouldNotBeDispensing();

   WhenTheDispensingRequestIs(DispensingRequestSelection_Water, DispensingAction_Start);
   TheDispensingRequestStatusAndSelectionAndActionShouldBe(
      DispenseStatus_Dispensing,
      DispensingRequestSelection_Water,
      DispensingAction_Start);
   ShouldBeDispensing();

   WhenTheDispensingRequestActionIs(DispensingAction_Stop);
   ShouldNotBeDispensing();
}
