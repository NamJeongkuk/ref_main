/*!
 * @file test file for door holdoff timer module
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "DataModelErdPointerAccess.h"
#include "SystemErds.h"
#include "utils.h"
#include "DefrostDoorHoldoffTimer.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "PersonalityParametricData_TestDouble.h"
#include "uassert_test.h"

#define OPENS true
#define CLOSES false

#define Given
#define The
#define And
#define When

enum
{
   FfOnlyHoldoffTimeInMinutes = 50,
   FfAndFzHoldoffTimeInMinutes = 60,
   MaxHoldoffTimeInMinutes = 250,
   ALongTime = 1000 * MSEC_PER_MIN
};

static const DefrostData_t defrostData = {
   .defrostDoorHoldoffTimeForFfAndFzInMinutes = FfAndFzHoldoffTimeInMinutes,
   .defrostDoorHoldoffTimeForFfOnlyInMinutes = FfOnlyHoldoffTimeInMinutes,
   .defrostMaxHoldoffTimeInMinutes = MaxHoldoffTimeInMinutes
};

static const EvaporatorData_t singleEvapData = {
   .numberOfEvaporators = 1
};

static const EvaporatorData_t dualEvapData = {
   .numberOfEvaporators = 2
};

static const DefrostDoorHoldoffTimerConfiguration_t configuration = {

   .allFreshFoodDoorsAreClosedState = Erd_AllFreshFoodDoorsAreClosed,
   .freezerDoorOpenState = Erd_FzDoorIsOpen,
   .ccDoorOpenState = Erd_CcDoorIsOpen,

   .doorHoldoffRequest = Erd_DefrostDoorHoldOffRequest,
   .doorHoldoffSatisfied = Erd_DefrostDoorHoldoffTimeSatisfied,

   .freshFoodOnlyDefrost = Erd_DefrostIsFreshFoodOnly,

   .doorHoldoffTimerFsmState = Erd_DefrostDoorHoldoffTimerFsmState
};

TEST_GROUP(DefrostDoorHoldoffTimer)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   PersonalityParametricData_t personalityParametricData;

   DefrostDoorHoldoffTimer_t instance;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);
      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &timerModuleTestDouble->timerModule);
   }

   void PersonalityInitializedWith(const EvaporatorData_t *evapData)
   {
      PersonalityParametricData_TestDouble_Init(&personalityParametricData);
      PersonalityParametricData_TestDouble_SetDefrost(&personalityParametricData, &defrostData);
      PersonalityParametricData_TestDouble_SetEvaporator(&personalityParametricData, evapData);
      DataModelErdPointerAccess_Write(dataModel, Erd_PersonalityParametricData, &personalityParametricData);
   }

   void After(TimerTicks_t ticks, TimeSourceTickCount_t ticksToElapseAtATime = 1000)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks, ticksToElapseAtATime);
   }

   void DefrostHoldoffTimeIsInitialized()
   {
      DefrostDoorHoldoffTimer_Init(&instance, &configuration, dataModel);
   }

   void AllFreshFoodDoorAreClosed()
   {
      bool state = true;
      DataModel_Write(dataModel, Erd_AllFreshFoodDoorsAreClosed, &state);
   }

   void AFreshFoodDoorOpens()
   {
      bool state = false;
      DataModel_Write(dataModel, Erd_AllFreshFoodDoorsAreClosed, &state);
   }

   void FreezerDoor(bool state)
   {
      DataModel_Write(dataModel, Erd_FzDoorIsOpen, &state);
   }

   void CcDoor(bool state)
   {
      DataModel_Write(dataModel, Erd_CcDoorIsOpen, &state);
   }

   void DoorHoldoffRequestIs(bool enabled)
   {
      DataModel_Write(dataModel, Erd_DefrostDoorHoldOffRequest, &enabled);
   }

   void DoorHoldoffTimeSatisfiedShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_DefrostDoorHoldoffTimeSatisfied, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void FreshFoodOnlyDefrostIs(bool actual)
   {
      DataModel_Write(dataModel, Erd_DefrostIsFreshFoodOnly, &actual);
   }

   void DoorHoldoffFsmStateShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel,
         Erd_DefrostDoorHoldoffTimerFsmState,
         &actual);
      CHECK_EQUAL(expected, actual);
   }
};

TEST(DefrostDoorHoldoffTimer, ShouldKeepDoorHoldoffTimeSatisfiedDisabledForALongTimeWhenDisabled)
{
   Given PersonalityInitializedWith(&dualEvapData);
   And DefrostHoldoffTimeIsInitialized();
   And DoorHoldoffRequestIs(DISABLED);

   DoorHoldoffTimeSatisfiedShouldBe(DISABLED);
   After(ALongTime);
   DoorHoldoffTimeSatisfiedShouldBe(DISABLED);
}

TEST(DefrostDoorHoldoffTimer, ShouldInitialize)
{
   Given PersonalityInitializedWith(&dualEvapData);
   And DefrostHoldoffTimeIsInitialized();
}

TEST(DefrostDoorHoldoffTimer, ShouldSatisfyWhenDoorRemainsClosedAndDefrostIsFfAndFz)
{
   Given AllFreshFoodDoorAreClosed();
   And FreezerDoor(CLOSES);

   And DoorHoldoffRequestIs(DISABLED);
   And PersonalityInitializedWith(&dualEvapData);
   And DefrostHoldoffTimeIsInitialized();

   When FreshFoodOnlyDefrostIs(DISABLED);
   And DoorHoldoffRequestIs(ENABLED);

   DoorHoldoffTimeSatisfiedShouldBe(DISABLED);

   After(FfAndFzHoldoffTimeInMinutes * MSEC_PER_MIN - 1);
   DoorHoldoffTimeSatisfiedShouldBe(DISABLED);

   After(1);
   DoorHoldoffTimeSatisfiedShouldBe(ENABLED);
}

TEST(DefrostDoorHoldoffTimer, ShouldSatisfyWhenDoorRemainsClosedAndDefrostIsFfOnly)
{
   Given DoorHoldoffRequestIs(DISABLED);
   And PersonalityInitializedWith(&dualEvapData);
   And DefrostHoldoffTimeIsInitialized();

   When FreshFoodOnlyDefrostIs(ENABLED);
   And DoorHoldoffRequestIs(ENABLED);

   DoorHoldoffTimeSatisfiedShouldBe(DISABLED);

   After(FfOnlyHoldoffTimeInMinutes * MSEC_PER_MIN - 1);
   DoorHoldoffTimeSatisfiedShouldBe(DISABLED);

   After(1);
   DoorHoldoffTimeSatisfiedShouldBe(ENABLED);
}

TEST(DefrostDoorHoldoffTimer, ShouldRestartTimerWhenFreezerDoorsAreClosed)
{
   Given AllFreshFoodDoorAreClosed();
   And PersonalityInitializedWith(&dualEvapData);

   When FreezerDoor(OPENS);
   And DefrostHoldoffTimeIsInitialized();
   And DoorHoldoffRequestIs(ENABLED);

   After(FfAndFzHoldoffTimeInMinutes - 1);
   The DoorHoldoffTimeSatisfiedShouldBe(DISABLED);

   When The FreezerDoor(CLOSES);

   After(FfAndFzHoldoffTimeInMinutes * MSEC_PER_MIN - 1);
   DoorHoldoffTimeSatisfiedShouldBe(DISABLED);

   After(1);
   The DoorHoldoffTimeSatisfiedShouldBe(ENABLED);
}

TEST(DefrostDoorHoldoffTimer, ShouldStopTimerWhenFreezerDoorOpens)
{
   Given AllFreshFoodDoorAreClosed();
   And FreezerDoor(CLOSES);
   And PersonalityInitializedWith(&dualEvapData);
   And DefrostHoldoffTimeIsInitialized();

   When DoorHoldoffRequestIs(ENABLED);
   And FreezerDoor(OPENS);
   DoorHoldoffTimeSatisfiedShouldBe(DISABLED);

   After(FfAndFzHoldoffTimeInMinutes * MSEC_PER_MIN);
   DoorHoldoffTimeSatisfiedShouldBe(DISABLED);
}

TEST(DefrostDoorHoldoffTimer, ShouldStopTimerWhenFreshFoodDoorOpens)
{
   Given AllFreshFoodDoorAreClosed();
   And FreezerDoor(CLOSES);
   And PersonalityInitializedWith(&dualEvapData);
   And DefrostHoldoffTimeIsInitialized();

   When DoorHoldoffRequestIs(ENABLED);
   And AFreshFoodDoorOpens();

   DoorHoldoffTimeSatisfiedShouldBe(DISABLED);
   After(FfAndFzHoldoffTimeInMinutes * MSEC_PER_MIN);
   DoorHoldoffTimeSatisfiedShouldBe(DISABLED);
}

TEST(DefrostDoorHoldoffTimer, ShouldSetSatisfiedToTrueAfterMaxTimeoutRegardlessOfTheDoorsBeingOpenedOrClosed)
{
   Given AllFreshFoodDoorAreClosed();
   And FreezerDoor(CLOSES);
   And PersonalityInitializedWith(&dualEvapData);
   And DefrostHoldoffTimeIsInitialized();

   When DoorHoldoffRequestIs(ENABLED);
   After(1);
   And AFreshFoodDoorOpens();
   After(1);
   FreezerDoor(OPENS);
   After(1);
   AllFreshFoodDoorAreClosed();
   After(1);

   After(MaxHoldoffTimeInMinutes * MSEC_PER_MIN - 5);
   DoorHoldoffTimeSatisfiedShouldBe(DISABLED);

   After(1);
   DoorHoldoffTimeSatisfiedShouldBe(ENABLED);
}

TEST(DefrostDoorHoldoffTimer, ShouldRestartTimerWhenFreshFoodDoorsAreClosedForSingleEvap)
{
   Given AllFreshFoodDoorAreClosed();
   And FreezerDoor(CLOSES);
   And PersonalityInitializedWith(&singleEvapData);
   And DefrostHoldoffTimeIsInitialized();

   When DoorHoldoffRequestIs(ENABLED);
   And AFreshFoodDoorOpens();

   After(FfAndFzHoldoffTimeInMinutes * MSEC_PER_MIN);
   DoorHoldoffTimeSatisfiedShouldBe(DISABLED);

   When AllFreshFoodDoorAreClosed();
   After(FfAndFzHoldoffTimeInMinutes * MSEC_PER_MIN - 1);
   DoorHoldoffTimeSatisfiedShouldBe(DISABLED);

   After(1);
   DoorHoldoffTimeSatisfiedShouldBe(ENABLED);
}

TEST(DefrostDoorHoldoffTimer, ShouldSatisfyHoldoffRequestAfterMaxTimeEvenIfDoorRemainsOpen)
{
   Given AllFreshFoodDoorAreClosed();
   And FreezerDoor(CLOSES);
   And PersonalityInitializedWith(&singleEvapData);
   And FreshFoodOnlyDefrostIs(DISABLED);
   And DefrostHoldoffTimeIsInitialized();

   When DoorHoldoffRequestIs(ENABLED);
   And AFreshFoodDoorOpens();

   After(MaxHoldoffTimeInMinutes * MSEC_PER_MIN - 1);
   DoorHoldoffTimeSatisfiedShouldBe(DISABLED);

   After(1);
   DoorHoldoffTimeSatisfiedShouldBe(ENABLED);
}

TEST(DefrostDoorHoldoffTimer, ShouldNotSatisfyHoldoffForDualEvapUnlessCcDrawerRemainsClosed)
{
   Given PersonalityInitializedWith(&dualEvapData);
   And FreezerDoor(CLOSES);
   And AllFreshFoodDoorAreClosed();
   And CcDoor(CLOSES);
   And FreshFoodOnlyDefrostIs(DISABLED);
   And DefrostHoldoffTimeIsInitialized();

   When DoorHoldoffRequestIs(ENABLED);
   And CcDoor(OPENS);

   After(FfAndFzHoldoffTimeInMinutes * MSEC_PER_MIN);
   DoorHoldoffTimeSatisfiedShouldBe(DISABLED);

   When CcDoor(CLOSES);

   After(FfAndFzHoldoffTimeInMinutes * MSEC_PER_MIN - 1);
   DoorHoldoffTimeSatisfiedShouldBe(DISABLED);

   After(1);
   DoorHoldoffTimeSatisfiedShouldBe(ENABLED);
}

TEST(DefrostDoorHoldoffTimer, ShouldEnableDoorHoldoffTimeSatisfiedWhenFreezerDoorClosesAfterHoldoffTimeEvenIfFreshFoodDoorsOpenInDualEvap)
{
   Given PersonalityInitializedWith(&dualEvapData);
   And FreezerDoor(CLOSES);
   And AllFreshFoodDoorAreClosed();
   And CcDoor(CLOSES);
   And FreshFoodOnlyDefrostIs(DISABLED);
   And DefrostHoldoffTimeIsInitialized();

   When AFreshFoodDoorOpens();
   And The FreezerDoor(OPENS);
   And DoorHoldoffRequestIs(ENABLED);

   After(FfAndFzHoldoffTimeInMinutes * MSEC_PER_MIN);
   DoorHoldoffTimeSatisfiedShouldBe(DISABLED);

   When FreezerDoor(CLOSES);
   After(FfAndFzHoldoffTimeInMinutes * MSEC_PER_MIN);
   DoorHoldoffTimeSatisfiedShouldBe(ENABLED);
}

TEST(DefrostDoorHoldoffTimer, ShouldCorrectlyUpdateFsmStateErdWhenFsmStateChanges)
{
   Given PersonalityInitializedWith(&dualEvapData);
   And DefrostHoldoffTimeIsInitialized();

   DoorHoldoffFsmStateShouldBe(DISABLED);

   When DoorHoldoffRequestIs(ENABLED);
   DoorHoldoffFsmStateShouldBe(ENABLED);

   When DoorHoldoffRequestIs(DISABLED);
   DoorHoldoffFsmStateShouldBe(DISABLED);
}
