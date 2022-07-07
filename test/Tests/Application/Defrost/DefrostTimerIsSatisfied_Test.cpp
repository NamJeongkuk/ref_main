/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "DefrostTimerIsSatisfied.h"
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

#define MinTimeBetweenDefrostsInMinutes (defrostData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes)
#define MinTimeBetweenDefrostsInSeconds (defrostData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE)

static DefrostTimerIsSatisfiedConfiguration_t config = {
   .compressorOnTimeInSecondsErd = Erd_DefrostCompressorOnTimeInSeconds,
   .freshFoodDoorAccelerationCountErd = Erd_DefrostFreshFoodDoorAccelerationCount,
   .freezerDoorAccelerationCountErd = Erd_DefrostFreezerDoorAccelerationCount,
   .convertibleCompartmentDoorAccelerationCountErd = Erd_DefrostConvertibleCompartmentDoorAccelerationCount,
   .defrostTimerIsSatisfiedErd = Erd_DefrostTimerIsSatisfied,
   .timeInMinutesWhenDefrostTimerIsSatisfiedErd = Erd_TimeInMinutesWhenDefrostTimerIsSatisfied
};

TEST_GROUP(DefrostTimerIsSatisfied)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   PersonalityParametricData_t personalityParametricData;
   DefrostData_t defrostData;
   DefrostTimerIsSatisfied_t instance;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &timerModuleTestDouble->timerModule);

      DefrostData_TestDouble_Init(&defrostData);

      PersonalityParametricData_TestDouble_Init(&personalityParametricData);
      PersonalityParametricData_TestDouble_SetDefrost(&personalityParametricData, &defrostData);
      DataModelErdPointerAccess_Write(dataModel, Erd_PersonalityParametricData, &personalityParametricData);
   }

   void DefrostTimerIsSatisfiedModuleIsInitialized()
   {
      DefrostTimerIsSatisfied_Init(&instance, dataModel, &config);
   }

   void DefrostTimerIsSatisfiedIs(bool state)
   {
      DataModel_Write(dataModel, Erd_DefrostTimerIsSatisfied, &state);
   }

   void CompressorOnTimeInSecondsIs(uint32_t seconds)
   {
      DataModel_Write(dataModel, Erd_DefrostCompressorOnTimeInSeconds, &seconds);
   }

   void FreshFoodDoorAccelerationIs(uint32_t count)
   {
      DataModel_Write(dataModel, Erd_DefrostFreshFoodDoorAccelerationCount, &count);
   }

   void FreezerDoorAccelerationIs(uint32_t count)
   {
      DataModel_Write(dataModel, Erd_DefrostFreezerDoorAccelerationCount, &count);
   }

   void ConvertibleCompartmentDoorAccelerationIs(uint32_t count)
   {
      DataModel_Write(dataModel, Erd_DefrostConvertibleCompartmentDoorAccelerationCount, &count);
   }

   void DefrostTimerIsSatisfiedShouldBe(bool expectedState)
   {
      bool actualState;
      DataModel_Read(dataModel, Erd_DefrostTimerIsSatisfied, &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void TimeInMinutesWhenDefrostTimerIsSatisfiedIs(uint16_t timeInMinutes)
   {
      DataModel_Write(dataModel, Erd_TimeInMinutesWhenDefrostTimerIsSatisfied, &timeInMinutes);
   }
};

TEST(DefrostTimerIsSatisfied, ShouldInitialize)
{
   DefrostTimerIsSatisfiedModuleIsInitialized();
}

TEST(DefrostTimerIsSatisfied, ShouldClearDefrostTimerIsSatisfiedErdWhenCompressorOnTimeIsLessThanMinimumTimeBetweenDefrostsAbnormalRunTimeOnInit)
{
   Given DefrostTimerIsSatisfiedIs(SET);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds - 1);
   And DefrostTimerIsSatisfiedModuleIsInitialized();

   DefrostTimerIsSatisfiedShouldBe(CLEAR);
}

TEST(DefrostTimerIsSatisfied, ShouldClearDefrostTimerIsSatisfiedErdWhenCompressorOnTimeHasNotReachedMinimumTimeBetweenDefrostAbnormalRunTimeInMinutesAndSumOfCompressorOnTimeAndDoorAccelerationsAreEqualToTimeWhenDefrostTimerIsSatisfiedOnInit)
{
   Given DefrostTimerIsSatisfiedIs(SET);
   And TimeInMinutesWhenDefrostTimerIsSatisfiedIs(MinTimeBetweenDefrostsInMinutes * 2);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds - 1);
   And FreshFoodDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds);
   And FreezerDoorAccelerationIs(1);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And DefrostTimerIsSatisfiedModuleIsInitialized();

   DefrostTimerIsSatisfiedShouldBe(CLEAR);
}

