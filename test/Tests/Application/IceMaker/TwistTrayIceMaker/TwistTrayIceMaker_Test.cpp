/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include <stdint.h>
#include "TwistTrayIceMaker.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"
#include "Signal.h"
#include "TwistTrayIceMakerHighLevelState.h"
#include "TwistTrayIceMakerMotorController.h"
#include "SystemErds.h"
#include "TddPersonality.h"
#include "utils.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "uassert_test.h"
#include "ReferDataModel_TestDouble.h"
#include "PersonalityParametricDataTestDouble.h"

#define Given
#define When
#define Still
#define And
#define Then
#define OPEN HIGH
#define CLOSED LOW
#define ACTIVE HIGH

enum
{
   Idle = IceMakerMotorAction_Idle,
   Home = IceMakerMotorAction_RunHomingRoutine,
   Harvest = IceMakerMotorAction_RunCycle,
   Homed = IceMakerMotorActionResult_Homed,
   Harvesting = IceMakerMotorActionResult_Harvesting,
   Harvested = IceMakerMotorActionResult_Harvested,
   BucketWasFull = IceMakerMotorActionResult_BucketWasFull,
   MotorError = IceMakerMotorActionResult_MotorError,

   FaultState = TwistTrayIceMakerHighLevelState_Fault,

   VeryCold = 200,

   IceTemperaturePollingTime = 3 * MSEC_PER_SEC,
   ALongTime = 3 * SECONDS_PER_HOUR * MSEC_PER_SEC,
   OneSecond = 1 * MSEC_PER_SEC,
};

static const TwistTrayIceMakerConfiguration_t config = {
   .highLevelStateErd = Erd_TwistTrayIceMaker_HighLevelState,
   .operationStateErd = Erd_TwistTrayIceMaker_OperationState,
   .thermistorIsValidResolvedErd = Erd_TwistTrayIceMakerThermistor_IsValidResolved,
   .filteredTemperatureResolvedInDegFx100Erd = Erd_TwistTrayIceMaker_FilteredTemperatureResolvedInDegFx100,
   .testRequestErd = Erd_IceMaker0_TestRequest,
   .stopFillSignalErd = Erd_IceMaker0_StopFillSignal,
   .harvestCountIsReadyToHarvestErd = Erd_IceMaker0_HarvestCountIsReadyToHarvest,
   .harvestCountCalculationRequestErd = Erd_IceMaker0_HarvestCountCalculationRequest,
   .motorIceMakerVoteErd = Erd_TwistTrayIceMakerMotor_IceMakerVote,
   .waterValveIceMakerVoteErd = Erd_TwistTrayIceMakerWaterValve_IceMakerVote,
   .motorActionResultErd = Erd_IceMaker0_MotorActionResult,
   .motorFaultActiveErd = Erd_TwistTrayIceMaker_MotorFaultActive,
   .waterFillMonitoringRequestErd = Erd_IceMaker0_WaterFillMonitoringRequest,
   .isolationWaterValveVoteErd = Erd_IsolationWaterValve_TwistTrayIceMakerVote,
   .iceMakerEnabledResolvedErd = Erd_IceMakerEnabledResolved,
   .sabbathModeErd = Erd_SabbathModeEnable,
   .enhancedSabbathModeErd = Erd_EnhancedSabbathModeEnable,
   .freezerIceRateTriggerSignalErd = Erd_FreezerIceRateTriggerSignal,
   .fillTubeHeaterVoteErd = Erd_FillTubeHeater_TwistTrayIceMakerVote,
   .coolingOffStatusErd = Erd_CoolingOffStatus,
   .freezerIceRateIsActiveErd = Erd_Freezer_IceRateIsActive,
   .dispensingRequestStatusErd = Erd_DispensingRequestStatus,
   .leftSideFreezerDoorStatusResolvedErd = Erd_LeftSideFreezerDoorStatusResolved,
   .dispensingInhibitedReasonErd = Erd_DispensingInhibitedReason,
   .iceMakerFillInhibitedReasonErd = Erd_IceMakerFillInhibitedReason,
   .iceMakerFullStatusErd = Erd_IceMaker0FullStatus
};

static void OnDataModelChange(void *context, const void *_args)
{
   REINTERPRET(args, _args, const DataModelOnDataChangeArgs_t *);

   if(args->erd == Erd_TwistTrayIceMakerMotor_IceMakerVote)
   {
      REINTERPRET(data, args->data, const TwistTrayIceMakerMotorVotedAction_t *);

      mock()
         .actualCall("Write Motor Control")
         .onObject(context)
         .withParameter("Erd", args->erd)
         .withParameter("Data", data->action);
   }
   else if(args->erd == Erd_TwistTrayIceMakerWaterValve_IceMakerVote)
   {
      REINTERPRET(data, args->data, const WaterValveVotedState_t *);

      mock()
         .actualCall("Write Water Valve")
         .onObject(context)
         .withParameter("Erd", args->erd)
         .withParameter("Data", (bool)data->state);
   }
   else if(args->erd == Erd_TwistTrayIceMaker_MotorFaultActive)
   {
      REINTERPRET(data, args->data, const bool *);

      mock()
         .actualCall("Write Motor Fault")
         .onObject(context)
         .withParameter("Erd", args->erd)
         .withParameter("Data", *data);
   }
   else if(args->erd == Erd_TwistTrayIceMaker_HighLevelState)
   {
      REINTERPRET(data, args->data, const uint8_t *);

      mock()
         .actualCall("Write High Level State")
         .onObject(context)
         .withParameter("Erd", args->erd)
         .withParameter("Data", *data);
   }
   else if(args->erd == Erd_FreezerIceRateTriggerSignal)
   {
      mock()
         .actualCall("Freezer Ice Rate Signal Has Been Triggered")
         .onObject(context)
         .withParameter("Erd", args->erd);
   }

   else if(args->erd == Erd_FillTubeHeater_TwistTrayIceMakerVote)
   {
      REINTERPRET(data, args->data, const PercentageDutyCycleVote_t *);
      mock()
         .actualCall("Write Fill Tube Heater")
         .onObject(context)
         .withParameter("Erd", args->erd)
         .withParameter("Percentage Duty Cycle", (PercentageDutyCycle_t)data->percentageDutyCycle)
         .withParameter("Care", (bool)data->care);
   }
}

