/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "DefrostReadyTimerIsSatisfied.h"
#include "DataModelErdPointerAccess.h"
#include "SystemErds.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "PersonalityParametricData_TestDouble.h"
#include "DefrostData_TestDouble.h"
#include "uassert_test.h"

#define Given
#define When
#define Then
#define And

#define MinTimeBetweenDefrostsInMinutes (defrostData->minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes)
#define MinTimeBetweenDefrostsInSeconds (defrostData->minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE)

static DefrostReadyTimerIsSatisfiedConfiguration_t config = {
   .compressorOnTimeInSecondsErd = Erd_DefrostCompressorOnTimeInSeconds,
   .freshFoodScaledDoorAccelerationInSecondsErd = Erd_DefrostFreshFoodScaledDoorAccelerationInSeconds,
   .freezerScaledDoorAccelerationInSecondsErd = Erd_DefrostFreezerScaledDoorAccelerationInSeconds,
   .convertibleCompartmentScaledDoorAccelerationInSecondsErd = Erd_DefrostConvertibleCompartmentScaledDoorAccelerationInSeconds,
   .defrostReadyTimerIsSatisfiedErd = Erd_DefrostReadyTimerIsSatisfied,
   .timeInMinutesWhenDefrostReadyTimerIsSatisfiedErd = Erd_TimeInMinutesWhenDefrostReadyTimerIsSatisfied,
   .defrostCompressorOnTimeCounterReadyErd = Erd_DefrostCompressorOnTimeCounterReady,
   .doorAccelerationCounterReadyErd = Erd_DoorAccelerationCounterReady
};

TEST_GROUP(DefrostReadyTimerIsSatisfied)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   const DefrostData_t *defrostData;

   DefrostReadyTimerIsSatisfied_t instance;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &timerModuleTestDouble->timerModule);

      defrostData = PersonalityParametricData_Get(dataModel)->defrostData;
   }

   void DefrostReadyTimerIsSatisfiedModuleIsInitialized()
   {
      DataModel_Write(dataModel, Erd_DefrostCompressorOnTimeCounterReady, set);
      DataModel_Write(dataModel, Erd_DoorAccelerationCounterReady, set);

      DefrostReadyTimerIsSatisfied_Init(&instance, dataModel, &config);
   }

   void DefrostReadyTimerIsSatisfiedIs(bool state)
   {
      DataModel_Write(dataModel, Erd_DefrostReadyTimerIsSatisfied, &state);
   }

   void CompressorOnTimeInSecondsIs(uint32_t seconds)
   {
      DataModel_Write(dataModel, Erd_DefrostCompressorOnTimeInSeconds, &seconds);
   }

   void FreshFoodDoorAccelerationIs(uint32_t count)
   {
      DataModel_Write(dataModel, Erd_DefrostFreshFoodScaledDoorAccelerationInSeconds, &count);
   }

   void FreezerDoorAccelerationIs(uint32_t count)
   {
      DataModel_Write(dataModel, Erd_DefrostFreezerScaledDoorAccelerationInSeconds, &count);
   }

   void ConvertibleCompartmentDoorAccelerationIs(uint32_t count)
   {
      DataModel_Write(dataModel, Erd_DefrostConvertibleCompartmentScaledDoorAccelerationInSeconds, &count);
   }

   void DefrostReadyTimerIsSatisfiedShouldBe(bool expectedState)
   {
      bool actualState;
      DataModel_Read(dataModel, Erd_DefrostReadyTimerIsSatisfied, &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void TimeInMinutesWhenDefrostReadyTimerIsSatisfiedIs(uint16_t timeInMinutes)
   {
      DataModel_Write(dataModel, Erd_TimeInMinutesWhenDefrostReadyTimerIsSatisfied, &timeInMinutes);
   }
};

TEST(DefrostReadyTimerIsSatisfied, ShouldInitialize)
{
   DefrostReadyTimerIsSatisfiedModuleIsInitialized();
}

TEST(DefrostReadyTimerIsSatisfied, ShouldClearDefrostReadyTimerIsSatisfiedErdWhenCompressorOnTimeIsLessThanMinimumTimeBetweenDefrostsAbnormalRunTimeOnInit)
{
   Given DefrostReadyTimerIsSatisfiedIs(SET);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds - 1);
   And DefrostReadyTimerIsSatisfiedModuleIsInitialized();

   DefrostReadyTimerIsSatisfiedShouldBe(CLEAR);
}

