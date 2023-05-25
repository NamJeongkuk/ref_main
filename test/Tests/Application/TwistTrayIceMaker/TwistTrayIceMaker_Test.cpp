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
   Idle = TwistTrayIceMakerMotorAction_Idle,
   Home = TwistTrayIceMakerMotorAction_RunHomingRoutine,
   Harvest = TwistTrayIceMakerMotorAction_RunCycle,
   Homed = TwistTrayIceMakerMotorActionResult_Homed,
   Harvested = TwistTrayIceMakerMotorActionResult_Harvested,
   BucketWasFull = TwistTrayIceMakerMotorActionResult_BucketWasFull,
   MotorError = TwistTrayIceMakerMotorActionResult_MotorError,

   FaultState = TwistTrayIceMakerHighLevelState_Fault,

   VeryCold = 200,

   IceTemperaturePollingTime = 3 * MSEC_PER_SEC,
   ALongTime = 3 * SECONDS_PER_HOUR * MSEC_PER_SEC,
   OneSecond = 1 * MSEC_PER_SEC,
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

      iceMakerData = PersonalityParametricData_Get(dataModel)->iceMakerData->twistTrayIceMakerData;

      EventSubscription_Init(&dataModelTestDoubleOnChangeEventSubscription, dataModel, OnDataModelChange);
      Event_Subscribe(dataModel->OnDataChange, &dataModelTestDoubleOnChangeEventSubscription);
   }

   void TheModuleIsInitialized()
   {
      TwistTrayIceMaker_Init(
         &instance,
         &timerModuleTestDouble.timerModule,
         DataModel_AsDataSource(dataModel),
         iceMakerData);
   }

   void TheTemperatureIs(TemperatureDegFx100_t temp)
   {
      DataModel_Write(dataModel, Erd_TwistTrayIceMakerThermistor_IsValidResolved, set);
      DataModel_Write(dataModel, Erd_TwistTrayIceMaker_FilteredTemperatureResolvedInDegFx100, &temp);
   }

   void WhenTheTemperatureIs(TemperatureDegFx100_t temperature)
   {
      TheTemperatureIs(temperature);
   }

   void GivenTheTemperatureIs(TemperatureDegFx100_t temperature)
   {
      WhenTheTemperatureIs(temperature);
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
         Erd_IceMaker0EnableStatus,
         set);
   }

   void WhenTheIceMakerBecomesEnabled()
   {
      GivenTheIceMakerIsEnabled();
   }

   void GivenTheIceMakerIsDisabled()
   {
      DataModel_Write(
         dataModel,
         Erd_IceMaker0EnableStatus,
         clear);
   }

   void WhenTheIceMakerBecomesDisabled()
   {
      GivenTheIceMakerIsDisabled();
   }

   void TheMotorShouldBeRequestedTo(TwistTrayIceMakerMotorAction_t expectedMotorState)
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

   void WhenTheMotorActionResultIs(TwistTrayIceMakerMotorActionResult_t motorActionResult)
   {
      DataModel_Write(
         dataModel,
         Erd_TwistTrayIceMaker_MotorActionResult,
         &motorActionResult);
   }

   void TheFreezerDoorIs(bool freezerDoorState)
   {
      DataModel_Write(
         dataModel,
         Erd_LeftSideFreezerDoorStatus,
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
         .selection = DispenseSelection_CubedIce,
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
         .selection = DispenseSelection_CubedIce,
         .specialOptions = DispensingSpecialOptions_None,
         .status = DispenseStatus_CompletedSuccessfully,
         .preciseFillOuncesx100 = 0
      };

      DataModel_Write(
         dataModel,
         Erd_DispensingRequestStatus,
         &status);
   }

   void WhenWaterStartsDispensing(void)
   {
      DispensingRequestStatus_t status = {
         .action = DispensingAction_Start,
         .selection = DispenseSelection_ColdWater,
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
         .selection = DispenseSelection_ColdWater,
         .specialOptions = DispensingSpecialOptions_None,
         .status = DispenseStatus_CompletedSuccessfully,
         .preciseFillOuncesx100 = 0
      };

      DataModel_Write(
         dataModel,
         Erd_DispensingRequestStatus,
         &status);
   }

   void SabbathModeIs(bool state)
   {
      DataModel_Write(
         dataModel,
         Erd_SabbathMode,
         &state);
   }

   void EnhancedSabbathModeIs(bool state)
   {
      DataModel_Write(
         dataModel,
         Erd_EnhancedSabbathMode,
         &state);
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
      FillTubeHeaterVoteAndCareShouldBecome(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterDutyCyclePercentage, Vote_Care);
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

   void GivenHomingIsCompleted()
   {
      GivenTheIceMakerThermistorIsValid();

      TheMotorShouldBeRequestedTo(Home);
      When TheModuleIsInitialized();

      TheMotorShouldBeRequestedTo(Idle);
      WhenTheMotorActionResultIs(Homed);
   }

   void GivenFreezingIsCompletedAndHarvestingIsStarted()
   {
      GivenHomingIsCompleted();

      When TheTemperatureIs(iceMakerData->freezeData.maximumHarvestTemperatureInDegFx100 - 1);

      HarvestingShouldStart();
      WhenHarvestCountIsReadyToHarvestIs(SET);
      TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Harvesting);
   }

   void FreezingIsCompletedAndHarvestingDoesNotStartBecauseFreezerDoorOpens()
   {
      TemperatureDegFx100_t actualTempx100 = iceMakerData->freezeData.maximumHarvestTemperatureInDegFx100;
      Given TheTemperatureIs(actualTempx100);
      GivenHomingIsCompleted();

      TheFreezerDoorIs(OPEN);

      NothingShouldHappen();
      When TheTemperatureIs(actualTempx100 - 1);
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
      After(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC);
   }

   void NothingShouldHappen()
   {
   }

   void WhenTheTestRequestIs(IceMakerTestRequest_t request)
   {
      DataModel_Write(
         dataModel,
         Erd_TwistTrayIceMakerTestRequest,
         &request);
   }

   void TheTestRequestShouldBe(IceMakerTestRequest_t expectedValue)
   {
      IceMakerTestRequest_t actualValue;
      DataModel_Read(
         dataModel,
         Erd_TwistTrayIceMakerTestRequest,
         &actualValue);

      CHECK_EQUAL(expectedValue, actualValue);
   }

   void GivenTheOperationStateIsInHoming(void)
   {
      GivenTheIceMakerThermistorIsValid();

      TheMotorShouldBeRequestedTo(Home);
      When TheModuleIsInitialized();
      TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Homing);
   }

   void GivenTheOperationStateIsInIdleFreeze(void)
   {
      GivenTheIceMakerIsDisabled();
      GivenHomingIsCompleted();
      And TheTemperatureIs(VeryCold);
      TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_IdleFreeze);
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

   void HarvestCountCalculationRequestShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_TwistTrayIceMaker_HarvestCountCalculationRequest, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void WhenHarvestCountIsReadyToHarvestIs(bool state)
   {
      DataModel_Write(dataModel, Erd_TwistTrayIceMaker_HarvestCountIsReadyToHarvest, &state);
   }
};