TEST(DefrostTimerIsSatisfied, ShouldClearDefrostTimerIsSatisfiedErdWhenCompressorOnTimeHasReachedMinimumTimeBetweenDefrostAbnormalRunTimeInMinutesAndSumOfCompressorOnTimeAndDoorAccelerationsAreLessThanTimeWhenDefrostTimerIsSatisfiedOnInit)
{
   Given DefrostTimerIsSatisfiedIs(SET);
   And TimeInMinutesWhenDefrostTimerIsSatisfiedIs(MinTimeBetweenDefrostsInMinutes * 2);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds - 1);
   And FreezerDoorAccelerationIs(0);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And DefrostTimerIsSatisfiedModuleIsInitialized();

   DefrostTimerIsSatisfiedShouldBe(CLEAR);
}

TEST(DefrostTimerIsSatisfied, ShouldSetDefrostTimerIsSatisfiedErdWhenCompressorOnTimeHasReachedMinimumTimeBetweenDefrostAbnormalRunTimeInMinutesAndSumOfCompressorOnTimeAndDoorAccelerationsAreEqualToTimeWhenDefrostTimerIsSatisfiedOnInit)
{
   Given DefrostTimerIsSatisfiedIs(CLEAR);
   And TimeInMinutesWhenDefrostTimerIsSatisfiedIs(MinTimeBetweenDefrostsInMinutes * 2);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds);
   And FreezerDoorAccelerationIs(0);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And DefrostTimerIsSatisfiedModuleIsInitialized();

   DefrostTimerIsSatisfiedShouldBe(SET);
}

TEST(DefrostTimerIsSatisfied, ShouldSetDefrostTimerIsSatisfiedErdWhenCompressorOnTimeHasReachedMinimumTimeBetweenDefrostAbnormalRunTimeInMinutesAndSumOfCompressorOnTimeAndDoorAccelerationsAreGreaterThanTimeWhenDefrostTimerIsSatisfiedOnInit)
{
   Given DefrostTimerIsSatisfiedIs(CLEAR);
   And TimeInMinutesWhenDefrostTimerIsSatisfiedIs(MinTimeBetweenDefrostsInMinutes * 2);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds);
   And FreezerDoorAccelerationIs(1);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And DefrostTimerIsSatisfiedModuleIsInitialized();

   DefrostTimerIsSatisfiedShouldBe(SET);
}

TEST(DefrostTimerIsSatisfied, ShouldClearDefrostTimerIsSatisfiedErdWhenCompressorOnTimeIsGreaterThanMinimumTimeBetweenDefrostAbnormalRunTimeInMinutesAndSumOfCompressorOnTimeAndDoorAccelerationsAreLessThanTimeWhenDefrostTimerIsSatisfiedOnInit)
{
   Given DefrostTimerIsSatisfiedIs(SET);
   And TimeInMinutesWhenDefrostTimerIsSatisfiedIs(MinTimeBetweenDefrostsInMinutes * 2);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds + 1);
   And FreshFoodDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds - 2);
   And FreezerDoorAccelerationIs(0);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And DefrostTimerIsSatisfiedModuleIsInitialized();

   DefrostTimerIsSatisfiedShouldBe(CLEAR);
}

TEST(DefrostTimerIsSatisfied, ShouldSetDefrostTimerIsSatisfiedErdWhenCompressorOnTimeIsGreaterThanMinimumTimeBetweenDefrostAbnormalRunTimeInMinutesAndSumOfCompressorOnTimeAndDoorAccelerationsAreEqualToTimeWhenDefrostTimerIsSatisfiedOnInit)
{
   Given DefrostTimerIsSatisfiedIs(CLEAR);
   And TimeInMinutesWhenDefrostTimerIsSatisfiedIs(MinTimeBetweenDefrostsInMinutes * 2);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds + 1);
   And FreshFoodDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds - 1);
   And FreezerDoorAccelerationIs(0);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And DefrostTimerIsSatisfiedModuleIsInitialized();

   DefrostTimerIsSatisfiedShouldBe(SET);
}