TEST_GROUP(TwistTrayIceMaker)
{
   TwistTrayIceMaker_t instance;
   ReferDataModel_TestDouble_t referDataModelTestDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t timerModuleTestDouble;
   const TwistTrayIceMakerData_t *iceMakerData;

   EventSubscription_t dataModelTestDoubleOnChangeEventSubscription;

   void setup()
   {
      TimerModule_TestDouble_Init(&timerModuleTestDouble);
      ReferDataModel_TestDouble_Init(&referDataModelTestDouble);
      dataModel = referDataModelTestDouble.dataModel;

      iceMakerData = PersonalityParametricData_Get(dataModel)->iceMakerData->iceMakerSlots->slot0Data->twistTrayData;

      EventSubscription_Init(&dataModelTestDoubleOnChangeEventSubscription, dataModel, OnDataModelChange);
      Event_Subscribe(dataModel->OnDataChange, &dataModelTestDoubleOnChangeEventSubscription);
   }

   void GivenTheModuleIsInitialized()
   {
      TwistTrayIceMaker_Init(
         &instance,
         &timerModuleTestDouble.timerModule,
         DataModel_AsDataSource(dataModel),
         &config,
         iceMakerData);
   }

   void GivenTheTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_TwistTrayIceMakerThermistor_IsValidResolved, set);
      DataModel_Write(dataModel, Erd_TwistTrayIceMaker_FilteredTemperatureResolvedInDegFx100, &temperature);
   }

   void WhenTheTemperatureIs(TemperatureDegFx100_t temperature)
   {
      GivenTheTemperatureIs(temperature);
   }

   void GivenTheIceMakerThermistorIsValid()
   {
      DataModel_Write(
         dataModel,
         Erd_TwistTrayIceMakerThermistor_IsValidResolved,
         set);
   }

   void WhenTheIceMakerThermistorIsValid()
   {
      GivenTheIceMakerThermistorIsValid();
   }

   void GivenTheIceMakerThermistorIsInvalid()
   {
      DataModel_Write(
         dataModel,
         Erd_TwistTrayIceMakerThermistor_IsValidResolved,
         clear);
   }

   void WhenTheIceMakerThermistorIsInvalid()
   {
      GivenTheIceMakerThermistorIsInvalid();
   }

   void GivenTheIceMakerIsEnabled()
   {
      DataModel_Write(
         dataModel,
         Erd_IceMakerEnabledResolved,
         set);
   }

   void GivenTheCoolingSystemOffIs(bool state)
   {
      DataModel_Write(
         dataModel,
         Erd_CoolingOffStatus,
         &state);
   }

   void WhenTheCoolingSystemOffChangesTo(bool state)
   {
      GivenTheCoolingSystemOffIs(state);
   }

   void WhenTheIceMakerBecomesEnabled()
   {
      GivenTheIceMakerIsEnabled();
   }

   void GivenTheIceMakerIsDisabled()
   {
      DataModel_Write(
         dataModel,
         Erd_IceMakerEnabledResolved,
         clear);
   }

   void WhenTheIceMakerBecomesDisabled()
   {
      GivenTheIceMakerIsDisabled();
   }

   void TheMotorShouldBeRequestedTo(IceMakerMotorAction_t expectedMotorState)
   {
      mock()
         .expectOneCall("Write Motor Control")
         .onObject(dataModel)
         .withParameter("Erd", Erd_TwistTrayIceMakerMotor_IceMakerVote)
         .withParameter("Data", expectedMotorState);
   }

   void TheMotorShouldBeRequestedToIdleThenHome()
   {
      TheMotorShouldBeRequestedTo(Idle);
      Then TheMotorShouldBeRequestedTo(Home);
   }

   void WhenTheMotorActionResultIs(IceMakerMotorActionResult_t motorActionResult)
   {
      DataModel_Write(
         dataModel,
         Erd_IceMaker0_MotorActionResult,
         &motorActionResult);
   }

   void GivenTheMotorActionResultIs(IceMakerMotorActionResult_t motorActionResult)
   {
      WhenTheMotorActionResultIs(motorActionResult);
   }

   void WhenTheFreezerDoorIs(bool freezerDoorState)
   {
      DataModel_Write(
         dataModel,
         Erd_LeftSideFreezerDoorStatusResolved,
         &freezerDoorState);
   }

   void TheWaterValveShouldBecome(bool state)
   {
      mock()
         .expectOneCall("Write Water Valve")
         .onObject(dataModel)
         .withParameter("Erd", Erd_TwistTrayIceMakerWaterValve_IceMakerVote)
         .withParameter("Data", state);
   }

   void WhenIceStartsDispensing(void)
   {
      DispensingRequestStatus_t status = {
         .action = DispensingAction_Start,
         .selection = DispensingRequestSelection_CubedIce,
         .specialOptions = DispensingSpecialOptions_None,
         .status = DispenseStatus_Dispensing,
         .preciseFillOuncesx100 = 0
      };

      DataModel_Write(
         dataModel,
         Erd_DispensingRequestStatus,
         &status);
   }

   void WhenIceStopsDispensing(void)
   {
      DispensingRequestStatus_t status = {
         .action = DispensingAction_Stop,
         .selection = DispensingRequestSelection_CubedIce,
         .specialOptions = DispensingSpecialOptions_None,
         .status = DispenseStatus_CompletedSuccessfully,
         .preciseFillOuncesx100 = 0
      };

      DataModel_Write(
         dataModel,
         Erd_DispensingRequestStatus,
         &status);
   }

   void WhenDispensingRequestStatusIs(DispenseStatus_t newStatus, DispensingRequestSelection_t newSelection)
   {
      DispensingRequestStatus_t status;
      DataModel_Read(
         dataModel,
         Erd_DispensingRequestStatus,
         &status);

      status.status = newStatus;
      status.selection = newSelection;

      DataModel_Write(
         dataModel,
         Erd_DispensingRequestStatus,
         &status);
   }

   void GivenDispensingRequestStatusIs(DispenseStatus_t newStatus, DispensingRequestSelection_t newSelection)
   {
      WhenDispensingRequestStatusIs(newStatus, newSelection);
   }

   void WhenWaterStartsDispensing(void)
   {
      DispensingRequestStatus_t status = {
         .action = DispensingAction_Start,
         .selection = DispensingRequestSelection_Water,
         .specialOptions = DispensingSpecialOptions_None,
         .status = DispenseStatus_Dispensing,
         .preciseFillOuncesx100 = 0
      };

      DataModel_Write(
         dataModel,
         Erd_DispensingRequestStatus,
         &status);
   }

   void WhenWaterStopsDispensing(void)
   {
      DispensingRequestStatus_t status = {
         .action = DispensingAction_Stop,
         .selection = DispensingRequestSelection_Water,
         .specialOptions = DispensingSpecialOptions_None,
         .status = DispenseStatus_CompletedSuccessfully,
         .preciseFillOuncesx100 = 0
      };

      DataModel_Write(
         dataModel,
         Erd_DispensingRequestStatus,
         &status);
   }

   void GivenSabbathModeIs(bool state)
   {
      DataModel_Write(
         dataModel,
         Erd_SabbathModeEnable,
         &state);
   }

   void WhenSabbathModeIs(bool state)
   {
      GivenSabbathModeIs(state);
   }

   void GivenEnhancedSabbathModeIs(bool state)
   {
      DataModel_Write(
         dataModel,
         Erd_EnhancedSabbathModeEnable,
         &state);
   }

   void WhenEnhancedSabbathModeIs(bool state)
   {
      GivenEnhancedSabbathModeIs(state);
   }

   void TheMotorFaultShouldBecome(bool expectedState)
   {
      mock()
         .expectOneCall("Write Motor Fault")
         .onObject(dataModel)
         .withParameter("Erd", Erd_TwistTrayIceMaker_MotorFaultActive)
         .withParameter("Data", expectedState);
   }

   void TheHighLevelStateShouldBecome(uint8_t expectedState)
   {
      mock()
         .expectOneCall("Write High Level State")
         .onObject(dataModel)
         .withParameter("Erd", Erd_TwistTrayIceMaker_HighLevelState)
         .withParameter("Data", expectedState);
   }

   void After(TimerTicks_t time)
   {
      TimerModule_TestDouble_ElapseTime(&timerModuleTestDouble, time);
   }

   void SomeTimePasses(TimerTicks_t time)
   {
      After(time);
   }

   void FillingShouldStart()
   {
      TheWaterValveShouldBecome(OPEN);
   }

   void HarvestingShouldStart()
   {
      TheMotorShouldBeRequestedTo(Harvest);
      FillTubeHeaterVoteAndCareShouldBecome(iceMakerData->harvestData.fillTubeHeaterDutyCyclePercentage, Vote_Care);
   }

   void HomingShouldStart()
   {
      TheMotorShouldBeRequestedTo(Home);
   }

   void FreezerTriggerIceRateSignalShouldIncrement()
   {
      mock()
         .expectOneCall("Freezer Ice Rate Signal Has Been Triggered")
         .onObject(dataModel)
         .withParameter("Erd", Erd_FreezerIceRateTriggerSignal)
         .ignoreOtherParameters();
   }

   void FreezerTriggerIceRateSignalShouldNotIncrement()
   {
      mock()
         .expectNoCall("Freezer Ice Rate Signal Has Been Triggered");
   }

   void MotorRequestShouldNotBeCalled()
   {
      mock()
         .expectNoCall("Write Motor Control");
   }

   void GivenHomingIsCompleted()
   {
      GivenTheIceMakerThermistorIsValid();

      TheMotorShouldBeRequestedTo(Home);
      GivenTheModuleIsInitialized();

      TheMotorShouldBeRequestedTo(Idle);
      WhenTheMotorActionResultIs(Homed);
   }

   void GivenFreezingIsCompletedAndHarvestingIsStarted()
   {
      GivenTheTemperatureIs(iceMakerData->freezeData.maximumHarvestTemperatureInDegFx100 - 1);
      GivenHomingIsCompleted();

      HarvestingShouldStart();
      WhenHarvestCountIsReadyToHarvestIs(SET);
      TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Harvesting);
   }

   void FreezingIsCompletedAndHarvestingDoesNotStartBecauseFreezerDoorOpens()
   {
      TemperatureDegFx100_t actualTempx100 = iceMakerData->freezeData.maximumHarvestTemperatureInDegFx100;
      GivenTheTemperatureIs(actualTempx100);
      GivenHomingIsCompleted();
      GivenFreezerIceRateActiveBecomes(SET);

      WhenTheFreezerDoorIs(OPEN);

      NothingShouldHappen();
      WhenTheTemperatureIs(actualTempx100 - 1);
      And WhenHarvestCountIsReadyToHarvestIs(SET);
   }

   void HarvestingIsCompletedAndFillingIsStarted()
   {
      FillingShouldStart();
      TheMotorShouldBeRequestedTo(Idle);
      FillTubeHeaterVoteAndCareShouldBecome(OFF, Vote_DontCare);
      AfterTheFillTubeHeaterTimerHasExpired();
      WhenTheMotorActionResultIs(Harvested);
   }

   void TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_t expectedState)
   {
      TwistTrayIceMakerOperationState_t actualState;
      DataModel_Read(
         dataModel,
         Erd_TwistTrayIceMaker_OperationState,
         &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void FillTubeHeaterVoteAndCareShouldBe(PercentageDutyCycle_t expectedDutyCycle, Vote_t expectedCare)
   {
      PercentageDutyCycleVote_t actual;
      DataModel_Read(
         dataModel,
         Erd_FillTubeHeater_TwistTrayIceMakerVote,
         &actual);

      CHECK_EQUAL(expectedDutyCycle, actual.percentageDutyCycle);
      CHECK_EQUAL(expectedCare, actual.care);
   }

   void FillTubeHeaterVoteAndCareShouldBecome(PercentageDutyCycle_t expectedDutyCycle, Vote_t expectedCare)
   {
      mock()
         .expectOneCall("Write Fill Tube Heater")
         .onObject(dataModel)
         .withParameter("Erd", Erd_FillTubeHeater_TwistTrayIceMakerVote)
         .withParameter("Percentage Duty Cycle", (PercentageDutyCycle_t)expectedDutyCycle)
         .withParameter("Care", (bool)expectedCare);
   }

   void AfterTheFillTubeHeaterTimerHasExpired()
   {
      After(iceMakerData->harvestData.fillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC);
   }

   void NothingShouldHappen()
   {
   }

   void WhenTheTestRequestIs(IceMakerTestRequest_t request)
   {
      DataModel_Write(
         dataModel,
         Erd_IceMaker0_TestRequest,
         &request);
   }

   void TheTestRequestShouldBe(IceMakerTestRequest_t expectedValue)
   {
      IceMakerTestRequest_t actualValue;
      DataModel_Read(
         dataModel,
         Erd_IceMaker0_TestRequest,
         &actualValue);

      CHECK_EQUAL(expectedValue, actualValue);
   }

   void GivenTheOperationStateIsInHoming(void)
   {
      GivenTheIceMakerThermistorIsValid();

      TheMotorShouldBeRequestedTo(Home);
      GivenTheModuleIsInitialized();
      TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Homing);
   }

   void GivenTheOperationStateIsInFreeze(void)
   {
      GivenTheOperationStateIsInHoming();

      TheMotorShouldBeRequestedTo(Idle);
      GivenTheIceMakerIsEnabled();
      WhenTheMotorActionResultIs(Homed);
      TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Freeze);
   }

   void GivenTheOperationStateIsInHarvesting(void)
   {
      GivenTheIceMakerIsEnabled();
      GivenFreezingIsCompletedAndHarvestingIsStarted();
      TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Harvesting);
   }

   void GivenTheOperationStateIsInFillingTrayWithWater(void)
   {
      GivenTheOperationStateIsInHarvesting();
      HarvestingIsCompletedAndFillingIsStarted();
      TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);
   }

   void GivenTheOperationStateIsInMotorError(void)
   {
      GivenTheOperationStateIsInHarvesting();

      TheMotorShouldBeRequestedTo(Idle);
      FillTubeHeaterVoteAndCareShouldBecome(OFF, Vote_DontCare);
      TheHighLevelStateShouldBecome(FaultState);
      TheMotorFaultShouldBecome(ACTIVE);
      WhenTheMotorActionResultIs(MotorError);
      TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_MotorError);
   }

   void GivenTheOperationStateIsInBucketIsFull(void)
   {
      GivenTheOperationStateIsInHarvesting();

      FillTubeHeaterVoteAndCareShouldBecome(OFF, Vote_DontCare);
      TheMotorShouldBeRequestedTo(Idle);
      WhenTheMotorActionResultIs(BucketWasFull);
      TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_BucketIsFull);
   }

   void GivenFillingHasStartedAfterCompletingFreezingAndHarvesting(void)
   {
      GivenFreezingIsCompletedAndHarvestingIsStarted();
      HarvestingIsCompletedAndFillingIsStarted();
   }

   void GivenTheOperationStateIsInIdleFill(void)
   {
      GivenTheOperationStateIsInFillingTrayWithWater();

      TheWaterValveShouldBecome(CLOSED);
      WhenWaterStartsDispensing();
      TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_IdleFill);
   }

   void WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_t expected)
   {
      IceMakerWaterFillMonitoringRequest_t actual;
      DataModel_Read(
         dataModel,
         Erd_IceMaker0_WaterFillMonitoringRequest,
         &actual);

      CHECK_EQUAL(expected, actual);
   }

   void MinimumFreezeTimeCounterInMinutesShouldBe(uint8_t expected)
   {
      uint8_t actual;
      DataModel_Read(dataModel, Erd_IceMaker0_MinimumFreezeTimeCounterInMinutes, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void WhenTwistTrayIceMakerTrayIsFilled()
   {
      DataModel_Write(
         dataModel,
         Erd_IceMaker0_StopFillSignal,
         set);
   }

   void HarvestCountCalculationRequestShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(
         dataModel,
         Erd_IceMaker0_HarvestCountCalculationRequest,
         &actual);

      CHECK_EQUAL(expected, actual);
   }

   void GivenFreezerIceRateActiveBecomes(bool state)
   {
      DataModel_Write(
         dataModel,
         Erd_Freezer_IceRateIsActive,
         &state);
   }

   void WhenFreezerIceRateActiveBecomes(bool state)
   {
      GivenFreezerIceRateActiveBecomes(state);
   }

   void GivenTheIceMakerIsInStateMotorError()
   {
      GivenTheIceMakerIsEnabled();
      GivenTheIceMakerThermistorIsValid();

      TheMotorShouldBeRequestedTo(Home);
      GivenTheModuleIsInitialized();

      TheMotorShouldBeRequestedTo(Idle);
      TheHighLevelStateShouldBecome(FaultState);
      TheMotorFaultShouldBecome(ACTIVE);
      WhenTheMotorActionResultIs(MotorError);
   }

   void WhenHarvestCountIsReadyToHarvestIs(bool state)
   {
      DataModel_Write(dataModel, Erd_IceMaker0_HarvestCountIsReadyToHarvest, &state);
   }

   void GivenHarvestCountIsReadyToHarvestIs(bool state)
   {
      WhenHarvestCountIsReadyToHarvestIs(state);
   }

   void GivenFillPausedDuringFillingTrayWithWaterStateAndNowInIdleFillState(void)
   {
      GivenSabbathModeIs(OFF);
      GivenTheOperationStateIsInFillingTrayWithWater();
      WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Start);

      TheWaterValveShouldBecome(CLOSED);
      WhenWaterStartsDispensing();
      TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_IdleFill);
      And WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Pause);
   }

   void GivenSabbathIsDisabledAndInFillingTrayWithWaterState(void)
   {
      GivenSabbathModeIs(OFF);
      GivenTheOperationStateIsInFillingTrayWithWater();
      WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Start);
   }

   void GivenIceMakerHasResumedFillAndIsInFillingTrayWithWater()
   {
      GivenFillPausedDuringFillingTrayWithWaterStateAndNowInIdleFillState();

      WhenWaterStopsDispensing();
      TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);
      And WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Resume);
   }

   void ShouldTransitionFromFillingTrayWithWaterToFreezeWhenTrayIsFull()
   {
      FreezerTriggerIceRateSignalShouldIncrement();
      WhenTwistTrayIceMakerTrayIsFilled();
      GivenFreezerIceRateActiveBecomes(SET);
      TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Freeze);
      WhenHarvestCountIsReadyToHarvestIs(CLEAR);
   }

   void ShouldTransitionFromFreezeToHarvestingWhenHarvestConditionsAreMet()
   {
      WhenTheTemperatureIs(iceMakerData->freezeData.maximumHarvestTemperatureInDegFx100 - 2);

      TheMotorShouldBeRequestedTo(Harvest);
      FillTubeHeaterVoteAndCareShouldBecome(iceMakerData->harvestData.fillTubeHeaterDutyCyclePercentage, Vote_Care);
      WhenHarvestCountIsReadyToHarvestIs(SET);
      TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Harvesting);
   }

   void WhenMotorActionResultChangesToHarvestingThenHarvested()
   {
      WhenTheMotorActionResultIs(Harvesting);
      WhenTheMotorActionResultIs(Harvested);
   }

   void GivenDispensingIsInhibitedByRfid()
   {
      DispensingInhibitedReasonBitmap_t bitmap;
      DataModel_Read(dataModel, Erd_DispensingInhibitedReason, &bitmap);

      BITMAP_SET(bitmap.bitmap, DispensingInhibitedReason_WaterDueToRfidFilter);
      DataModel_Write(dataModel, Erd_DispensingInhibitedReason, &bitmap);
   }

   void WhenDispensingIsInhibitedByRfid()
   {
      GivenDispensingIsInhibitedByRfid();
   }

   void WhenDispensingIsNotInhibitedByRfid()
   {
      DispensingInhibitedReasonBitmap_t bitmap;
      DataModel_Read(dataModel, Erd_DispensingInhibitedReason, &bitmap);

      BITMAP_CLEAR(bitmap.bitmap, DispensingInhibitedReason_WaterDueToRfidFilter);
      DataModel_Write(dataModel, Erd_DispensingInhibitedReason, &bitmap);
   }

   void GivenDispensingIsNotInhibitedByRfid()
   {
      WhenDispensingIsNotInhibitedByRfid();
   }

   void WhenIceMakerFillInhibitedDueToNewFilterIs(bool status)
   {
      IceMakerFillInhibitedReasonBitmap_t iceMakerFillInhibitedReason;
      DataModel_Read(
         dataModel,
         Erd_IceMakerFillInhibitedReason,
         &iceMakerFillInhibitedReason);

      if(status)
      {
         BITMAP_SET(&iceMakerFillInhibitedReason, IceMakerFillInhibitedReason_DueToNewFilter);
      }
      else
      {
         BITMAP_CLEAR(&iceMakerFillInhibitedReason, IceMakerFillInhibitedReason_DueToNewFilter);
      }

      DataModel_Write(
         dataModel,
         Erd_IceMakerFillInhibitedReason,
         &iceMakerFillInhibitedReason);
   }

   void WhenIceMakerFillInhibitedDueToRfidFilterIs(bool status)
   {
      IceMakerFillInhibitedReasonBitmap_t iceMakerFillInhibitedReason;
      DataModel_Read(
         dataModel,
         Erd_IceMakerFillInhibitedReason,
         &iceMakerFillInhibitedReason);

      if(status)
      {
         BITMAP_SET(&iceMakerFillInhibitedReason, IceMakerFillInhibitedReason_DueToRfidFilter);
      }
      else
      {
         BITMAP_CLEAR(&iceMakerFillInhibitedReason, IceMakerFillInhibitedReason_DueToRfidFilter);
      }

      DataModel_Write(
         dataModel,
         Erd_IceMakerFillInhibitedReason,
         &iceMakerFillInhibitedReason);
   }

   void IceMakerFullStatusShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_IceMaker0FullStatus, &actual);

      CHECK_EQUAL(expected, actual);
   }
};