TEST(DefrostReadyTimerIsSatisfied, ShouldClearDefrostReadyTimerIsSatisfiedErdWhenCompressorOnTimeHasNotReachedMinimumTimeBetweenDefrostAbnormalRunTimeInMinutesAndSumOfCompressorOnTimeAndDoorAccelerationsAreEqualToTimeWhenDefrostReadyTimerIsSatisfiedOnInit)
{
   Given DefrostReadyTimerIsSatisfiedIs(SET);
   And TimeInMinutesWhenDefrostReadyTimerIsSatisfiedIs(MinTimeBetweenDefrostsInMinutes * 2);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds - 1);
   And FreshFoodDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds);
   And FreezerDoorAccelerationIs(1);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And DefrostReadyTimerIsSatisfiedModuleIsInitialized();

   DefrostReadyTimerIsSatisfiedShouldBe(CLEAR);
}

TEST(DefrostReadyTimerIsSatisfied, ShouldClearDefrostReadyTimerIsSatisfiedErdWhenCompressorOnTimeHasReachedMinimumTimeBetweenDefrostAbnormalRunTimeInMinutesAndSumOfCompressorOnTimeAndDoorAccelerationsAreLessThanTimeWhenDefrostReadyTimerIsSatisfiedOnInit)
{
   Given DefrostReadyTimerIsSatisfiedIs(SET);
   And TimeInMinutesWhenDefrostReadyTimerIsSatisfiedIs(MinTimeBetweenDefrostsInMinutes * 2);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds - 1);
   And FreezerDoorAccelerationIs(0);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And DefrostReadyTimerIsSatisfiedModuleIsInitialized();

   DefrostReadyTimerIsSatisfiedShouldBe(CLEAR);
}

TEST(DefrostReadyTimerIsSatisfied, ShouldSetDefrostReadyTimerIsSatisfiedErdWhenCompressorOnTimeHasReachedMinimumTimeBetweenDefrostAbnormalRunTimeInMinutesAndSumOfCompressorOnTimeAndDoorAccelerationsAreEqualToTimeWhenDefrostReadyTimerIsSatisfiedOnInit)
{
   Given DefrostReadyTimerIsSatisfiedIs(CLEAR);
   And TimeInMinutesWhenDefrostReadyTimerIsSatisfiedIs(MinTimeBetweenDefrostsInMinutes * 2);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds);
   And FreezerDoorAccelerationIs(0);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And DefrostReadyTimerIsSatisfiedModuleIsInitialized();

   DefrostReadyTimerIsSatisfiedShouldBe(SET);
}

TEST(DefrostReadyTimerIsSatisfied, ShouldSetDefrostReadyTimerIsSatisfiedErdWhenCompressorOnTimeHasReachedMinimumTimeBetweenDefrostAbnormalRunTimeInMinutesAndSumOfCompressorOnTimeAndDoorAccelerationsAreGreaterThanTimeWhenDefrostReadyTimerIsSatisfiedOnInit)
{
   Given DefrostReadyTimerIsSatisfiedIs(CLEAR);
   And TimeInMinutesWhenDefrostReadyTimerIsSatisfiedIs(MinTimeBetweenDefrostsInMinutes * 2);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds);
   And FreezerDoorAccelerationIs(1);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And DefrostReadyTimerIsSatisfiedModuleIsInitialized();

   DefrostReadyTimerIsSatisfiedShouldBe(SET);
}