TEST(DefrostTimerIsSatisfied, ShouldSetDefrostTimerIsSatisfiedErdWhenCompressorOnTimeIsGreaterThanMinimumTimeBetweenDefrostAbnormalRunTimeInMinutesAndSumOfCompressorOnTimeAndDoorAccelerationsAreGreaterThanTimeWhenDefrostTimerIsSatisfiedOnInit)
{
   Given DefrostTimerIsSatisfiedIs(CLEAR);
   And TimeInMinutesWhenDefrostTimerIsSatisfiedIs(MinTimeBetweenDefrostsInMinutes * 2);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds + 1);
   And FreshFoodDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds);
   And FreezerDoorAccelerationIs(1);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And DefrostTimerIsSatisfiedModuleIsInitialized();

   DefrostTimerIsSatisfiedShouldBe(SET);
}

TEST(DefrostTimerIsSatisfied, ShouldClearDefrostTimerIsSatisfiedErdWhenCompressorOnTimeIsLessThanMinimumTimeBetweenDefrostAbnormalRunTimeInMinutes)
{
   Given DefrostTimerIsSatisfiedIs(CLEAR);
   And TimeInMinutesWhenDefrostTimerIsSatisfiedIs(MinTimeBetweenDefrostsInMinutes * 2);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds);
   And FreezerDoorAccelerationIs(1);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And DefrostTimerIsSatisfiedModuleIsInitialized();

   DefrostTimerIsSatisfiedShouldBe(SET);

   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds - 1);
   DefrostTimerIsSatisfiedShouldBe(CLEAR);
}

TEST(DefrostTimerIsSatisfied, ShouldSetDefrostTimerIsSatisfiedErdWhenCompressorOnTimeIsEqualToMinimumTimeBetweenDefrostAbnormalRunTimeInMinutes)
{
   Given DefrostTimerIsSatisfiedIs(SET);
   And TimeInMinutesWhenDefrostTimerIsSatisfiedIs(MinTimeBetweenDefrostsInMinutes * 2);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds - 1);
   And FreshFoodDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds);
   And FreezerDoorAccelerationIs(1);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And DefrostTimerIsSatisfiedModuleIsInitialized();

   DefrostTimerIsSatisfiedShouldBe(CLEAR);

   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   DefrostTimerIsSatisfiedShouldBe(SET);
}

TEST(DefrostTimerIsSatisfied, ShouldSetDefrostTimerIsSatisfiedErdWhenCompressorOnTimeIsGreaterThanMinimumTimeBetweenDefrostAbnormalRunTimeInMinutes)
{
   Given DefrostTimerIsSatisfiedIs(SET);
   And TimeInMinutesWhenDefrostTimerIsSatisfiedIs(MinTimeBetweenDefrostsInMinutes * 2);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds - 1);
   And FreshFoodDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds);
   And FreezerDoorAccelerationIs(1);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And DefrostTimerIsSatisfiedModuleIsInitialized();

   DefrostTimerIsSatisfiedShouldBe(CLEAR);

   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds + 1);
   DefrostTimerIsSatisfiedShouldBe(SET);
}

TEST(DefrostTimerIsSatisfied, ShouldClearDefrostTimerIsSatisfiedErdWhenFreshFoodDoorAccelerationChangesSuchThatSumOfCompressorOnTimeAndDoorAccelerationIsLessThanTimeWhenDefrostTimerIsSatisfied)
{
   Given DefrostTimerIsSatisfiedIs(CLEAR);
   And TimeInMinutesWhenDefrostTimerIsSatisfiedIs(MinTimeBetweenDefrostsInMinutes * 2);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds);
   And FreezerDoorAccelerationIs(0);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And DefrostTimerIsSatisfiedModuleIsInitialized();

   DefrostTimerIsSatisfiedShouldBe(SET);

   And FreshFoodDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds - 1);
   DefrostTimerIsSatisfiedShouldBe(CLEAR);
}

TEST(DefrostTimerIsSatisfied, ShouldClearDefrostTimerIsSatisfiedErdWhenFreezerDoorAccelerationChangesSuchThatSumOfCompressorOnTimeAndDoorAccelerationIsLessThanTimeWhenDefrostTimerIsSatisfied)
{
   Given DefrostTimerIsSatisfiedIs(CLEAR);
   And TimeInMinutesWhenDefrostTimerIsSatisfiedIs(MinTimeBetweenDefrostsInMinutes * 2);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(0);
   And FreezerDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And DefrostTimerIsSatisfiedModuleIsInitialized();

   DefrostTimerIsSatisfiedShouldBe(SET);

   And FreezerDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds - 1);
   DefrostTimerIsSatisfiedShouldBe(CLEAR);
}

