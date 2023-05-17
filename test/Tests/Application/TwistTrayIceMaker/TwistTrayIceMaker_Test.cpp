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
#include "utils.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "uassert_test.h"
#include "DataModel_TestDouble.h"
#include "TimerModule_TestDouble.h"

#define Given
#define When
#define Still
#define And
#define Then
#define OPEN HIGH
#define CLOSED LOW
#define ACTIVE HIGH
#define INACTIVE LOW

enum
{
   Idle = TwistTrayIceMakerMotorAction_Idle,
   Home = TwistTrayIceMakerMotorAction_RunHomingRoutine,
   Harvest = TwistTrayIceMakerMotorAction_RunCycle,

   NoAction = TwistTrayIceMakerMotorActionResult_NoAction,
   Harvesting = TwistTrayIceMakerMotorActionResult_Harvesting,
   Homing = TwistTrayIceMakerMotorActionResult_Homing,
   Homed = TwistTrayIceMakerMotorActionResult_Homed,
   Harvested = TwistTrayIceMakerMotorActionResult_Harvested,
   BucketWasFull = TwistTrayIceMakerMotorActionResult_BucketWasFull,
   MotorError = TwistTrayIceMakerMotorActionResult_MotorError,

   NormalRunState = TwistTrayIceMakerHighLevelState_NormalRun,
   ThermFaultState = TwistTrayIceMakerHighLevelState_ThermistorFault,
   FaultState = TwistTrayIceMakerHighLevelState_Fault,
   FactoryDiagsState = TwistTrayIceMakerHighLevelState_FactoryDiags,

   MaximumHarvestTemperaturex100 = 1900,
   FreezingPointx100 = 3200,
   VeryCold = 200,

   WaterFillTime = ((51 * MSEC_PER_SEC) / 10),
   MinimumFreezeTimeInMinutes = 1,
   FullIceBucketWaitTime = 3 * SECONDS_PER_HOUR * MSEC_PER_SEC,
   DelayToHarvestAfterDoorCloses = 3 * MSEC_PER_MIN,
   IceTemperaturePollingTime = 3 * MSEC_PER_SEC,
   FullBucketDispenseCheckTimeInSec = 12,
   FullBucketDispenseCheckTimeInMsec = FullBucketDispenseCheckTimeInSec * MSEC_PER_SEC,

   TargetFreezeIntegrationSum = 16200,

   ALongTime = FullIceBucketWaitTime + 1,
   OneSecond = 1 * MSEC_PER_SEC,

   TemperatureThatCausesFreezeIntegrationLimitToBeReachedInMinimumFreezeTimeInMinutes = FreezingPointx100 - TargetFreezeIntegrationSum / (MinimumFreezeTimeInMinutes * SECONDS_PER_MINUTE),
   TemperatureBelowFreezeIntegrationTemperature = 3000
};

static const TwistTrayIceMakerData_t parametric = {
   .typeInformation{
      .type = IceMakerType_Twist,
      .location = IceMakerLocation_Freezer,
       },

   .freezeData{
      .maximumHarvestTemperatureInDegFx100 = MaximumHarvestTemperaturex100,
      .startIntegrationTemperatureInDegFx100 = FreezingPointx100,
      .targetFreezeIntegrationSum = TargetFreezeIntegrationSum,
      .minimumFreezeTimeMinutes = MinimumFreezeTimeInMinutes,
   },

   .harvestData{
      .fullBucketWaitPeriodMinutes = FullIceBucketWaitTime / MSEC_PER_MIN,
      .fullBucketDetectionPeriodSecX10 = 0,
      .initialHomingTwistPeriodSecX10 = 0,
      .homeLandingDelayPeriodSecX10 = 0,
      .longMotorErrorTimeoutPeriodSec = 0,
      .shortMotorErrorTimeoutPeriodSec = 0,
      .delayToHarvestAfterDoorClosesSeconds = DelayToHarvestAfterDoorCloses / MSEC_PER_SEC,
      .fullBucketDispenseCheckTimeInSeconds = FullBucketDispenseCheckTimeInSec,
   },

   .fillData{
      .waterFillTimeSecX10 = WaterFillTime / 100,
   },

   .fillTubeHeaterData{
      .freezeThawFillTubeHeaterDutyCyclePercentage = 100,
      .freezeThawFillTubeHeaterOnTimeInSeconds = 400,
      .nonHarvestFillTubeHeaterDutyCyclePercentage = 10,
   }
};

