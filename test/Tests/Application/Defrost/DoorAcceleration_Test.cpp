/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "DoorAcceleration.h"
#include "DataModelErdPointerAccess.h"
#include "SystemErds.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "PersonalityParametricData_TestDouble.h"
#include "uassert_test.h"

#define Given
#define When
#define Then
#define And

enum
{
   Closed = 0,
   Open = 1
};

enum
{
   SomeRandomAcceleration = 999,
   AnotherRandomAcceleration = 333
};

static const DefrostData_t defrostData = {
   .fzDoorIncrementFactorInSecondsPerSecond = 348,
   .ffDoorIncrementFactorInSecondsPerSecond = 87,
   .fzAbnormalRunTimeInMinutes = 6 * 60,
   .maxTimeBetweenDefrostsInMinutes = 32 * 60,
   .dmFzDefrostTemperatureInDegFx100 = 1500,
   .prechillFzSetpointInDegFx100 = 600,
   .prechillFfSetpointInDegFx100 = 4600,
   .prechillCcSetpointInDegFx100 = 600,
   .prechillFzEvapExitTemperatureInDegFx100 = -3000,
   .prechillCcEvapExitTemperatureInDegFx100 = -3000,
   .maxPrechillTimeInMinutes = 10,
   .defrostDoorHoldoffTimeForFfAndFzInMinutes = 60,
   .defrostMaxHoldoffTimeInMinutes = 60,
   .maxPrechillHoldoffTimeAfterDefrostTimerSatisfiedInSeconds = 0,
   .ffFanDefrostFfEvapExitTemperatureInDegFx100 = 3600,
   .ffFanDefrostFfFanMaxOnTimeInMinutes = 10,
   .ccFanDefrostCcEvapExitTemperatureInDegFx100 = 3200,
   .ccFanDefrostCcFanMaxOnTimeInMinutes = 10,
   .fzDefrostHeaterMaxOnTimeInMinutes = 60,
   .fzAbnormalDefrostHeaterMaxOnTimeInMinutes = 32,
   .fzDefrostTerminationTemperatureInDegFx100 = 5900,
   .ffDefrostTerminationTemperatureInDegFx100 = 4460,
   .ccDefrostTerminationTemperatureInDegFx100 = 4460,
   .ffDefrostHeaterMaxOnTimeInMinutes = 60,
   .ffAbnormalDefrostHeaterMaxOnTimeInMinutes = 21,
   .ccDefrostHeaterMaxOnTimeInMinutes = 60,
   .ccAsFfAbnormalDefrostHeaterMaxOnTimeInMinutes = 21,
   .ccAsFzAbnormalDefrostHeaterMaxOnTimeInMinutes = 35,
   .defrostDwellTimeInMinutes = 7,
   .ffAndFzPostDwellFzExitTemperatureInDegFx100 = -1000,
   .ffAndFzPostDwellFzExitTimeInMinutes = 10,
   .dwellThreeWayValvePosition = ValvePos_A,
   .postDwellThreeWayValvePositionForFfAndFz = ValvePos_A,
   .ffPostDefrostPullDownExitTemperatureInDegFx100 = 4000,
   .fzPostDefrostPullDownExitTemperatureInDegFx100 = 4000,
   .numberOfFfDefrostsBeforeFzDefrost = 2,
   .numberOfFfDefrostsBeforeAbnormalFzDefrost = 1,
   .doorHoldoffTimeForFfAndFzInMinutes = 60,
   .ffOnlyPostDwellExitTimeInMinutes = 10,
   .dsmFzSetpointTemperatureInDegFx100 = 200,
   .defrostPeriodicTimeoutInSeconds = 1
};

