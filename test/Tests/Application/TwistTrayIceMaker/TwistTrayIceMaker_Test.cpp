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

   FreezingPointx100 = 3200,

   WaterFillTime = ((51 * MSEC_PER_SEC) / 10),
   MinimumFreezeTime = 1 * MSEC_PER_MIN,
   IntegrationInterval = 1 * MSEC_PER_SEC,
   FullIceBucketWaitTime = 3 * SECONDS_PER_HOUR * MSEC_PER_SEC,
   DelayToHarvestAfterDoorCloses = 3 * MSEC_PER_MIN,
   IceTemperaturePollingTime = 3 * MSEC_PER_SEC,

   TargetFreezeIntegrationSum = 16200,

   ALongTime = FullIceBucketWaitTime + 1,
   OneSecond = 1 * MSEC_PER_SEC,

   OutOfRangeHigh = 20000,
   OutOfRangeLow = -4100 - 1,
};

static const TwistTrayIceMakerData_t parametric = {
   .freezeData{
      .freezingTemperatureDegreesFx100 = FreezingPointx100,
      .targetFreezeIntegrationSum = TargetFreezeIntegrationSum,
      .minimumFreezeTimeMinutes = MinimumFreezeTime / MSEC_PER_MIN },

   .harvestData{
      .fullBucketWaitPeriodMinutes = FullIceBucketWaitTime / MSEC_PER_MIN,
      .fullBucketDetectionPeriodSecX10 = 0,
      .initialHomingTwistPeriodSecX10 = 0,
      .homeLandingDelayPeriodSecX10 = 0,
      .longMotorErrorTimeoutPeriodSec = 0,
      .shortMotorErrorTimeoutPeriodSec = 0,
      .delayToHarvestAfterDoorClosesSeconds = DelayToHarvestAfterDoorCloses / MSEC_PER_SEC },

   .fillData{
      .waterFillTimeSecX10 = WaterFillTime / 100 }
};

static const DataModel_TestDoubleConfigurationEntry_t erdDefinitions[] = {
   { Erd_TwistTrayIceMakerMotor_IceMakerVote, sizeof(TwistTrayIceMakerMotorVotedAction_t) },
   { Erd_TwistTrayIceMaker_MotorActionResult, sizeof(uint8_t) },
   { Erd_TwistTrayIceMaker_FilteredTemperatureInDegFx100, sizeof(TemperatureDegFx100_t) },
   { Erd_TwistTrayIceMaker_ThermistorIsValid, sizeof(bool) },
   { Erd_TwistTrayIceMakerWaterValve_IceMakerVote, sizeof(WaterValveVotedState_t) },
   { Erd_IsolationWaterValve_TwistTrayIceMakerVote, sizeof(WaterValveVotedState_t) },
   { Erd_TwistTrayIceMaker_IceDispenserState, sizeof(bool) },
   { Erd_TwistTrayIceMaker_FreezeIntegrationCount, sizeof(uint32_t) },
   { Erd_TwistTrayIceMaker_MinimumFreezeTimerRemainingTimeInMsec, sizeof(TimerTicks_t) },
   { Erd_TwistTrayIceMaker_MinimumFreezeTimerRemainingTimeRequest, sizeof(uint8_t) },
   { Erd_TwistTrayIceMaker_OperationState, sizeof(uint8_t) },
   { Erd_TwistTrayIceMaker_HighLevelState, sizeof(uint8_t) },
   { Erd_TwistTrayIceMaker_MotorFaultActive, sizeof(bool) },
   { Erd_SabbathMode, sizeof(bool) },
   { Erd_EnhancedSabbathMode, sizeof(bool) },
   { Erd_FreezerDoorIsOpen, sizeof(bool) },
   { Erd_TwistTrayIceMaker_ForceHarvest, sizeof(bool) },
   { Erd_FreezerIceRateTriggerSignal, sizeof(Signal_t) }
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
      DataModel_Write(dataModel, Erd_TwistTrayIceMaker_FilteredTemperatureInDegFx100, &temp);
      DataModel_Write(dataModel, Erd_TwistTrayIceMaker_ThermistorIsValid, set);
   }

   void TheTemperatureIsInvalid()
   {
      DataModel_Write(dataModel, Erd_TwistTrayIceMaker_ThermistorIsValid, clear);
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

   void TheMotorActionResultIs(uint8_t motorActionResult)
   {
      DataModel_Write(dataModel, Erd_TwistTrayIceMaker_MotorActionResult, &motorActionResult);
   }

   void TheFreezerDoorIs(bool freezerDoorState)
   {
      DataModel_Write(dataModel, Erd_FreezerDoorIsOpen, &freezerDoorState);
   }

   void TheWaterValveShouldBecome(bool state)
   {
      mock()
         .expectOneCall("Write Water Valve")
         .onObject(dataModel)
         .withParameter("Erd", Erd_TwistTrayIceMakerWaterValve_IceMakerVote)
         .withParameter("Data", state);
   }

   void TheIceDispenserBecomes(bool state)
   {
      DataModel_Write(dataModel, Erd_TwistTrayIceMaker_IceDispenserState, &state);
   }

   void SabbathModeIs(bool state)
   {
      DataModel_Write(dataModel, Erd_SabbathMode, &state);
   }

   void EnhancedSabbathModeIs(bool state)
   {
      DataModel_Write(dataModel, Erd_EnhancedSabbathMode, &state);
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
      TimerModule_TestDouble_ElapseTime(&timerModuleTestDouble, time, 1);
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
      TheMotorShouldBeRequestedTo(Home);
      When TheModuleIsInitialized();

      TheMotorShouldBeRequestedTo(Idle);
      When TheMotorActionResultIs(Homed);
   }

   void FreezingIsCompletedAndHarvestingIsStarted()
   {
      TemperatureDegFx100_t actualTempx100 = 3100;
      Given TheTemperatureIs(actualTempx100);
      And HomingIsCompleted();

      HarvestingShouldStart();
      After(TheTimeToReachIntegrationSumGiven(actualTempx100));
   }

   void FreezingIsCompletedAndHarvestingDoesNotStartBecauseFreezerDoorOpens()
   {
      TemperatureDegFx100_t actualTempx100 = 3100;
      Given TheTemperatureIs(actualTempx100);
      And HomingIsCompleted();

      TheFreezerDoorIs(OPEN);

      NothingShouldHappen();
      After(TheTimeToReachIntegrationSumGiven(actualTempx100));
   }

   void HarvestingIsCompletedAndFillingIsStarted()
   {
      FillingShouldStart();
      TheMotorShouldBeRequestedTo(Idle);
      When TheMotorActionResultIs(Harvested);
   }

   void NothingShouldHappen()
   {
   }
};