TEST(TwistTrayIceMaker, ShouldHomeOnInitializationAndNotIncrementFreezerIceRateSignalIfIceMakerThermistorIsValid)
{
   GivenTheIceMakerThermistorIsValid();
   GivenTheIceMakerIsEnabled();

   TheMotorShouldBeRequestedTo(Home);
   GivenTheModuleIsInitialized();

   TheMotorShouldBeRequestedTo(Idle);
   FreezerTriggerIceRateSignalShouldNotIncrement();
   WhenTheMotorActionResultIs(Homed);
}

TEST(TwistTrayIceMaker, ShouldHomeWhenIceMakerThermistorIsInvalidAndBecomesValid)
{
   GivenTheIceMakerThermistorIsInvalid();
   GivenTheIceMakerIsEnabled();
   GivenTheModuleIsInitialized();

   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_ThermistorFault);
   TheMotorShouldBeRequestedTo(Home);

   TheMotorShouldBeRequestedTo(Idle);
   WhenTheIceMakerThermistorIsValid();
   WhenTheMotorActionResultIs(Homed);
}

TEST(TwistTrayIceMaker, ShouldNotTransitionToThermistorFaultStateWhenHomingAndThermistorBecomesInvalid)
{
   GivenTheIceMakerThermistorIsValid();
   GivenTheIceMakerIsEnabled();

   TheMotorShouldBeRequestedTo(Home);
   GivenTheModuleIsInitialized();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Homing);

   WhenTheIceMakerThermistorIsInvalid();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Homing);
}

// Freezing

TEST(TwistTrayIceMaker, ShouldSetHarvestCountCalculationRequestWhenEnteringIntoFreezeState)
{
   GivenTheOperationStateIsInFreeze();
   HarvestCountCalculationRequestShouldBe(SET);
}

TEST(TwistTrayIceMaker, ShouldClearHarvestCountCalculationRequestAndStartHarvestingIfThermistorTemperatureChangesToBelowMaximumHarvestTemperatureWhileHarvestCountIsReadyToHarvest)
{
   GivenTheOperationStateIsInFreeze();
   GivenFreezerIceRateActiveBecomes(SET);
   GivenTheTemperatureIs(iceMakerData->freezeData.maximumHarvestTemperatureInDegFx100);
   HarvestCountCalculationRequestShouldBe(SET);

   WhenHarvestCountIsReadyToHarvestIs(SET);

   HarvestingShouldStart();
   WhenTheTemperatureIs(iceMakerData->freezeData.maximumHarvestTemperatureInDegFx100 - 1);
   HarvestCountCalculationRequestShouldBe(CLEAR);
}

TEST(TwistTrayIceMaker, ShouldClearHarvestCountCalculationRequestAndStartHarvestingIfHarvestCountIsReadyToHarvestWhileThermistorTemperatureIsBelowMaximumHarvestTemperature)
{
   GivenTheOperationStateIsInFreeze();
   GivenFreezerIceRateActiveBecomes(SET);
   HarvestCountCalculationRequestShouldBe(SET);

   WhenTheTemperatureIs(iceMakerData->freezeData.maximumHarvestTemperatureInDegFx100 - 1);

   HarvestingShouldStart();
   WhenHarvestCountIsReadyToHarvestIs(SET);
   HarvestCountCalculationRequestShouldBe(CLEAR);
}

TEST(TwistTrayIceMaker, ShouldWaitUntilSabbathModeChangesFromEnabledToDisabledInFreezeStateBeforeClearingHarvestCountCalculationRequestAndStartingHarvestIfHarvestCountIsReadyToHarvestWhileThermistorTemperatureIsBelowMaximumHarvestTemperature)
{
   GivenTheOperationStateIsInFreeze();
   GivenFreezerIceRateActiveBecomes(SET);
   HarvestCountCalculationRequestShouldBe(SET);

   WhenTheTemperatureIs(iceMakerData->freezeData.maximumHarvestTemperatureInDegFx100 - 1);
   WhenSabbathModeIs(ENABLED);

   MotorRequestShouldNotBeCalled();
   WhenHarvestCountIsReadyToHarvestIs(SET);
   HarvestCountCalculationRequestShouldBe(SET);

   HarvestingShouldStart();
   WhenSabbathModeIs(DISABLED);
   HarvestCountCalculationRequestShouldBe(CLEAR);
}

TEST(TwistTrayIceMaker, ShouldWaitUntilIceMakerChangesFromDisabledToEnabledInFreezeStateBeforeClearingHarvestCountCalculationRequestAndStartingHarvestIfHarvestCountIsReadyToHarvestWhileThermistorTemperatureIsBelowMaximumHarvestTemperature)
{
   GivenTheOperationStateIsInFreeze();
   GivenFreezerIceRateActiveBecomes(SET);
   HarvestCountCalculationRequestShouldBe(SET);

   WhenTheTemperatureIs(iceMakerData->freezeData.maximumHarvestTemperatureInDegFx100 - 1);
   WhenTheIceMakerBecomesDisabled();

   MotorRequestShouldNotBeCalled();
   WhenHarvestCountIsReadyToHarvestIs(SET);
   HarvestCountCalculationRequestShouldBe(SET);

   HarvestingShouldStart();
   WhenTheIceMakerBecomesEnabled();
   HarvestCountCalculationRequestShouldBe(CLEAR);
}

TEST(TwistTrayIceMaker, ShouldWaitUntilCoolingSystemTurnsOnInFreezeStateBeforeClearingHarvestCountCalculationRequestAndStartingHarvestIfHarvestCountIsReadyToHarvestWhileThermistorTemperatureIsBelowMaximumHarvestTemperature)
{
   GivenTheOperationStateIsInFreeze();
   GivenFreezerIceRateActiveBecomes(SET);
   HarvestCountCalculationRequestShouldBe(SET);

   WhenTheTemperatureIs(iceMakerData->freezeData.maximumHarvestTemperatureInDegFx100 - 1);
   WhenTheCoolingSystemOffChangesTo(true);

   MotorRequestShouldNotBeCalled();
   WhenHarvestCountIsReadyToHarvestIs(SET);
   HarvestCountCalculationRequestShouldBe(SET);

   HarvestingShouldStart();
   WhenTheCoolingSystemOffChangesTo(false);
   HarvestCountCalculationRequestShouldBe(CLEAR);
}

TEST(TwistTrayIceMaker, ShouldWaitUntilIceMakerIsEnabledAndSabbathModeIsDisabledAndCoolingSystemIsOnWhileInFreezeStateBeforeClearingHarvestCountCalculationRequestAndStartingHarvestIfHarvestCountIsReadyToHarvestWhileThermistorTemperatureIsBelowMaximumHarvestTemperature)
{
   GivenTheOperationStateIsInFreeze();
   GivenFreezerIceRateActiveBecomes(SET);
   HarvestCountCalculationRequestShouldBe(SET);

   WhenTheTemperatureIs(iceMakerData->freezeData.maximumHarvestTemperatureInDegFx100 - 1);
   WhenTheIceMakerBecomesDisabled();
   WhenTheCoolingSystemOffChangesTo(true);

   MotorRequestShouldNotBeCalled();
   WhenHarvestCountIsReadyToHarvestIs(SET);
   HarvestCountCalculationRequestShouldBe(SET);

   WhenSabbathModeIs(ENABLED);
   HarvestCountCalculationRequestShouldBe(SET);

   WhenTheIceMakerBecomesEnabled();
   HarvestCountCalculationRequestShouldBe(SET);

   WhenTheCoolingSystemOffChangesTo(false);
   HarvestCountCalculationRequestShouldBe(SET);

   HarvestingShouldStart();
   WhenSabbathModeIs(DISABLED);
   HarvestCountCalculationRequestShouldBe(CLEAR);
}

TEST(TwistTrayIceMaker, ShouldWaitUntilIceMakerIsEnabledThenSabbathModeIsDisabledAndCoolingSystemIsOnWhileInFreezeStateBeforeClearingHarvestCountCalculationRequestAndStartingHarvestIfHarvestCountIsReadyToHarvestWhileThermistorTemperatureIsBelowMaximumHarvestTemperature)
{
   GivenTheOperationStateIsInFreeze();
   GivenFreezerIceRateActiveBecomes(SET);
   GivenTheTemperatureIs(iceMakerData->freezeData.maximumHarvestTemperatureInDegFx100);
   HarvestCountCalculationRequestShouldBe(SET);

   WhenTheTemperatureIs(iceMakerData->freezeData.maximumHarvestTemperatureInDegFx100 - 1);
   WhenSabbathModeIs(ENABLED);

   MotorRequestShouldNotBeCalled();
   WhenHarvestCountIsReadyToHarvestIs(SET);
   HarvestCountCalculationRequestShouldBe(SET);

   WhenTheIceMakerBecomesDisabled();
   HarvestCountCalculationRequestShouldBe(SET);

   WhenTheCoolingSystemOffChangesTo(true);
   HarvestCountCalculationRequestShouldBe(SET);

   WhenSabbathModeIs(DISABLED);
   HarvestCountCalculationRequestShouldBe(SET);

   HarvestingShouldStart();
   WhenTheIceMakerBecomesEnabled();
   WhenTheCoolingSystemOffChangesTo(false);
   HarvestCountCalculationRequestShouldBe(CLEAR);
}

TEST(TwistTrayIceMaker, ShouldWaitUntilHarvestCountIsReadyToHarvestBeforeTransitioningToStateHarvestWhenInStateFreezeAndSabbathModeChangesFromEnabledToDisabled)
{
   GivenTheOperationStateIsInFreeze();
   GivenFreezerIceRateActiveBecomes(SET);
   HarvestCountCalculationRequestShouldBe(SET);

   WhenTheTemperatureIs(iceMakerData->freezeData.maximumHarvestTemperatureInDegFx100 - 1);

   WhenSabbathModeIs(ENABLED);
   HarvestCountCalculationRequestShouldBe(SET);

   WhenSabbathModeIs(DISABLED);
   HarvestCountCalculationRequestShouldBe(SET);

   HarvestingShouldStart();
   WhenHarvestCountIsReadyToHarvestIs(SET);
   HarvestCountCalculationRequestShouldBe(CLEAR);
}

TEST(TwistTrayIceMaker, ShouldWaitUntilHarvestCountIsReadyToHarvestBeforeTransitioningToStateHarvestWhenInStateFreezeAndIceMakerChangesFromDisabledToEnabled)
{
   GivenTheOperationStateIsInFreeze();
   GivenFreezerIceRateActiveBecomes(SET);
   HarvestCountCalculationRequestShouldBe(SET);

   WhenTheTemperatureIs(iceMakerData->freezeData.maximumHarvestTemperatureInDegFx100 - 1);

   WhenTheIceMakerBecomesDisabled();
   HarvestCountCalculationRequestShouldBe(SET);

   WhenTheIceMakerBecomesEnabled();
   HarvestCountCalculationRequestShouldBe(SET);

   HarvestingShouldStart();
   WhenHarvestCountIsReadyToHarvestIs(SET);
   HarvestCountCalculationRequestShouldBe(CLEAR);
}

TEST(TwistTrayIceMaker, ShouldWaitUntilFreezerDoorHarvestDelayHasElapsedBeforeTransitioningToHarvestStateWhileTheOtherConditionsAreMet)
{
   GivenTheOperationStateIsInFreeze();
   GivenFreezerIceRateActiveBecomes(SET);
   WhenTheFreezerDoorIs(OPEN);

   WhenTheTemperatureIs(iceMakerData->freezeData.maximumHarvestTemperatureInDegFx100 - 1);

   NothingShouldHappen();
   WhenHarvestCountIsReadyToHarvestIs(SET);

   After(iceMakerData->harvestData.delayToHarvestAfterDoorOpensMinutes * MSEC_PER_MIN - 1);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Freeze);

   HarvestingShouldStart();
   After(1);
}

TEST(TwistTrayIceMaker, ShouldNotDelayHarvestAgainIfFreezerDoorBecomesClosedWhileHarvestDoorDelayIsOccurringFromOpenFreezerDoor)
{
   GivenTheOperationStateIsInFreeze();
   GivenFreezerIceRateActiveBecomes(SET);
   WhenTheFreezerDoorIs(OPEN);

   WhenTheTemperatureIs(iceMakerData->freezeData.maximumHarvestTemperatureInDegFx100 - 1);

   NothingShouldHappen();
   WhenHarvestCountIsReadyToHarvestIs(SET);

   After(iceMakerData->harvestData.delayToHarvestAfterDoorOpensMinutes * MSEC_PER_MIN - 1);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Freeze);

   WhenTheFreezerDoorIs(CLOSED);

   HarvestingShouldStart();
   After(1);
}

TEST(TwistTrayIceMaker, ShouldClearHarvestCountCalculationRequestAndTransitionToThermistorFaultWhenThermistorIsInvalidWhileFreezing)
{
   GivenTheOperationStateIsInFreeze();
   HarvestCountCalculationRequestShouldBe(SET);

   WhenTheIceMakerThermistorIsInvalid();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_ThermistorFault);
   HarvestCountCalculationRequestShouldBe(CLEAR);
}

TEST(TwistTrayIceMaker, ShouldNotClearHarvestCountCalculationRequestWhenIceMakerBecomesDisabledAndThenEnabled)
{
   GivenTheIceMakerIsEnabled();
   GivenTheTemperatureIs(VeryCold);
   GivenHomingIsCompleted();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Freeze);
   HarvestCountCalculationRequestShouldBe(SET);

   WhenTheIceMakerBecomesDisabled();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Freeze);
   HarvestCountCalculationRequestShouldBe(SET);

   FreezerTriggerIceRateSignalShouldIncrement();
   WhenTheIceMakerBecomesEnabled();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Freeze);
   HarvestCountCalculationRequestShouldBe(SET);
}

