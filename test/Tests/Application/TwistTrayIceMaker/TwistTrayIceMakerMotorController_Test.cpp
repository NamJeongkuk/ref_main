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
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
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

   RunHomingRoutine = TwistTrayIceMakerMotorAction_RunHomingRoutine,
   RunCycle = TwistTrayIceMakerMotorAction_RunCycle,
   DoNothing = TwistTrayIceMakerMotorAction_Idle,
};

static uint8_t lastMotorState;

static void WriteMotorState(I_Output_t *context, const void *data)
{
   uint8_t newMotorState = *(const TwistTrayIceMakerMotorAction_t *)data;

   if(newMotorState != lastMotorState)
   {
      mock()
         .actualCall("Write")
         .onObject(context)
         .withParameter("Motor State", newMotorState);
   }

   lastMotorState = newMotorState;
}

static const I_Output_Api_t motorStateMockOutputApi = { WriteMotorState };

static const TwistTrayIceMakerData_t config = {
   .freezeData{
      .maximumHarvestTemperatureInDegFx100 = 0,
      .startIntegrationTemperatureInDegFx100 = 0,
      .targetFreezeIntegrationSum = 0,
      .minimumFreezeTimeMinutes = 0,
   },
   .harvestData{
      .fullBucketWaitPeriodMinutes = 0,
      .fullBucketDetectionPeriodSecX10 = FullBucketDetectionTime / 100,
      .initialHomingTwistPeriodSecX10 = InitialHomingTwistTime / 100,
      .homeLandingDelayPeriodSecX10 = HomePositionLandingDelay / 100,
      .longMotorErrorTimeoutPeriodSec = LongMotorErrorTimeout / MSEC_PER_SEC,
      .shortMotorErrorTimeoutPeriodSec = ShortMotorErrorTimeout / MSEC_PER_SEC,
      .delayToHarvestAfterDoorClosesSeconds = 0,
      .fullBucketDispenseCheckTimeInSeconds = FullBucketDispenseCheckTimeInSec,
   },
   .fillData{
      .waterFillTimeSecX10 = 0,
   },
   .fillTubeHeaterData = {
      .freezeThawFillTubeHeaterDutyCyclePercentage = 0,
      .freezeThawFillTubeHeaterOnTimeInSeconds = 400,
      .nonHarvestFillTubeHeaterDutyCyclePercentage = 10,
   }
};