TEST(TwistTrayIceMaker, ShouldHomeOnInitializationAndNotIncrementFreezerIceRateSignal)
{
   TheMotorShouldBeRequestedTo(Home);
   When TheModuleIsInitialized();

   TheMotorShouldBeRequestedTo(Idle);
   FreezerTriggerIceRateSignalShouldNotIncrement();
   When TheMotorActionResultIs(Homed);
}

// Freezing

TEST(TwistTrayIceMaker, ShouldInitiallyTryToFreezeWhateverIsInTheTray)
{
   Given TheTemperatureIs(200);
   And HomingIsCompleted();

   SomeTimePasses(MinimumFreezeTime - 1);

   HarvestingShouldStart();
   After(1);
}

TEST(TwistTrayIceMaker, ShouldFreezeForMinimumFreezeTimeIfIntegrationSumIsCompletedFirst)
{
   TemperatureDegFx100_t actualTempx100 = 200;
   Given TheTemperatureIs(actualTempx100);
   And HomingIsCompleted();

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
   And HomingIsCompleted();

   SomeTimePasses(OneSecond);

   // make sure that the timers are actually going off at the very last tick of these After's
   TheTemperatureIs(FreezingPointx100 + 3000);
   After(IceTemperaturePollingTime - OneSecond);
   TheTemperatureIs(belowFreezing);
   After(IceTemperaturePollingTime);

   NothingShouldHappen();
   When SomeTimePasses(TheTimeToReachIntegrationSumGiven(belowFreezing) - 1);

   HarvestingShouldStart();
   After(1);
}

TEST(TwistTrayIceMaker, ShouldResetMinimumFreezeTimeIfTempGoesAboveFreezing)
{
   TemperatureDegFx100_t actualTempx100 = 200;
   Given TheTemperatureIs(actualTempx100);
   And HomingIsCompleted();

   NothingShouldHappen();
   When SomeTimePasses(MinimumFreezeTime / 5);

   TheTemperatureIs(FreezingPointx100 + 3000);
   After(IceTemperaturePollingTime);
   TheTemperatureIs(actualTempx100);
   After(IceTemperaturePollingTime);

   NothingShouldHappen();
   When SomeTimePasses(MinimumFreezeTime - 1);

   HarvestingShouldStart();
   After(1);
}