static const DoorAccelerationConfig_t config = {
   .doorAccelerationRequestErd = Erd_DoorAccelerationRequest,
   .doorAccelerationFsmStateErd = Erd_DoorAccelerationFsmState,
   .fzDoorAccelerationCountsErd = Erd_DefrostFzDoorAccelerationCount,
   .ffDoorAccelerationCountsErd = Erd_DefrostFfDoorAccelerationCount,
   .leftHandFfDoorIsOpenErd = Erd_LeftHandFfDoorIsOpen,
   .rightHandFfDoorIsOpenErd = Erd_RightHandFfDoorIsOpen,
   .doorInDoorIsOpenErd = Erd_DoorInDoorIsOpen,
   .fzDoorIsOpenErd = Erd_FzDoorIsOpen,
   .timerModuleErd = Erd_TimerModule
};

TEST_GROUP(DoorAcceleration)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   PersonalityParametricData_t personalityParametricData;
   DoorAccelerationCalculator_t instance;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &timerModuleTestDouble->timerModule);

      PersonalityParametricData_TestDouble_Init(&personalityParametricData);
      PersonalityParametricData_TestDouble_SetDefrost(&personalityParametricData, &defrostData);
      DataModelErdPointerAccess_Write(dataModel, Erd_PersonalityParametricData, &personalityParametricData);
   }

   void DoorAccelerationIsInitialized()
   {
      DoorAcceleration_Init(&instance, dataModel, &config);
   }

   void DoorAccelerationFsmStateShouldBe(DoorAccelerationFsmState_t expectedState)
   {
      DoorAccelerationFsmState_t actualState;
      DataModel_Read(dataModel, Erd_DoorAccelerationFsmState, &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void DoorAccelerationRequestIs(DoorAcceleration_t request, Signal_t id)
   {
      DoorAccelerationRequest_t doorAccelerationRequest;
      doorAccelerationRequest.request = request;
      doorAccelerationRequest.requestId = id;
      DataModel_Write(dataModel, Erd_DoorAccelerationRequest, &doorAccelerationRequest);
   }

   void DoorAccelerationIsEnabled()
   {
      Given DoorAccelerationIsInitialized();

      When DoorAccelerationRequestIs(DoorAcceleration_Enable, 1);
      DoorAccelerationFsmStateShouldBe(DoorAccelerationFsmState_Enabled);
   }

   void FzDoorAccelerationIs(uint32_t count)
   {
      DataModel_Write(dataModel, Erd_DefrostFzDoorAccelerationCount, &count);
   }

   void FfDoorAccelerationIs(uint32_t count)
   {
      DataModel_Write(dataModel, Erd_DefrostFfDoorAccelerationCount, &count);
   }

   void FzDoorAccelerationShouldBe(uint32_t expectedCount)
   {
      uint32_t actualCount;
      DataModel_Read(dataModel, Erd_DefrostFzDoorAccelerationCount, &actualCount);

      CHECK_EQUAL(expectedCount, actualCount);
   }

   void FfDoorAccelerationShouldBe(uint32_t expectedCount)
   {
      uint32_t actualCount;
      DataModel_Read(dataModel, Erd_DefrostFfDoorAccelerationCount, &actualCount);

      CHECK_EQUAL(expectedCount, actualCount);
   }

   void LeftFfDoorIs(bool state)
   {
      DataModel_Write(dataModel, Erd_LeftHandFfDoorIsOpen, &state);
   }

   void RightFfDoorIs(bool state)
   {
      DataModel_Write(dataModel, Erd_RightHandFfDoorIsOpen, &state);
   }

   void DoorInDoorDoorIs(bool state)
   {
      DataModel_Write(dataModel, Erd_DoorInDoorIsOpen, &state);
   }

   void FzDoorIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FzDoorIsOpen, &state);
   }

   void After(TimerTicks_t ticks, TimeSourceTickCount_t ticksToElapseAtATime = 1000)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks, ticksToElapseAtATime);
   }
};

TEST(DoorAcceleration, ShouldInitializeAndBeDisabled)
{
   DoorAccelerationIsInitialized();
   DoorAccelerationFsmStateShouldBe(DoorAccelerationFsmState_Disabled);
}