TEST(TwistTrayIceMaker, ShouldHomeOnInitializationAndNotIncrementFreezerIceRateSignalIfIceMakerThermistorIsValid)
{
   GivenTheIceMakerThermistorIsValid();
   GivenTheIceMakerIsEnabled();

   TheMotorShouldBeRequestedTo(Home);
   When TheModuleIsInitialized();

   TheMotorShouldBeRequestedTo(Idle);
   FreezerTriggerIceRateSignalShouldNotIncrement();
   WhenTheMotorActionResultIs(Homed);
}

TEST(TwistTrayIceMaker, ShouldHomeWhenIceMakerThermistorIsInvalidAndBecomesValid)
{
   GivenTheIceMakerThermistorIsInvalid();
   GivenTheIceMakerIsEnabled();
   And TheModuleIsInitialized();

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
   When TheModuleIsInitialized();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Homing);

   WhenTheIceMakerThermistorIsInvalid();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Homing);
}

// Idle Freeze

TEST(TwistTrayIceMaker, ShouldHomeOnInitializationThenGoToIdleFreezeWhenIceMakerIsDisabled)
{
   GivenTheIceMakerIsDisabled();
   GivenHomingIsCompleted();
   And TheTemperatureIs(VeryCold);

   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_IdleFreeze);
}

