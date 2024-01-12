/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include <stdint.h>
#include "TwistTrayIceMakerMotorController.h"
#include "Timer.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"
#include "TwistTrayIceMakerMotorState.h"
#include "SystemErds.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

#define Given
#define When
#define Still
#define And
#define Then

enum
{
   Untwisting = TwistTrayIceMakerMotorState_Untwisting,
   Twisting = TwistTrayIceMakerMotorState_Twisting,
   Braking = TwistTrayIceMakerMotorState_Braking,
   Coasting = TwistTrayIceMakerMotorState_Coasting,

   InitialHomingTwistTime = 3 * MSEC_PER_SEC,
   CoastingTime = 1 * MSEC_PER_SEC,
   HomePositionLandingDelay = (MSEC_PER_SEC / 2),
   LongMotorErrorTimeout = 25 * MSEC_PER_SEC,
   ShortMotorErrorTimeout = 3 * MSEC_PER_SEC,
   FullBucketDetectionTime = 7 * MSEC_PER_SEC,
   FullBucketDispenseCheckTimeInSec = 12,

   ALongTime = 450000,
   OneSecond = MSEC_PER_SEC,

   RunHomingRoutine = IceMakerMotorAction_RunHomingRoutine,
   RunCycle = IceMakerMotorAction_RunCycle,
   DoNothing = IceMakerMotorAction_Idle,

   PollingFrequencyInMsec = 150
};

static uint8_t lastMotorState;

static void WriteMotorState(I_Output_t *context, const void *data)
{
   uint8_t newMotorState = *(const IceMakerMotorAction_t *)data;

   if(newMotorState != lastMotorState)
   {
      mock()
         .actualCall("Write")
         .onObject(context)
         .withParameter("Motor State", newMotorState);
   }

   lastMotorState = newMotorState;
}

static const TwistTrayIceMakerMotorControllerConfig_t config = {
   .motorDoActionErd = Erd_IceMaker0_MotorDoAction
};

static const I_Output_Api_t motorStateMockOutputApi = { WriteMotorState };

static const IceMakerFillMonitorData_t iceMakerFillMonitorData = {
   .timedIceMakerFillInSecondsx10 = 0,
   .preciseIceMakerFillMaxTimeInSecondsx10 = 0,
   .preciseIceMakerFillOuncesInOzx100 = 0
};

static const HarvestCountCalculatorData_t harvestCountCalculatorData = {
   .startIntegrationTemperatureInDegFx100 = 0,
   .minimumFreezeTimeInitiationTemperatureInDegFx100 = 0,
   .targetIntegrationLimitInDegFx100TimesSeconds = 0,
   .minimumFreezeTimeInMinutes = 0
};

static const TwistTrayIceMakerData_t iceMakerData = {
   .typeInformation{
      .type = IceMakerType_Twist,
      .location = IceMakerLocation_Freezer,
   },
   .fillData{
      .iceMakerFillMonitorData = &iceMakerFillMonitorData },
   .freezeData{
      .harvestCountCalculatorData = &harvestCountCalculatorData,
      .maximumHarvestTemperatureInDegFx100 = 0,
   },
   .harvestData{
      .fullBucketWaitPeriodMinutes = 0,
      .fullBucketDetectionPeriodSecX10 = FullBucketDetectionTime / 100,
      .initialHomingTwistPeriodSecX10 = InitialHomingTwistTime / 100,
      .homeLandingDelayPeriodSecX10 = HomePositionLandingDelay / 100,
      .motorErrorRetryInitializeMinutes = 180,
      .longMotorErrorTimeoutPeriodSec = LongMotorErrorTimeout / MSEC_PER_SEC,
      .shortMotorErrorTimeoutPeriodSec = ShortMotorErrorTimeout / MSEC_PER_SEC,
      .delayToHarvestAfterDoorOpensMinutes = 0,
      .fullBucketDispenseCheckTimeInSeconds = FullBucketDispenseCheckTimeInSec,
      .fillTubeHeaterOnTimeInSeconds = 400,
      .fillTubeHeaterDutyCyclePercentage = 0,
      .fullBucketToFreezeStateTemperatureInDegFx100 = 200,
      .fullBucketDoorOpenCheckTimeInMinutes = 5,
   }
};

