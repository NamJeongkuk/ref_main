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
   .freezerDoorIncrementFactorInSecondsPerSecond = 348,
   .freshFoodDoorIncrementFactorInSecondsPerSecond = 87,
   .freezerAbnormalRunTimeInMinutes = 6 * 60,
   .maxTimeBetweenDefrostsInMinutes = 32 * 60,
   .dmFreezerDefrostTemperatureInDegFx100 = 1500,
   .prechillFreezerSetpointInDegFx100 = 600,
   .prechillFreshFoodSetpointInDegFx100 = 4600,
   .prechillConvertibleCompartmentSetpointInDegFx100 = 600,
   .prechillFreezerEvapExitTemperatureInDegFx100 = -3000,
   .prechillConvertibleCompartmentEvapExitTemperatureInDegFx100 = -3000,
   .maxPrechillTimeInMinutes = 10,
   .maxPrechillTimeForFreshFoodOnlyDefrostInMinutes = 20,
   .defrostDoorHoldoffTimeForFreshFoodAndFreezerInMinutes = 60,
   .defrostDoorHoldoffTimeForFreshFoodOnlyInMinutes = 50,
   .defrostMaxHoldoffTimeInMinutes = 60,
   .maxPrechillHoldoffTimeAfterDefrostTimerSatisfiedInSeconds = 0,
   .freshFoodFanDefrostFreshFoodEvapExitTemperatureInDegFx100 = 3600,
   .freshFoodFanDefrostFreshFoodFanMaxOnTimeInMinutes = 10,
   .convertibleCompartmentFanDefrostConvertibleCompartmentEvapExitTemperatureInDegFx100 = 3200,
   .convertibleCompartmentFanDefrostConvertibleCompartmentFanMaxOnTimeInMinutes = 10,
   .freezerDefrostHeaterMaxOnTimeInMinutes = 60,
   .freezerInvalidThermistorDefrostHeaterMaxOnTimeInMinutes = 30,
   .freezerAbnormalDefrostHeaterMaxOnTimeInMinutes = 32,
   .freezerDefrostTerminationTemperatureInDegFx100 = 5900,
   .freshFoodDefrostTerminationTemperatureInDegFx100 = 4460,
   .convertibleCompartmentDefrostTerminationTemperatureInDegFx100 = 4460,
   .freshFoodDefrostHeaterMaxOnTimeInMinutes = 60,
   .freshFoodInvalidThermistorDefrostHeaterMaxOnTimeInMinutes = 20,
   .freshFoodAbnormalDefrostHeaterMaxOnTimeInMinutes = 21,
   .convertibleCompartmentDefrostHeaterMaxOnTimeInMinutes = 60,
   .convertibleCompartmentAsFreshFoodAbnormalDefrostHeaterMaxOnTimeInMinutes = 21,
   .convertibleCompartmentAsFreezerAbnormalDefrostHeaterMaxOnTimeInMinutes = 35,
   .defrostDwellTimeInMinutes = 7,
   .freshFoodAndFreezerPostDwellFreezerExitTemperatureInDegFx100 = -1000,
   .freshFoodAndFreezerPostDwellFreezerExitTimeInMinutes = 10,
   .dwellThreeWayValvePosition = ValvePosition_A,
   .postDwellThreeWayValvePositionForFreshFoodAndFreezer = ValvePosition_A,
   .freshFoodPostDefrostPullDownExitTemperatureInDegFx100 = 4000,
   .freezerPostDefrostPullDownExitTemperatureInDegFx100 = 4000,
   .numberOfFreshFoodDefrostsBeforeFreezerDefrost = 2,
   .numberOfFreshFoodDefrostsBeforeAbnormalFreezerDefrost = 1,
   .freshFoodOnlyPostDwellExitTimeInMinutes = 10,
   .dsmFreezerSetpointTemperatureInDegFx100 = 200,
   .defrostPeriodicTimeoutInSeconds = 1,
   .threeWayValvePositionToExitIdle = ValvePosition_B,
   .threeWayValvePositionForMaxPrechillHoldoff = ValvePosition_B,
   .threeWayValvePositionToExtendDefrostWithFreshFoodCycleDefrost = ValvePosition_B
};