TEST(DefrostReadyTimerIsSatisfied, ShouldClearDefrostReadyTimerIsSatisfiedErdWhenCompressorOnTimeIsGreaterThanMinimumTimeBetweenDefrostAbnormalRunTimeInMinutesAndSumOfCompressorOnTimeAndDoorAccelerationsAreLessThanTimeWhenDefrostReadyTimerIsSatisfiedOnInit)
{
   Given DefrostReadyTimerIsSatisfiedIs(SET);
   And TimeInMinutesWhenDefrostReadyTimerIsSatisfiedIs(MinTimeBetweenDefrostsInMinutes * 2);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds + 1);
   And FreshFoodDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds - 2);
   And FreezerDoorAccelerationIs(0);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And DefrostReadyTimerIsSatisfiedModuleIsInitialized();

   DefrostReadyTimerIsSatisfiedShouldBe(CLEAR);
}

TEST(DefrostReadyTimerIsSatisfied, ShouldSetDefrostReadyTimerIsSatisfiedErdWhenCompressorOnTimeIsGreaterThanMinimumTimeBetweenDefrostAbnormalRunTimeInMinutesAndSumOfCompressorOnTimeAndDoorAccelerationsAreEqualToTimeWhenDefrostReadyTimerIsSatisfiedOnInit)
{
   Given DefrostReadyTimerIsSatisfiedIs(CLEAR);
   And TimeInMinutesWhenDefrostReadyTimerIsSatisfiedIs(MinTimeBetweenDefrostsInMinutes * 2);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds + 1);
   And FreshFoodDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds - 1);
   And FreezerDoorAccelerationIs(0);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And DefrostReadyTimerIsSatisfiedModuleIsInitialized();

   DefrostReadyTimerIsSatisfiedShouldBe(SET);
}

TEST(DefrostReadyTimerIsSatisfied, ShouldSetDefrostReadyTimerIsSatisfiedErdWhenCompressorOnTimeIsGreaterThanMinimumTimeBetweenDefrostAbnormalRunTimeInMinutesAndSumOfCompressorOnTimeAndDoorAccelerationsAreGreaterThanTimeWhenDefrostReadyTimerIsSatisfiedOnInit)
{
   Given DefrostReadyTimerIsSatisfiedIs(CLEAR);
   And TimeInMinutesWhenDefrostReadyTimerIsSatisfiedIs(MinTimeBetweenDefrostsInMinutes * 2);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds + 1);
   And FreshFoodDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds);
   And FreezerDoorAccelerationIs(1);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And DefrostReadyTimerIsSatisfiedModuleIsInitialized();

   DefrostReadyTimerIsSatisfiedShouldBe(SET);
}

TEST(DefrostReadyTimerIsSatisfied, ShouldClearDefrostReadyTimerIsSatisfiedErdWhenCompressorOnTimeIsLessThanMinimumTimeBetweenDefrostAbnormalRunTimeInMinutes)
{
   Given DefrostReadyTimerIsSatisfiedIs(CLEAR);
   And TimeInMinutesWhenDefrostReadyTimerIsSatisfiedIs(MinTimeBetweenDefrostsInMinutes * 2);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds);
   And FreezerDoorAccelerationIs(1);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And DefrostReadyTimerIsSatisfiedModuleIsInitialized();

   DefrostReadyTimerIsSatisfiedShouldBe(SET);

   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds - 1);
   DefrostReadyTimerIsSatisfiedShouldBe(CLEAR);
}

TEST(DefrostReadyTimerIsSatisfied, ShouldSetDefrostReadyTimerIsSatisfiedErdWhenCompressorOnTimeIsEqualToMinimumTimeBetweenDefrostAbnormalRunTimeInMinutes)
{
   Given DefrostReadyTimerIsSatisfiedIs(SET);
   And TimeInMinutesWhenDefrostReadyTimerIsSatisfiedIs(MinTimeBetweenDefrostsInMinutes * 2);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds - 1);
   And FreshFoodDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds);
   And FreezerDoorAccelerationIs(1);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And DefrostReadyTimerIsSatisfiedModuleIsInitialized();

   DefrostReadyTimerIsSatisfiedShouldBe(CLEAR);

   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   DefrostReadyTimerIsSatisfiedShouldBe(SET);
}