TEST(TwistTrayIceMaker, ShouldTransitionFromIdleFreezeStateToFreezeStateWhenIceMakerIsEnabled)
{
   GivenTheIceMakerIsDisabled();
   GivenHomingIsCompleted();
   And TheTemperatureIs(VeryCold);

   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_IdleFreeze);

   WhenTheIceMakerBecomesEnabled();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Freeze);
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
   GivenTheTemperatureIs(iceMakerData->freezeData.maximumHarvestTemperatureInDegFx100);
   HarvestCountCalculationRequestShouldBe(SET);

   WhenHarvestCountIsReadyToHarvestIs(SET);

   HarvestingShouldStart();
   When TheTemperatureIs(iceMakerData->freezeData.maximumHarvestTemperatureInDegFx100 - 1);
   HarvestCountCalculationRequestShouldBe(CLEAR);
}

TEST(TwistTrayIceMaker, ShouldClearHarvestCountCalculationRequestAndStartHarvestingIfHarvestCountIsReadyToHarvestWhileThermistorTemperatureIsBelowMaximumHarvestTemperature)
{
   GivenTheOperationStateIsInFreeze();
   HarvestCountCalculationRequestShouldBe(SET);

   When TheTemperatureIs(iceMakerData->freezeData.maximumHarvestTemperatureInDegFx100 - 1);

   HarvestingShouldStart();
   WhenHarvestCountIsReadyToHarvestIs(SET);
   HarvestCountCalculationRequestShouldBe(CLEAR);
}

TEST(TwistTrayIceMaker, ShouldClearHarvestCountCalculationRequestAndTransitionToThermistorFaultWhenThermistorIsInvalidWhileFreezing)
{
   GivenTheOperationStateIsInFreeze();
   HarvestCountCalculationRequestShouldBe(SET);

   WhenTheIceMakerThermistorIsInvalid();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_ThermistorFault);
   HarvestCountCalculationRequestShouldBe(CLEAR);
}

TEST(TwistTrayIceMaker, ShouldClearHarvestCountCalculationRequestAndTransitionFromFreezeToIdleFreezeWhenIceMakerBecomesDisabled)
{
   GivenTheIceMakerIsEnabled();
   Given TheTemperatureIs(VeryCold);
   GivenHomingIsCompleted();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Freeze);
   HarvestCountCalculationRequestShouldBe(SET);

   WhenTheIceMakerBecomesDisabled();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_IdleFreeze);
   HarvestCountCalculationRequestShouldBe(CLEAR);
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

TEST(TwistTrayIceMaker, ShouldTransitionFromHarvestToIdleFreezeWhenHarvestCompletesAndIceMakerIsDisabled)
{
   GivenTheIceMakerIsEnabled();
   GivenFreezingIsCompletedAndHarvestingIsStarted();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Harvesting);

   WhenTheIceMakerBecomesDisabled();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Harvesting);

   TheMotorShouldBeRequestedTo(Idle);
   FillTubeHeaterVoteAndCareShouldBecome(OFF, Vote_DontCare);
   WhenTheMotorActionResultIs(Harvested);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_IdleFreeze);
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

TEST(TwistTrayIceMaker, ShouldTransitionFromHarvestToIdleFreezeWhenBucketIsFullAndIceMakerIsDisabled)
{
   GivenTheIceMakerIsEnabled();
   GivenFreezingIsCompletedAndHarvestingIsStarted();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Harvesting);

   WhenTheIceMakerBecomesDisabled();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Harvesting);

   TheMotorShouldBeRequestedTo(Idle);
   FillTubeHeaterVoteAndCareShouldBecome(OFF, Vote_DontCare);
   WhenTheMotorActionResultIs(BucketWasFull);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_IdleFreeze);
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

   NothingShouldHappen();
   After((iceMakerData->fillData.waterFillTimeSecX10 * 100) - 1);

   TheWaterValveShouldBecome(CLOSED);
   FreezerTriggerIceRateSignalShouldIncrement();
   After(1);
}