TEST_GROUP(TwistTrayIceMakerMotorController)
{
   TwistTrayIceMakerMotorController_t instance;
   I_Output_t _motorStateOutput;
   I_Output_t *motorStateOutput;

   ReferDataModel_TestDouble_t referDataModelTestDouble;
   I_DataModel_t *dataModel;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&referDataModelTestDouble);
      dataModel = referDataModelTestDouble.dataModel;

      lastMotorState = UINT8_MAX;
      _motorStateOutput.api = &motorStateMockOutputApi;
      motorStateOutput = &_motorStateOutput;
   }

   void After(TimerTicks_t time)
   {
      for(TimerTicks_t i = 0; i < time; i++)
      {
         TwistTrayIceMakerMotorController_Run(&instance);
      }
   }

   void SomeTimePasses(TimerTicks_t time)
   {
      After(time);
   }

   void TheModuleIsInitialized()
   {
      When TwistTrayIceMakerMotorController_Init(
         &instance,
         &iceMakerData,
         motorStateOutput,
         dataModel,
         &config);
   }

   void TheMotorShouldBecome(uint8_t state)
   {
      mock()
         .expectOneCall("Write")
         .onObject(motorStateOutput)
         .withParameter("Motor State", state);
   }

   void TheMotorShouldSwitchDirectionsTo(uint8_t newState)
   {
      TheMotorShouldBecome(Coasting);
      Then TheMotorShouldBecome(newState);
   }

   void TheModuleIsToldTo(IceMakerMotorAction_t newAction)
   {
      IceMakerMotorDoAction_t doAction;
      DataModel_Read(dataModel, Erd_IceMaker0_MotorDoAction, &doAction);

      doAction.action = newAction;
      doAction.signal++;
      DataModel_Write(dataModel, Erd_IceMaker0_MotorDoAction, &doAction);
   }

   void TheSwitchGoes(bool newState)
   {
      TwistTrayIceMakerMotorController_UpdateSwitchState(&instance, newState);
   }

   void TheSwitchIs(bool newState)
   {
      TheSwitchGoes(newState);
   }

   void TheMotorActionResultShouldBe(IceMakerMotorActionResult_t expected)
   {
      IceMakerMotorActionResult_t actual =
         TwistTrayIceMakerMotorController_MotorActionResult(&instance);
      CHECK_EQUAL(expected, actual);
   }

   void TheMotorErrorReasonShouldBe(IceMakerMotorErrorReason_t expected)
   {
      IceMakerMotorErrorReason_t actual =
         TwistTrayIceMakerMotorController_MotorErrorReason(&instance);
      CHECK_EQUAL(expected, actual);
   }

   void TheSwitchBlipsQuicklyForRoutineHomewardMotionSwitchCheck()
   {
      // see the motor spec for a definition
      // the switch goes high for a short blip of time
      // while traveling home from twisting
      TheSwitchGoes(HIGH);
      TheSwitchGoes(LOW);
   }

   void AfterHomingDelayTheMotorShouldSwitchDirectionsTo(uint8_t newState)
   {
      NothingShouldHappen();
      After(HomePositionLandingDelay - 1);

      TheMotorShouldSwitchDirectionsTo(newState);
      After(1);
      TheMotorOperationStateShouldBe(IceMakerMotorOperationState_HomingReadyToLandInHomePosition);
   }

   void AfterHomingDelayTheMotorShouldBecome(uint8_t newState)
   {
      NothingShouldHappen();
      After(HomePositionLandingDelay - 1);

      TheMotorShouldBecome(newState);
      After(1);
   }

   void GivenTheMotorHasHarvested()
   {
      TheMotorShouldBecome(Twisting);
      TheModuleIsToldTo(RunCycle);

      SomeTimePasses(FullBucketDetectionTime);
      TheMotorShouldBecome(Coasting);
      TheSwitchGoes(HIGH);
      TheSwitchGoes(LOW);

      AfterCoastingDelayTheMotorShouldBecome(Untwisting);
      TheSwitchBlipsQuicklyForRoutineHomewardMotionSwitchCheck();
      TheSwitchGoes(HIGH);

      TheMotorShouldBecome(Braking);
      After(HomePositionLandingDelay);

      NothingShouldHappen();

      TheMotorShouldBecome(Coasting);
      After(CoastingTime);
   }

   void AfterCoastingDelayTheMotorShouldBecome(uint8_t newState)
   {
      NothingShouldHappen();
      After(CoastingTime - 1);

      TheMotorShouldBecome(newState);
      After(1);
   }