TEST(TwistTrayIceMaker, ShouldTriggerFreezerIceRateAgainIfItCompletesWhileInStateFreeze)
{
   GivenTheOperationStateIsInFreeze();
   GivenFreezerIceRateActiveBecomes(SET);

   FreezerTriggerIceRateSignalShouldIncrement();
   WhenFreezerIceRateActiveBecomes(CLEAR);
}

TEST(TwistTrayIceMaker, ShouldNotTriggerFreezerIceRateAgainIfItCompletesWhileInStateFreezeButSabbathModeIsActive)
{
   GivenTheOperationStateIsInFreeze();
   GivenFreezerIceRateActiveBecomes(SET);
   GivenSabbathModeIs(ON);

   FreezerTriggerIceRateSignalShouldNotIncrement();
   WhenFreezerIceRateActiveBecomes(CLEAR);
}

TEST(TwistTrayIceMaker, ShouldNotTriggerFreezerIceRateAgainIfItCompletesWhileInStateFreezeButIceMakerIsDisabled)
{
   GivenTheOperationStateIsInFreeze();
   GivenFreezerIceRateActiveBecomes(SET);
   GivenTheIceMakerIsDisabled();

   FreezerTriggerIceRateSignalShouldNotIncrement();
   WhenFreezerIceRateActiveBecomes(CLEAR);
}

TEST(TwistTrayIceMaker, ShouldNotTriggerFreezerIceRateAgainWhenSabbathModeBecomesDisabledButIceRateIsAlreadyActive)
{
   GivenTheIceMakerIsEnabled();
   GivenTheTemperatureIs(VeryCold);
   GivenHomingIsCompleted();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Freeze);

   WhenFreezerIceRateActiveBecomes(SET);
   WhenSabbathModeIs(ENABLED);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Freeze);

   FreezerTriggerIceRateSignalShouldNotIncrement();
   WhenSabbathModeIs(DISABLED);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Freeze);
}

TEST(TwistTrayIceMaker, ShouldNotTriggerFreezerIceRateAgainWhenIceMakerBecomesEnabledButIceRateIsAlreadyActive)
{
   GivenTheIceMakerIsEnabled();
   GivenTheTemperatureIs(VeryCold);
   GivenHomingIsCompleted();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Freeze);

   WhenFreezerIceRateActiveBecomes(SET);
   WhenTheIceMakerBecomesDisabled();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Freeze);

   FreezerTriggerIceRateSignalShouldNotIncrement();
   WhenTheIceMakerBecomesEnabled();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Freeze);
}

TEST(TwistTrayIceMaker, ShouldStayInFreezeWhenTheOtherConditionsAreMetWhileDispensingIsInhibitedByRfid)
{
   GivenTheIceMakerIsEnabled();
   GivenTheTemperatureIs(iceMakerData->freezeData.maximumHarvestTemperatureInDegFx100 - 1);
   GivenHomingIsCompleted();
   GivenDispensingIsInhibitedByRfid();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Freeze);

   FreezerTriggerIceRateSignalShouldIncrement();
   WhenHarvestCountIsReadyToHarvestIs(SET);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Freeze);
}

TEST(TwistTrayIceMaker, ShouldTransitionToHarvestWhenDispensingIsNotInhibitedByRfidWhileTheOtherConditionsAreMet)
{
   GivenTheIceMakerIsEnabled();
   GivenTheTemperatureIs(iceMakerData->freezeData.maximumHarvestTemperatureInDegFx100 - 1);
   GivenHarvestCountIsReadyToHarvestIs(SET);
   GivenDispensingIsInhibitedByRfid();
   GivenHomingIsCompleted();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Freeze);

   HarvestingShouldStart();
   WhenDispensingIsNotInhibitedByRfid();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Harvesting);
}

TEST(TwistTrayIceMaker, ShouldStayInFreezeWhenDispensingIsInhibitedByRfidWhileTheOtherConditionsAreMet)
{
   GivenTheIceMakerIsEnabled();
   GivenTheTemperatureIs(iceMakerData->freezeData.maximumHarvestTemperatureInDegFx100 - 1);
   GivenHarvestCountIsReadyToHarvestIs(SET);
   GivenDispensingIsNotInhibitedByRfid();
   GivenHomingIsCompleted();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Freeze);

   WhenDispensingIsInhibitedByRfid();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Freeze);
}

// Harvesting

TEST(TwistTrayIceMaker, ShouldTryToHarvestWhateverIsInTheTrayOnInitAfterTryingToFreezeIt)
{
   GivenTheIceMakerIsEnabled();
   GivenFreezingIsCompletedAndHarvestingIsStarted();

   TheMotorShouldBeRequestedTo(Idle);
   FillTubeHeaterVoteAndCareShouldBecome(OFF, Vote_DontCare);
   FillingShouldStart();
   AfterTheFillTubeHeaterTimerHasExpired();
   WhenTheMotorActionResultIs(Harvested);
}

TEST(TwistTrayIceMaker, ShouldTryToHarvestIceAgainAfterEnoughTimeHasPassedSinceTheBucketWasFull)
{
   GivenTheIceMakerIsEnabled();
   GivenFreezingIsCompletedAndHarvestingIsStarted();

   TheMotorShouldBeRequestedTo(Idle);
   FillTubeHeaterVoteAndCareShouldBecome(OFF, Vote_DontCare);
   WhenTheMotorActionResultIs(BucketWasFull);

   NothingShouldHappen();
   After(iceMakerData->harvestData.fullBucketWaitPeriodMinutes * MSEC_PER_MIN - 1);

   HarvestingShouldStart();
   After(1);
}

TEST(TwistTrayIceMaker, ShouldTransitionFromHarvestToFreezeWhenBucketIsFullAndIceMakerIsDisabled)
{
   GivenTheIceMakerIsEnabled();
   GivenFreezingIsCompletedAndHarvestingIsStarted();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Harvesting);

   WhenTheIceMakerBecomesDisabled();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Harvesting);

   TheMotorShouldBeRequestedTo(Idle);
   FillTubeHeaterVoteAndCareShouldBecome(OFF, Vote_DontCare);
   WhenTheMotorActionResultIs(BucketWasFull);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Freeze);
}

TEST(TwistTrayIceMaker, ShouldTryToHarvestIceAfterIceHasBeenDispensedLongEnough)
{
   GivenTheIceMakerIsEnabled();
   GivenFreezingIsCompletedAndHarvestingIsStarted();
   TheMotorShouldBeRequestedTo(Idle);
   FillTubeHeaterVoteAndCareShouldBecome(OFF, Vote_DontCare);
   WhenTheMotorActionResultIs(BucketWasFull);

   WhenIceStartsDispensing();

   After((iceMakerData->harvestData.fullBucketDispenseCheckTimeInSeconds * MSEC_PER_SEC) - 1);
   NothingShouldHappen();

   After(1);

   HarvestingShouldStart();
   WhenIceStopsDispensing();
}

TEST(TwistTrayIceMaker, ShouldNotTryToHarvestIceAfterIceHasNotBeenDispensedForLongEnough)
{
   GivenTheIceMakerIsEnabled();
   GivenFreezingIsCompletedAndHarvestingIsStarted();
   TheMotorShouldBeRequestedTo(Idle);
   FillTubeHeaterVoteAndCareShouldBecome(OFF, Vote_DontCare);
   WhenTheMotorActionResultIs(BucketWasFull);

   WhenIceStartsDispensing();

   After((iceMakerData->harvestData.fullBucketDispenseCheckTimeInSeconds * MSEC_PER_SEC) - 1);
   NothingShouldHappen();

   NothingShouldHappen();
   WhenIceStopsDispensing();
}

TEST(TwistTrayIceMaker, ShouldNotTryToHarvestIceAfterWaterHasBeenDispensedInsteadOfIce)
{
   GivenTheIceMakerIsEnabled();
   GivenFreezingIsCompletedAndHarvestingIsStarted();
   TheMotorShouldBeRequestedTo(Idle);
   FillTubeHeaterVoteAndCareShouldBecome(OFF, Vote_DontCare);
   WhenTheMotorActionResultIs(BucketWasFull);

   WhenWaterStartsDispensing();

   After((iceMakerData->harvestData.fullBucketDispenseCheckTimeInSeconds * MSEC_PER_SEC) - 1);
   NothingShouldHappen();

   After(1);

   NothingShouldHappen();
   WhenWaterStopsDispensing();
}

TEST(TwistTrayIceMaker, ShouldFillTheTrayWithWaterAfterHarvestingAndIncrementFreezerIceRateSignal)
{
   GivenTheIceMakerIsEnabled();
   GivenFillingHasStartedAfterCompletingFreezingAndHarvesting();

   TheWaterValveShouldBecome(CLOSED);
   FreezerTriggerIceRateSignalShouldIncrement();
   WhenTwistTrayIceMakerTrayIsFilled();
}

TEST(TwistTrayIceMaker, ShouldFreezeAfterFillingAndIncrementFreezerIceRateSignal)
{
   GivenTheIceMakerIsEnabled();
   GivenFillingHasStartedAfterCompletingFreezingAndHarvesting();

   TheWaterValveShouldBecome(CLOSED);
   FreezerTriggerIceRateSignalShouldIncrement();
   WhenTwistTrayIceMakerTrayIsFilled();
}

TEST(TwistTrayIceMaker, ShouldGoToThermistorFaultStateWhenEnteringFreezeStateIfThermistorIsInvalid)
{
   GivenTheIceMakerIsEnabled();
   GivenFillingHasStartedAfterCompletingFreezingAndHarvesting();

   WhenTheIceMakerThermistorIsInvalid();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);

   TheWaterValveShouldBecome(CLOSED);
   WhenTwistTrayIceMakerTrayIsFilled();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_ThermistorFault);
}

TEST(TwistTrayIceMaker, ShouldStartHarvestCountCalculationRequestButNotTriggerFreezerIceRateWhenEnteringFreezeStateIfIceMakerIsDisabledAndThenTriggerFreezerIceRateWhenIceMakerIsEnabled)
{
   GivenTheIceMakerIsEnabled();
   GivenFillingHasStartedAfterCompletingFreezingAndHarvesting();

   WhenTheIceMakerBecomesDisabled();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);

   TheWaterValveShouldBecome(CLOSED);
   FreezerTriggerIceRateSignalShouldNotIncrement();
   WhenTwistTrayIceMakerTrayIsFilled();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Freeze);
   HarvestCountCalculationRequestShouldBe(SET);

   WhenTheTemperatureIs(iceMakerData->freezeData.maximumHarvestTemperatureInDegFx100);

   FreezerTriggerIceRateSignalShouldIncrement();
   WhenTheIceMakerBecomesEnabled();
}

TEST(TwistTrayIceMaker, ShouldNotTriggerFreezerIceRateWhenInFreezeStateAndIceMakerBecomesEnabledButHarvestConditionsAreAlreadyMet)
{
   GivenTheIceMakerIsEnabled();
   GivenFillingHasStartedAfterCompletingFreezingAndHarvesting();

   WhenTheIceMakerBecomesDisabled();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);

   TheWaterValveShouldBecome(CLOSED);
   FreezerTriggerIceRateSignalShouldNotIncrement();
   WhenTwistTrayIceMakerTrayIsFilled();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Freeze);
   HarvestCountCalculationRequestShouldBe(SET);

   HarvestingShouldStart();
   FreezerTriggerIceRateSignalShouldNotIncrement();
   WhenTheIceMakerBecomesEnabled();
}

TEST(TwistTrayIceMaker, ShouldTransitionToFreezeStateAndStartHarvestCountCalculationRequestButNotTriggerFreezerIceRateAfterFillingIsFinishedWhileSabbathIsEnabledAndThenTriggerFreezerIceRateWhenSabbathIsDisabled)
{
   GivenTheIceMakerIsEnabled();
   GivenFillingHasStartedAfterCompletingFreezingAndHarvesting();

   WhenSabbathModeIs(ENABLED);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);

   TheWaterValveShouldBecome(CLOSED);
   FreezerTriggerIceRateSignalShouldNotIncrement();
   WhenTwistTrayIceMakerTrayIsFilled();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Freeze);
   HarvestCountCalculationRequestShouldBe(SET);

   WhenTheTemperatureIs(iceMakerData->freezeData.maximumHarvestTemperatureInDegFx100);

   FreezerTriggerIceRateSignalShouldIncrement();
   WhenSabbathModeIs(DISABLED);
}

TEST(TwistTrayIceMaker, ShouldNotTriggerFreezerIceRateWhenInFreezeStateAndSabbathBecomesDisabledButHarvestConditionsAreAlreadyMet)
{
   GivenTheIceMakerIsEnabled();
   GivenFillingHasStartedAfterCompletingFreezingAndHarvesting();

   WhenSabbathModeIs(ENABLED);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);

   TheWaterValveShouldBecome(CLOSED);
   FreezerTriggerIceRateSignalShouldNotIncrement();
   WhenTwistTrayIceMakerTrayIsFilled();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Freeze);
   HarvestCountCalculationRequestShouldBe(SET);

   HarvestingShouldStart();
   FreezerTriggerIceRateSignalShouldNotIncrement();
   WhenSabbathModeIs(DISABLED);
}

TEST(TwistTrayIceMaker, ShouldBeAbleToHarvestTwice)
{
   GivenTheIceMakerIsEnabled();
   GivenFillingHasStartedAfterCompletingFreezingAndHarvesting();

   TheWaterValveShouldBecome(CLOSED);
   FreezerTriggerIceRateSignalShouldIncrement();
   WhenTwistTrayIceMakerTrayIsFilled();

   HarvestingShouldStart();
   WhenTheTemperatureIs(VeryCold);
   WhenHarvestCountIsReadyToHarvestIs(SET);
}