TEST(TwistTrayIceMaker, ShouldFreezeAfterFillingAndIncrementFreezerIceRateSignal)
{
   GivenTheIceMakerIsEnabled();
   GivenFillingHasStartedAfterCompletingFreezingAndHarvesting();

   TheWaterValveShouldBecome(CLOSED);
   FreezerTriggerIceRateSignalShouldIncrement();
   After((iceMakerData->fillData.waterFillTimeSecX10 * 100));
}

TEST(TwistTrayIceMaker, ShouldGoToThermistorFaultStateWhenEnteringFreezeStateIfThermistorIsInvalid)
{
   GivenTheIceMakerIsEnabled();
   GivenFillingHasStartedAfterCompletingFreezingAndHarvesting();

   WhenTheIceMakerThermistorIsInvalid();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);

   TheWaterValveShouldBecome(CLOSED);
   After((iceMakerData->fillData.waterFillTimeSecX10 * 100));

   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_ThermistorFault);
}

TEST(TwistTrayIceMaker, ShouldGoToIdleFreezeStateWhenEnteringFreezeStateIfIceMakerIsDisabled)
{
   GivenTheIceMakerIsEnabled();
   GivenFillingHasStartedAfterCompletingFreezingAndHarvesting();

   WhenTheIceMakerBecomesDisabled();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);

   TheWaterValveShouldBecome(CLOSED);
   After((iceMakerData->fillData.waterFillTimeSecX10 * 100));
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_IdleFreeze);
}

TEST(TwistTrayIceMaker, ShouldTransitionToIdleFreezeAfterFillingIsFinishedWhileSabbathIsEnabled)
{
   GivenTheIceMakerIsEnabled();
   GivenFillingHasStartedAfterCompletingFreezingAndHarvesting();

   When SabbathModeIs(ON);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);

   TheWaterValveShouldBecome(CLOSED);
   After((iceMakerData->fillData.waterFillTimeSecX10 * 100));
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_IdleFreeze);
}

TEST(TwistTrayIceMaker, ShouldBeAbleToHarvestTwice)
{
   GivenTheIceMakerIsEnabled();
   GivenFillingHasStartedAfterCompletingFreezingAndHarvesting();

   TheWaterValveShouldBecome(CLOSED);
   FreezerTriggerIceRateSignalShouldIncrement();
   After((iceMakerData->fillData.waterFillTimeSecX10 * 100));

   HarvestingShouldStart();
   When TheTemperatureIs(VeryCold);
   WhenHarvestCountIsReadyToHarvestIs(SET);
}

TEST(TwistTrayIceMaker, ShouldWaitToHarvestUntilDoorIsClosed)
{
   GivenTheIceMakerIsEnabled();
   FreezingIsCompletedAndHarvestingDoesNotStartBecauseFreezerDoorOpens();

   NothingShouldHappen();
   After((iceMakerData->harvestData.fullBucketWaitPeriodMinutes * MSEC_PER_MIN) + 1);

   TheFreezerDoorIs(CLOSED);

   NothingShouldHappen();
   After((iceMakerData->harvestData.delayToHarvestAfterDoorClosesSeconds * MSEC_PER_SEC) - 1);

   HarvestingShouldStart();
   After(1);
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

TEST(TwistTrayIceMaker, ShouldTransitionToIdleFreezeStateWhenIceMakerBecomesDisabledWhileInBucketIsFullState)
{
   GivenTheIceMakerIsEnabled();
   GivenFreezingIsCompletedAndHarvestingIsStarted();
   TheMotorShouldBeRequestedTo(Idle);
   FillTubeHeaterVoteAndCareShouldBecome(OFF, Vote_DontCare);
   WhenTheMotorActionResultIs(BucketWasFull);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_BucketIsFull);

   WhenTheIceMakerBecomesDisabled();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_IdleFreeze);
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