static const DoorAccelerationConfig_t config = {
   .doorAccelerationRequestErd = Erd_DoorAccelerationRequest,
   .doorAccelerationFsmStateErd = Erd_DoorAccelerationFsmState,
   .freezerDoorAccelerationCountsErd = Erd_DefrostFreezerDoorAccelerationCount,
   .freshFoodDoorAccelerationCountsErd = Erd_DefrostFreshFoodDoorAccelerationCount,
   .leftHandFreshFoodDoorIsOpenErd = Erd_LeftHandFreshFoodDoorIsOpen,
   .rightHandFreshFoodDoorIsOpenErd = Erd_RightHandFreshFoodDoorIsOpen,
   .doorInDoorIsOpenErd = Erd_DoorInDoorIsOpen,
   .freezerDoorIsOpenErd = Erd_FreezerDoorIsOpen,
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

   void FreezerDoorAccelerationIs(uint32_t count)
   {
      DataModel_Write(dataModel, Erd_DefrostFreezerDoorAccelerationCount, &count);
   }

   void FreshFoodDoorAccelerationIs(uint32_t count)
   {
      DataModel_Write(dataModel, Erd_DefrostFreshFoodDoorAccelerationCount, &count);
   }

   void FreezerDoorAccelerationShouldBe(uint32_t expectedCount)
   {
      uint32_t actualCount;
      DataModel_Read(dataModel, Erd_DefrostFreezerDoorAccelerationCount, &actualCount);

      CHECK_EQUAL(expectedCount, actualCount);
   }

   void FreshFoodDoorAccelerationShouldBe(uint32_t expectedCount)
   {
      uint32_t actualCount;
      DataModel_Read(dataModel, Erd_DefrostFreshFoodDoorAccelerationCount, &actualCount);

      CHECK_EQUAL(expectedCount, actualCount);
   }

   void LeftFreshFoodDoorIs(bool state)
   {
      DataModel_Write(dataModel, Erd_LeftHandFreshFoodDoorIsOpen, &state);
   }

   void RightFreshFoodDoorIs(bool state)
   {
      DataModel_Write(dataModel, Erd_RightHandFreshFoodDoorIsOpen, &state);
   }

   void DoorInDoorDoorIs(bool state)
   {
      DataModel_Write(dataModel, Erd_DoorInDoorIsOpen, &state);
   }

   void FreezerDoorIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreezerDoorIsOpen, &state);
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
   Given FreshFoodDoorAccelerationIs(SomeRandomAcceleration);
   Given FreezerDoorAccelerationIs(AnotherRandomAcceleration);

   When DoorAccelerationIsInitialized();

   FreshFoodDoorAccelerationShouldBe(0);
   FreezerDoorAccelerationShouldBe(0);
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
   Given FreshFoodDoorAccelerationIs(SomeRandomAcceleration);
   Given FreezerDoorAccelerationIs(AnotherRandomAcceleration);
   Given DoorAccelerationIsEnabled();

   When DoorAccelerationRequestIs(DoorAcceleration_Disable, 2);

   FreshFoodDoorAccelerationShouldBe(0);
   FreezerDoorAccelerationShouldBe(0);
   DoorAccelerationFsmStateShouldBe(DoorAccelerationFsmState_Disabled);
}

TEST(DoorAcceleration, ShouldResetDoorAccelerationsToZeroWhenResetWhileEnabled)
{
   Given DoorAccelerationIsEnabled();
   Given FreshFoodDoorAccelerationIs(SomeRandomAcceleration);
   Given FreezerDoorAccelerationIs(AnotherRandomAcceleration);

   When DoorAccelerationRequestIs(DoorAcceleration_Reset, 2);

   FreshFoodDoorAccelerationShouldBe(0);
   FreezerDoorAccelerationShouldBe(0);
   DoorAccelerationFsmStateShouldBe(DoorAccelerationFsmState_Enabled);
}

TEST(DoorAcceleration, ShouldIncrementFreshFoodDoorAccelerationWhenLeftHandFreshFoodDoorIsOpenAfterPeriodicTimeout)
{
   Given FreshFoodDoorAccelerationIs(0);
   Given DoorAccelerationIsEnabled();

   When LeftFreshFoodDoorIs(Open);

   After(1 * MSEC_PER_SEC - 1);
   FreshFoodDoorAccelerationShouldBe(0);

   After(1);
   FreshFoodDoorAccelerationShouldBe(0 + defrostData.freshFoodDoorIncrementFactorInSecondsPerSecond);
}

TEST(DoorAcceleration, ShouldIncrementFreshFoodDoorAccelerationWhenRightHandFreshFoodDoorIsOpenAfterPeriodicTimeout)
{
   Given FreshFoodDoorAccelerationIs(0);
   Given DoorAccelerationIsEnabled();

   When RightFreshFoodDoorIs(Open);

   After(1 * MSEC_PER_SEC - 1);
   FreshFoodDoorAccelerationShouldBe(0);

   After(1);
   FreshFoodDoorAccelerationShouldBe(0 + defrostData.freshFoodDoorIncrementFactorInSecondsPerSecond);
}