#define AfterBrakingDelayTheMotorShouldBecome AfterCoastingDelayTheMotorShouldBecome

   void NothingShouldHappen()
   {
   }

   void TheOperationStateShouldBe(IceMakerMotorOperationState_t expected)
   {
      TwistTrayIceMakerOperationState_t actual;
      DataModel_Read(dataModel, Erd_IceMaker0_MotorOperationState, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void TheMotorOperationStateShouldBe(IceMakerMotorOperationState_t expected)
   {
      IceMakerMotorOperationState_t actual =
         TwistTrayIceMakerMotorController_MotorOperationState(&instance);

      CHECK_EQUAL(expected, actual);
   }
};

TEST(TwistTrayIceMakerMotorController, ShouldHaveNoActionResultAndIdleMotorOperationStateOnInit)
{
   Given TheModuleIsInitialized();
   TheMotorActionResultShouldBe(IceMakerMotorActionResult_NoAction);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_Idle);
}

TEST(TwistTrayIceMakerMotorController, ShouldCorrectlyRunThroughTheHomingRoutineAndPublishTheCorrectActionResultsWhenTheTrayIsAlreadyHome)
{
   Given TheModuleIsInitialized();
   And TheSwitchIs(HIGH);

   TheMotorShouldBecome(Untwisting);
   When TheModuleIsToldTo(RunHomingRoutine);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_HomingMakingSureTheTrayIsHome);

   NothingShouldHappen();
   After(InitialHomingTwistTime - 1);

   TheMotorShouldSwitchDirectionsTo(Twisting);
   After(1);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_HomingJumpingOutOfHome);

   TheSwitchGoes(LOW);

   NothingShouldHappen();
   After(HomePositionLandingDelay - 1);

   TheMotorShouldSwitchDirectionsTo(Untwisting);
   After(1);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_HomingReadyToLandInHomePosition);

   TheSwitchGoes(HIGH);

   NothingShouldHappen();
   After(HomePositionLandingDelay - 1);
   TheMotorActionResultShouldBe(IceMakerMotorActionResult_Homing);

   TheMotorShouldBecome(Braking);
   After(1);

   NothingShouldHappen();
   After(CoastingTime - 1);

   TheMotorShouldBecome(Coasting);
   After(1);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_Idle);
   TheMotorActionResultShouldBe(IceMakerMotorActionResult_Homed);
}

TEST(TwistTrayIceMakerMotorController, ShouldCorrectlyRunThroughTheHomingRoutineWhenTheTrayIsALittleTwisted)
{
   Given TheModuleIsInitialized();
   And TheSwitchIs(LOW);

   TheMotorShouldBecome(Untwisting);
   When TheModuleIsToldTo(RunHomingRoutine);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_HomingMakingSureTheTrayIsHome);

   NothingShouldHappen();
   After(OneSecond);

   TheSwitchGoes(HIGH);

   TheMotorShouldSwitchDirectionsTo(Twisting);
   After(InitialHomingTwistTime);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_HomingJumpingOutOfHome);

   TheSwitchGoes(LOW);

   AfterHomingDelayTheMotorShouldSwitchDirectionsTo(Untwisting);

   TheSwitchGoes(HIGH);

   AfterHomingDelayTheMotorShouldBecome(Braking);

   AfterCoastingDelayTheMotorShouldBecome(Coasting);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_Idle);
}

TEST(TwistTrayIceMakerMotorController, ShouldCorrectlyRunThroughTheHomingRoutineWhenTheTrayIsFullyTwisted)
{
   Given TheModuleIsInitialized();
   And TheSwitchIs(HIGH);

   TheMotorShouldBecome(Untwisting);
   When TheModuleIsToldTo(RunHomingRoutine);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_HomingMakingSureTheTrayIsHome);

   NothingShouldHappen();
   After(InitialHomingTwistTime - 1);

   TheSwitchGoes(LOW);

   SomeTimePasses(OneSecond);
   TheSwitchGoes(HIGH);
   SomeTimePasses(InitialHomingTwistTime - 1);
   TheSwitchGoes(LOW);
   SomeTimePasses(OneSecond);
   TheSwitchGoes(HIGH);

   TheMotorShouldSwitchDirectionsTo(Twisting);
   After(InitialHomingTwistTime);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_HomingJumpingOutOfHome);

   TheSwitchGoes(LOW);

   AfterHomingDelayTheMotorShouldSwitchDirectionsTo(Untwisting);

   TheSwitchGoes(HIGH);

   AfterHomingDelayTheMotorShouldBecome(Braking);

   TheMotorShouldBecome(Coasting);
   After(CoastingTime);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_Idle);
}