TEST(DefrostReadyTimerIsSatisfied, ShouldSetDefrostReadyTimerIsSatisfiedErdWhenCompressorOnTimeIsGreaterThanMinimumTimeBetweenDefrostAbnormalRunTimeInMinutes)
{
   Given DefrostReadyTimerIsSatisfiedIs(SET);
   And TimeInMinutesWhenDefrostReadyTimerIsSatisfiedIs(MinTimeBetweenDefrostsInMinutes * 2);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds - 1);
   And FreshFoodDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds);
   And FreezerDoorAccelerationIs(1);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And DefrostReadyTimerIsSatisfiedModuleIsInitialized();

   DefrostReadyTimerIsSatisfiedShouldBe(CLEAR);

   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds + 1);
   DefrostReadyTimerIsSatisfiedShouldBe(SET);
}

TEST(DefrostReadyTimerIsSatisfied, ShouldClearDefrostReadyTimerIsSatisfiedErdWhenFreshFoodDoorAccelerationChangesSuchThatSumOfCompressorOnTimeAndDoorAccelerationIsLessThanTimeWhenDefrostReadyTimerIsSatisfied)
{
   Given DefrostReadyTimerIsSatisfiedIs(CLEAR);
   And TimeInMinutesWhenDefrostReadyTimerIsSatisfiedIs(MinTimeBetweenDefrostsInMinutes * 2);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds);
   And FreezerDoorAccelerationIs(0);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And DefrostReadyTimerIsSatisfiedModuleIsInitialized();

   DefrostReadyTimerIsSatisfiedShouldBe(SET);

   And FreshFoodDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds - 1);
   DefrostReadyTimerIsSatisfiedShouldBe(CLEAR);
}

TEST(DefrostReadyTimerIsSatisfied, ShouldClearDefrostReadyTimerIsSatisfiedErdWhenFreezerDoorAccelerationChangesSuchThatSumOfCompressorOnTimeAndDoorAccelerationIsLessThanTimeWhenDefrostReadyTimerIsSatisfied)
{
   Given DefrostReadyTimerIsSatisfiedIs(CLEAR);
   And TimeInMinutesWhenDefrostReadyTimerIsSatisfiedIs(MinTimeBetweenDefrostsInMinutes * 2);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(0);
   And FreezerDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And DefrostReadyTimerIsSatisfiedModuleIsInitialized();

   DefrostReadyTimerIsSatisfiedShouldBe(SET);

   And FreezerDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds - 1);
   DefrostReadyTimerIsSatisfiedShouldBe(CLEAR);
}

TEST(DefrostReadyTimerIsSatisfied, ShouldClearDefrostReadyTimerIsSatisfiedErdWhenConvertibleCompartmentDoorAccelerationChangesSuchThatSumOfCompressorOnTimeAndDoorAccelerationIsLessThanTimeWhenDefrostReadyTimerIsSatisfied)
{
   Given DefrostReadyTimerIsSatisfiedIs(CLEAR);
   And TimeInMinutesWhenDefrostReadyTimerIsSatisfiedIs(MinTimeBetweenDefrostsInMinutes * 2);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(0);
   And FreezerDoorAccelerationIs(0);
   And ConvertibleCompartmentDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds);
   And DefrostReadyTimerIsSatisfiedModuleIsInitialized();

   DefrostReadyTimerIsSatisfiedShouldBe(SET);

   And ConvertibleCompartmentDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds - 1);
   DefrostReadyTimerIsSatisfiedShouldBe(CLEAR);
}