static const TwistTrayIceMakerData_t fillTubeHeaterDutyCycleIsZeroParametric = {
   .typeInformation{
      .type = IceMakerType_Twist,
      .location = IceMakerLocation_Freezer,
      },
   .freezeData{
      .maximumHarvestTemperatureInDegFx100 = MaximumHarvestTemperaturex100,
      .startIntegrationTemperatureInDegFx100 = FreezingPointx100,
      .targetFreezeIntegrationSum = TargetFreezeIntegrationSum,
      .minimumFreezeTimeMinutes = MinimumFreezeTimeInMinutes,
   },

   .harvestData{
      .fullBucketWaitPeriodMinutes = FullIceBucketWaitTime / MSEC_PER_MIN,
      .fullBucketDetectionPeriodSecX10 = 0,
      .initialHomingTwistPeriodSecX10 = 0,
      .homeLandingDelayPeriodSecX10 = 0,
      .longMotorErrorTimeoutPeriodSec = 0,
      .shortMotorErrorTimeoutPeriodSec = 0,
      .delayToHarvestAfterDoorClosesSeconds = DelayToHarvestAfterDoorCloses / MSEC_PER_SEC,
      .fullBucketDispenseCheckTimeInSeconds = FullBucketDispenseCheckTimeInSec,
   },

   .fillData{
      .waterFillTimeSecX10 = WaterFillTime / 100,
   },

   .fillTubeHeaterData{
      .freezeThawFillTubeHeaterDutyCyclePercentage = 0,
      .freezeThawFillTubeHeaterOnTimeInSeconds = 400,
      .nonHarvestFillTubeHeaterDutyCyclePercentage = 10,
   }
};

static const TwistTrayIceMakerData_t fillTubeHeaterOnTimeIsZeroParametric = {
   .typeInformation{
      .type = IceMakerType_Twist,
      .location = IceMakerLocation_Freezer,
      },
   .freezeData{
      .maximumHarvestTemperatureInDegFx100 = MaximumHarvestTemperaturex100,
      .startIntegrationTemperatureInDegFx100 = FreezingPointx100,
      .targetFreezeIntegrationSum = TargetFreezeIntegrationSum,
      .minimumFreezeTimeMinutes = MinimumFreezeTimeInMinutes,
   },

   .harvestData{
      .fullBucketWaitPeriodMinutes = FullIceBucketWaitTime / MSEC_PER_MIN,
      .fullBucketDetectionPeriodSecX10 = 0,
      .initialHomingTwistPeriodSecX10 = 0,
      .homeLandingDelayPeriodSecX10 = 0,
      .longMotorErrorTimeoutPeriodSec = 0,
      .shortMotorErrorTimeoutPeriodSec = 0,
      .delayToHarvestAfterDoorClosesSeconds = DelayToHarvestAfterDoorCloses / MSEC_PER_SEC,
      .fullBucketDispenseCheckTimeInSeconds = FullBucketDispenseCheckTimeInSec,
   },

   .fillData{
      .waterFillTimeSecX10 = WaterFillTime / 100,
   },

   .fillTubeHeaterData{
      .freezeThawFillTubeHeaterDutyCyclePercentage = 100,
      .freezeThawFillTubeHeaterOnTimeInSeconds = 0,
      .nonHarvestFillTubeHeaterDutyCyclePercentage = 10,
   }
};