TEST(TwistTrayIceMaker, ShouldNotResetFreezeIntegrationSumIfTempChangesButDoesNotGoAboveFreezing)
{
   TemperatureDegFx100_t actualTempx100 = 3100;
   Given TheTemperatureIs(actualTempx100);
   And HomingIsCompleted();

   SomeTimePasses(TheTimeToReachIntegrationSumGiven(actualTempx100) - 1);

   TheTemperatureIs(actualTempx100 - 1);

   HarvestingShouldStart();
   After(1);
}

TEST(TwistTrayIceMaker, ShouldNotResetMinimumFreezeTimeIfTempChangesButDoesNotGoAboveFreezing)
{
   TemperatureDegFx100_t actualTempx100 = 200;
   Given TheTemperatureIs(actualTempx100);
   And HomingIsCompleted();

   SomeTimePasses(MinimumFreezeTime - 1);

   TheTemperatureIs(actualTempx100 - 1);

   HarvestingShouldStart();
   After(1);
}

TEST(TwistTrayIceMaker, ShouldNotStartIntegrationSumOrMinimumFreezeTimerIfTempIsAboveFreezingToBeginWith)
{
   Given TheTemperatureIs(FreezingPointx100 + 1000);
   And HomingIsCompleted();

   NothingShouldHappen();
   After(ALongTime);

   TheTemperatureIs(FreezingPointx100 / 3);

   SomeTimePasses(IceTemperaturePollingTime);

   HarvestingShouldStart();
   SomeTimePasses(MinimumFreezeTime);
}

TEST(TwistTrayIceMaker, ShouldReturnTheCorrectTimeForTheMinimumFreezeTimerRemainingTime)
{
   TemperatureDegFx100_t actualTempx100 = 3100;
   Given TheTemperatureIs(actualTempx100);
   And HomingIsCompleted();

   TheMinimumFreezeTimerRemainingTimeShouldBecome(MinimumFreezeTime);
   When TheMinimumFreezeTimerRemainingTimeIsRequested();

   SomeTimePasses(34);

   TheMinimumFreezeTimerRemainingTimeShouldBecome(MinimumFreezeTime - 34);
   When TheMinimumFreezeTimerRemainingTimeIsRequested();
}

TEST(TwistTrayIceMaker, ShouldStopIfThereIsAThermistorFaultAndBeginPollingThermistorIndefinately)
{
   Given TheTemperatureIs(200);
   And HomingIsCompleted();

   NothingShouldHappen();
   When SomeTimePasses(MinimumFreezeTime / 5);

   TheTemperatureIsInvalid();

   After(IceTemperaturePollingTime);

   NothingShouldHappen();
   After(ALongTime);
}

// Harvesting

TEST(TwistTrayIceMaker, ShouldTryToHarvestWhateverIsInTheTrayOnInitAfterTryingToFreezeIt)
{
   Given FreezingIsCompletedAndHarvestingIsStarted();

   TheMotorShouldBeRequestedTo(Idle);
   FillingShouldStart();
   When TheMotorActionResultIs(Harvested);
}

TEST(TwistTrayIceMaker, ShouldTryToHarvestIceAgainAfterEnoughTimeHasPassedSinceTheBucketWasFull)
{
   Given FreezingIsCompletedAndHarvestingIsStarted();

   TheMotorShouldBeRequestedTo(Idle);
   When TheMotorActionResultIs(BucketWasFull);

   NothingShouldHappen();
   After(FullIceBucketWaitTime - 1);

   HarvestingShouldStart();
   After(1);
}

TEST(TwistTrayIceMaker, ShouldTryToHarvestIceAgainAfterIceDispenserKicksOff)
{
   Given FreezingIsCompletedAndHarvestingIsStarted();

   TheMotorShouldBeRequestedTo(Idle);
   When TheMotorActionResultIs(BucketWasFull);

   NothingShouldHappen();
   After(FullIceBucketWaitTime - 10);

   TheIceDispenserBecomes(ON);

   HarvestingShouldStart();
   TheIceDispenserBecomes(OFF);
}