TEST(DefrostReadyTimerIsSatisfied, ShouldSetDefrostReadyTimerIsSatisfiedErdWhenFreshFoodDoorAccelerationChangesSuchThatSumOfCompressorOnTimeAndDoorAccelerationIsEqualToTimeWhenDefrostReadyTimerIsSatisfied)
{
   Given DefrostReadyTimerIsSatisfiedIs(SET);
   And TimeInMinutesWhenDefrostReadyTimerIsSatisfiedIs(MinTimeBetweenDefrostsInMinutes * 2);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds - 1);
   And FreezerDoorAccelerationIs(0);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And DefrostReadyTimerIsSatisfiedModuleIsInitialized();

   DefrostReadyTimerIsSatisfiedShouldBe(CLEAR);

   And FreshFoodDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds);
   DefrostReadyTimerIsSatisfiedShouldBe(SET);
}

TEST(DefrostReadyTimerIsSatisfied, ShouldSetDefrostReadyTimerIsSatisfiedErdWhenFreezerDoorAccelerationChangesSuchThatSumOfCompressorOnTimeAndDoorAccelerationIsEqualToTimeWhenDefrostReadyTimerIsSatisfied)
{
   Given DefrostReadyTimerIsSatisfiedIs(SET);
   And TimeInMinutesWhenDefrostReadyTimerIsSatisfiedIs(MinTimeBetweenDefrostsInMinutes * 2);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(0);
   And FreezerDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds - 1);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And DefrostReadyTimerIsSatisfiedModuleIsInitialized();

   DefrostReadyTimerIsSatisfiedShouldBe(CLEAR);

   And FreshFoodDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds);
   DefrostReadyTimerIsSatisfiedShouldBe(SET);
}

TEST(DefrostReadyTimerIsSatisfied, ShouldSetDefrostReadyTimerIsSatisfiedErdWhenConvertibleCompartmentDoorAccelerationChangesSuchThatSumOfCompressorOnTimeAndDoorAccelerationIsEqualToTimeWhenDefrostReadyTimerIsSatisfied)
{
   Given DefrostReadyTimerIsSatisfiedIs(SET);
   And TimeInMinutesWhenDefrostReadyTimerIsSatisfiedIs(MinTimeBetweenDefrostsInMinutes * 2);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(0);
   And FreezerDoorAccelerationIs(0);
   And ConvertibleCompartmentDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds - 1);
   And DefrostReadyTimerIsSatisfiedModuleIsInitialized();

   DefrostReadyTimerIsSatisfiedShouldBe(CLEAR);

   And ConvertibleCompartmentDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds);
   DefrostReadyTimerIsSatisfiedShouldBe(SET);
}

TEST(DefrostReadyTimerIsSatisfied, ShouldSetDefrostReadyTimerIsSatisfiedErdWhenFreshFoodDoorAccelerationChangesSuchThatSumOfCompressorOnTimeAndDoorAccelerationIsGreaterThanTimeWhenDefrostReadyTimerIsSatisfied)
{
   Given DefrostReadyTimerIsSatisfiedIs(SET);
   And TimeInMinutesWhenDefrostReadyTimerIsSatisfiedIs(MinTimeBetweenDefrostsInMinutes * 2);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds - 1);
   And FreezerDoorAccelerationIs(0);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And DefrostReadyTimerIsSatisfiedModuleIsInitialized();

   DefrostReadyTimerIsSatisfiedShouldBe(CLEAR);

   And FreshFoodDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds + 1);
   DefrostReadyTimerIsSatisfiedShouldBe(SET);
}

TEST(DefrostReadyTimerIsSatisfied, ShouldSetDefrostReadyTimerIsSatisfiedErdWhenFreezerDoorAccelerationChangesSuchThatSumOfCompressorOnTimeAndDoorAccelerationIsGreaterThanTimeWhenDefrostReadyTimerIsSatisfied)
{
   Given DefrostReadyTimerIsSatisfiedIs(SET);
   And TimeInMinutesWhenDefrostReadyTimerIsSatisfiedIs(MinTimeBetweenDefrostsInMinutes * 2);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(0);
   And FreezerDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds - 1);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And DefrostReadyTimerIsSatisfiedModuleIsInitialized();

   DefrostReadyTimerIsSatisfiedShouldBe(CLEAR);

   And FreshFoodDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds + 1);
   DefrostReadyTimerIsSatisfiedShouldBe(SET);
}