TEST(DoorAcceleration, ShouldIncrementFreshFoodDoorAccelerationWhenDoorInDoorIsOpenAfterPeriodicTimeout)
{
   Given FreshFoodDoorAccelerationIs(0);
   Given DoorAccelerationIsEnabled();

   When DoorInDoorDoorIs(Open);

   After(1 * MSEC_PER_SEC - 1);
   FreshFoodDoorAccelerationShouldBe(0);

   After(1);
   FreshFoodDoorAccelerationShouldBe(0 + defrostData.freshFoodDoorIncrementFactorInSecondsPerSecond);
}

TEST(DoorAcceleration, ShouldIncrementFreshFoodDoorAccelerationWhenLeftHandFreshFoodDoorIsOpenForMultipleSeconds)
{
   Given FreshFoodDoorAccelerationIs(0);
   Given DoorAccelerationIsEnabled();

   When LeftFreshFoodDoorIs(Open);

   for(uint8_t i = 0; i < 10; i++)
   {
      After(1 * MSEC_PER_SEC - 1);
      FreshFoodDoorAccelerationShouldBe(i * defrostData.freshFoodDoorIncrementFactorInSecondsPerSecond);

      After(1);
      FreshFoodDoorAccelerationShouldBe((i + 1) * defrostData.freshFoodDoorIncrementFactorInSecondsPerSecond);
   }
}

TEST(DoorAcceleration, ShouldIncrementFreshFoodDoorAccelerationWhenRightHandFreshFoodDoorIsOpenForMultipleSeconds)
{
   Given FreshFoodDoorAccelerationIs(0);
   Given DoorAccelerationIsEnabled();

   When RightFreshFoodDoorIs(Open);

   for(uint8_t i = 0; i < 10; i++)
   {
      After(1 * MSEC_PER_SEC - 1);
      FreshFoodDoorAccelerationShouldBe(i * defrostData.freshFoodDoorIncrementFactorInSecondsPerSecond);

      After(1);
      FreshFoodDoorAccelerationShouldBe((i + 1) * defrostData.freshFoodDoorIncrementFactorInSecondsPerSecond);
   }
}

TEST(DoorAcceleration, ShouldIncrementFreshFoodDoorAccelerationWhenDoorInDoorIsOpenForMultipleSeconds)
{
   Given FreshFoodDoorAccelerationIs(0);
   Given DoorAccelerationIsEnabled();

   When DoorInDoorDoorIs(Open);

   for(uint8_t i = 0; i < 10; i++)
   {
      After(1 * MSEC_PER_SEC - 1);
      FreshFoodDoorAccelerationShouldBe(i * defrostData.freshFoodDoorIncrementFactorInSecondsPerSecond);

      After(1);
      FreshFoodDoorAccelerationShouldBe((i + 1) * defrostData.freshFoodDoorIncrementFactorInSecondsPerSecond);
   }
}

TEST(DoorAcceleration, ShouldIncrementFreezerDoorAccelerationWhenFreezerDoorIsOpenAfterPeriodicTimeout)
{
   Given FreezerDoorAccelerationIs(0);
   Given DoorAccelerationIsEnabled();

   When FreezerDoorIs(Open);

   After(1 * MSEC_PER_SEC - 1);
   FreezerDoorAccelerationShouldBe(0);

   After(1);
   FreezerDoorAccelerationShouldBe(0 + defrostData.freezerDoorIncrementFactorInSecondsPerSecond);
}

TEST(DoorAcceleration, ShouldIncrementFreezerDoorAccelerationWhenFreezerDoorIsOpenForMultipleSeconds)
{
   Given FreezerDoorAccelerationIs(0);
   Given DoorAccelerationIsEnabled();

   When FreezerDoorIs(Open);

   for(uint8_t i = 0; i < 10; i++)
   {
      After(1 * MSEC_PER_SEC - 1);
      FreezerDoorAccelerationShouldBe(i * defrostData.freezerDoorIncrementFactorInSecondsPerSecond);

      After(1);
      FreezerDoorAccelerationShouldBe((i + 1) * defrostData.freezerDoorIncrementFactorInSecondsPerSecond);
   }
}