TEST(DefrostTimerIsSatisfied, ShouldClearDefrostTimerIsSatisfiedErdWhenConvertibleCompartmentDoorAccelerationChangesSuchThatSumOfCompressorOnTimeAndDoorAccelerationIsLessThanTimeWhenDefrostTimerIsSatisfied)
{
   Given DefrostTimerIsSatisfiedIs(CLEAR);
   And TimeInMinutesWhenDefrostTimerIsSatisfiedIs(MinTimeBetweenDefrostsInMinutes * 2);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(0);
   And FreezerDoorAccelerationIs(0);
   And ConvertibleCompartmentDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds);
   And DefrostTimerIsSatisfiedModuleIsInitialized();

   DefrostTimerIsSatisfiedShouldBe(SET);

   And ConvertibleCompartmentDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds - 1);
   DefrostTimerIsSatisfiedShouldBe(CLEAR);
}

TEST(DefrostTimerIsSatisfied, ShouldSetDefrostTimerIsSatisfiedErdWhenFreshFoodDoorAccelerationChangesSuchThatSumOfCompressorOnTimeAndDoorAccelerationIsEqualToTimeWhenDefrostTimerIsSatisfied)
{
   Given DefrostTimerIsSatisfiedIs(SET);
   And TimeInMinutesWhenDefrostTimerIsSatisfiedIs(MinTimeBetweenDefrostsInMinutes * 2);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds - 1);
   And FreezerDoorAccelerationIs(0);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And DefrostTimerIsSatisfiedModuleIsInitialized();

   DefrostTimerIsSatisfiedShouldBe(CLEAR);

   And FreshFoodDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds);
   DefrostTimerIsSatisfiedShouldBe(SET);
}

TEST(DefrostTimerIsSatisfied, ShouldSetDefrostTimerIsSatisfiedErdWhenFreezerDoorAccelerationChangesSuchThatSumOfCompressorOnTimeAndDoorAccelerationIsEqualToTimeWhenDefrostTimerIsSatisfied)
{
   Given DefrostTimerIsSatisfiedIs(SET);
   And TimeInMinutesWhenDefrostTimerIsSatisfiedIs(MinTimeBetweenDefrostsInMinutes * 2);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(0);
   And FreezerDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds - 1);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And DefrostTimerIsSatisfiedModuleIsInitialized();

   DefrostTimerIsSatisfiedShouldBe(CLEAR);

   And FreshFoodDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds);
   DefrostTimerIsSatisfiedShouldBe(SET);
}

TEST(DefrostTimerIsSatisfied, ShouldSetDefrostTimerIsSatisfiedErdWhenConvertibleCompartmentDoorAccelerationChangesSuchThatSumOfCompressorOnTimeAndDoorAccelerationIsEqualToTimeWhenDefrostTimerIsSatisfied)
{
   Given DefrostTimerIsSatisfiedIs(SET);
   And TimeInMinutesWhenDefrostTimerIsSatisfiedIs(MinTimeBetweenDefrostsInMinutes * 2);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(0);
   And FreezerDoorAccelerationIs(0);
   And ConvertibleCompartmentDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds - 1);
   And DefrostTimerIsSatisfiedModuleIsInitialized();

   DefrostTimerIsSatisfiedShouldBe(CLEAR);

   And ConvertibleCompartmentDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds);
   DefrostTimerIsSatisfiedShouldBe(SET);
}

TEST(DefrostTimerIsSatisfied, ShouldSetDefrostTimerIsSatisfiedErdWhenFreshFoodDoorAccelerationChangesSuchThatSumOfCompressorOnTimeAndDoorAccelerationIsGreaterThanTimeWhenDefrostTimerIsSatisfied)
{
   Given DefrostTimerIsSatisfiedIs(SET);
   And TimeInMinutesWhenDefrostTimerIsSatisfiedIs(MinTimeBetweenDefrostsInMinutes * 2);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds - 1);
   And FreezerDoorAccelerationIs(0);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And DefrostTimerIsSatisfiedModuleIsInitialized();

   DefrostTimerIsSatisfiedShouldBe(CLEAR);

   And FreshFoodDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds + 1);
   DefrostTimerIsSatisfiedShouldBe(SET);
}