TEST(TwistTrayIceMaker, ShouldNotTransitionToIdleFreezeStateWhenIceMakerBecomesDisabledWhileFillingTrayWithWater)
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
   And TheTemperatureIs(VeryCold);
   Then SabbathModeIs(ON);

   NothingShouldHappen();
   WhenHarvestCountIsReadyToHarvestIs(SET);
   After((iceMakerData->freezeData.minimumFreezeTimeMinutes * MSEC_PER_MIN) + 1);

   HarvestingShouldStart();
   When SabbathModeIs(OFF);
}

TEST(TwistTrayIceMaker, SabbathModeShouldAlsoWorkIfTheEnhancedSabbathModeErdComesOn)
{
   GivenTheIceMakerIsEnabled();
   GivenHomingIsCompleted();
   And TheTemperatureIs(VeryCold);
   Then EnhancedSabbathModeIs(ON);

   NothingShouldHappen();
   WhenHarvestCountIsReadyToHarvestIs(SET);
   After((iceMakerData->freezeData.minimumFreezeTimeMinutes * MSEC_PER_MIN) + 1);

   HarvestingShouldStart();
   When EnhancedSabbathModeIs(OFF);
}

TEST(TwistTrayIceMaker, SabbathModeShouldAlsoWorkIfBothSabbathErdsComeOn)
{
   GivenTheIceMakerIsEnabled();
   GivenHomingIsCompleted();
   And TheTemperatureIs(VeryCold);
   And EnhancedSabbathModeIs(ON);
   And SabbathModeIs(ON);

   NothingShouldHappen();
   WhenHarvestCountIsReadyToHarvestIs(SET);
   After((iceMakerData->freezeData.minimumFreezeTimeMinutes * MSEC_PER_MIN) + 1);

   Then SabbathModeIs(OFF);

   NothingShouldHappen();
   After((iceMakerData->freezeData.minimumFreezeTimeMinutes * MSEC_PER_MIN) + 1);

   HarvestingShouldStart();
   When EnhancedSabbathModeIs(OFF);
}

TEST(TwistTrayIceMaker, ShouldTransitionToIdleFreezeWhenMotorActionResultIsHarvestedWhileIceMakerStateIsInHarvestingAndSabbathIsEnabled)
{
   GivenTheIceMakerIsEnabled();
   GivenFreezingIsCompletedAndHarvestingIsStarted();

   SabbathModeIs(ON);

   FillTubeHeaterVoteAndCareShouldBecome(OFF, Vote_DontCare);
   AfterTheFillTubeHeaterTimerHasExpired();

   TheMotorShouldBeRequestedTo(Idle);
   WhenTheMotorActionResultIs(Harvested);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_IdleFreeze);
}

// MOTOR ERRORS

TEST(TwistTrayIceMaker, ShouldGoToErrorStateIfAHomingMovementResultsInError)
{
   GivenTheIceMakerIsEnabled();
   GivenTheIceMakerThermistorIsValid();

   TheMotorShouldBeRequestedTo(Home);
   When TheModuleIsInitialized();

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
   When TheModuleIsInitialized();
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
   When TheModuleIsInitialized();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_ThermistorFault);

   WhenTheTestRequestIs(IceMakerTestRequest_Fill);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_ThermistorFault);
   And TheTestRequestShouldBe(IceMakerTestRequest_None);
}

TEST(TwistTrayIceMaker, ShouldNotTransitionToHarvestingAndClearTestRequestWhenTestRequestIsHarvestWhileIceMakerThermistorIsInvalid)
{
   GivenTheIceMakerThermistorIsInvalid();
   When TheModuleIsInitialized();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_ThermistorFault);

   WhenTheTestRequestIs(IceMakerTestRequest_Harvest);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_ThermistorFault);
   And TheTestRequestShouldBe(IceMakerTestRequest_None);
}