TEST(DoorAcceleration, ShouldResetDoorAccelerationsToZeroWhenInitialized)
{
   Given FfDoorAccelerationIs(SomeRandomAcceleration);
   Given FzDoorAccelerationIs(AnotherRandomAcceleration);

   When DoorAccelerationIsInitialized();

   FfDoorAccelerationShouldBe(0);
   FzDoorAccelerationShouldBe(0);
}

TEST(DoorAcceleration, ShouldEnableWhenEnableRequestReceivedWhileDisabled)
{
   Given DoorAccelerationIsInitialized();

   When DoorAccelerationRequestIs(DoorAcceleration_Enable, 1);
   DoorAccelerationFsmStateShouldBe(DoorAccelerationFsmState_Enabled);
}

TEST(DoorAcceleration, ShouldNotEnableWhenResetRequestReceivedWhileDisabled)
{
   Given DoorAccelerationIsInitialized();

   When DoorAccelerationRequestIs(DoorAcceleration_Reset, 1);
   DoorAccelerationFsmStateShouldBe(DoorAccelerationFsmState_Disabled);
}

TEST(DoorAcceleration, ShouldDisableWhenDisableRequestReceivedWhileEnabled)
{
   Given DoorAccelerationIsEnabled();

   When DoorAccelerationRequestIs(DoorAcceleration_Disable, 2);
   DoorAccelerationFsmStateShouldBe(DoorAccelerationFsmState_Disabled);
}

TEST(DoorAcceleration, ShouldResetDoorAccelerationsToZeroWhenDisabled)
{
   Given FfDoorAccelerationIs(SomeRandomAcceleration);
   Given FzDoorAccelerationIs(AnotherRandomAcceleration);
   Given DoorAccelerationIsEnabled();

   When DoorAccelerationRequestIs(DoorAcceleration_Disable, 2);

   FfDoorAccelerationShouldBe(0);
   FzDoorAccelerationShouldBe(0);
   DoorAccelerationFsmStateShouldBe(DoorAccelerationFsmState_Disabled);
}

TEST(DoorAcceleration, ShouldResetDoorAccelerationsToZeroWhenResetWhileEnabled)
{
   Given DoorAccelerationIsEnabled();
   Given FfDoorAccelerationIs(SomeRandomAcceleration);
   Given FzDoorAccelerationIs(AnotherRandomAcceleration);

   When DoorAccelerationRequestIs(DoorAcceleration_Reset, 2);

   FfDoorAccelerationShouldBe(0);
   FzDoorAccelerationShouldBe(0);
   DoorAccelerationFsmStateShouldBe(DoorAccelerationFsmState_Enabled);
}

TEST(DoorAcceleration, ShouldIncrementFfDoorAccelerationWhenLeftHandFfDoorIsOpenAfterPeriodicTimeout)
{
   Given FfDoorAccelerationIs(0);
   Given DoorAccelerationIsEnabled();

   When LeftFfDoorIs(Open);

   After(1 * MSEC_PER_SEC - 1);
   FfDoorAccelerationShouldBe(0);

   After(1);
   FfDoorAccelerationShouldBe(0 + defrostData.ffDoorIncrementFactorInSecondsPerSecond);
}

TEST(DoorAcceleration, ShouldIncrementFfDoorAccelerationWhenRightHandFfDoorIsOpenAfterPeriodicTimeout)
{
   Given FfDoorAccelerationIs(0);
   Given DoorAccelerationIsEnabled();

   When RightFfDoorIs(Open);

   After(1 * MSEC_PER_SEC - 1);
   FfDoorAccelerationShouldBe(0);

   After(1);
   FfDoorAccelerationShouldBe(0 + defrostData.ffDoorIncrementFactorInSecondsPerSecond);
}

TEST(DoorAcceleration, ShouldIncrementFfDoorAccelerationWhenDoorInDoorIsOpenAfterPeriodicTimeout)
{
   Given FfDoorAccelerationIs(0);
   Given DoorAccelerationIsEnabled();

   When DoorInDoorDoorIs(Open);

   After(1 * MSEC_PER_SEC - 1);
   FfDoorAccelerationShouldBe(0);

   After(1);
   FfDoorAccelerationShouldBe(0 + defrostData.ffDoorIncrementFactorInSecondsPerSecond);
}