TEST(TwistTrayIceMakerMotorController, ShouldCorrectlyRunThroughTheHarvestRoutineAndPublishTheCorrectActionResultsWhenBucketIsEmpty)
{
   Given TheModuleIsInitialized();

   TheMotorShouldBecome(Twisting);
   When TheModuleIsToldTo(RunCycle);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_HarvestCheckingIfBucketIsFull);

   SomeTimePasses(FullBucketDetectionTime);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_HarvestStoppingAtFullTwistPosition);

   TheMotorShouldBecome(Coasting);
   When TheSwitchGoes(HIGH);

   TheSwitchGoes(LOW);

   AfterCoastingDelayTheMotorShouldBecome(Untwisting);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_HarvestUntwisting);

   TheSwitchBlipsQuicklyForRoutineHomewardMotionSwitchCheck();
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_HarvestReadyToLandInHomePosition);

   TheSwitchGoes(HIGH);

   NothingShouldHappen();
   After(HomePositionLandingDelay - 1);
   TheMotorActionResultShouldBe(IceMakerMotorActionResult_Harvesting);

   TheMotorShouldBecome(Braking);
   After(1);

   NothingShouldHappen();
   After(CoastingTime - 1);

   TheMotorShouldBecome(Coasting);
   After(1);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_Idle);
   TheMotorActionResultShouldBe(IceMakerMotorActionResult_Harvested);
}

TEST(TwistTrayIceMakerMotorController, ShouldStopHarvestingIfTheIceBucketIsFull)
{
   Given TheModuleIsInitialized();

   TheMotorShouldBecome(Twisting);
   When TheModuleIsToldTo(RunCycle);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_HarvestCheckingIfBucketIsFull);

   SomeTimePasses(FullBucketDetectionTime - 1);

   TheMotorShouldBecome(Coasting);
   When TheSwitchGoes(HIGH);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_HarvestBucketWasFoundToBeFull);

   TheSwitchGoes(LOW);

   AfterCoastingDelayTheMotorShouldBecome(Untwisting);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_HarvestReadyToLandInHomePosition);

   TheSwitchGoes(HIGH);
   AfterHomingDelayTheMotorShouldBecome(Braking);
   AfterBrakingDelayTheMotorShouldBecome(Coasting);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_Idle);
   TheMotorActionResultShouldBe(IceMakerMotorActionResult_BucketWasFull);
}

TEST(TwistTrayIceMakerMotorController, ShouldDoNothingWhileNotHavingAnActionRequest)
{
   Given TheModuleIsInitialized();

   TheMotorShouldBecome(Twisting);
   When TheModuleIsToldTo(RunCycle);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_HarvestCheckingIfBucketIsFull);

   SomeTimePasses(FullBucketDetectionTime - 1);

   TheMotorShouldBecome(Coasting);
   When TheSwitchGoes(HIGH);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_HarvestBucketWasFoundToBeFull);

   TheSwitchGoes(LOW);

   AfterCoastingDelayTheMotorShouldBecome(Untwisting);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_HarvestReadyToLandInHomePosition);

   TheSwitchGoes(HIGH);
   AfterHomingDelayTheMotorShouldBecome(Braking);
   AfterBrakingDelayTheMotorShouldBecome(Coasting);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_Idle);
   TheMotorActionResultShouldBe(IceMakerMotorActionResult_BucketWasFull);

   NothingShouldHappen();
   After(ALongTime);
}

// MOTOR ERRORS

TEST(TwistTrayIceMakerMotorController, ShouldProduceAnErrorIfMotorStallsDuringFirstHomingMovement)
{
   Given TheModuleIsInitialized();
   And TheSwitchIs(LOW);

   TheMotorShouldBecome(Untwisting);
   When TheModuleIsToldTo(RunHomingRoutine);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_HomingMakingSureTheTrayIsHome);

   After(LongMotorErrorTimeout - 1);
   TheMotorActionResultShouldBe(IceMakerMotorActionResult_Homing);

   TheMotorShouldBecome(Coasting);
   After(1);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_Idle);
   TheMotorActionResultShouldBe(IceMakerMotorActionResult_MotorError);
   TheMotorErrorReasonShouldBe(IceMakerMotorErrorReason_MotorStallsInStateHomingCheckingTrayIsHome);
}