static const DataModel_TestDoubleConfigurationEntry_t erdDefinitions[] = {
   { Erd_TwistTrayIceMakerMotor_IceMakerVote, sizeof(TwistTrayIceMakerMotorVotedAction_t) },
   { Erd_TwistTrayIceMaker_MotorActionResult, sizeof(uint8_t) },
   { Erd_TwistTrayIceMaker_FilteredTemperatureInDegFx100, sizeof(TemperatureDegFx100_t) },
   { Erd_TwistTrayIceMakerThermistor_IsValidResolved, sizeof(bool) },
   { Erd_TwistTrayIceMakerWaterValve_IceMakerVote, sizeof(WaterValveVotedState_t) },
   { Erd_IsolationWaterValve_TwistTrayIceMakerVote, sizeof(WaterValveVotedState_t) },
   { Erd_DispensingRequestStatus, sizeof(DispensingRequestStatus_t) },
   { Erd_TwistTrayIceMaker_FreezeIntegrationCount, sizeof(uint32_t) },
   { Erd_TwistTrayIceMaker_MinimumFreezeTimeCounterInMinutes, sizeof(uint8_t) },
   { Erd_TwistTrayIceMaker_OperationState, sizeof(uint8_t) },
   { Erd_TwistTrayIceMaker_HighLevelState, sizeof(uint8_t) },
   { Erd_TwistTrayIceMaker_MotorFaultActive, sizeof(bool) },
   { Erd_SabbathMode, sizeof(bool) },
   { Erd_EnhancedSabbathMode, sizeof(bool) },
   { Erd_LeftSideFreezerDoorStatus, sizeof(bool) },
   { Erd_TwistTrayIceMakerTestRequest, sizeof(IceMakerTestRequest_t) },
   { Erd_FreezerIceRateTriggerSignal, sizeof(Signal_t) },
   { Erd_FillTubeHeater_TwistTrayIceMakerVote, sizeof(PercentageDutyCycleVote_t) },
   { Erd_IceMaker0EnableStatus, sizeof(bool) }
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
   DataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t timerModuleTestDouble;

   EventSubscription_t dataModelTestDoubleOnChangeEventSubscription;

   void setup()
   {
      TimerModule_TestDouble_Init(&timerModuleTestDouble);
      DataModel_TestDouble_Init(&dataModelTestDouble, erdDefinitions, NUM_ELEMENTS(erdDefinitions));
      dataModel = dataModelTestDouble.dataModel;

      EventSubscription_Init(&dataModelTestDoubleOnChangeEventSubscription, dataModel, OnDataModelChange);
      Event_Subscribe(dataModel->OnDataChange, &dataModelTestDoubleOnChangeEventSubscription);
   }

   void TheModuleIsInitialized()
   {
      TwistTrayIceMaker_Init(
         &instance,
         &timerModuleTestDouble.timerModule,
         DataModel_AsDataSource(dataModel),
         &parametric);
   }

   void TheModuleIsInitializedWithFillTubeHeaterDutyCycleZeroParametric()
   {
      TwistTrayIceMaker_Init(
         &instance,
         &timerModuleTestDouble.timerModule,
         DataModel_AsDataSource(dataModel),
         &fillTubeHeaterDutyCycleIsZeroParametric);
   }

   void TheModuleIsInitializedWithFillTubeHeaterOnTimeZeroParametric()
   {
      TwistTrayIceMaker_Init(
         &instance,
         &timerModuleTestDouble.timerModule,
         DataModel_AsDataSource(dataModel),
         &fillTubeHeaterOnTimeIsZeroParametric);
   }

   void TheTemperatureIs(TemperatureDegFx100_t temp)
   {
      DataModel_Write(dataModel, Erd_TwistTrayIceMakerThermistor_IsValidResolved, set);
      DataModel_Write(dataModel, Erd_TwistTrayIceMaker_FilteredTemperatureInDegFx100, &temp);
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
      FillTubeHeaterVoteAndCareShouldBecome(parametric.fillTubeHeaterData.freezeThawFillTubeHeaterDutyCyclePercentage, Vote_Care);
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

   TimerTicks_t TheTimeToReachIntegrationSumGiven(TemperatureDegFx100_t actualTempx100)
   {
      return (MSEC_PER_SEC * TargetFreezeIntegrationSum) / (FreezingPointx100 - actualTempx100);
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

      When TheTemperatureIs(MaximumHarvestTemperaturex100 - 1);

      HarvestingShouldStart();
      After(parametric.freezeData.minimumFreezeTimeMinutes * MSEC_PER_MIN);
      TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Harvesting);
   }

   void WhenFreezingIsCompletedAndHarvestingIsStartedWithFillTubeHeaterOnTimeZeroParametric()
   {
      GivenTheIceMakerThermistorIsValid();

      TheMotorShouldBeRequestedTo(Home);
      When TheModuleIsInitializedWithFillTubeHeaterOnTimeZeroParametric();

      TheMotorShouldBeRequestedTo(Idle);
      WhenTheMotorActionResultIs(Homed);

      When TheTemperatureIs(MaximumHarvestTemperaturex100 - 1);
      TheMotorShouldBeRequestedTo(Harvest);

      After(fillTubeHeaterOnTimeIsZeroParametric.freezeData.minimumFreezeTimeMinutes * MSEC_PER_MIN);
      TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Harvesting);
   }

   void WhenFreezingIsCompletedAndHarvestingIsStartedWithFillTubeHeaterDutyCycleZeroParametric()
   {
      GivenTheIceMakerThermistorIsValid();

      TheMotorShouldBeRequestedTo(Home);
      When TheModuleIsInitializedWithFillTubeHeaterDutyCycleZeroParametric();

      TheMotorShouldBeRequestedTo(Idle);
      WhenTheMotorActionResultIs(Homed);

      When TheTemperatureIs(MaximumHarvestTemperaturex100 - 1);
      TheMotorShouldBeRequestedTo(Harvest);

      After(fillTubeHeaterDutyCycleIsZeroParametric.freezeData.minimumFreezeTimeMinutes * MSEC_PER_MIN);
      TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Harvesting);
   }

   void FreezingIsCompletedAndHarvestingDoesNotStartBecauseFreezerDoorOpens()
   {
      TemperatureDegFx100_t actualTempx100 = MaximumHarvestTemperaturex100;
      Given TheTemperatureIs(actualTempx100);
      GivenHomingIsCompleted();

      TheFreezerDoorIs(OPEN);

      NothingShouldHappen();
      When TheTemperatureIs(actualTempx100 - 1);
      And After(TheTimeToReachIntegrationSumGiven(FreezingPointx100 - 1));
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
      After(parametric.fillTubeHeaterData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC);
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

   void MinimumFreezeTimeCounterInMinutesShouldBe(uint8_t expected)
   {
      uint8_t actual;
      DataModel_Read(dataModel, Erd_TwistTrayIceMaker_MinimumFreezeTimeCounterInMinutes, &actual);
      CHECK_EQUAL(expected, actual);
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

TEST(TwistTrayIceMaker, ShouldInitiallyTryToFreezeWhateverIsInTheTray)
{
   GivenTheIceMakerIsEnabled();
   GivenHomingIsCompleted();
   And TheTemperatureIs(VeryCold);

   SomeTimePasses(parametric.freezeData.minimumFreezeTimeMinutes * MSEC_PER_MIN - 1);

   HarvestingShouldStart();
   After(1);
}

TEST(TwistTrayIceMaker, ShouldFreezeForMinimumFreezeTimeIfIntegrationSumIsCompletedFirst)
{
   TemperatureDegFx100_t actualTempx100 = VeryCold;

   GivenTheIceMakerIsEnabled();
   GivenHomingIsCompleted();
   And TheTemperatureIs(actualTempx100);

   TimerTicks_t timeToReachIntegrationSum = TheTimeToReachIntegrationSumGiven(actualTempx100);
   NothingShouldHappen();
   After(timeToReachIntegrationSum);

   SomeTimePasses(parametric.freezeData.minimumFreezeTimeMinutes * MSEC_PER_MIN - timeToReachIntegrationSum - 1);

   HarvestingShouldStart();
   After(1);
}

TEST(TwistTrayIceMaker, ShouldResetFreezeIntegrationSumIfTempGoesAboveFreezing)
{
   // this test is based on two simultaneous periodic timers
   // so it is very touchy
   // if it is failing, you probably changed the intervals around
   TemperatureDegFx100_t belowFreezing = 3100;
   Given TheTemperatureIs(belowFreezing);
   GivenTheIceMakerIsEnabled();
   GivenHomingIsCompleted();

   SomeTimePasses(OneSecond);

   // make sure that the timers are actually going off at the very last tick of these After's
   TheTemperatureIs(FreezingPointx100 + 3000);

   TheTemperatureIs(belowFreezing);

   NothingShouldHappen();
   When SomeTimePasses(TheTimeToReachIntegrationSumGiven(belowFreezing) - 1);

   When TheTemperatureIs(MaximumHarvestTemperaturex100 - 1);

   HarvestingShouldStart();
   After(1);
}

TEST(TwistTrayIceMaker, ShouldResetMinimumFreezeTimeIfTempGoesAboveFreezing)
{
   TemperatureDegFx100_t actualTempx100 = VeryCold;
   Given TheTemperatureIs(actualTempx100);
   GivenTheIceMakerIsEnabled();
   GivenHomingIsCompleted();

   NothingShouldHappen();
   When SomeTimePasses(parametric.freezeData.minimumFreezeTimeMinutes * MSEC_PER_MIN / 5);

   TheTemperatureIs(FreezingPointx100 + 3000);

   TheTemperatureIs(actualTempx100);

   NothingShouldHappen();
   When SomeTimePasses(parametric.freezeData.minimumFreezeTimeMinutes * MSEC_PER_MIN - 1);

   HarvestingShouldStart();
   After(1);
}

TEST(TwistTrayIceMaker, ShouldNotResetFreezeIntegrationSumIfTempChangesButDoesNotGoAboveFreezing)
{
   TemperatureDegFx100_t actualTempx100 = FreezingPointx100 - 1;
   GivenTheIceMakerIsEnabled();
   And TheTemperatureIs(actualTempx100);
   GivenHomingIsCompleted();

   SomeTimePasses(TheTimeToReachIntegrationSumGiven(actualTempx100) - 1);

   TheTemperatureIs(MaximumHarvestTemperaturex100 - 1);

   HarvestingShouldStart();
   After(1);
}

TEST(TwistTrayIceMaker, ShouldNotResetMinimumFreezeTimeIfTempChangesButDoesNotGoAboveFreezing)
{
   TemperatureDegFx100_t actualTempx100 = VeryCold;
   Given TheTemperatureIs(actualTempx100);
   GivenTheIceMakerIsEnabled();
   GivenHomingIsCompleted();

   SomeTimePasses(parametric.freezeData.minimumFreezeTimeMinutes * MSEC_PER_MIN - 1);

   TheTemperatureIs(actualTempx100 - 1);

   HarvestingShouldStart();
   After(1);
}

TEST(TwistTrayIceMaker, ShouldNotStartIntegrationSumOrMinimumFreezeTimerIfTempIsAboveFreezingToBeginWith)
{
   GivenTheIceMakerIsEnabled();
   Given TheTemperatureIs(FreezingPointx100 + 1000);
   GivenHomingIsCompleted();

   NothingShouldHappen();
   After(ALongTime);

   TheTemperatureIs(FreezingPointx100 / 3);

   SomeTimePasses(IceTemperaturePollingTime);

   HarvestingShouldStart();
   When SomeTimePasses(parametric.freezeData.minimumFreezeTimeMinutes * MSEC_PER_MIN);
}

TEST(TwistTrayIceMaker, ShouldNotTransitionToHarvestIfThermistorTemperatureIsNotBelowMaximumHarvestTemperature)
{
   GivenTheIceMakerIsEnabled();
   GivenHomingIsCompleted();

   NothingShouldHappen();
   When TheTemperatureIs(FreezingPointx100 - 1);
   After(TheTimeToReachIntegrationSumGiven(FreezingPointx100 - 1));
   After(parametric.freezeData.minimumFreezeTimeMinutes * MSEC_PER_MIN);
   And TheTemperatureIs(MaximumHarvestTemperaturex100);

   HarvestingShouldStart();
   When TheTemperatureIs(MaximumHarvestTemperaturex100 - 1);
}

TEST(TwistTrayIceMaker, ShouldIncrementFreezeTimeCounterUntilMinimumFreezeTimeIsSatisfied)
{
   Given TheTemperatureIs(TemperatureThatCausesFreezeIntegrationLimitToBeReachedInMinimumFreezeTimeInMinutes);
   GivenTheIceMakerIsEnabled();
   GivenHomingIsCompleted();

   for(uint8_t i = 1; i <= parametric.freezeData.minimumFreezeTimeMinutes; i++)
   {
      After(MSEC_PER_MIN - 1);
      MinimumFreezeTimeCounterInMinutesShouldBe(i - 1);
      After(1);
      MinimumFreezeTimeCounterInMinutesShouldBe(i);
   }
}

TEST(TwistTrayIceMaker, ShouldIncrementFreezeTimeCounterIfFreezingAfterMinimumFreezeTimeIsSatisfied)
{
   Given TheTemperatureIs(TemperatureBelowFreezeIntegrationTemperature);
   GivenTheIceMakerIsEnabled();
   GivenHomingIsCompleted();

   After(parametric.freezeData.minimumFreezeTimeMinutes * MSEC_PER_MIN);
   MinimumFreezeTimeCounterInMinutesShouldBe(MinimumFreezeTimeInMinutes);

   After(3 * MSEC_PER_MIN);
   MinimumFreezeTimeCounterInMinutesShouldBe(MinimumFreezeTimeInMinutes + 3);
}

TEST(TwistTrayIceMaker, ShouldNotExceedUINT8_MAXForFreezeTimeCounter)
{
   Given TheTemperatureIs(parametric.freezeData.startIntegrationTemperatureInDegFx100);
   GivenTheIceMakerIsEnabled();
   GivenHomingIsCompleted();

   After(UINT8_MAX * MSEC_PER_MIN);
   MinimumFreezeTimeCounterInMinutesShouldBe(UINT8_MAX);

   After(1 * MSEC_PER_MIN);
   MinimumFreezeTimeCounterInMinutesShouldBe(UINT8_MAX);
}

TEST(TwistTrayIceMaker, ShouldClearFreezeTimeCounterIfIceMakerTemperatureExceedsStartIntegrationTemperature)
{
   Given TheTemperatureIs(parametric.freezeData.startIntegrationTemperatureInDegFx100);
   GivenTheIceMakerIsEnabled();
   GivenHomingIsCompleted();

   After(1 * MSEC_PER_MIN);
   MinimumFreezeTimeCounterInMinutesShouldBe(1);

   When TheTemperatureIs(parametric.freezeData.startIntegrationTemperatureInDegFx100 + 1);
   MinimumFreezeTimeCounterInMinutesShouldBe(0);
}

TEST(TwistTrayIceMaker, ShouldStopIfThereIsAThermistorFaultAndBeginPollingThermistorIndefinitely)
{
   GivenTheIceMakerIsEnabled();
   Given TheTemperatureIs(VeryCold);
   GivenHomingIsCompleted();

   NothingShouldHappen();
   When SomeTimePasses(parametric.freezeData.minimumFreezeTimeMinutes * MSEC_PER_MIN / 5);

   WhenTheIceMakerThermistorIsInvalid();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_ThermistorFault);

   After(IceTemperaturePollingTime);

   NothingShouldHappen();
   After(ALongTime);

   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_ThermistorFault);
}