TEST(TwistTrayIceMaker, ShouldNotTransitionToThermistorFaultStateWhenThermistorBecomesInvalidDuringTheHarvestState)
{
   GivenTheIceMakerIsEnabled();
   GivenFreezingIsCompletedAndHarvestingIsStarted();

   WhenTheIceMakerThermistorIsInvalid();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Harvesting);
}

TEST(TwistTrayIceMaker, ShouldTransitionToThermistorFaultStateWhenInBucketIsFullStateAndThermistorBecomesInvalid)
{
   GivenTheIceMakerIsEnabled();
   GivenFreezingIsCompletedAndHarvestingIsStarted();

   TheMotorShouldBeRequestedTo(Idle);
   FillTubeHeaterVoteAndCareShouldBecome(OFF, Vote_DontCare);
   WhenTheMotorActionResultIs(BucketWasFull);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_BucketIsFull);

   WhenTheIceMakerThermistorIsInvalid();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_ThermistorFault);
}

TEST(TwistTrayIceMaker, ShouldTransitionToFreezeStateWhenIceMakerBecomesDisabledWhileInBucketIsFullState)
{
   GivenTheIceMakerIsEnabled();
   GivenFreezingIsCompletedAndHarvestingIsStarted();
   TheMotorShouldBeRequestedTo(Idle);
   FillTubeHeaterVoteAndCareShouldBecome(OFF, Vote_DontCare);
   WhenTheMotorActionResultIs(BucketWasFull);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_BucketIsFull);

   WhenTheIceMakerBecomesDisabled();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Freeze);
}

TEST(TwistTrayIceMaker, ShouldTransitionToFreezeStateWhenThermistorFilteredTemperatureIsGreaterThanFullToFreezeStateTemperatureAndInBucketFullState)
{
   GivenTheOperationStateIsInBucketIsFull();

   FreezerTriggerIceRateSignalShouldIncrement();
   WhenTheTemperatureIs((iceMakerData->harvestData.fullBucketToFreezeStateTemperatureInDegFx100) + 1);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Freeze);
}

TEST(TwistTrayIceMaker, ShouldStayInBucketFullStateWhenThermistorFilteredTemperatureIsLessThanOrEqualToFullToFreezeStateTemperature)
{
   GivenTheOperationStateIsInBucketIsFull();

   WhenTheTemperatureIs((iceMakerData->harvestData.fullBucketToFreezeStateTemperatureInDegFx100) - 1);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_BucketIsFull);

   WhenTheTemperatureIs(iceMakerData->harvestData.fullBucketToFreezeStateTemperatureInDegFx100);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_BucketIsFull);
}

TEST(TwistTrayIceMaker, ShouldNotTransitionToThermistorFaultStateWhenFillingAndThermistorBecomesInvalid)
{
   GivenTheIceMakerIsEnabled();
   GivenTheIceMakerThermistorIsValid();
   GivenFillingHasStartedAfterCompletingFreezingAndHarvesting();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);

   WhenTheIceMakerThermistorIsInvalid();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);
}

TEST(TwistTrayIceMaker, ShouldNotTransitionToFreezeStateWhenIceMakerBecomesDisabledWhileFillingTrayWithWater)
{
   GivenTheIceMakerIsEnabled();
   GivenTheIceMakerThermistorIsValid();
   GivenFillingHasStartedAfterCompletingFreezingAndHarvesting();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);

   WhenTheIceMakerBecomesDisabled();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);
}

// SABBATH MODE

TEST(TwistTrayIceMaker, ShouldNotHarvestIceIfSabbathModeComesOnWhileFreezing)
{
   GivenTheIceMakerIsEnabled();
   GivenHomingIsCompleted();
   GivenFreezerIceRateActiveBecomes(SET);
   GivenTheTemperatureIs(VeryCold);
   GivenSabbathModeIs(ON);

   NothingShouldHappen();
   WhenHarvestCountIsReadyToHarvestIs(SET);
   After((iceMakerData->freezeData.harvestCountCalculatorData->minimumFreezeTimeInMinutes * MSEC_PER_MIN) + 1);

   HarvestingShouldStart();
   WhenSabbathModeIs(DISABLED);
}

TEST(TwistTrayIceMaker, SabbathModeShouldAlsoWorkIfTheEnhancedSabbathModeErdComesOn)
{
   GivenTheIceMakerIsEnabled();
   GivenHomingIsCompleted();
   GivenFreezerIceRateActiveBecomes(SET);
   GivenTheTemperatureIs(VeryCold);
   GivenEnhancedSabbathModeIs(ON);

   NothingShouldHappen();
   WhenHarvestCountIsReadyToHarvestIs(SET);
   After((iceMakerData->freezeData.harvestCountCalculatorData->minimumFreezeTimeInMinutes * MSEC_PER_MIN) + 1);

   HarvestingShouldStart();
   WhenEnhancedSabbathModeIs(OFF);
}

TEST(TwistTrayIceMaker, SabbathModeShouldAlsoWorkIfBothSabbathErdsComeOn)
{
   GivenTheIceMakerIsEnabled();
   GivenHomingIsCompleted();
   GivenFreezerIceRateActiveBecomes(SET);
   GivenTheTemperatureIs(VeryCold);
   GivenEnhancedSabbathModeIs(ON);
   GivenSabbathModeIs(ON);

   NothingShouldHappen();
   WhenHarvestCountIsReadyToHarvestIs(SET);
   After((iceMakerData->freezeData.harvestCountCalculatorData->minimumFreezeTimeInMinutes * MSEC_PER_MIN) + 1);

   WhenSabbathModeIs(DISABLED);

   NothingShouldHappen();
   After((iceMakerData->freezeData.harvestCountCalculatorData->minimumFreezeTimeInMinutes * MSEC_PER_MIN) + 1);

   HarvestingShouldStart();
   WhenEnhancedSabbathModeIs(OFF);
}

TEST(TwistTrayIceMaker, ShouldTransitionToFillAndStartFillingWhenMotorActionResultIsHarvestedWhileIceMakerStateIsInHarvestingAndSabbathIsEnabled)
{
   GivenTheIceMakerIsEnabled();
   GivenFreezingIsCompletedAndHarvestingIsStarted();

   GivenSabbathModeIs(ON);

   FillTubeHeaterVoteAndCareShouldBecome(OFF, Vote_DontCare);
   AfterTheFillTubeHeaterTimerHasExpired();

   TheMotorShouldBeRequestedTo(Idle);
   FillingShouldStart();
   WhenTheMotorActionResultIs(Harvested);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);
}

TEST(TwistTrayIceMaker, ShouldNotClearHarvestCountCalculationRequestWhenSabbathModeBecomesEnabledAndThenDisabled)
{
   GivenTheIceMakerIsEnabled();
   GivenTheTemperatureIs(VeryCold);
   GivenHomingIsCompleted();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Freeze);
   HarvestCountCalculationRequestShouldBe(SET);

   WhenSabbathModeIs(ENABLED);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Freeze);
   HarvestCountCalculationRequestShouldBe(SET);

   FreezerTriggerIceRateSignalShouldIncrement();
   WhenSabbathModeIs(DISABLED);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Freeze);
   HarvestCountCalculationRequestShouldBe(SET);
}

TEST(TwistTrayIceMaker, ShouldTransitionToFreezeWhenSabbathModeBecomesOnWhileBucketIsFull)
{
   GivenTheOperationStateIsInBucketIsFull();
   WhenSabbathModeIs(ENABLED);

   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Freeze);
}

TEST(TwistTrayIceMaker, ShouldSetIceMakerFullStatusWhenMotorActionResultIsBucketWasFullWhileInHarvesting)
{
   GivenTheOperationStateIsInHarvesting();
   IceMakerFullStatusShouldBe(CLEAR);

   FillTubeHeaterVoteAndCareShouldBecome(OFF, Vote_DontCare);
   TheMotorShouldBeRequestedTo(Idle);
   WhenTheMotorActionResultIs(BucketWasFull);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_BucketIsFull);
   IceMakerFullStatusShouldBe(SET);
}

TEST(TwistTrayIceMaker, ShouldNotClearIceMakerFullStatusWhenTransitioningToFreezeWithSabbathModeOnWhileBucketIsFull)
{
   GivenTheOperationStateIsInBucketIsFull();
   IceMakerFullStatusShouldBe(SET);

   WhenSabbathModeIs(ENABLED);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Freeze);
   IceMakerFullStatusShouldBe(SET);
}

TEST(TwistTrayIceMaker, ShouldNotClearIceMakerFullStatusWhenTransitioningToFreezeWithFullBucketToFreezeStateTemperatureChangeWhileBucketIsFull)
{
   GivenTheOperationStateIsInBucketIsFull();
   IceMakerFullStatusShouldBe(SET);

   FreezerTriggerIceRateSignalShouldIncrement();
   WhenTheTemperatureIs((iceMakerData->harvestData.fullBucketToFreezeStateTemperatureInDegFx100) + 1);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Freeze);
   IceMakerFullStatusShouldBe(SET);
}

TEST(TwistTrayIceMaker, ShouldClearIceMakerFullStatusWhenMotorActionIsSuccessfullyHarvestedWhileIceMakerFullStatusIsSet)
{
   GivenTheOperationStateIsInBucketIsFull();
   IceMakerFullStatusShouldBe(SET);

   HarvestingShouldStart();
   After(iceMakerData->harvestData.fullBucketWaitPeriodMinutes * MSEC_PER_MIN);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Harvesting);

   WhenTheMotorActionResultIs(Harvested);
   IceMakerFullStatusShouldBe(CLEAR);
}

// MOTOR ERRORS

TEST(TwistTrayIceMaker, ShouldGoToErrorStateIfAHomingMovementResultsInError)
{
   GivenTheIceMakerIsEnabled();
   GivenTheIceMakerThermistorIsValid();

   TheMotorShouldBeRequestedTo(Home);
   GivenTheModuleIsInitialized();

   TheMotorShouldBeRequestedTo(Idle);
   TheHighLevelStateShouldBecome(FaultState);
   TheMotorFaultShouldBecome(ACTIVE);
   WhenTheMotorActionResultIs(MotorError);
}

TEST(TwistTrayIceMaker, ShouldGoToErrorStateIfAHomingMovementResultsInErrorAndIceMakerIsDisabled)
{
   GivenTheIceMakerIsEnabled();
   GivenTheIceMakerThermistorIsValid();

   TheMotorShouldBeRequestedTo(Home);
   GivenTheModuleIsInitialized();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Homing);

   WhenTheIceMakerBecomesDisabled();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Homing);

   TheMotorShouldBeRequestedTo(Idle);
   TheHighLevelStateShouldBecome(FaultState);
   TheMotorFaultShouldBecome(ACTIVE);
   WhenTheMotorActionResultIs(MotorError);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_MotorError);
}

TEST(TwistTrayIceMaker, ShouldGoToErrorStateIfAHarvestMovementResultsInErrorAndIceMakerIsDisabled)
{
   GivenTheIceMakerIsEnabled();
   GivenFreezingIsCompletedAndHarvestingIsStarted();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Harvesting);

   WhenTheIceMakerBecomesDisabled();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Harvesting);

   TheMotorShouldBeRequestedTo(Idle);
   FillTubeHeaterVoteAndCareShouldBecome(OFF, Vote_DontCare);
   TheHighLevelStateShouldBecome(FaultState);
   TheMotorFaultShouldBecome(ACTIVE);
   WhenTheMotorActionResultIs(MotorError);
}

TEST(TwistTrayIceMaker, ShouldGoToErrorStateIfAHarvestMovementResultsInError)
{
   GivenTheIceMakerIsEnabled();
   GivenFreezingIsCompletedAndHarvestingIsStarted();

   TheMotorShouldBeRequestedTo(Idle);
   FillTubeHeaterVoteAndCareShouldBecome(OFF, Vote_DontCare);
   TheHighLevelStateShouldBecome(FaultState);
   TheMotorFaultShouldBecome(ACTIVE);
   WhenTheMotorActionResultIs(MotorError);
}

TEST(TwistTrayIceMaker, ShouldNotTransitionToThermistorFaultStateWhenThermistorBecomesInvalidFromTheMotorErrorState)
{
   GivenTheIceMakerIsEnabled();
   GivenFreezingIsCompletedAndHarvestingIsStarted();

   TheMotorShouldBeRequestedTo(Idle);
   FillTubeHeaterVoteAndCareShouldBecome(OFF, Vote_DontCare);
   TheHighLevelStateShouldBecome(FaultState);
   TheMotorFaultShouldBecome(ACTIVE);
   WhenTheMotorActionResultIs(MotorError);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_MotorError);

   WhenTheIceMakerThermistorIsInvalid();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_MotorError);
}

TEST(TwistTrayIceMaker, ShouldNotTransitionToFillingTrayWithWaterAndClearTestRequestWhenTestRequestIsFillWhileIceMakerThermistorIsInvalid)
{
   GivenTheIceMakerThermistorIsInvalid();
   GivenTheModuleIsInitialized();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_ThermistorFault);

   WhenTheTestRequestIs(IceMakerTestRequest_Fill);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_ThermistorFault);
   And TheTestRequestShouldBe(IceMakerTestRequest_None);
}

TEST(TwistTrayIceMaker, ShouldNotTransitionToHarvestingAndClearTestRequestWhenTestRequestIsHarvestWhileIceMakerThermistorIsInvalid)
{
   GivenTheIceMakerThermistorIsInvalid();
   GivenTheModuleIsInitialized();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_ThermistorFault);

   WhenTheTestRequestIs(IceMakerTestRequest_Harvest);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_ThermistorFault);
   And TheTestRequestShouldBe(IceMakerTestRequest_None);
}

TEST(TwistTrayIceMaker, ShouldTransitionToFillingTrayWithWaterAndClearTestRequestWhenTestRequestIsFillWhileIceMakerStateIsInFreeze)
{
   GivenTheOperationStateIsInFreeze();
   HarvestCountCalculationRequestShouldBe(SET);

   FillingShouldStart();
   WhenTheTestRequestIs(IceMakerTestRequest_Fill);
   HarvestCountCalculationRequestShouldBe(CLEAR);
   And TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);
   And TheTestRequestShouldBe(IceMakerTestRequest_None);
}