TEST(TwistTrayIceMakerMotorController, ShouldProduceAnErrorIfMotorStallsDuringSecondHomingMovement)
{
   Given TheModuleIsInitialized();
   And TheSwitchIs(HIGH);

   TheMotorShouldBecome(Untwisting);
   When TheModuleIsToldTo(RunHomingRoutine);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_HomingMakingSureTheTrayIsHome);

   TheMotorShouldSwitchDirectionsTo(Twisting);
   After(InitialHomingTwistTime);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_HomingJumpingOutOfHome);

   After(ShortMotorErrorTimeout - 1);
   TheMotorActionResultShouldBe(IceMakerMotorActionResult_Homing);

   TheMotorShouldBecome(Coasting);
   After(1);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_Idle);
   TheMotorActionResultShouldBe(IceMakerMotorActionResult_MotorError);
   TheMotorErrorReasonShouldBe(IceMakerMotorErrorReason_MotorStallsInStateHomingJumpingOutOfHome);
}

TEST(TwistTrayIceMakerMotorController, ShouldProduceAnErrorIfMotorStallsDuringThirdHomingMovement)
{
   Given TheModuleIsInitialized();
   And TheSwitchIs(HIGH);

   TheMotorShouldBecome(Untwisting);
   When TheModuleIsToldTo(RunHomingRoutine);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_HomingMakingSureTheTrayIsHome);

   TheMotorShouldSwitchDirectionsTo(Twisting);
   After(InitialHomingTwistTime);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_HomingJumpingOutOfHome);

   TheSwitchGoes(LOW);

   TheMotorShouldSwitchDirectionsTo(Untwisting);
   After(HomePositionLandingDelay);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_HomingReadyToLandInHomePosition);

   After(ShortMotorErrorTimeout - 1);
   TheMotorActionResultShouldBe(IceMakerMotorActionResult_Homing);

   TheMotorShouldBecome(Coasting);
   After(1);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_Idle);
   TheMotorActionResultShouldBe(IceMakerMotorActionResult_MotorError);
   TheMotorErrorReasonShouldBe(IceMakerMotorErrorReason_MotorStallsInStateHomingReadyToLandInHomePosition);
}

TEST(TwistTrayIceMakerMotorController, ShouldProduceAnErrorIfMotorStallsDuringThirdHomingMovementEvenIfSwitchCheckOccurs)
{
   Given TheModuleIsInitialized();
   And TheSwitchIs(HIGH);

   TheMotorShouldBecome(Untwisting);
   When TheModuleIsToldTo(RunHomingRoutine);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_HomingMakingSureTheTrayIsHome);

   TheMotorShouldSwitchDirectionsTo(Twisting);
   After(InitialHomingTwistTime);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_HomingJumpingOutOfHome);

   TheSwitchGoes(LOW);

   TheMotorShouldSwitchDirectionsTo(Untwisting);
   After(HomePositionLandingDelay);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_HomingReadyToLandInHomePosition);

   After(ShortMotorErrorTimeout - 1);
   TheMotorActionResultShouldBe(IceMakerMotorActionResult_Homing);

   TheSwitchBlipsQuicklyForRoutineHomewardMotionSwitchCheck();

   TheMotorShouldBecome(Coasting);
   After(1);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_Idle);
   TheMotorActionResultShouldBe(IceMakerMotorActionResult_MotorError);
   TheMotorErrorReasonShouldBe(IceMakerMotorErrorReason_MotorStallsInStateHomingReadyToLandInHomePosition);
}

TEST(TwistTrayIceMakerMotorController, ShouldProduceAnErrorIfMotorStallsDuringFirstHarvestingMovement)
{
   Given TheModuleIsInitialized();

   TheMotorShouldBecome(Twisting);
   When TheModuleIsToldTo(RunCycle);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_HarvestCheckingIfBucketIsFull);

   After(LongMotorErrorTimeout - 1);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_HarvestStoppingAtFullTwistPosition);
   TheMotorActionResultShouldBe(IceMakerMotorActionResult_Harvesting);

   TheMotorShouldBecome(Coasting);
   After(1);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_Idle);
   TheMotorActionResultShouldBe(IceMakerMotorActionResult_MotorError);
   TheMotorErrorReasonShouldBe(IceMakerMotorErrorReason_MotorStallsInStateHarvestStoppingAtFullTwistPosition);
}