TEST(TwistTrayIceMaker, ShouldTransitionFromFreezeToIdleFreezeWhenIceMakerBecomesDisabled)
{
   GivenTheIceMakerIsEnabled();
   Given TheTemperatureIs(VeryCold);
   GivenHomingIsCompleted();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Freeze);

   WhenTheIceMakerBecomesDisabled();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_IdleFreeze);
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
   After(FullIceBucketWaitTime - 1);

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

   After(FullBucketDispenseCheckTimeInMsec - 1);
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

   After(FullBucketDispenseCheckTimeInMsec - 1);
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

   After(FullBucketDispenseCheckTimeInMsec - 1);
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
   After(WaterFillTime - 1);

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
   After(WaterFillTime);
}

TEST(TwistTrayIceMaker, ShouldGoToThermistorFaultStateWhenEnteringFreezeStateIfThermistorIsInvalid)
{
   GivenTheIceMakerIsEnabled();
   GivenFillingHasStartedAfterCompletingFreezingAndHarvesting();

   WhenTheIceMakerThermistorIsInvalid();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);

   TheWaterValveShouldBecome(CLOSED);
   After(WaterFillTime);

   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_ThermistorFault);
}

TEST(TwistTrayIceMaker, ShouldGoToIdleFreezeStateWhenEnteringFreezeStateIfIceMakerIsDisabled)
{
   GivenTheIceMakerIsEnabled();
   GivenFillingHasStartedAfterCompletingFreezingAndHarvesting();

   WhenTheIceMakerBecomesDisabled();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);

   TheWaterValveShouldBecome(CLOSED);
   After(WaterFillTime);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_IdleFreeze);
}

