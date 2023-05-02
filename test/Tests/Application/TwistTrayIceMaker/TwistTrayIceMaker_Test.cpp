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
   MinimumFreezeTime = 1 * MSEC_PER_MIN,
   IntegrationInterval = 1 * MSEC_PER_SEC,
   FullIceBucketWaitTime = 3 * SECONDS_PER_HOUR * MSEC_PER_SEC,
   DelayToHarvestAfterDoorCloses = 3 * MSEC_PER_MIN,
   IceTemperaturePollingTime = 3 * MSEC_PER_SEC,
   FullBucketDispenseCheckTimeInSec = 12,
   FullBucketDispenseCheckTimeInMsec = FullBucketDispenseCheckTimeInSec * MSEC_PER_SEC,

   TargetFreezeIntegrationSum = 16200,

   ALongTime = FullIceBucketWaitTime + 1,
   OneSecond = 1 * MSEC_PER_SEC,
};

static const TwistTrayIceMakerData_t parametric = {
   .freezeData{
      .maximumHarvestTemperatureInDegFx100 = MaximumHarvestTemperaturex100,
      .startIntegrationTemperatureInDegFx100 = FreezingPointx100,
      .targetFreezeIntegrationSum = TargetFreezeIntegrationSum,
      .minimumFreezeTimeMinutes = MinimumFreezeTime / MSEC_PER_MIN,
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
   .fillTubeHeaterData = {
      .freezeThawFillTubeHeaterDutyCyclePercentage = 0,
      .freezeThawFillTubeHeaterOnTimeInSeconds = 400,
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
   { Erd_TwistTrayIceMaker_MinimumFreezeTimerRemainingTimeInMsec, sizeof(TimerTicks_t) },
   { Erd_TwistTrayIceMaker_MinimumFreezeTimerRemainingTimeRequest, sizeof(uint8_t) },
   { Erd_TwistTrayIceMaker_OperationState, sizeof(uint8_t) },
   { Erd_TwistTrayIceMaker_HighLevelState, sizeof(uint8_t) },
   { Erd_TwistTrayIceMaker_MotorFaultActive, sizeof(bool) },
   { Erd_SabbathMode, sizeof(bool) },
   { Erd_EnhancedSabbathMode, sizeof(bool) },
   { Erd_LeftSideFreezerDoorStatus, sizeof(bool) },
   { Erd_TwistTrayIceMakerTestRequest, sizeof(IceMakerTestRequest_t) },
   { Erd_FreezerIceRateTriggerSignal, sizeof(Signal_t) },
   { Erd_IceMakerEnabledOverrideResolved, sizeof(bool) }
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
   else if(args->erd == Erd_TwistTrayIceMaker_MinimumFreezeTimerRemainingTimeInMsec)
   {
      REINTERPRET(data, args->data, const TimerTicks_t *);

      mock()
         .actualCall("Write Minimum Freeze Time")
         .onObject(context)
         .withParameter("Erd", args->erd)
         .withParameter("Data", *data);
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
         Erd_IceMakerEnabledOverrideResolved,
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
         Erd_IceMakerEnabledOverrideResolved,
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

   void TheMotorActionResultIs(TwistTrayIceMakerMotorActionResult_t motorActionResult)
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

   void TheMinimumFreezeTimerRemainingTimeShouldBecome(TimerTicks_t expectedTimeRemaining)
   {
      mock()
         .expectOneCall("Write Minimum Freeze Time")
         .onObject(dataModel)
         .withParameter("Erd", Erd_TwistTrayIceMaker_MinimumFreezeTimerRemainingTimeInMsec)
         .withParameter("Data", expectedTimeRemaining);
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

   void TheMinimumFreezeTimerRemainingTimeIsRequested()
   {
      Signal_SendViaErd(DataModel_AsDataSource(dataModel), Erd_TwistTrayIceMaker_MinimumFreezeTimerRemainingTimeRequest);
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

   void HomingIsCompleted()
   {
      GivenTheIceMakerThermistorIsValid();

      TheMotorShouldBeRequestedTo(Home);
      When TheModuleIsInitialized();

      TheMotorShouldBeRequestedTo(Idle);
      When TheMotorActionResultIs(Homed);
   }

   void FreezingIsCompletedAndHarvestingIsStarted()
   {
      GivenTheIceMakerThermistorIsValid();
      And HomingIsCompleted();

      When TheTemperatureIs(MaximumHarvestTemperaturex100 - 1);
      HarvestingShouldStart();
      After(TheTimeToReachIntegrationSumGiven(FreezingPointx100 - 1));
   }

   void FreezingIsCompletedAndHarvestingDoesNotStartBecauseFreezerDoorOpens()
   {
      TemperatureDegFx100_t actualTempx100 = MaximumHarvestTemperaturex100;
      Given TheTemperatureIs(actualTempx100);
      And HomingIsCompleted();

      TheFreezerDoorIs(OPEN);

      NothingShouldHappen();
      When TheTemperatureIs(actualTempx100 - 1);
      And After(TheTimeToReachIntegrationSumGiven(FreezingPointx100 - 1));
   }

   void HarvestingIsCompletedAndFillingIsStarted()
   {
      FillingShouldStart();
      TheMotorShouldBeRequestedTo(Idle);
      When TheMotorActionResultIs(Harvested);
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
      And TheTemperatureIs(VeryCold);
      And HomingIsCompleted();
      TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_IdleFreeze);
   }

   void GivenTheOperationStateIsInFreeze(void)
   {
      GivenTheOperationStateIsInHoming();

      TheMotorShouldBeRequestedTo(Idle);
      GivenTheIceMakerIsEnabled();
      When TheMotorActionResultIs(Homed);
      TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Freeze);
   }

   void GivenTheOperationStateIsInHarvesting(void)
   {
      GivenTheIceMakerIsEnabled();
      Given FreezingIsCompletedAndHarvestingIsStarted();
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
      TheHighLevelStateShouldBecome(FaultState);
      TheMotorFaultShouldBecome(ACTIVE);
      When TheMotorActionResultIs(MotorError);
      TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_MotorError);
   }

   void GivenTheOperationStateIsInBucketIsFull(void)
   {
      GivenTheOperationStateIsInHarvesting();

      TheMotorShouldBeRequestedTo(Idle);
      When TheMotorActionResultIs(BucketWasFull);
      TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_BucketIsFull);
   }

   void GivenFillingHasStartedAfterCompletingFreezingAndHarvesting(void)
   {
      FreezingIsCompletedAndHarvestingIsStarted();
      HarvestingIsCompletedAndFillingIsStarted();
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
   When TheMotorActionResultIs(Homed);
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
   TheMotorActionResultIs(Homed);
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
   And TheTemperatureIs(VeryCold);
   And HomingIsCompleted();

   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_IdleFreeze);
}

TEST(TwistTrayIceMaker, ShouldTransitionFromIdleFreezeStateToFreezeStateWhenIceMakerIsEnabled)
{
   GivenTheIceMakerIsDisabled();
   And TheTemperatureIs(VeryCold);
   And HomingIsCompleted();

   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_IdleFreeze);

   WhenTheIceMakerBecomesEnabled();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Freeze);
}

