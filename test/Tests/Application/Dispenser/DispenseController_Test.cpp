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
   .flowMeterWaterDispensedOzX100Erd = Erd_FlowMeterWaterDispensedOzX100,
   .flowMeterMonitoringRequestErd = Erd_FlowMeterMonitoringRequest,
   .timerModuleErd = Erd_TimerModule
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

   void GivenDispensingInhibitedByRfid()
   {
      DataModel_Write(dataModel, Erd_DispensingInhibitedByRfid, set);
   }

   void WhenDispensingNotInhibitedByRfid()
   {
      DataModel_Write(dataModel, Erd_DispensingInhibitedByRfid, clear);
   }

   void GivenIceDispensingInhibitedByDoor()
   {
      DataModel_Write(dataModel, Erd_IceDispensingInhibitedByDoor, set);
   }

   void GivenWaterDispensingInhibitedByDoor()
   {
      DataModel_Write(dataModel, Erd_WaterDispensingInhibitedByDoor, set);
   }

   void WhenIceDispensingNotInhibitedByDoor()
   {
      DataModel_Write(dataModel, Erd_IceDispensingInhibitedByDoor, clear);
   }

   void WhenWaterDispensingNotInhibitedByDoor()
   {
      DataModel_Write(dataModel, Erd_WaterDispensingInhibitedByDoor, clear);
   }

   void GivenDispensingInhibitedByAutofillSensorError()
   {
      DataModel_Write(dataModel, Erd_AutofillSensorError, set);
   }

   void WhenDispensingNotInhibitedByAutofillSensorError()
   {
      DataModel_Write(dataModel, Erd_AutofillSensorError, clear);
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

   void WhenTheDispensingRequestActionIs(DispensingAction_t action)
   {
      DispensingRequest_t request;
      DataModel_Read(dataModel, Erd_PrivateDispensingRequest, &request);

      request.action = action;
      DataModel_Write(dataModel, Erd_PrivateDispensingRequest, &request);
   }

   void GivenTheDispensingRequestSelectionIs(DispensingRequestSelection_t selection)
   {
      DispensingRequest_t request;
      DataModel_Read(dataModel, Erd_PrivateDispensingRequest, &request);

      request.selection = selection;
      DataModel_Write(dataModel, Erd_PrivateDispensingRequest, &request);
   }

   void ShouldNotBeDispensing()
   {
      TheAugerMotorDispensingVoteShouldBe(AugerMotorIceType_Off, Vote_DontCare);
      TheDispensingValveDispensingVoteShouldBe(WaterValveState_Off, Vote_DontCare);
      TheIsolationValveDispensingVoteShouldBe(WaterValveState_Off, Vote_DontCare);
   }

   void ShouldBeDispensing()
   {
      TheDispenseStatusShouldBe(DispenseStatus_Dispensing);
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
      GivenTheDispensingRequestSelectionIs(selection);
      WhenDispensingIsEnabled();
      WhenWaterDispensingNotInhibitedByDoor();
      WhenIceDispensingNotInhibitedByDoor();
      WhenDispensingNotInhibitedByRfid();
      WhenTheDispensingRequestActionIs(DispensingAction_Start);
   }

   void GivenTheFsmIsInitializedIntoNotDispensingState()
   {
      WhenTheDispensingRequestActionIs(DispensingAction_Stop);
      GivenTheModuleIsInitialized();
   }
};

TEST(DispenseController, ShouldVoteIsolationValveOnAndDispensingValveOnUponEnteringDispensingStateIfRequestIsWater)
{
   GivenTheFsmIsInitializedAndInDispensingStateWithSelection(DispensingRequestSelection_Water);

   TheDispensingValveDispensingVoteShouldBe(WaterValveState_On, Vote_Care);
   TheIsolationValveDispensingVoteShouldBe(WaterValveState_On, Vote_Care);
   TheAugerMotorDispensingVoteShouldBe(AugerMotorIceType_Off, Vote_DontCare);
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
   TheDispenseStatusShouldBe(DispenseStatus_Dispensing);

   WhenTheDispensingRequestActionIs(DispensingAction_Stop);
   TheMaxDispenseTimerShouldNotRun();
}

TEST(DispenseController, ShouldSetStatusToHitMaxTimeWhenTimerExpires)
{
   GivenTheFsmIsInitializedAndInDispensingStateWithSelection(DispensingRequestSelection_Water);

   After(dispenserData->maximumDispenseTimeInSeconds * MSEC_PER_SEC - 1);
   TheDispenseStatusShouldBe(DispenseStatus_Dispensing);

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
   GivenTheFsmIsInitializedAndInDispensingStateWithSelection(DispensingRequestSelection_Water);
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
   GivenIceDispensingInhibitedByDoor();

   TheDispenseStatusShouldBe(DispenseStatus_DispenseInhibitedDueToDoorOpen);
}

TEST(DispenseController, ShouldSetStatusToInhibitedDueToRFIDWhenInNotDispensingStateAndRFIDisInvalidErdIsTrue)
{
   GivenTheFsmIsInitializedIntoNotDispensingState();
   GivenDispensingInhibitedByRfid();

   TheDispenseStatusShouldBe(DispenseStatus_DispenseInhibitedDueToRfidErrorOrLeak);
}

TEST(DispenseController, ShouldTransitionToDispensingStateIfSelectionIsAutofillAndNothingInhibitsDispense)
{
   GivenTheFsmIsInitializedIntoNotDispensingState();
   GivenDispensingIsDisabled();
   GivenWaterDispensingInhibitedByDoor();
   GivenDispensingInhibitedByRfid();
   GivenDispensingInhibitedByAutofillSensorError();
   GivenTheDispensingRequestSelectionIs(DispensingRequestSelection_Autofill);

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
   GivenTheDispensingRequestSelectionIs(DispensingRequestSelection_Water);

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
   GivenTheDispensingRequestSelectionIs(DispensingRequestSelection_CubedIce);

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
   GivenTheDispensingRequestSelectionIs(DispensingRequestSelection_CrushedIce);

   WhenDispensingRequestActionIsStopThenStart();
   ShouldNotBeDispensing();

   WhenDispensingIsEnabled();
   WhenDispensingRequestActionIsStopThenStart();
   ShouldNotBeDispensing();

   WhenIceDispensingNotInhibitedByDoor();
   WhenDispensingRequestActionIsStopThenStart();
   ShouldBeDispensing();
}