TEST(DefrostReadyTimerIsSatisfied, ShouldSetDefrostReadyTimerIsSatisfiedErdWhenConvertibleCompartmentDoorAccelerationChangesSuchThatSumOfCompressorOnTimeAndDoorAccelerationIsGreaterThanTimeWhenDefrostReadyTimerIsSatisfied)
{
   Given DefrostReadyTimerIsSatisfiedIs(SET);
   And TimeInMinutesWhenDefrostReadyTimerIsSatisfiedIs(MinTimeBetweenDefrostsInMinutes * 2);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(0);
   And FreezerDoorAccelerationIs(0);
   And ConvertibleCompartmentDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds - 1);
   And DefrostReadyTimerIsSatisfiedModuleIsInitialized();

   DefrostReadyTimerIsSatisfiedShouldBe(CLEAR);

   And ConvertibleCompartmentDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds + 1);
   DefrostReadyTimerIsSatisfiedShouldBe(SET);
}

TEST(DefrostReadyTimerIsSatisfied, ShouldClearDefrostReadyTimerIsSatisfiedErdWhenTimeWhenDefrostReadyTimerIsSatisfiedChangesSoThatSumOfCompressorOnTimeAndDoorAccelerationAreNowLessThanIt)
{
   Given DefrostReadyTimerIsSatisfiedIs(CLEAR);
   And TimeInMinutesWhenDefrostReadyTimerIsSatisfiedIs(MinTimeBetweenDefrostsInMinutes * 2);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(0);
   And FreezerDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And DefrostReadyTimerIsSatisfiedModuleIsInitialized();

   DefrostReadyTimerIsSatisfiedShouldBe(SET);

   And TimeInMinutesWhenDefrostReadyTimerIsSatisfiedIs(MinTimeBetweenDefrostsInMinutes * 2 + 1);
   DefrostReadyTimerIsSatisfiedShouldBe(CLEAR);
}

TEST(DefrostReadyTimerIsSatisfied, ShouldSetDefrostReadyTimerIsSatisfiedErdWhenTimeWhenDefrostReadyTimerIsSatisfiedChangesSoThatSumOfCompressorOnTimeAndDoorAccelerationAreNowEqualToIt)
{
   Given DefrostReadyTimerIsSatisfiedIs(SET);
   And TimeInMinutesWhenDefrostReadyTimerIsSatisfiedIs(MinTimeBetweenDefrostsInMinutes * 2 + 1);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(0);
   And FreezerDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And DefrostReadyTimerIsSatisfiedModuleIsInitialized();

   DefrostReadyTimerIsSatisfiedShouldBe(CLEAR);

   And TimeInMinutesWhenDefrostReadyTimerIsSatisfiedIs(MinTimeBetweenDefrostsInMinutes * 2);
   DefrostReadyTimerIsSatisfiedShouldBe(SET);
}

TEST(DefrostReadyTimerIsSatisfied, ShouldSetDefrostReadyTimerIsSatisfiedErdWhenTimeWhenDefrostReadyTimerIsSatisfiedChangesSoThatSumOfCompressorOnTimeAndDoorAccelerationAreNowGreaterThanIt)
{
   Given DefrostReadyTimerIsSatisfiedIs(SET);
   And TimeInMinutesWhenDefrostReadyTimerIsSatisfiedIs(MinTimeBetweenDefrostsInMinutes * 2 + 1);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(0);
   And FreezerDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And DefrostReadyTimerIsSatisfiedModuleIsInitialized();

   DefrostReadyTimerIsSatisfiedShouldBe(CLEAR);

   And TimeInMinutesWhenDefrostReadyTimerIsSatisfiedIs(MinTimeBetweenDefrostsInMinutes * 2 - 1);
   DefrostReadyTimerIsSatisfiedShouldBe(SET);
}