TEST(DoorAcceleration, ShouldIncrementFfDoorAccelerationWhenLeftHandFfDoorIsOpenForMultipleSeconds)
{
   Given FfDoorAccelerationIs(0);
   Given DoorAccelerationIsEnabled();

   When LeftFfDoorIs(Open);

   for(uint8_t i = 0; i < 10; i++)
   {
      After(1 * MSEC_PER_SEC - 1);
      FfDoorAccelerationShouldBe(i * defrostData.ffDoorIncrementFactorInSecondsPerSecond);

      After(1);
      FfDoorAccelerationShouldBe((i + 1) * defrostData.ffDoorIncrementFactorInSecondsPerSecond);
   }
}

TEST(DoorAcceleration, ShouldIncrementFfDoorAccelerationWhenRightHandFfDoorIsOpenForMultipleSeconds)
{
   Given FfDoorAccelerationIs(0);
   Given DoorAccelerationIsEnabled();

   When RightFfDoorIs(Open);

   for(uint8_t i = 0; i < 10; i++)
   {
      After(1 * MSEC_PER_SEC - 1);
      FfDoorAccelerationShouldBe(i * defrostData.ffDoorIncrementFactorInSecondsPerSecond);

      After(1);
      FfDoorAccelerationShouldBe((i + 1) * defrostData.ffDoorIncrementFactorInSecondsPerSecond);
   }
}

TEST(DoorAcceleration, ShouldIncrementFfDoorAccelerationWhenDoorInDoorIsOpenForMultipleSeconds)
{
   Given FfDoorAccelerationIs(0);
   Given DoorAccelerationIsEnabled();

   When DoorInDoorDoorIs(Open);

   for(uint8_t i = 0; i < 10; i++)
   {
      After(1 * MSEC_PER_SEC - 1);
      FfDoorAccelerationShouldBe(i * defrostData.ffDoorIncrementFactorInSecondsPerSecond);

      After(1);
      FfDoorAccelerationShouldBe((i + 1) * defrostData.ffDoorIncrementFactorInSecondsPerSecond);
   }
}

TEST(DoorAcceleration, ShouldIncrementFzDoorAccelerationWhenFzDoorIsOpenAfterPeriodicTimeout)
{
   Given FzDoorAccelerationIs(0);
   Given DoorAccelerationIsEnabled();

   When FzDoorIs(Open);

   After(1 * MSEC_PER_SEC - 1);
   FzDoorAccelerationShouldBe(0);

   After(1);
   FzDoorAccelerationShouldBe(0 + defrostData.fzDoorIncrementFactorInSecondsPerSecond);
}

TEST(DoorAcceleration, ShouldIncrementFzDoorAccelerationWhenFzDoorIsOpenForMultipleSeconds)
{
   Given FzDoorAccelerationIs(0);
   Given DoorAccelerationIsEnabled();

   When FzDoorIs(Open);

   for(uint8_t i = 0; i < 10; i++)
   {
      After(1 * MSEC_PER_SEC - 1);
      FzDoorAccelerationShouldBe(i * defrostData.fzDoorIncrementFactorInSecondsPerSecond);

      After(1);
      FzDoorAccelerationShouldBe((i + 1) * defrostData.fzDoorIncrementFactorInSecondsPerSecond);
   }
}

TEST(DoorAcceleration, ShouldIncrementBothFfAndFzDoorAccelerationsWhenAFfDoorIsOpenAndAFzDoorIsOpenAfterPeriodicTimeout)
{
   Given FfDoorAccelerationIs(0);
   Given FzDoorAccelerationIs(0);
   Given DoorAccelerationIsEnabled();

   When LeftFfDoorIs(Open);
   When FzDoorIs(Open);

   After(1 * MSEC_PER_SEC - 1);
   FfDoorAccelerationShouldBe(0);
   FzDoorAccelerationShouldBe(0);

   After(1);
   FfDoorAccelerationShouldBe(0 + defrostData.ffDoorIncrementFactorInSecondsPerSecond);
   FzDoorAccelerationShouldBe(0 + defrostData.fzDoorIncrementFactorInSecondsPerSecond);
}