TEST(TwistTrayIceMaker, ShouldTransitionToFillingTrayWithWaterAndClearTestRequestWhenTestRequestIsFillWhileIceMakerStateIsInIdleFreeze)
{
   GivenTheOperationStateIsInIdleFreeze();

   FillingShouldStart();
   WhenTheTestRequestIs(IceMakerTestRequest_Fill);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);
   And TheTestRequestShouldBe(IceMakerTestRequest_None);
}

TEST(TwistTrayIceMaker, ShouldTransitionToHarvestingAndClearTestRequestWhenTestRequestIsHarvestWhileIceMakerStateIsInIdleFreeze)
{
   GivenTheOperationStateIsInIdleFreeze();

   HarvestingShouldStart();
   WhenTheTestRequestIs(IceMakerTestRequest_Harvest);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Harvesting);
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
   FillTubeHeaterVoteAndCareShouldBecome(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterDutyCyclePercentage, Vote_Care);
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
   FillTubeHeaterVoteAndCareShouldBecome(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterDutyCyclePercentage, Vote_Care);
   WhenTheTestRequestIs(IceMakerTestRequest_Harvest);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Harvesting);
   And TheTestRequestShouldBe(IceMakerTestRequest_None);
}

TEST(TwistTrayIceMaker, ShouldNotTransitionToFillingTrayWithWaterAndClearTestRequestWhenTestRequestIsFillWhileIceMakerStateIsInMotorError)
{
   GivenTheOperationStateIsInMotorError();

   WhenTheTestRequestIs(IceMakerTestRequest_Fill);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_MotorError);
   And TheTestRequestShouldBe(IceMakerTestRequest_None);
}

TEST(TwistTrayIceMaker, ShouldNotTransitionToHarvestingAndClearTestRequestWhenTestRequestIsHarvestWhileIceMakerStateIsInMotorError)
{
   GivenTheOperationStateIsInMotorError();

   WhenTheTestRequestIs(IceMakerTestRequest_Harvest);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_MotorError);
   And TheTestRequestShouldBe(IceMakerTestRequest_None);
}

// FILL TUBE HEATER

TEST(TwistTrayIceMaker, ShouldVoteFillTubeHeaterOnForFillTubeHeaterOnTime)
{
   GivenTheIceMakerIsEnabled();
   GivenFreezingIsCompletedAndHarvestingIsStarted();
   FillTubeHeaterVoteAndCareShouldBe(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterDutyCyclePercentage, Vote_Care);

   After(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);
   FillTubeHeaterVoteAndCareShouldBe(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterDutyCyclePercentage, Vote_Care);

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

TEST(TwistTrayIceMaker, ShouldTransitionFromHarvestToIdleFreezeWhenFillTubeTimerCompletesAndIceMakerBecomesDisabled)
{
   GivenTheIceMakerIsEnabled();
   GivenFreezingIsCompletedAndHarvestingIsStarted();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Harvesting);

   WhenTheMotorActionResultIs(Harvested);
   And WhenTheIceMakerBecomesDisabled();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Harvesting);

   FillTubeHeaterVoteAndCareShouldBecome(OFF, Vote_DontCare);
   TheMotorShouldBeRequestedTo(Idle);
   AfterTheFillTubeHeaterTimerHasExpired();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_IdleFreeze);
}

TEST(TwistTrayIceMaker, ShouldTransitionFromHarvestToIdleFreezeWhenFillTubeTimerCompletesAndSabbathModeBecomesEnabled)
{
   GivenTheIceMakerIsEnabled();
   GivenFreezingIsCompletedAndHarvestingIsStarted();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Harvesting);

   WhenTheMotorActionResultIs(Harvested);
   And SabbathModeIs(ON);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Harvesting);

   FillTubeHeaterVoteAndCareShouldBecome(OFF, Vote_DontCare);
   TheMotorShouldBeRequestedTo(Idle);
   AfterTheFillTubeHeaterTimerHasExpired();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_IdleFreeze);
}