TEST_GROUP(TwistTrayIceMakerMotorController)
{
   TwistTrayIceMakerMotorController_t instance;
   I_Output_t _motorStateOutput;
   I_Output_t *motorStateOutput;

   void setup()
   {
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
         &config,
         motorStateOutput);
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

   void TheModuleIsToldTo(TwistTrayIceMakerMotorAction_t newAction)
   {
      TwistTrayIceMakerMotorController_DoAction(&instance, newAction);
   }

   void TheSwitchGoes(bool newState)
   {
      TwistTrayIceMakerMotorController_UpdateSwitchState(&instance, newState);
   }

   void TheSwitchIs(bool newState)
   {
      TheSwitchGoes(newState);
   }

   void TheActionResultShouldBe(TwistTrayIceMakerMotorActionResult_t expectedResult)
   {
      CHECK_EQUAL(expectedResult, TwistTrayIceMakerMotorController_ActionResult(&instance));
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
   }

   void AfterHomingDelayTheMotorShouldBecome(uint8_t newState)
   {
      NothingShouldHappen();
      After(HomePositionLandingDelay - 1);

      TheMotorShouldBecome(newState);
      After(1);
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
};

TEST(TwistTrayIceMakerMotorController, ShouldHaveNoActionResultOnInit)
{
   Given TheModuleIsInitialized();
   TheActionResultShouldBe(TwistTrayIceMakerMotorActionResult_NoAction);
}

TEST(TwistTrayIceMakerMotorController, ShouldCorrectlyRunThroughTheHomingRoutineAndPublishTheCorrectActionResultsWhenTheTrayIsAlreadyHome)
{
   Given TheModuleIsInitialized();
   And TheSwitchIs(HIGH);

   TheMotorShouldBecome(Untwisting);
   When TheModuleIsToldTo(RunHomingRoutine);

   NothingShouldHappen();
   After(InitialHomingTwistTime - 1);

   TheMotorShouldSwitchDirectionsTo(Twisting);
   After(1);

   TheSwitchGoes(LOW);

   NothingShouldHappen();
   After(HomePositionLandingDelay - 1);

   TheMotorShouldSwitchDirectionsTo(Untwisting);
   After(1);

   TheSwitchGoes(HIGH);

   NothingShouldHappen();
   After(HomePositionLandingDelay - 1);
   TheActionResultShouldBe(TwistTrayIceMakerMotorActionResult_Homing);

   TheMotorShouldBecome(Braking);
   After(1);

   NothingShouldHappen();
   After(CoastingTime - 1);

   TheMotorShouldBecome(Coasting);
   After(1);

   TheActionResultShouldBe(TwistTrayIceMakerMotorActionResult_Homed);
}

TEST(TwistTrayIceMakerMotorController, ShouldCorrectlyRunThroughTheHomingRoutineWhenTheTrayIsALittleTwisted)
{
   Given TheModuleIsInitialized();
   And TheSwitchIs(LOW);

   TheMotorShouldBecome(Untwisting);
   When TheModuleIsToldTo(RunHomingRoutine);

   NothingShouldHappen();
   After(OneSecond);

   TheSwitchGoes(HIGH);

   TheMotorShouldSwitchDirectionsTo(Twisting);
   After(InitialHomingTwistTime);

   TheSwitchGoes(LOW);

   AfterHomingDelayTheMotorShouldSwitchDirectionsTo(Untwisting);

   TheSwitchGoes(HIGH);

   AfterHomingDelayTheMotorShouldBecome(Braking);

   AfterCoastingDelayTheMotorShouldBecome(Coasting);
}

TEST(TwistTrayIceMakerMotorController, ShouldCorrectlyRunThroughTheHomingRoutineWhenTheTrayIsFullyTwisted)
{
   Given TheModuleIsInitialized();
   And TheSwitchIs(HIGH);

   TheMotorShouldBecome(Untwisting);
   When TheModuleIsToldTo(RunHomingRoutine);

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

   TheSwitchGoes(LOW);

   AfterHomingDelayTheMotorShouldSwitchDirectionsTo(Untwisting);

   TheSwitchGoes(HIGH);

   AfterHomingDelayTheMotorShouldBecome(Braking);

   TheMotorShouldBecome(Coasting);
   After(CoastingTime);
}

TEST(TwistTrayIceMakerMotorController, ShouldCorrectlyRunThroughTheHarvestRoutineAndPublishTheCorrectActionResultsWhenBucketIsEmpty)
{
   Given TheModuleIsInitialized();

   TheMotorShouldBecome(Twisting);
   When TheModuleIsToldTo(RunCycle);

   SomeTimePasses(FullBucketDetectionTime);

   TheMotorShouldBecome(Coasting);
   When TheSwitchGoes(HIGH);

   TheSwitchGoes(LOW);

   AfterCoastingDelayTheMotorShouldBecome(Untwisting);

   TheSwitchBlipsQuicklyForRoutineHomewardMotionSwitchCheck();

   TheSwitchGoes(HIGH);

   NothingShouldHappen();
   After(HomePositionLandingDelay - 1);
   TheActionResultShouldBe(TwistTrayIceMakerMotorActionResult_Harvesting);

   TheMotorShouldBecome(Braking);
   After(1);

   NothingShouldHappen();
   After(CoastingTime - 1);

   TheMotorShouldBecome(Coasting);
   After(1);

   TheActionResultShouldBe(TwistTrayIceMakerMotorActionResult_Harvested);
}

TEST(TwistTrayIceMakerMotorController, ShouldStopHarvestingIfTheIceBucketIsFull)
{
   Given TheModuleIsInitialized();

   TheMotorShouldBecome(Twisting);
   When TheModuleIsToldTo(RunCycle);

   SomeTimePasses(FullBucketDetectionTime - 1);

   TheMotorShouldBecome(Coasting);
   When TheSwitchGoes(HIGH);

   TheSwitchGoes(LOW);

   AfterCoastingDelayTheMotorShouldBecome(Untwisting);

   TheSwitchGoes(HIGH);
   AfterHomingDelayTheMotorShouldBecome(Braking);
   AfterBrakingDelayTheMotorShouldBecome(Coasting);

   TheActionResultShouldBe(TwistTrayIceMakerMotorActionResult_BucketWasFull);
}

TEST(TwistTrayIceMakerMotorController, ShouldDoNothingWhileNotHavingAnActionRequest)
{
   Given TheModuleIsInitialized();

   TheMotorShouldBecome(Twisting);
   When TheModuleIsToldTo(RunCycle);

   SomeTimePasses(FullBucketDetectionTime - 1);

   TheMotorShouldBecome(Coasting);
   When TheSwitchGoes(HIGH);

   TheSwitchGoes(LOW);

   AfterCoastingDelayTheMotorShouldBecome(Untwisting);

   TheSwitchGoes(HIGH);
   AfterHomingDelayTheMotorShouldBecome(Braking);
   AfterBrakingDelayTheMotorShouldBecome(Coasting);

   TheActionResultShouldBe(TwistTrayIceMakerMotorActionResult_BucketWasFull);

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

   After(LongMotorErrorTimeout - 1);
   TheActionResultShouldBe(TwistTrayIceMakerMotorActionResult_Homing);

   TheMotorShouldBecome(Coasting);
   After(1);
   TheActionResultShouldBe(TwistTrayIceMakerMotorActionResult_MotorError);
}

TEST(TwistTrayIceMakerMotorController, ShouldProduceAnErrorIfMotorStallsDuringSecondHomingMovement)
{
   Given TheModuleIsInitialized();
   And TheSwitchIs(HIGH);

   TheMotorShouldBecome(Untwisting);
   When TheModuleIsToldTo(RunHomingRoutine);

   TheMotorShouldSwitchDirectionsTo(Twisting);
   After(InitialHomingTwistTime);

   After(ShortMotorErrorTimeout - 1);
   TheActionResultShouldBe(TwistTrayIceMakerMotorActionResult_Homing);

   TheMotorShouldBecome(Coasting);
   After(1);
   TheActionResultShouldBe(TwistTrayIceMakerMotorActionResult_MotorError);
}

TEST(TwistTrayIceMakerMotorController, ShouldProduceAnErrorIfMotorStallsDuringThirdHomingMovement)
{
   Given TheModuleIsInitialized();
   And TheSwitchIs(HIGH);

   TheMotorShouldBecome(Untwisting);
   When TheModuleIsToldTo(RunHomingRoutine);

   TheMotorShouldSwitchDirectionsTo(Twisting);
   After(InitialHomingTwistTime);

   TheSwitchGoes(LOW);

   TheMotorShouldSwitchDirectionsTo(Untwisting);
   After(HomePositionLandingDelay);

   After(ShortMotorErrorTimeout - 1);
   TheActionResultShouldBe(TwistTrayIceMakerMotorActionResult_Homing);

   TheMotorShouldBecome(Coasting);
   After(1);
   TheActionResultShouldBe(TwistTrayIceMakerMotorActionResult_MotorError);
}

TEST(TwistTrayIceMakerMotorController, ShouldProduceAnErrorIfMotorStallsDuringThirdHomingMovementEvenIfSwitchCheckOccurs)
{
   Given TheModuleIsInitialized();
   And TheSwitchIs(HIGH);

   TheMotorShouldBecome(Untwisting);
   When TheModuleIsToldTo(RunHomingRoutine);

   TheMotorShouldSwitchDirectionsTo(Twisting);
   After(InitialHomingTwistTime);

   TheSwitchGoes(LOW);

   TheMotorShouldSwitchDirectionsTo(Untwisting);
   After(HomePositionLandingDelay);

   After(ShortMotorErrorTimeout - 1);
   TheActionResultShouldBe(TwistTrayIceMakerMotorActionResult_Homing);

   TheSwitchBlipsQuicklyForRoutineHomewardMotionSwitchCheck();

   TheMotorShouldBecome(Coasting);
   After(1);
   TheActionResultShouldBe(TwistTrayIceMakerMotorActionResult_MotorError);
}

TEST(TwistTrayIceMakerMotorController, ShouldProduceAnErrorIfMotorStallsDuringFirstHarvestingMovement)
{
   Given TheModuleIsInitialized();

   TheMotorShouldBecome(Twisting);
   When TheModuleIsToldTo(RunCycle);

   After(LongMotorErrorTimeout - 1);
   TheActionResultShouldBe(TwistTrayIceMakerMotorActionResult_Harvesting);

   TheMotorShouldBecome(Coasting);
   After(1);
   TheActionResultShouldBe(TwistTrayIceMakerMotorActionResult_MotorError);
}

TEST(TwistTrayIceMakerMotorController, ShouldProduceAnErrorIfMotorStallsDuringHarvestingAfterBucketWasFoundToBeFull)
{
   Given TheModuleIsInitialized();

   TheMotorShouldBecome(Twisting);
   When TheModuleIsToldTo(RunCycle);

   SomeTimePasses(FullBucketDetectionTime - 1);

   TheMotorShouldBecome(Coasting);
   When TheSwitchGoes(HIGH);

   TheSwitchGoes(LOW);

   AfterCoastingDelayTheMotorShouldBecome(Untwisting);

   After(LongMotorErrorTimeout - 1);
   TheActionResultShouldBe(TwistTrayIceMakerMotorActionResult_Harvesting);

   TheMotorShouldBecome(Coasting);
   After(1);
   TheActionResultShouldBe(TwistTrayIceMakerMotorActionResult_MotorError);
}

TEST(TwistTrayIceMakerMotorController, ShouldProduceAnErrorIfMotorStallsDuringHarvestingAfterActuallyHarvesting)
{
   Given TheModuleIsInitialized();

   TheMotorShouldBecome(Twisting);
   When TheModuleIsToldTo(RunCycle);

   SomeTimePasses(FullBucketDetectionTime);

   TheMotorShouldBecome(Coasting);
   When TheSwitchGoes(HIGH);

   TheSwitchGoes(LOW);

   AfterCoastingDelayTheMotorShouldBecome(Untwisting);

   After(LongMotorErrorTimeout - 1);
   TheActionResultShouldBe(TwistTrayIceMakerMotorActionResult_Harvesting);

   TheMotorShouldBecome(Coasting);
   After(1);
   TheActionResultShouldBe(TwistTrayIceMakerMotorActionResult_MotorError);
}

TEST(TwistTrayIceMakerMotorController, ShouldProduceAnErrorIfMotorStallsDuringHarvestingAfterActuallyHarvestingEvenIfTheSwitchCheckOccurs)
{
   Given TheModuleIsInitialized();

   TheMotorShouldBecome(Twisting);
   When TheModuleIsToldTo(RunCycle);

   SomeTimePasses(FullBucketDetectionTime);

   TheMotorShouldBecome(Coasting);
   When TheSwitchGoes(HIGH);

   TheSwitchGoes(LOW);

   AfterCoastingDelayTheMotorShouldBecome(Untwisting);

   After(LongMotorErrorTimeout - 1);
   TheActionResultShouldBe(TwistTrayIceMakerMotorActionResult_Harvesting);

   TheSwitchBlipsQuicklyForRoutineHomewardMotionSwitchCheck();

   TheMotorShouldBecome(Coasting);
   After(1);
   TheActionResultShouldBe(TwistTrayIceMakerMotorActionResult_MotorError);
}