// Freezing

TEST(TwistTrayIceMaker, ShouldInitiallyTryToFreezeWhateverIsInTheTray)
{
   GivenTheIceMakerIsEnabled();
   And HomingIsCompleted();
   And TheTemperatureIs(VeryCold);

   SomeTimePasses(MinimumFreezeTime - 1);

   HarvestingShouldStart();
   After(1);
}

TEST(TwistTrayIceMaker, ShouldFreezeForMinimumFreezeTimeIfIntegrationSumIsCompletedFirst)
{
   TemperatureDegFx100_t actualTempx100 = VeryCold;

   GivenTheIceMakerIsEnabled();
   And HomingIsCompleted();
   And TheTemperatureIs(actualTempx100);

   TimerTicks_t timeToReachIntegrationSum = TheTimeToReachIntegrationSumGiven(actualTempx100);
   NothingShouldHappen();
   After(timeToReachIntegrationSum);

   SomeTimePasses(MinimumFreezeTime - timeToReachIntegrationSum - 1);

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
   And HomingIsCompleted();

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
   And HomingIsCompleted();

   NothingShouldHappen();
   When SomeTimePasses(MinimumFreezeTime / 5);

   TheTemperatureIs(FreezingPointx100 + 3000);

   TheTemperatureIs(actualTempx100);

   NothingShouldHappen();
   When SomeTimePasses(MinimumFreezeTime - 1);

   HarvestingShouldStart();
   After(1);
}

TEST(TwistTrayIceMaker, ShouldNotResetFreezeIntegrationSumIfTempChangesButDoesNotGoAboveFreezing)
{
   TemperatureDegFx100_t actualTempx100 = FreezingPointx100 - 1;
   GivenTheIceMakerIsEnabled();
   And TheTemperatureIs(actualTempx100);
   And HomingIsCompleted();

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
   And HomingIsCompleted();

   SomeTimePasses(MinimumFreezeTime - 1);

   TheTemperatureIs(actualTempx100 - 1);

   HarvestingShouldStart();
   After(1);
}