TEST(DoorAcceleration, ShouldIncrementBothFreshFoodAndFreezerDoorAccelerationsWhenAFreshFoodDoorIsOpenAndAFreezerDoorIsOpenAfterPeriodicTimeout)
{
   Given FreshFoodDoorAccelerationIs(0);
   Given FreezerDoorAccelerationIs(0);
   Given DoorAccelerationIsEnabled();

   When LeftFreshFoodDoorIs(Open);
   When FreezerDoorIs(Open);

   After(1 * MSEC_PER_SEC - 1);
   FreshFoodDoorAccelerationShouldBe(0);
   FreezerDoorAccelerationShouldBe(0);

   After(1);
   FreshFoodDoorAccelerationShouldBe(0 + defrostData.freshFoodDoorIncrementFactorInSecondsPerSecond);
   FreezerDoorAccelerationShouldBe(0 + defrostData.freezerDoorIncrementFactorInSecondsPerSecond);
}

TEST(DoorAcceleration, ShouldIncrementBothFreshFoodAndFreezerDoorAccelerationsWhenAFreshFoodDoorIsOpenAndAFreezerDoorIsOpenForMultipleSeconds)
{
   Given FreshFoodDoorAccelerationIs(0);
   Given FreezerDoorAccelerationIs(0);
   Given DoorAccelerationIsEnabled();

   When LeftFreshFoodDoorIs(Open);
   When FreezerDoorIs(Open);

   for(uint8_t i = 0; i < 10; i++)
   {
      After(1 * MSEC_PER_SEC - 1);
      FreshFoodDoorAccelerationShouldBe(i * defrostData.freshFoodDoorIncrementFactorInSecondsPerSecond);
      FreezerDoorAccelerationShouldBe(i * defrostData.freezerDoorIncrementFactorInSecondsPerSecond);

      After(1);
      FreshFoodDoorAccelerationShouldBe((i + 1) * defrostData.freshFoodDoorIncrementFactorInSecondsPerSecond);
      FreezerDoorAccelerationShouldBe((i + 1) * defrostData.freezerDoorIncrementFactorInSecondsPerSecond);
   }
}

TEST(DoorAcceleration, ShouldStopIncrementingFreshFoodDoorAccelerationWhenAllFreshFoodDoorsAreClosed)
{
   Given FreshFoodDoorAccelerationIs(0);
   Given DoorAccelerationIsEnabled();

   When LeftFreshFoodDoorIs(Open);

   for(uint8_t i = 0; i < 10; i++)
   {
      After(1 * MSEC_PER_SEC - 1);
      FreshFoodDoorAccelerationShouldBe(i * defrostData.freshFoodDoorIncrementFactorInSecondsPerSecond);

      After(1);
      FreshFoodDoorAccelerationShouldBe((i + 1) * defrostData.freshFoodDoorIncrementFactorInSecondsPerSecond);
   }

   When LeftFreshFoodDoorIs(Closed);

   After(1 * MSEC_PER_SEC - 1);
   FreshFoodDoorAccelerationShouldBe((10) * defrostData.freshFoodDoorIncrementFactorInSecondsPerSecond);

   After(1);
   FreshFoodDoorAccelerationShouldBe((10) * defrostData.freshFoodDoorIncrementFactorInSecondsPerSecond);
}

TEST(DoorAcceleration, ShouldStopIncrementingFreezerDoorAccelerationWhenFreezerDoorIsClosed)
{
   Given FreezerDoorAccelerationIs(0);
   Given DoorAccelerationIsEnabled();

   When FreezerDoorIs(Open);

   for(uint8_t i = 0; i < 10; i++)
   {
      After(1 * MSEC_PER_SEC - 1);
      FreezerDoorAccelerationShouldBe(i * defrostData.freezerDoorIncrementFactorInSecondsPerSecond);

      After(1);
      FreezerDoorAccelerationShouldBe((i + 1) * defrostData.freezerDoorIncrementFactorInSecondsPerSecond);
   }

   When FreezerDoorIs(Closed);

   After(1 * MSEC_PER_SEC - 1);
   FreezerDoorAccelerationShouldBe((10) * defrostData.freezerDoorIncrementFactorInSecondsPerSecond);

   After(1);
   FreezerDoorAccelerationShouldBe((10) * defrostData.freezerDoorIncrementFactorInSecondsPerSecond);
}

TEST(DoorAcceleration, ShouldOnlyIncrementFreshFoodDoorAccelerationByOneTimesTheFactorPerSecondEvenIfMultipleFreshFoodDoorAreOpen)
{
   Given FreshFoodDoorAccelerationIs(0);
   Given DoorAccelerationIsEnabled();

   When LeftFreshFoodDoorIs(Open);
   When RightFreshFoodDoorIs(Open);

   After(1 * MSEC_PER_SEC - 1);
   FreshFoodDoorAccelerationShouldBe(0);

   After(1);
   FreshFoodDoorAccelerationShouldBe(0 + defrostData.freshFoodDoorIncrementFactorInSecondsPerSecond);
}