TEST(TwistTrayIceMaker, ShouldFillTheTrayWithWaterAfterHarvestingAndIncrementFreezerIceRateSignal)
{
   Given FreezingIsCompletedAndHarvestingIsStarted();
   HarvestingIsCompletedAndFillingIsStarted();

   NothingShouldHappen();
   After(WaterFillTime - 1);

   TheWaterValveShouldBecome(CLOSED);
   FreezerTriggerIceRateSignalShouldIncrement();
   After(1);
}

TEST(TwistTrayIceMaker, ShouldFreezeAfterFillingAndIncrementFreezerIceRateSignal)
{
   Given FreezingIsCompletedAndHarvestingIsStarted();
   HarvestingIsCompletedAndFillingIsStarted();

   TheWaterValveShouldBecome(CLOSED);
   FreezerTriggerIceRateSignalShouldIncrement();
   After(WaterFillTime);

   FreezingIsCompletedAndHarvestingIsStarted();
}

TEST(TwistTrayIceMaker, ShouldBeAbleToHarvestTwice)
{
   Given FreezingIsCompletedAndHarvestingIsStarted();
   HarvestingIsCompletedAndFillingIsStarted();

   TheWaterValveShouldBecome(CLOSED);
   FreezerTriggerIceRateSignalShouldIncrement();
   After(WaterFillTime);

   FreezingIsCompletedAndHarvestingIsStarted();

   HarvestingIsCompletedAndFillingIsStarted();
}

TEST(TwistTrayIceMaker, ShouldWaitToHarvestUntilDoorIsClosed)
{
   FreezingIsCompletedAndHarvestingDoesNotStartBecauseFreezerDoorOpens();

   NothingShouldHappen();
   After(ALongTime);

   TheFreezerDoorIs(CLOSED);

   NothingShouldHappen();
   After(DelayToHarvestAfterDoorCloses - 1);

   HarvestingShouldStart();
   After(1);
}

// SABBATH MODE

TEST(TwistTrayIceMaker, ShouldNotHarvestIceIfSabbathModeComesOnWhileFreezing)
{
   HomingIsCompleted();
   And TheTemperatureIs(200);
   Then SabbathModeIs(ON);

   NothingShouldHappen();
   After(ALongTime);

   HarvestingShouldStart();
   When SabbathModeIs(OFF);
}

TEST(TwistTrayIceMaker, SabbathModeShouldAlsoWorkIfTheEnhancedSabbathModeErdComesOn)
{
   HomingIsCompleted();
   And TheTemperatureIs(200);
   Then EnhancedSabbathModeIs(ON);

   NothingShouldHappen();
   After(ALongTime);

   HarvestingShouldStart();
   When EnhancedSabbathModeIs(OFF);
}

TEST(TwistTrayIceMaker, SabbathModeShouldAlsoWorkIfBothSabbathErdsComeOn)
{
   HomingIsCompleted();
   Given TheTemperatureIs(200);
   Then EnhancedSabbathModeIs(ON);
   And SabbathModeIs(ON);

   NothingShouldHappen();
   After(ALongTime);

   Then SabbathModeIs(OFF);

   NothingShouldHappen();
   After(ALongTime);

   HarvestingShouldStart();
   When EnhancedSabbathModeIs(OFF);
}

TEST(TwistTrayIceMaker, ShouldNotStartFillingTheTrayWithWaterWhileInSabbathMode)
{
   Given FreezingIsCompletedAndHarvestingIsStarted();

   SabbathModeIs(ON);

   TheMotorShouldBeRequestedTo(Idle);
   When TheMotorActionResultIs(Harvested);

   NothingShouldHappen();
   After(ALongTime);

   FillingShouldStart();
   When SabbathModeIs(OFF);
}

// MOTOR ERRORS

TEST(TwistTrayIceMaker, ShouldGoToErrorStateIfAHomingMovementResultsInError)
{
   TheMotorShouldBeRequestedTo(Home);
   When TheModuleIsInitialized();

   TheMotorShouldBeRequestedTo(Idle);
   TheHighLevelStateShouldBecome(FaultState);
   TheMotorFaultShouldBecome(ACTIVE);
   When TheMotorActionResultIs(MotorError);
}

TEST(TwistTrayIceMaker, ShouldGoToErrorStateIfAHarvestMovementResultsInError)
{
   Given FreezingIsCompletedAndHarvestingIsStarted();

   TheMotorShouldBeRequestedTo(Idle);
   TheHighLevelStateShouldBecome(FaultState);
   TheMotorFaultShouldBecome(ACTIVE);
   When TheMotorActionResultIs(MotorError);
}