TEST(TwistTrayIceMaker, ShouldTransitionToHarvestingAndClearTestRequestWhenTestRequestIsHarvestWhileIceMakerStateIsInFreeze)
{
   GivenTheOperationStateIsInFreeze();
   HarvestCountCalculationRequestShouldBe(SET);

   HarvestingShouldStart();
   WhenTheTestRequestIs(IceMakerTestRequest_Harvest);
   HarvestCountCalculationRequestShouldBe(CLEAR);
   And TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Harvesting);
   And TheTestRequestShouldBe(IceMakerTestRequest_None);
}

TEST(TwistTrayIceMaker, ShouldStayInHarvestAndClearTestRequestWhenTestRequestIsHarvestWhileIceMakerStateIsInHarvesting)
{
   GivenTheOperationStateIsInHarvesting();

   WhenTheTestRequestIs(IceMakerTestRequest_Harvest);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Harvesting);
   And TheTestRequestShouldBe(IceMakerTestRequest_None);
}

TEST(TwistTrayIceMaker, ShouldStayInFillingTrayWithWaterAndClearTestRequestWhenTestRequestIsHarvestWhileIceMakerStateIsInFillingTrayWithWater)
{
   GivenTheOperationStateIsInFillingTrayWithWater();

   WhenTheTestRequestIs(IceMakerTestRequest_Fill);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);
   And TheTestRequestShouldBe(IceMakerTestRequest_None);
}

TEST(TwistTrayIceMaker, ShouldTransitionToHarvestingAndClearTestRequestWhenTestRequestIsHarvestWhileIceMakerStateIsInFillingTrayWithWater)
{
   GivenTheOperationStateIsInFillingTrayWithWater();

   TheWaterValveShouldBecome(CLOSED);
   FillTubeHeaterVoteAndCareShouldBecome(iceMakerData->harvestData.fillTubeHeaterDutyCyclePercentage, Vote_Care);
   TheMotorShouldBeRequestedTo(Harvest);
   WhenTheTestRequestIs(IceMakerTestRequest_Harvest);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Harvesting);
   And TheTestRequestShouldBe(IceMakerTestRequest_None);
}

TEST(TwistTrayIceMaker, ShouldTransitionToFillingTrayWithWaterAndClearTestRequestWhenTestRequestIsFillWhileIceMakerStateIsInBucketIsFull)
{
   GivenTheOperationStateIsInBucketIsFull();

   FillingShouldStart();
   WhenTheTestRequestIs(IceMakerTestRequest_Fill);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);
   And TheTestRequestShouldBe(IceMakerTestRequest_None);
}

TEST(TwistTrayIceMaker, ShouldTransitionToHarvestingAndClearTestRequestWhenTestRequestIsHarvestWhileIceMakerStateIsInBucketIsFull)
{
   GivenTheOperationStateIsInBucketIsFull();

   TheMotorShouldBeRequestedTo(Harvest);
   FillTubeHeaterVoteAndCareShouldBecome(iceMakerData->harvestData.fillTubeHeaterDutyCyclePercentage, Vote_Care);
   WhenTheTestRequestIs(IceMakerTestRequest_Harvest);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Harvesting);
   And TheTestRequestShouldBe(IceMakerTestRequest_None);
}

TEST(TwistTrayIceMaker, ShouldTransitionToFillingTrayWithWaterAndClearTestRequestWhenTestRequestIsFillWhileIceMakerStateIsInMotorError)
{
   GivenTheOperationStateIsInMotorError();

   TheWaterValveShouldBecome(OPEN);
   WhenTheTestRequestIs(IceMakerTestRequest_Fill);
   And TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);
   And TheTestRequestShouldBe(IceMakerTestRequest_None);
}

TEST(TwistTrayIceMaker, ShouldTransitionToHarvestingAndClearTestRequestWhenTestRequestIsHarvestWhileIceMakerStateIsInMotorError)
{
   GivenTheOperationStateIsInMotorError();

   TheMotorShouldBeRequestedTo(Harvest);
   FillTubeHeaterVoteAndCareShouldBecome(iceMakerData->harvestData.fillTubeHeaterDutyCyclePercentage, Vote_Care);
   WhenTheTestRequestIs(IceMakerTestRequest_Harvest);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Harvesting);
   And TheTestRequestShouldBe(IceMakerTestRequest_None);
}

TEST(TwistTrayIceMaker, ShouldRetryInitalizingMotor)
{
   GivenTheIceMakerIsInStateMotorError();

   TheMotorShouldBeRequestedTo(Home);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_MotorError);

   After(((instance._private.parametric->harvestData.motorErrorRetryInitializeMinutes) * MSEC_PER_MIN) - 1);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_MotorError);

   After(1);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Homing);
}

// FILL TUBE HEATER

TEST(TwistTrayIceMaker, iceFormationIsActiveForFillTubeHeaterOnTime)
{
   GivenTheIceMakerIsEnabled();
   GivenFreezingIsCompletedAndHarvestingIsStarted();
   FillTubeHeaterVoteAndCareShouldBe(iceMakerData->harvestData.fillTubeHeaterDutyCyclePercentage, Vote_Care);

   After(iceMakerData->harvestData.fillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);
   FillTubeHeaterVoteAndCareShouldBe(iceMakerData->harvestData.fillTubeHeaterDutyCyclePercentage, Vote_Care);

   FillTubeHeaterVoteAndCareShouldBecome(OFF, Vote_DontCare);
   After(1);
}

TEST(TwistTrayIceMaker, ShouldTurnOffFillTubeHeaterWhenTheMotorActionResultIsBucketWasFull)
{
   GivenTheIceMakerIsEnabled();
   GivenFreezingIsCompletedAndHarvestingIsStarted();

   TheMotorShouldBeRequestedTo(Idle);
   FillTubeHeaterVoteAndCareShouldBecome(OFF, Vote_DontCare);
   WhenTheMotorActionResultIs(BucketWasFull);
}

TEST(TwistTrayIceMaker, ShouldNotBeginFillingTrayUntilFillTuberHeaterTimerExpires)
{
   GivenTheIceMakerIsEnabled();
   GivenFreezingIsCompletedAndHarvestingIsStarted();

   FillTubeHeaterVoteAndCareShouldBecome(OFF, Vote_DontCare);
   AfterTheFillTubeHeaterTimerHasExpired();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Harvesting);

   TheMotorShouldBeRequestedTo(Idle);
   FillingShouldStart();
   WhenTheMotorActionResultIs(Harvested);
}

TEST(TwistTrayIceMaker, ShouldNotBeginFillingTrayUntilMotorActionResultIsHarvested)
{
   GivenTheIceMakerIsEnabled();
   GivenFreezingIsCompletedAndHarvestingIsStarted();

   WhenTheMotorActionResultIs(Harvested);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Harvesting);

   FillTubeHeaterVoteAndCareShouldBecome(OFF, Vote_DontCare);
   TheMotorShouldBeRequestedTo(Idle);
   FillingShouldStart();
   AfterTheFillTubeHeaterTimerHasExpired();
}

TEST(TwistTrayIceMaker, ShouldTransitionFromHarvestToFillAndStartFillingWhenFillTubeTimerCompletesAndSabbathModeBecomesEnabled)
{
   GivenTheIceMakerIsEnabled();
   GivenFreezingIsCompletedAndHarvestingIsStarted();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Harvesting);

   WhenTheMotorActionResultIs(Harvested);
   WhenSabbathModeIs(ENABLED);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Harvesting);

   FillTubeHeaterVoteAndCareShouldBecome(OFF, Vote_DontCare);
   TheMotorShouldBeRequestedTo(Idle);
   FillingShouldStart();
   AfterTheFillTubeHeaterTimerHasExpired();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);
}

TEST(TwistTrayIceMaker, ShouldTransitionToIdleFillWhenDispenseStatusIsDispensingAndSelectionIsWaterWhileIceMakerStateIsInFillingTrayWithWater)
{
   GivenTheOperationStateIsInFillingTrayWithWater();

   TheWaterValveShouldBecome(CLOSED);
   WhenWaterStartsDispensing();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_IdleFill);
   And WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Pause);
}

TEST(TwistTrayIceMaker, ShouldTransitionToIdleFillWhenDispenseStatusIsDispensingAndSelectionIsAutofillWhileIceMakerStateIsInFillingTrayWithWater)
{
   GivenTheOperationStateIsInFillingTrayWithWater();

   TheWaterValveShouldBecome(CLOSED);
   WhenDispensingRequestStatusIs(DispenseStatus_Dispensing, DispensingRequestSelection_Autofill);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_IdleFill);
   And WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Pause);
}

TEST(TwistTrayIceMaker, ShouldTransitionToIdleFillWhenIceMakerFillIsInhibitedDueToNewFilterWhileIceMakerStateIsInFillingTrayWithWater)
{
   GivenTheOperationStateIsInFillingTrayWithWater();

   TheWaterValveShouldBecome(CLOSED);
   WhenIceMakerFillInhibitedDueToNewFilterIs(SET);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_IdleFill);
   And WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Pause);
}

TEST(TwistTrayIceMaker, ShouldTransitionToIdleFillWhenIceMakerFillIsInhibitedDueToRfidFilterWhileIceMakerStateIsInFillingTrayWithWater)
{
   GivenTheOperationStateIsInFillingTrayWithWater();

   TheWaterValveShouldBecome(CLOSED);
   WhenIceMakerFillInhibitedDueToRfidFilterIs(SET);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_IdleFill);
   And WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Pause);
}

TEST(TwistTrayIceMaker, ShouldNotTransitionToIdleFillWhenDispenseStatusIsDispensingAndSelectionIsCrushedIceWhileIceMakerStateIsInFillingTrayWithWater)
{
   GivenTheOperationStateIsInFillingTrayWithWater();

   WhenDispensingRequestStatusIs(DispenseStatus_Dispensing, DispensingRequestSelection_CrushedIce);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);
}

TEST(TwistTrayIceMaker, ShouldNotTransitionToIdleFillWhenDispenseStatusIsDispensingAndSelectionIsCubedIceWhileIceMakerStateIsInFillingTrayWithWater)
{
   GivenTheOperationStateIsInFillingTrayWithWater();

   WhenDispensingRequestStatusIs(DispenseStatus_Dispensing, DispensingRequestSelection_CubedIce);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);
}

TEST(TwistTrayIceMaker, ShouldTransitionToFillingTrayWithWaterWhenDispenseStatusIsCompletedSuccessfullyWhileIceMakerStateIsInIdleFill)
{
   GivenTheOperationStateIsInIdleFill();

   WhenWaterStopsDispensing();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);
}

TEST(TwistTrayIceMaker, ShouldNotTransitionToFillingTrayWithWaterWhenDispensingIsActiveWhileIceMakerStateIsInIdleFill)
{
   GivenTheOperationStateIsInIdleFill();

   WhenDispensingRequestStatusIs(DispenseStatus_Dispensing, DispensingRequestSelection_Autofill);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_IdleFill);
}

TEST(TwistTrayIceMaker, ShouldTransitionToFillingTrayWithWaterWhenDispenseStatusIsSensorErrorWhileIceMakerStateIsInIdleFill)
{
   GivenTheOperationStateIsInIdleFill();

   WhenDispensingRequestStatusIs(DispenseStatus_SensorError, DispensingRequestSelection_Water);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);
}

TEST(TwistTrayIceMaker, ShouldTransitionToFillingTrayWithWaterWhenDispenseStatusIsDisabledOrBlockedWhileIceMakerStateIsInIdleFill)
{
   GivenTheOperationStateIsInIdleFill();

   WhenDispensingRequestStatusIs(DispenseStatus_DisabledOrBlocked, DispensingRequestSelection_Water);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);
}

TEST(TwistTrayIceMaker, ShouldTransitionToFillingTrayWithWaterWhenDispenseStatusIsHitMaxVolumeWhileIceMakerStateIsInIdleFill)
{
   GivenTheOperationStateIsInIdleFill();

   WhenDispensingRequestStatusIs(DispenseStatus_HitMaxVolume, DispensingRequestSelection_Water);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);
}

TEST(TwistTrayIceMaker, ShouldTransitionToFillingTrayWithWaterWhenDispenseStatusIsHitMaxTimeWhileIceMakerStateIsInIdleFill)
{
   GivenTheOperationStateIsInIdleFill();

   WhenDispensingRequestStatusIs(DispenseStatus_HitMaxTime, DispensingRequestSelection_Water);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);
}

TEST(TwistTrayIceMaker, ShouldTransitionToFillingTrayWithWaterWhenDispenseStatusIsDidNotReceiveDispenseRequestAfterHeartbeatWhileIceMakerStateIsInIdleFill)
{
   GivenTheOperationStateIsInIdleFill();

   WhenDispensingRequestStatusIs(DispenseStatus_DidNotReceiveDispenseRequestAfterHeartbeat, DispensingRequestSelection_Water);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);
}

TEST(TwistTrayIceMaker, ShouldTransitionToFillingTrayWithWaterWhenDispenseStatusIsPreciseFillAmountReachedWhileIceMakerStateIsInIdleFill)
{
   GivenTheOperationStateIsInIdleFill();

   WhenDispensingRequestStatusIs(DispenseStatus_PreciseFillAmountReached, DispensingRequestSelection_Water);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);
}

TEST(TwistTrayIceMaker, ShouldTransitionToFillingTrayWithWaterWhenDispenseStatusIsBadCommandWhileIceMakerStateIsInIdleFill)
{
   GivenTheOperationStateIsInIdleFill();

   WhenDispensingRequestStatusIs(DispenseStatus_BadCommand, DispensingRequestSelection_Water);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);
}