TEST(TwistTrayIceMaker, ShouldTransitionToIdleFreezeAfterFillingIsFinishedWhileSabbathIsEnabled)
{
   GivenTheIceMakerIsEnabled();
   GivenFillingHasStartedAfterCompletingFreezingAndHarvesting();

   When SabbathModeIs(ON);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);

   TheWaterValveShouldBecome(CLOSED);
   After(WaterFillTime);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_IdleFreeze);
}

TEST(TwistTrayIceMaker, ShouldBeAbleToHarvestTwice)
{
   GivenTheIceMakerIsEnabled();
   GivenFillingHasStartedAfterCompletingFreezingAndHarvesting();

   TheWaterValveShouldBecome(CLOSED);
   FreezerTriggerIceRateSignalShouldIncrement();
   After(WaterFillTime);

   HarvestingShouldStart();
   When TheTemperatureIs(VeryCold);
   And After(parametric.freezeData.minimumFreezeTimeMinutes * MSEC_PER_MIN);
}

TEST(TwistTrayIceMaker, ShouldWaitToHarvestUntilDoorIsClosed)
{
   GivenTheIceMakerIsEnabled();
   FreezingIsCompletedAndHarvestingDoesNotStartBecauseFreezerDoorOpens();

   NothingShouldHappen();
   After(ALongTime);

   TheFreezerDoorIs(CLOSED);

   NothingShouldHappen();
   After(DelayToHarvestAfterDoorCloses - 1);

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
   After(ALongTime);

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
   After(ALongTime);

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
   After(ALongTime);

   Then SabbathModeIs(OFF);

   NothingShouldHappen();
   After(ALongTime);

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

   FillingShouldStart();
   WhenTheTestRequestIs(IceMakerTestRequest_Fill);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);
   And TheTestRequestShouldBe(IceMakerTestRequest_None);
}