TEST(TwistTrayIceMaker, ShouldNotStartIntegrationSumOrMinimumFreezeTimerIfTempIsAboveFreezingToBeginWith)
{
   GivenTheIceMakerIsEnabled();
   Given TheTemperatureIs(FreezingPointx100 + 1000);
   And HomingIsCompleted();

   NothingShouldHappen();
   After(ALongTime);

   TheTemperatureIs(FreezingPointx100 / 3);

   SomeTimePasses(IceTemperaturePollingTime);

   HarvestingShouldStart();
   SomeTimePasses(MinimumFreezeTime);
}

TEST(TwistTrayIceMaker, ShouldNotTransitionToHarvestIfThermistorTemperatureIsNotBelowMaximumHarvestTemperature)
{
   GivenTheIceMakerIsEnabled();
   And HomingIsCompleted();

   NothingShouldHappen();
   When TheTemperatureIs(FreezingPointx100 - 1);
   After(TheTimeToReachIntegrationSumGiven(FreezingPointx100 - 1));
   After(MinimumFreezeTime);
   And TheTemperatureIs(MaximumHarvestTemperaturex100);

   HarvestingShouldStart();
   When TheTemperatureIs(MaximumHarvestTemperaturex100 - 1);
}

TEST(TwistTrayIceMaker, ShouldReturnTheCorrectTimeForTheMinimumFreezeTimerRemainingTime)
{
   TemperatureDegFx100_t actualTempx100 = 3100;
   Given TheTemperatureIs(actualTempx100);
   GivenTheIceMakerIsEnabled();
   And HomingIsCompleted();

   TheMinimumFreezeTimerRemainingTimeShouldBecome(MinimumFreezeTime);
   When TheMinimumFreezeTimerRemainingTimeIsRequested();

   SomeTimePasses(34);

   TheMinimumFreezeTimerRemainingTimeShouldBecome(MinimumFreezeTime - 34);
   When TheMinimumFreezeTimerRemainingTimeIsRequested();
}

TEST(TwistTrayIceMaker, ShouldStopIfThereIsAThermistorFaultAndBeginPollingThermistorIndefinitely)
{
   GivenTheIceMakerIsEnabled();
   Given TheTemperatureIs(VeryCold);
   And HomingIsCompleted();

   NothingShouldHappen();
   When SomeTimePasses(MinimumFreezeTime / 5);

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
   And HomingIsCompleted();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Freeze);

   WhenTheIceMakerBecomesDisabled();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_IdleFreeze);
}

// Harvesting

TEST(TwistTrayIceMaker, ShouldTryToHarvestWhateverIsInTheTrayOnInitAfterTryingToFreezeIt)
{
   GivenTheIceMakerIsEnabled();
   Given FreezingIsCompletedAndHarvestingIsStarted();

   TheMotorShouldBeRequestedTo(Idle);
   FillingShouldStart();
   When TheMotorActionResultIs(Harvested);
}

TEST(TwistTrayIceMaker, ShouldTransitionFromHarvestToIdleFreezeWhenHarvestCompletesAndIceMakerIsDisabled)
{
   GivenTheIceMakerIsEnabled();
   Given FreezingIsCompletedAndHarvestingIsStarted();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Harvesting);

   WhenTheIceMakerBecomesDisabled();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Harvesting);

   TheMotorShouldBeRequestedTo(Idle);
   When TheMotorActionResultIs(Harvested);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_IdleFreeze);
}

TEST(TwistTrayIceMaker, ShouldTryToHarvestIceAgainAfterEnoughTimeHasPassedSinceTheBucketWasFull)
{
   GivenTheIceMakerIsEnabled();
   Given FreezingIsCompletedAndHarvestingIsStarted();

   TheMotorShouldBeRequestedTo(Idle);
   When TheMotorActionResultIs(BucketWasFull);

   NothingShouldHappen();
   After(FullIceBucketWaitTime - 1);

   HarvestingShouldStart();
   After(1);
}

TEST(TwistTrayIceMaker, ShouldTransitionFromHarvestToIdleFreezeWhenBucketIsFullAndIceMakerIsDisabled)
{
   GivenTheIceMakerIsEnabled();
   Given FreezingIsCompletedAndHarvestingIsStarted();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Harvesting);

   WhenTheIceMakerBecomesDisabled();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Harvesting);

   TheMotorShouldBeRequestedTo(Idle);
   When TheMotorActionResultIs(BucketWasFull);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_IdleFreeze);
}