TEST(TwistTrayIceMaker, ShouldTransitionToFillingTrayWithWaterWhenDispenseStatusIsDispenseInhibitedDueToDoorOpenWhileIceMakerStateIsInIdleFill)
{
   GivenTheOperationStateIsInIdleFill();

   WhenDispensingRequestStatusIs(DispenseStatus_DispenseInhibitedDueToDoorOpen, DispensingRequestSelection_Water);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);
}

TEST(TwistTrayIceMaker, ShouldTransitionToFillingTrayWithWaterWhenDispenseStatusIsDispenseInhibitedBecauseRedundantCupSwitchPressIsRequiredWhileIceMakerStateIsInIdleFill)
{
   GivenTheOperationStateIsInIdleFill();

   WhenDispensingRequestStatusIs(DispenseStatus_DispenseInhibitedBecauseRedundantCupSwitchPressIsRequired, DispensingRequestSelection_Water);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);
}

TEST(TwistTrayIceMaker, ShouldTransitionToFillingTrayWithWaterWhenDispenseStatusIsDispenseInhibitedDueToRfidErrorOrLeakWhileIceMakerStateIsInIdleFill)
{
   GivenTheOperationStateIsInIdleFill();

   WhenDispensingRequestStatusIs(DispenseStatus_DispenseInhibitedDueToRfidErrorOrLeak, DispensingRequestSelection_Water);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);
}

TEST(TwistTrayIceMaker, ShouldStartWaterFillMonitoringWhenEnteringIntoFillingTrayWithWaterStateWhileSabbathModeIsOff)
{
   GivenSabbathModeIs(OFF);
   GivenTheOperationStateIsInFillingTrayWithWater();
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Start);
}

TEST(TwistTrayIceMaker, ShouldResumeWaterFillMonitoringWhenUserDispensingIsFinishedWhilePaused)
{
   GivenFillPausedDuringFillingTrayWithWaterStateAndNowInIdleFillState();

   WhenWaterStopsDispensing();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);
   And WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Resume);
}

TEST(TwistTrayIceMaker, ShouldStartWaterFillMonitoringAfterAPreviouslyPausedAndResumedWaterFill)
{
   GivenIceMakerHasResumedFillAndIsInFillingTrayWithWater();
   ShouldTransitionFromFillingTrayWithWaterToFreezeWhenTrayIsFull();
   ShouldTransitionFromFreezeToHarvestingWhenHarvestConditionsAreMet();

   WhenMotorActionResultChangesToHarvestingThenHarvested();

   FillTubeHeaterVoteAndCareShouldBecome(OFF, Vote_DontCare);
   TheMotorShouldBeRequestedTo(Idle);
   TheWaterValveShouldBecome(OPEN);
   After(iceMakerData->harvestData.fillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC);

   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Start);
}

TEST(TwistTrayIceMaker, ShouldStartWaterFillMonitoringWhenSabbathIsDisabledWhileIceMakerStateIsInFillingTrayWithWaterState)
{
   GivenSabbathIsDisabledAndInFillingTrayWithWaterState();
   GivenSabbathModeIs(ON);

   WhenSabbathModeIs(DISABLED);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Start);
}

TEST(TwistTrayIceMaker, ShouldResumeWaterFillMonitoringWhenSabbathIsDisabledWhileIceMakerStateIsInFillingTrayWithWaterFillMonitoringPaused)
{
   GivenFillPausedDuringFillingTrayWithWaterStateAndNowInIdleFillState();
   GivenSabbathModeIs(ON);
   GivenDispensingRequestStatusIs(DispenseStatus_CompletedSuccessfully, DispensingRequestSelection_Water);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);

   WhenSabbathModeIs(DISABLED);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Resume);
}

TEST(TwistTrayIceMaker, ShouldTransitionToThermistorFaultStateWhenStopFillSignalIsSetWhileThermistorBecomesInvalidAndInFillingTrayWithWater)
{
   GivenSabbathIsDisabledAndInFillingTrayWithWaterState();
   GivenTheIceMakerThermistorIsInvalid();

   TheWaterValveShouldBecome(CLOSED);
   WhenTwistTrayIceMakerTrayIsFilled();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_ThermistorFault);
}

TEST(TwistTrayIceMaker, ShouldTransitionToFreezeStateAndStopWaterFillMonitoringWhenStopFillSignalIsSetWhileThermistorIsValidAndInFillingTrayWithWater)
{
   GivenSabbathIsDisabledAndInFillingTrayWithWaterState();
   GivenTheIceMakerThermistorIsValid();

   TheWaterValveShouldBecome(CLOSED);
   And FreezerTriggerIceRateSignalShouldIncrement();
   WhenTwistTrayIceMakerTrayIsFilled();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Freeze);
   And WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Stop);
}

TEST(TwistTrayIceMaker, ShouldDelayFillMonitoringWhenEnteringFillStateDuringWaterDispensingUntilDispensingIsCompleted)
{
   GivenTheIceMakerIsEnabled();
   GivenFreezingIsCompletedAndHarvestingIsStarted();

   After(iceMakerData->harvestData.fillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);
   WhenWaterStartsDispensing();

   TheMotorShouldBeRequestedTo(Idle);
   FillTubeHeaterVoteAndCareShouldBecome(OFF, Vote_DontCare);
   After(1);
   WhenTheMotorActionResultIs(Harvested);

   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_IdleFill);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Stop);

   FillingShouldStart();
   WhenWaterStopsDispensing();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Start);
}

TEST(TwistTrayIceMaker, ShouldDelayFillMonitoringWhenEnteringFillStateWhenFillIsInhibitedDueToNewFilterUntilFillIsNotInhibitedDueToNewFilter)
{
   GivenTheIceMakerIsEnabled();
   GivenFreezingIsCompletedAndHarvestingIsStarted();

   WhenIceMakerFillInhibitedDueToNewFilterIs(SET);

   After(iceMakerData->harvestData.fillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);
   TheMotorShouldBeRequestedTo(Idle);
   FillTubeHeaterVoteAndCareShouldBecome(OFF, Vote_DontCare);
   After(1);
   WhenTheMotorActionResultIs(Harvested);

   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_IdleFill);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Stop);

   FillingShouldStart();
   WhenIceMakerFillInhibitedDueToNewFilterIs(CLEAR);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Start);
}

TEST(TwistTrayIceMaker, ShouldDelayFillMonitoringWhenEnteringFillStateWhenFillIsInhibitedDueToRfidFilterUntilFillIsNotInhibitedDueToRfidFilter)
{
   GivenTheIceMakerIsEnabled();
   GivenFreezingIsCompletedAndHarvestingIsStarted();

   WhenIceMakerFillInhibitedDueToRfidFilterIs(SET);

   After(iceMakerData->harvestData.fillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);
   TheMotorShouldBeRequestedTo(Idle);
   FillTubeHeaterVoteAndCareShouldBecome(OFF, Vote_DontCare);
   After(1);
   WhenTheMotorActionResultIs(Harvested);

   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_IdleFill);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Stop);

   FillingShouldStart();
   WhenIceMakerFillInhibitedDueToRfidFilterIs(CLEAR);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Start);
}

TEST(TwistTrayIceMaker, ShouldNotDelayFillMonitoringIfWaterDispensingFinishesBeforeFillStateIsEntered)
{
   GivenTheIceMakerIsEnabled();
   GivenFreezingIsCompletedAndHarvestingIsStarted();

   After(iceMakerData->harvestData.fillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC - 2);
   WhenWaterStartsDispensing();

   After(1);
   WhenWaterStopsDispensing();

   TheMotorShouldBeRequestedTo(Idle);
   FillTubeHeaterVoteAndCareShouldBecome(OFF, Vote_DontCare);
   FillingShouldStart();
   After(1);
   WhenTheMotorActionResultIs(Harvested);

   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Start);
}

TEST(TwistTrayIceMaker, ShouldNotDelayFillMonitoringWhenEnteringFillStateDuringCubedIceDispensing)
{
   GivenTheIceMakerIsEnabled();
   GivenFreezingIsCompletedAndHarvestingIsStarted();

   After(iceMakerData->harvestData.fillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);
   WhenDispensingRequestStatusIs(DispenseStatus_Dispensing, DispensingRequestSelection_CubedIce);

   TheMotorShouldBeRequestedTo(Idle);
   FillTubeHeaterVoteAndCareShouldBecome(OFF, Vote_DontCare);
   FillingShouldStart();
   After(1);
   WhenTheMotorActionResultIs(Harvested);

   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Start);
}

TEST(TwistTrayIceMaker, ShouldNotDelayFillMonitoringWhenEnteringFillStateDuringCrushedIceDispensing)
{
   GivenTheIceMakerIsEnabled();
   GivenFreezingIsCompletedAndHarvestingIsStarted();

   After(iceMakerData->harvestData.fillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);
   WhenDispensingRequestStatusIs(DispenseStatus_Dispensing, DispensingRequestSelection_CrushedIce);

   TheMotorShouldBeRequestedTo(Idle);
   FillTubeHeaterVoteAndCareShouldBecome(OFF, Vote_DontCare);
   FillingShouldStart();
   After(1);
   WhenTheMotorActionResultIs(Harvested);

   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Start);
}

TEST(TwistTrayIceMaker, ShouldDelayFillMonitoringWhenEnteringFillStateIfDispensingIsNotCompletedInHomingOperation)
{
   GivenTheIceMakerIsEnabled();
   GivenTheTemperatureIs(iceMakerData->freezeData.maximumHarvestTemperatureInDegFx100 - 1);
   GivenTheIceMakerThermistorIsValid();

   TheMotorShouldBeRequestedTo(Home);
   GivenTheModuleIsInitialized();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Homing);

   WhenWaterStartsDispensing();

   TheMotorShouldBeRequestedTo(Idle);
   WhenTheMotorActionResultIs(Homed);

   HarvestingShouldStart();
   WhenHarvestCountIsReadyToHarvestIs(SET);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Harvesting);

   TheMotorShouldBeRequestedTo(Idle);
   FillTubeHeaterVoteAndCareShouldBecome(OFF, Vote_DontCare);
   After(iceMakerData->harvestData.fillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC);
   WhenTheMotorActionResultIs(Harvested);

   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_IdleFill);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Stop);
}

TEST(TwistTrayIceMaker, ShouldNotDelayFillMonitoringWhenEnteringFillStateIfDispensingIsCompletedInHomingOperation)
{
   GivenTheIceMakerIsEnabled();
   GivenTheTemperatureIs(iceMakerData->freezeData.maximumHarvestTemperatureInDegFx100 - 1);
   GivenTheIceMakerThermistorIsValid();

   TheMotorShouldBeRequestedTo(Home);
   GivenTheModuleIsInitialized();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Homing);

   WhenWaterStartsDispensing();
   WhenWaterStopsDispensing();

   TheMotorShouldBeRequestedTo(Idle);
   WhenTheMotorActionResultIs(Homed);

   HarvestingShouldStart();
   WhenHarvestCountIsReadyToHarvestIs(SET);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Harvesting);

   TheMotorShouldBeRequestedTo(Idle);
   FillTubeHeaterVoteAndCareShouldBecome(OFF, Vote_DontCare);
   FillingShouldStart();
   After(iceMakerData->harvestData.fillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC);
   WhenTheMotorActionResultIs(Harvested);

   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Start);
}

TEST(TwistTrayIceMaker, ShouldDelayFillMonitoringWhenFillingIsStartedWhileWaterIsDispensingFromFreezeOperation)
{
   GivenTheOperationStateIsInFreeze();
   GivenFreezerIceRateActiveBecomes(SET);
   GivenTheTemperatureIs(iceMakerData->freezeData.maximumHarvestTemperatureInDegFx100 - 1);

   WhenWaterStartsDispensing();

   HarvestingShouldStart();
   WhenHarvestCountIsReadyToHarvestIs(SET);

   TheMotorShouldBeRequestedTo(Idle);
   FillTubeHeaterVoteAndCareShouldBecome(OFF, Vote_DontCare);
   AfterTheFillTubeHeaterTimerHasExpired();
   WhenTheMotorActionResultIs(Harvested);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_IdleFill);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Stop);
}

TEST(TwistTrayIceMaker, ShouldNotDelayFillMonitoringWhenFillingIsStartedIfDispensingIsCompletedSuccessfullyInFreezeOperation)
{
   GivenTheOperationStateIsInFreeze();
   GivenFreezerIceRateActiveBecomes(SET);
   GivenTheTemperatureIs(iceMakerData->freezeData.maximumHarvestTemperatureInDegFx100 - 1);

   WhenWaterStartsDispensing();
   WhenWaterStopsDispensing();

   HarvestingShouldStart();
   WhenHarvestCountIsReadyToHarvestIs(SET);

   HarvestingIsCompletedAndFillingIsStarted();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Start);
}

TEST(TwistTrayIceMaker, ShouldNotDelayFillMonitoringAndClearTestRequestWhenWaterDispensingIsCompletedAndTestRequestIsFillWhileIceMakerStateIsInBucketIsFull)
{
   GivenTheIceMakerIsEnabled();
   GivenFreezingIsCompletedAndHarvestingIsStarted();

   WhenWaterStartsDispensing();

   TheMotorShouldBeRequestedTo(Idle);
   FillTubeHeaterVoteAndCareShouldBecome(OFF, Vote_DontCare);
   WhenTheMotorActionResultIs(BucketWasFull);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_BucketIsFull);

   WhenWaterStopsDispensing();

   FillingShouldStart();
   WhenTheTestRequestIs(IceMakerTestRequest_Fill);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Start);
   And TheTestRequestShouldBe(IceMakerTestRequest_None);
}

TEST(TwistTrayIceMaker, ShouldDelayFillMonitoringAndClearTestRequestWhenWaterDispensingIsStartedAndTestRequestIsFillWhileIceMakerStateIsInBucketIsFull)
{
   GivenTheOperationStateIsInBucketIsFull();

   WhenWaterStartsDispensing();

   WhenTheTestRequestIs(IceMakerTestRequest_Fill);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_IdleFill);
   And WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Stop);
   And TheTestRequestShouldBe(IceMakerTestRequest_None);
}