TEST(DoorAcceleration, ShouldIncrementBothFfAndFzDoorAccelerationsWhenAFfDoorIsOpenAndAFzDoorIsOpenForMultipleSeconds)
{
   Given FfDoorAccelerationIs(0);
   Given FzDoorAccelerationIs(0);
   Given DoorAccelerationIsEnabled();

   When LeftFfDoorIs(Open);
   When FzDoorIs(Open);

   for(uint8_t i = 0; i < 10; i++)
   {
      After(1 * MSEC_PER_SEC - 1);
      FfDoorAccelerationShouldBe(i * defrostData.ffDoorIncrementFactorInSecondsPerSecond);
      FzDoorAccelerationShouldBe(i * defrostData.fzDoorIncrementFactorInSecondsPerSecond);

      After(1);
      FfDoorAccelerationShouldBe((i + 1) * defrostData.ffDoorIncrementFactorInSecondsPerSecond);
      FzDoorAccelerationShouldBe((i + 1) * defrostData.fzDoorIncrementFactorInSecondsPerSecond);
   }
}

TEST(DoorAcceleration, ShouldStopIncrementingFfDoorAccelerationWhenAllFfDoorsAreClosed)
{
   Given FfDoorAccelerationIs(0);
   Given DoorAccelerationIsEnabled();

   When LeftFfDoorIs(Open);

   for(uint8_t i = 0; i < 10; i++)
   {
      After(1 * MSEC_PER_SEC - 1);
      FfDoorAccelerationShouldBe(i * defrostData.ffDoorIncrementFactorInSecondsPerSecond);

      After(1);
      FfDoorAccelerationShouldBe((i + 1) * defrostData.ffDoorIncrementFactorInSecondsPerSecond);
   }

   When LeftFfDoorIs(Closed);

   After(1 * MSEC_PER_SEC - 1);
   FfDoorAccelerationShouldBe((10) * defrostData.ffDoorIncrementFactorInSecondsPerSecond);

   After(1);
   FfDoorAccelerationShouldBe((10) * defrostData.ffDoorIncrementFactorInSecondsPerSecond);
}

TEST(DoorAcceleration, ShouldStopIncrementingFzDoorAccelerationWhenFzDoorIsClosed)
{
   Given FzDoorAccelerationIs(0);
   Given DoorAccelerationIsEnabled();

   When FzDoorIs(Open);

   for(uint8_t i = 0; i < 10; i++)
   {
      After(1 * MSEC_PER_SEC - 1);
      FzDoorAccelerationShouldBe(i * defrostData.fzDoorIncrementFactorInSecondsPerSecond);

      After(1);
      FzDoorAccelerationShouldBe((i + 1) * defrostData.fzDoorIncrementFactorInSecondsPerSecond);
   }

   When FzDoorIs(Closed);

   After(1 * MSEC_PER_SEC - 1);
   FzDoorAccelerationShouldBe((10) * defrostData.fzDoorIncrementFactorInSecondsPerSecond);

   After(1);
   FzDoorAccelerationShouldBe((10) * defrostData.fzDoorIncrementFactorInSecondsPerSecond);
}

TEST(DoorAcceleration, ShouldOnlyIncrementFfDoorAccelerationByOneTimesTheFactorPerSecondEvenIfMultipleFfDoorAreOpen)
{
   Given FfDoorAccelerationIs(0);
   Given DoorAccelerationIsEnabled();

   When LeftFfDoorIs(Open);
   When RightFfDoorIs(Open);

   After(1 * MSEC_PER_SEC - 1);
   FfDoorAccelerationShouldBe(0);

   After(1);
   FfDoorAccelerationShouldBe(0 + defrostData.ffDoorIncrementFactorInSecondsPerSecond);
}