TEST(TwistTrayIceMakerMotorController, ShouldProduceAnErrorIfMotorStallsDuringHarvestingAfterBucketWasFoundToBeFull)
{
   Given TheModuleIsInitialized();

   TheMotorShouldBecome(Twisting);
   When TheModuleIsToldTo(RunCycle);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_HarvestCheckingIfBucketIsFull);

   SomeTimePasses(FullBucketDetectionTime - 1);

   TheMotorShouldBecome(Coasting);
   When TheSwitchGoes(HIGH);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_HarvestBucketWasFoundToBeFull);

   TheSwitchGoes(LOW);

   AfterCoastingDelayTheMotorShouldBecome(Untwisting);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_HarvestReadyToLandInHomePosition);

   After(LongMotorErrorTimeout - 1);
   TheMotorActionResultShouldBe(IceMakerMotorActionResult_Harvesting);

   TheMotorShouldBecome(Coasting);
   After(1);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_Idle);
   TheMotorActionResultShouldBe(IceMakerMotorActionResult_MotorError);
   TheMotorErrorReasonShouldBe(IceMakerMotorErrorReason_MotorStallsInStateHarvestReadyToLandInHomePosition);
}

TEST(TwistTrayIceMakerMotorController, ShouldProduceAnErrorIfMotorStallsDuringHarvestingAfterActuallyHarvesting)
{
   Given TheModuleIsInitialized();

   TheMotorShouldBecome(Twisting);
   When TheModuleIsToldTo(RunCycle);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_HarvestCheckingIfBucketIsFull);

   SomeTimePasses(FullBucketDetectionTime);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_HarvestStoppingAtFullTwistPosition);

   TheMotorShouldBecome(Coasting);
   When TheSwitchGoes(HIGH);

   TheSwitchGoes(LOW);

   AfterCoastingDelayTheMotorShouldBecome(Untwisting);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_HarvestUntwisting);

   After(LongMotorErrorTimeout - 1);
   TheMotorActionResultShouldBe(IceMakerMotorActionResult_Harvesting);

   TheMotorShouldBecome(Coasting);
   After(1);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_Idle);
   TheMotorActionResultShouldBe(IceMakerMotorActionResult_MotorError);
   TheMotorErrorReasonShouldBe(IceMakerMotorErrorReason_MotorStallsInStateHarvestUntwisting);
}

TEST(TwistTrayIceMakerMotorController, ShouldProduceAnErrorIfMotorStallsDuringHarvestingAfterActuallyHarvestingEvenIfTheSwitchCheckOccurs)
{
   Given TheModuleIsInitialized();

   TheMotorShouldBecome(Twisting);
   When TheModuleIsToldTo(RunCycle);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_HarvestCheckingIfBucketIsFull);

   SomeTimePasses(FullBucketDetectionTime);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_HarvestStoppingAtFullTwistPosition);

   TheMotorShouldBecome(Coasting);
   When TheSwitchGoes(HIGH);

   TheSwitchGoes(LOW);

   AfterCoastingDelayTheMotorShouldBecome(Untwisting);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_HarvestUntwisting);

   After(LongMotorErrorTimeout - 1);
   TheMotorActionResultShouldBe(IceMakerMotorActionResult_Harvesting);

   TheSwitchBlipsQuicklyForRoutineHomewardMotionSwitchCheck();
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_HarvestReadyToLandInHomePosition);

   TheMotorShouldBecome(Coasting);
   After(1);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_Idle);
   TheMotorActionResultShouldBe(IceMakerMotorActionResult_MotorError);
   TheMotorErrorReasonShouldBe(IceMakerMotorErrorReason_MotorStallsInStateHarvestReadyToLandInHomePosition);
}

TEST(TwistTrayIceMakerMotorController, ShouldSetActionResultToNoActionWhenRequestIsIdle)
{
   Given TheModuleIsInitialized();
   GivenTheMotorHasHarvested();

   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_Idle);
   TheMotorActionResultShouldBe(IceMakerMotorActionResult_Harvested);

   When TheModuleIsToldTo(DoNothing);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_Idle);
   TheMotorActionResultShouldBe(IceMakerMotorActionResult_NoAction);
}