TEST(TwistTrayIceMaker, ShouldDelayFillMonitoringWhenEnteringFillStateIfWaterIsDispensingInMotorErrorOperation)
{
   GivenTheIceMakerIsInStateMotorError();
   TheMotorShouldBeRequestedTo(Home);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_MotorError);

   WhenWaterStartsDispensing();

   After(instance._private.parametric->harvestData.motorErrorRetryInitializeMinutes * MSEC_PER_MIN);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Homing);

   TheMotorShouldBeRequestedTo(Idle);
   WhenTheMotorActionResultIs(Homed);

   HarvestingShouldStart();
   WhenHarvestCountIsReadyToHarvestIs(SET);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Harvesting);

   TheMotorShouldBeRequestedTo(Idle);
   FillTubeHeaterVoteAndCareShouldBecome(OFF, Vote_DontCare);
   After(iceMakerData->harvestData.fillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC);
   WhenTheMotorActionResultIs(Harvested);

   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_IdleFill);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Stop);
}

TEST(TwistTrayIceMaker, ShouldNotDelayFillMonitoringWhenEnteringFillStateIfWaterDispensingIsCompletedInMotorErrorOperation)
{
   GivenTheIceMakerIsInStateMotorError();
   TheMotorShouldBeRequestedTo(Home);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_MotorError);

   WhenWaterStartsDispensing();

   After(instance._private.parametric->harvestData.motorErrorRetryInitializeMinutes * MSEC_PER_MIN - 1);
   WhenWaterStopsDispensing();

   After(1);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Homing);

   TheMotorShouldBeRequestedTo(Idle);
   WhenTheMotorActionResultIs(Homed);

   HarvestingShouldStart();
   WhenHarvestCountIsReadyToHarvestIs(SET);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Harvesting);

   TheMotorShouldBeRequestedTo(Idle);
   FillTubeHeaterVoteAndCareShouldBecome(OFF, Vote_DontCare);
   FillingShouldStart();
   After(iceMakerData->harvestData.fillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC);
   WhenTheMotorActionResultIs(Harvested);

   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Start);
}

TEST(TwistTrayIceMaker, ShouldDelayFillMonitoringWhenIceMakerThermistorBecomesValidAndEnteringFillStateIfWaterIsDispensingInThermistorFaultOperation)
{
   GivenTheIceMakerThermistorIsInvalid();
   GivenTheIceMakerIsEnabled();
   GivenTheModuleIsInitialized();

   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_ThermistorFault);
   TheMotorShouldBeRequestedTo(Home);

   WhenWaterStartsDispensing();

   TheMotorShouldBeRequestedTo(Idle);
   WhenTheIceMakerThermistorIsValid();
   WhenTheMotorActionResultIs(Homed);

   HarvestingShouldStart();
   WhenHarvestCountIsReadyToHarvestIs(SET);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Harvesting);

   TheMotorShouldBeRequestedTo(Idle);
   FillTubeHeaterVoteAndCareShouldBecome(OFF, Vote_DontCare);
   After(iceMakerData->harvestData.fillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC);
   WhenTheMotorActionResultIs(Harvested);

   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_IdleFill);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Stop);
}

TEST(TwistTrayIceMaker, ShouldNotDelayFillMonitoringWhenIceMakerThermistorBecomesValidAndEnteringFillStateIfWaterDispensingIsCompletedInThermistorFaultOperation)
{
   GivenTheIceMakerThermistorIsInvalid();
   GivenTheIceMakerIsEnabled();
   GivenTheModuleIsInitialized();

   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_ThermistorFault);
   TheMotorShouldBeRequestedTo(Home);

   WhenWaterStartsDispensing();
   WhenWaterStopsDispensing();

   TheMotorShouldBeRequestedTo(Idle);
   WhenTheIceMakerThermistorIsValid();
   WhenTheMotorActionResultIs(Homed);

   HarvestingShouldStart();
   WhenHarvestCountIsReadyToHarvestIs(SET);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Harvesting);

   TheMotorShouldBeRequestedTo(Idle);
   FillTubeHeaterVoteAndCareShouldBecome(OFF, Vote_DontCare);
   FillingShouldStart();
   After(iceMakerData->harvestData.fillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC);
   WhenTheMotorActionResultIs(Harvested);

   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Start);
}

TEST(TwistTrayIceMaker, ShouldNotTransitionToFillStateWhenTestRequestIsFillInHarvestingStateAndHarvestIsNotCompleted)
{
   GivenTheOperationStateIsInHarvesting();

   WhenTheTestRequestIs(IceMakerTestRequest_Fill);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Harvesting);
}

TEST(TwistTrayIceMaker, ShouldTransitionToFillStateWhenTestRequestIsFillInHarvestingStateAndHarvestIsCompleted)
{
   GivenTheOperationStateIsInHarvesting();
   GivenTheMotorActionResultIs(Harvested);

   TheMotorShouldBeRequestedTo(Idle);
   FillTubeHeaterVoteAndCareShouldBecome(OFF, Vote_DontCare);
   FillingShouldStart();
   WhenTheTestRequestIs(IceMakerTestRequest_Fill);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);
}

TEST(TwistTrayIceMaker, ShouldTransitionToHarvestStateWhenDoorOpenedForCheckTimeThenClosed)
{
   GivenTheOperationStateIsInBucketIsFull();

   WhenTheFreezerDoorIs(OPEN);
   After(iceMakerData->harvestData.fullBucketDoorOpenCheckTimeInMinutes * MSEC_PER_MIN - 1);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_BucketIsFull);

   After(1);
   HarvestingShouldStart();
   WhenTheFreezerDoorIs(CLOSED);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Harvesting);
}

TEST(TwistTrayIceMaker, ShouldRemainInFullBucketStateWhenDoorIsOpenedForLessThanOpenCheckTime)
{
   GivenTheOperationStateIsInBucketIsFull();

   WhenTheFreezerDoorIs(OPEN);
   After(iceMakerData->harvestData.fullBucketDoorOpenCheckTimeInMinutes * MSEC_PER_MIN - 1);

   WhenTheFreezerDoorIs(CLOSED);
   After(1);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_BucketIsFull);
}

TEST(TwistTrayIceMaker, ShouldRemainInFullBucketStateWhenDoorHasNotBeenOpenedBeforeOpenCheckTimeExpires)
{
   GivenTheOperationStateIsInBucketIsFull();
   After(iceMakerData->harvestData.fullBucketDoorOpenCheckTimeInMinutes * MSEC_PER_MIN);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_BucketIsFull);
}

TEST(TwistTrayIceMaker, ShouldRestartTheDoorOpenedCheckTimeEveryTimeTheDoorIsOpened)
{
   GivenTheOperationStateIsInBucketIsFull();

   WhenTheFreezerDoorIs(OPEN);
   After(iceMakerData->harvestData.fullBucketDoorOpenCheckTimeInMinutes * MSEC_PER_MIN - 1);

   WhenTheFreezerDoorIs(CLOSED);
   After(1);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_BucketIsFull);

   WhenTheFreezerDoorIs(OPEN);
   After(1);
   WhenTheFreezerDoorIs(CLOSED);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_BucketIsFull);
}

TEST(TwistTrayIceMaker, ShouldRestartTheDoorOpenedCheckTimeEveryTimeItExitsTheFullState)
{
   GivenTheOperationStateIsInBucketIsFull();

   WhenTheFreezerDoorIs(OPEN);
   After(iceMakerData->harvestData.fullBucketDoorOpenCheckTimeInMinutes * MSEC_PER_MIN - 1);

   HarvestingShouldStart();
   WhenTheTestRequestIs(IceMakerTestRequest_Harvest);
   HarvestCountCalculationRequestShouldBe(CLEAR);
   After(1);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Harvesting);

   GivenTheCoolingSystemOffIs(false);
   WhenTheMotorActionResultIs(Homed);

   FillTubeHeaterVoteAndCareShouldBecome(OFF, Vote_DontCare);
   TheMotorShouldBeRequestedTo(Idle);
   WhenTheMotorActionResultIs(BucketWasFull);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_BucketIsFull);

   After(1);
   WhenTheFreezerDoorIs(CLOSED);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_BucketIsFull);

   WhenTheFreezerDoorIs(OPEN);
   After(iceMakerData->harvestData.fullBucketDoorOpenCheckTimeInMinutes * MSEC_PER_MIN - 1);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_BucketIsFull);

   After(1);
   HarvestingShouldStart();
   WhenTheFreezerDoorIs(CLOSED);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Harvesting);
}

TEST(TwistTrayIceMaker, ShouldResetTheDoorOpenedCheckTimeWhenExitingTheFullState)
{
   GivenTheOperationStateIsInBucketIsFull();

   WhenTheFreezerDoorIs(OPEN);
   After(iceMakerData->harvestData.fullBucketDoorOpenCheckTimeInMinutes * MSEC_PER_MIN);

   HarvestingShouldStart();
   WhenTheFreezerDoorIs(CLOSED);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Harvesting);

   GivenTheCoolingSystemOffIs(false);
   WhenTheMotorActionResultIs(Homed);

   FillTubeHeaterVoteAndCareShouldBecome(OFF, Vote_DontCare);
   TheMotorShouldBeRequestedTo(Idle);
   WhenTheMotorActionResultIs(BucketWasFull);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_BucketIsFull);

   WhenTheFreezerDoorIs(OPEN);
   After(1);
   WhenTheFreezerDoorIs(CLOSED);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_BucketIsFull);
}

TEST(TwistTrayIceMaker, ShouldRestartHarvestDoorDelayTimerWhenExitingTheFreezeState)
{
   GivenTheOperationStateIsInFreeze();
   GivenFreezerIceRateActiveBecomes(SET);
   WhenTheFreezerDoorIs(OPEN);

   WhenTheTemperatureIs(iceMakerData->freezeData.maximumHarvestTemperatureInDegFx100 - 1);

   NothingShouldHappen();
   WhenHarvestCountIsReadyToHarvestIs(SET);

   After(iceMakerData->harvestData.delayToHarvestAfterDoorOpensMinutes * MSEC_PER_MIN - 1);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Freeze);

   FillingShouldStart();
   WhenTheTestRequestIs(IceMakerTestRequest_Fill);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);

   TheWaterValveShouldBecome(CLOSED);
   FreezerTriggerIceRateSignalShouldIncrement();
   WhenTwistTrayIceMakerTrayIsFilled();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Freeze);

   After(1);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Freeze);
}

TEST(TwistTrayIceMaker, ShouldResetTheHarvestDelayWhenExitingTheFreezeState)
{
   GivenTheOperationStateIsInFreeze();
   GivenFreezerIceRateActiveBecomes(SET);
   WhenTheFreezerDoorIs(OPEN);

   WhenTheTemperatureIs(iceMakerData->freezeData.maximumHarvestTemperatureInDegFx100 - 1);

   NothingShouldHappen();
   WhenHarvestCountIsReadyToHarvestIs(SET);

   After(iceMakerData->harvestData.delayToHarvestAfterDoorOpensMinutes * MSEC_PER_MIN - 1);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Freeze);

   FillingShouldStart();
   WhenTheTestRequestIs(IceMakerTestRequest_Fill);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);

   TheWaterValveShouldBecome(CLOSED);
   FreezerTriggerIceRateSignalShouldIncrement();
   WhenTwistTrayIceMakerTrayIsFilled();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Freeze);

   WhenTheIceMakerBecomesDisabled();
   GivenTheCoolingSystemOffIs(false);
   TheMotorShouldBeRequestedTo(Harvest);
   FillTubeHeaterVoteAndCareShouldBecome(iceMakerData->harvestData.fillTubeHeaterDutyCyclePercentage, Vote_Care);
   WhenTheIceMakerBecomesEnabled();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Harvesting);
}

TEST(TwistTrayIceMaker, ShouldTransitionToThermistorFaultAfterFillTubeHeaterOnTimeIsExpiredWhileIceIsHarvestedAndIceMakerThermistorIsInvalid)
{
   GivenTheOperationStateIsInHarvesting();
   GivenTheMotorActionResultIs(Harvested);
   GivenTheIceMakerThermistorIsInvalid();

   FillTubeHeaterVoteAndCareShouldBecome(OFF, Vote_DontCare);
   TheMotorShouldBeRequestedTo(Idle);
   AfterTheFillTubeHeaterTimerHasExpired();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_ThermistorFault);
}

TEST(TwistTrayIceMaker, ShouldTransitionToThermistorFaultWhenHarvestedWhileFillTubeHeaterOnTimeIsExpiredAndIceMakerThermistorIsInvalid)
{
   GivenTheOperationStateIsInHarvesting();
   GivenTheIceMakerThermistorIsInvalid();

   FillTubeHeaterVoteAndCareShouldBecome(OFF, Vote_DontCare);
   AfterTheFillTubeHeaterTimerHasExpired();

   TheMotorShouldBeRequestedTo(Idle);
   WhenTheMotorActionResultIs(Harvested);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_ThermistorFault);
}

TEST(TwistTrayIceMaker, ShouldTransitionToThermistorFaultWhenMotorIsErrorWhileIceMakerThermistorIsInvalid)
{
   GivenTheOperationStateIsInHarvesting();
   GivenTheIceMakerThermistorIsInvalid();

   FillTubeHeaterVoteAndCareShouldBecome(OFF, Vote_DontCare);
   TheMotorShouldBeRequestedTo(Idle);
   WhenTheMotorActionResultIs(MotorError);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_ThermistorFault);
}

TEST(TwistTrayIceMaker, ShouldTransitionToThermistorFaultWhenBucketIsFullWhileIceMakerThermistorIsInvalid)
{
   GivenTheOperationStateIsInHarvesting();
   GivenTheIceMakerThermistorIsInvalid();

   TheMotorShouldBeRequestedTo(Idle);
   FillTubeHeaterVoteAndCareShouldBecome(OFF, Vote_DontCare);
   WhenTheMotorActionResultIs(BucketWasFull);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_ThermistorFault);
}