TEST(TwistTrayIceMaker, ShouldTransitionToHarvestingAndClearTestRequestWhenTestRequestIsHarvestWhileIceMakerStateIsInFreeze)
{
   GivenTheOperationStateIsInFreeze();

   HarvestingShouldStart();
   WhenTheTestRequestIs(IceMakerTestRequest_Harvest);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Harvesting);
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
   FillTubeHeaterVoteAndCareShouldBecome(parametric.fillTubeHeaterData.freezeThawFillTubeHeaterDutyCyclePercentage, Vote_Care);
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
   FillTubeHeaterVoteAndCareShouldBecome(parametric.fillTubeHeaterData.freezeThawFillTubeHeaterDutyCyclePercentage, Vote_Care);
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
   FillTubeHeaterVoteAndCareShouldBe(parametric.fillTubeHeaterData.freezeThawFillTubeHeaterDutyCyclePercentage, Vote_Care);

   After(parametric.fillTubeHeaterData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);
   FillTubeHeaterVoteAndCareShouldBe(parametric.fillTubeHeaterData.freezeThawFillTubeHeaterDutyCyclePercentage, Vote_Care);

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

TEST(TwistTrayIceMaker, ShouldNotStartFillTubeHeaterWhenFillTubeHeaterOnTimeIsZero)
{
   GivenTheIceMakerIsEnabled();

   FillTubeHeaterVoteAndCareShouldBecome(parametric.fillTubeHeaterData.freezeThawFillTubeHeaterDutyCyclePercentage, Vote_Care);
   FillTubeHeaterVoteAndCareShouldBecome(OFF, Vote_DontCare);
   WhenFreezingIsCompletedAndHarvestingIsStartedWithFillTubeHeaterOnTimeZeroParametric();

   FillingShouldStart();
   TheMotorShouldBeRequestedTo(Idle);
   WhenTheMotorActionResultIs(Harvested);
}

TEST(TwistTrayIceMaker, ShouldNotStartFillTubeHeaterWhenFillTubeHeaterDutyCycleIsZero)
{
   GivenTheIceMakerIsEnabled();

   FillTubeHeaterVoteAndCareShouldBecome(OFF, Vote_Care);
   WhenFreezingIsCompletedAndHarvestingIsStartedWithFillTubeHeaterDutyCycleZeroParametric();

   FillingShouldStart();
   TheMotorShouldBeRequestedTo(Idle);
   FillTubeHeaterVoteAndCareShouldBecome(OFF, Vote_DontCare);
   WhenTheMotorActionResultIs(Harvested);
}