TEST(TwistTrayIceMaker, ShouldTryToHarvestIceAfterIceHasBeenDispensedLongEnough)
{
   GivenTheIceMakerIsEnabled();
   Given FreezingIsCompletedAndHarvestingIsStarted();
   TheMotorShouldBeRequestedTo(Idle);
   Then TheMotorActionResultIs(BucketWasFull);

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
   Given FreezingIsCompletedAndHarvestingIsStarted();
   TheMotorShouldBeRequestedTo(Idle);
   Then TheMotorActionResultIs(BucketWasFull);

   WhenIceStartsDispensing();

   After(FullBucketDispenseCheckTimeInMsec - 1);
   NothingShouldHappen();

   NothingShouldHappen();
   WhenIceStopsDispensing();
}

TEST(TwistTrayIceMaker, ShouldNotTryToHarvestIceAfterWaterHasBeenDispensedInsteadOfIce)
{
   GivenTheIceMakerIsEnabled();
   Given FreezingIsCompletedAndHarvestingIsStarted();
   TheMotorShouldBeRequestedTo(Idle);
   Then TheMotorActionResultIs(BucketWasFull);

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
   And After(MinimumFreezeTime);
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
   Given FreezingIsCompletedAndHarvestingIsStarted();

   WhenTheIceMakerThermistorIsInvalid();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Harvesting);
}

TEST(TwistTrayIceMaker, ShouldTransitionToThermistorFaultStateWhenInBucketIsFullStateAndThermistorBecomesInvalid)
{
   GivenTheIceMakerIsEnabled();
   Given FreezingIsCompletedAndHarvestingIsStarted();
   TheMotorShouldBeRequestedTo(Idle);
   Then TheMotorActionResultIs(BucketWasFull);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_BucketIsFull);

   WhenTheIceMakerThermistorIsInvalid();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_ThermistorFault);
}

TEST(TwistTrayIceMaker, ShouldTransitionToIdleFreezeStateWhenIceMakerBecomesDisabledWhileInBucketIsFullState)
{
   GivenTheIceMakerIsEnabled();
   Given FreezingIsCompletedAndHarvestingIsStarted();
   TheMotorShouldBeRequestedTo(Idle);
   Then TheMotorActionResultIs(BucketWasFull);
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
   And HomingIsCompleted();
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
   And HomingIsCompleted();
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
   And HomingIsCompleted();
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
   Given FreezingIsCompletedAndHarvestingIsStarted();

   SabbathModeIs(ON);

   TheMotorShouldBeRequestedTo(Idle);
   When TheMotorActionResultIs(Harvested);
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
   When TheMotorActionResultIs(MotorError);
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
   When TheMotorActionResultIs(MotorError);
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_MotorError);
}

TEST(TwistTrayIceMaker, ShouldGoToErrorStateIfAHarvestMovementResultsInErrorAndIceMakerIsDisabled)
{
   GivenTheIceMakerIsEnabled();
   Given FreezingIsCompletedAndHarvestingIsStarted();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Harvesting);

   WhenTheIceMakerBecomesDisabled();
   TwistTrayIceMakerOperationalStateShouldBe(TwistTrayIceMakerOperationState_Harvesting);

   TheMotorShouldBeRequestedTo(Idle);
   TheHighLevelStateShouldBecome(FaultState);
   TheMotorFaultShouldBecome(ACTIVE);
   When TheMotorActionResultIs(MotorError);
}

TEST(TwistTrayIceMaker, ShouldGoToErrorStateIfAHarvestMovementResultsInError)
{
   GivenTheIceMakerIsEnabled();
   Given FreezingIsCompletedAndHarvestingIsStarted();

   TheMotorShouldBeRequestedTo(Idle);
   TheHighLevelStateShouldBecome(FaultState);
   TheMotorFaultShouldBecome(ACTIVE);
   When TheMotorActionResultIs(MotorError);
}

TEST(TwistTrayIceMaker, ShouldNotTransitionToThermistorFaultStateWhenThermistorBecomesInvalidFromTheMotorErrorState)
{
   GivenTheIceMakerIsEnabled();
   Given FreezingIsCompletedAndHarvestingIsStarted();

   TheMotorShouldBeRequestedTo(Idle);
   TheHighLevelStateShouldBecome(FaultState);
   TheMotorFaultShouldBecome(ACTIVE);
   Then TheMotorActionResultIs(MotorError);
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