TEST(DefrostTimerIsSatisfied, ShouldSetDefrostTimerIsSatisfiedErdWhenFreezerDoorAccelerationChangesSuchThatSumOfCompressorOnTimeAndDoorAccelerationIsGreaterThanTimeWhenDefrostTimerIsSatisfied)
{
   Given DefrostTimerIsSatisfiedIs(SET);
   And TimeInMinutesWhenDefrostTimerIsSatisfiedIs(MinTimeBetweenDefrostsInMinutes * 2);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(0);
   And FreezerDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds - 1);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And DefrostTimerIsSatisfiedModuleIsInitialized();

   DefrostTimerIsSatisfiedShouldBe(CLEAR);

   And FreshFoodDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds + 1);
   DefrostTimerIsSatisfiedShouldBe(SET);
}

TEST(DefrostTimerIsSatisfied, ShouldSetDefrostTimerIsSatisfiedErdWhenConvertibleCompartmentDoorAccelerationChangesSuchThatSumOfCompressorOnTimeAndDoorAccelerationIsGreaterThanTimeWhenDefrostTimerIsSatisfied)
{
   Given DefrostTimerIsSatisfiedIs(SET);
   And TimeInMinutesWhenDefrostTimerIsSatisfiedIs(MinTimeBetweenDefrostsInMinutes * 2);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(0);
   And FreezerDoorAccelerationIs(0);
   And ConvertibleCompartmentDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds - 1);
   And DefrostTimerIsSatisfiedModuleIsInitialized();

   DefrostTimerIsSatisfiedShouldBe(CLEAR);

   And ConvertibleCompartmentDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds + 1);
   DefrostTimerIsSatisfiedShouldBe(SET);
}

TEST(DefrostTimerIsSatisfied, ShouldClearDefrostTimerIsSatisfiedErdWhenTimeWhenDefrostTimerIsSatisfiedChangesSoThatSumOfCompressorOnTimeAndDoorAccelerationAreNowLessThanIt)
{
   Given DefrostTimerIsSatisfiedIs(CLEAR);
   And TimeInMinutesWhenDefrostTimerIsSatisfiedIs(MinTimeBetweenDefrostsInMinutes * 2);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(0);
   And FreezerDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And DefrostTimerIsSatisfiedModuleIsInitialized();

   DefrostTimerIsSatisfiedShouldBe(SET);

   And TimeInMinutesWhenDefrostTimerIsSatisfiedIs(MinTimeBetweenDefrostsInMinutes * 2 + 1);
   DefrostTimerIsSatisfiedShouldBe(CLEAR);
}

TEST(DefrostTimerIsSatisfied, ShouldSetDefrostTimerIsSatisfiedErdWhenTimeWhenDefrostTimerIsSatisfiedChangesSoThatSumOfCompressorOnTimeAndDoorAccelerationAreNowEqualToIt)
{
   Given DefrostTimerIsSatisfiedIs(SET);
   And TimeInMinutesWhenDefrostTimerIsSatisfiedIs(MinTimeBetweenDefrostsInMinutes * 2 + 1);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(0);
   And FreezerDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And DefrostTimerIsSatisfiedModuleIsInitialized();

   DefrostTimerIsSatisfiedShouldBe(CLEAR);

   And TimeInMinutesWhenDefrostTimerIsSatisfiedIs(MinTimeBetweenDefrostsInMinutes * 2);
   DefrostTimerIsSatisfiedShouldBe(SET);
}

TEST(DefrostTimerIsSatisfied, ShouldSetDefrostTimerIsSatisfiedErdWhenTimeWhenDefrostTimerIsSatisfiedChangesSoThatSumOfCompressorOnTimeAndDoorAccelerationAreNowGreaterThanIt)
{
   Given DefrostTimerIsSatisfiedIs(SET);
   And TimeInMinutesWhenDefrostTimerIsSatisfiedIs(MinTimeBetweenDefrostsInMinutes * 2 + 1);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(0);
   And FreezerDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And DefrostTimerIsSatisfiedModuleIsInitialized();

   DefrostTimerIsSatisfiedShouldBe(CLEAR);

   And TimeInMinutesWhenDefrostTimerIsSatisfiedIs(MinTimeBetweenDefrostsInMinutes * 2 - 1);
   DefrostTimerIsSatisfiedShouldBe(SET);
}
