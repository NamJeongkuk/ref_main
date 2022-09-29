/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "ReadyToDefrost.h"
#include "DataModelErdPointerAccess.h"
#include "SystemErds.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

#define Given
#define When
#define Then
#define And

#define MinTimeBetweenDefrostsInMinutes (defrostData->minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes)
#define MinTimeBetweenDefrostsInSeconds (defrostData->minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE)

static ReadyToDefrostConfiguration_t config = {
   .compressorOnTimeInSecondsErd = Erd_DefrostCompressorOnTimeInSeconds,
   .freshFoodScaledDoorAccelerationInSecondsErd = Erd_DefrostFreshFoodScaledDoorAccelerationInSeconds,
   .freezerScaledDoorAccelerationInSecondsErd = Erd_DefrostFreezerScaledDoorAccelerationInSeconds,
   .convertibleCompartmentScaledDoorAccelerationInSecondsErd = Erd_DefrostConvertibleCompartmentScaledDoorAccelerationInSeconds,
   .readyToDefrostErd = Erd_ReadyToDefrost,
   .timeInMinutesUntilReadyToDefrostErd = Erd_TimeInMinutesUntilReadyToDefrost,
   .defrostCompressorOnTimeCounterReadyErd = Erd_DefrostCompressorOnTimeCounterReady,
   .doorAccelerationCounterReadyErd = Erd_DoorAccelerationCounterReady
};

TEST_GROUP(ReadyToDefrost)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   const DefrostData_t *defrostData;

   ReadyToDefrost_t instance;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &timerModuleTestDouble->timerModule);

      defrostData = PersonalityParametricData_Get(dataModel)->defrostData;
   }

   void ReadyToDefrostModuleIsInitialized()
   {
      DataModel_Write(dataModel, Erd_DefrostCompressorOnTimeCounterReady, set);
      DataModel_Write(dataModel, Erd_DoorAccelerationCounterReady, set);

      ReadyToDefrost_Init(&instance, dataModel, &config);
   }

   void ReadyToDefrostIs(bool state)
   {
      DataModel_Write(dataModel, Erd_ReadyToDefrost, &state);
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

   void ReadyToDefrostShouldBe(bool expectedState)
   {
      bool actualState;
      DataModel_Read(dataModel, Erd_ReadyToDefrost, &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void TimeInMinutesUntilReadyToDefrostIs(uint16_t timeInMinutes)
   {
      DataModel_Write(dataModel, Erd_TimeInMinutesUntilReadyToDefrost, &timeInMinutes);
   }
};

TEST(ReadyToDefrost, ShouldInitialize)
{
   ReadyToDefrostModuleIsInitialized();
}

TEST(ReadyToDefrost, ShouldClearReadyToDefrostErdWhenCompressorOnTimeIsLessThanMinimumTimeBetweenDefrostsAbnormalRunTimeOnInit)
{
   Given ReadyToDefrostIs(SET);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds - 1);
   And ReadyToDefrostModuleIsInitialized();

   ReadyToDefrostShouldBe(CLEAR);
}

TEST(ReadyToDefrost, ShouldClearReadyToDefrostErdWhenCompressorOnTimeHasNotReachedMinimumTimeBetweenDefrostAbnormalRunTimeInMinutesAndSumOfCompressorOnTimeAndDoorAccelerationsAreEqualToTimeWhenReadyToDefrostOnInit)
{
   Given ReadyToDefrostIs(SET);
   And TimeInMinutesUntilReadyToDefrostIs(MinTimeBetweenDefrostsInMinutes * 2);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds - 1);
   And FreshFoodDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds);
   And FreezerDoorAccelerationIs(1);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And ReadyToDefrostModuleIsInitialized();

   ReadyToDefrostShouldBe(CLEAR);
}

TEST(ReadyToDefrost, ShouldClearReadyToDefrostErdWhenCompressorOnTimeHasReachedMinimumTimeBetweenDefrostAbnormalRunTimeInMinutesAndSumOfCompressorOnTimeAndDoorAccelerationsAreLessThanTimeWhenReadyToDefrostOnInit)
{
   Given ReadyToDefrostIs(SET);
   And TimeInMinutesUntilReadyToDefrostIs(MinTimeBetweenDefrostsInMinutes * 2);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds - 1);
   And FreezerDoorAccelerationIs(0);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And ReadyToDefrostModuleIsInitialized();

   ReadyToDefrostShouldBe(CLEAR);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdWhenCompressorOnTimeHasReachedMinimumTimeBetweenDefrostAbnormalRunTimeInMinutesAndSumOfCompressorOnTimeAndDoorAccelerationsAreEqualToTimeWhenReadyToDefrostOnInit)
{
   Given ReadyToDefrostIs(CLEAR);
   And TimeInMinutesUntilReadyToDefrostIs(MinTimeBetweenDefrostsInMinutes * 2);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds);
   And FreezerDoorAccelerationIs(0);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And ReadyToDefrostModuleIsInitialized();

   ReadyToDefrostShouldBe(SET);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdWhenCompressorOnTimeHasReachedMinimumTimeBetweenDefrostAbnormalRunTimeInMinutesAndSumOfCompressorOnTimeAndDoorAccelerationsAreGreaterThanTimeWhenReadyToDefrostOnInit)
{
   Given ReadyToDefrostIs(CLEAR);
   And TimeInMinutesUntilReadyToDefrostIs(MinTimeBetweenDefrostsInMinutes * 2);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds);
   And FreezerDoorAccelerationIs(1);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And ReadyToDefrostModuleIsInitialized();

   ReadyToDefrostShouldBe(SET);
}

TEST(ReadyToDefrost, ShouldClearReadyToDefrostErdWhenCompressorOnTimeIsGreaterThanMinimumTimeBetweenDefrostAbnormalRunTimeInMinutesAndSumOfCompressorOnTimeAndDoorAccelerationsAreLessThanTimeWhenReadyToDefrostOnInit)
{
   Given ReadyToDefrostIs(SET);
   And TimeInMinutesUntilReadyToDefrostIs(MinTimeBetweenDefrostsInMinutes * 2);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds + 1);
   And FreshFoodDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds - 2);
   And FreezerDoorAccelerationIs(0);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And ReadyToDefrostModuleIsInitialized();

   ReadyToDefrostShouldBe(CLEAR);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdWhenCompressorOnTimeIsGreaterThanMinimumTimeBetweenDefrostAbnormalRunTimeInMinutesAndSumOfCompressorOnTimeAndDoorAccelerationsAreEqualToTimeWhenReadyToDefrostOnInit)
{
   Given ReadyToDefrostIs(CLEAR);
   And TimeInMinutesUntilReadyToDefrostIs(MinTimeBetweenDefrostsInMinutes * 2);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds + 1);
   And FreshFoodDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds - 1);
   And FreezerDoorAccelerationIs(0);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And ReadyToDefrostModuleIsInitialized();

   ReadyToDefrostShouldBe(SET);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdWhenCompressorOnTimeIsGreaterThanMinimumTimeBetweenDefrostAbnormalRunTimeInMinutesAndSumOfCompressorOnTimeAndDoorAccelerationsAreGreaterThanTimeWhenReadyToDefrostOnInit)
{
   Given ReadyToDefrostIs(CLEAR);
   And TimeInMinutesUntilReadyToDefrostIs(MinTimeBetweenDefrostsInMinutes * 2);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds + 1);
   And FreshFoodDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds);
   And FreezerDoorAccelerationIs(1);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And ReadyToDefrostModuleIsInitialized();

   ReadyToDefrostShouldBe(SET);
}

TEST(ReadyToDefrost, ShouldClearReadyToDefrostErdWhenCompressorOnTimeIsLessThanMinimumTimeBetweenDefrostAbnormalRunTimeInMinutes)
{
   Given ReadyToDefrostIs(CLEAR);
   And TimeInMinutesUntilReadyToDefrostIs(MinTimeBetweenDefrostsInMinutes * 2);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds);
   And FreezerDoorAccelerationIs(1);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And ReadyToDefrostModuleIsInitialized();

   ReadyToDefrostShouldBe(SET);

   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds - 1);
   ReadyToDefrostShouldBe(CLEAR);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdWhenCompressorOnTimeIsEqualToMinimumTimeBetweenDefrostAbnormalRunTimeInMinutes)
{
   Given ReadyToDefrostIs(SET);
   And TimeInMinutesUntilReadyToDefrostIs(MinTimeBetweenDefrostsInMinutes * 2);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds - 1);
   And FreshFoodDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds);
   And FreezerDoorAccelerationIs(1);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And ReadyToDefrostModuleIsInitialized();

   ReadyToDefrostShouldBe(CLEAR);

   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   ReadyToDefrostShouldBe(SET);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdWhenCompressorOnTimeIsGreaterThanMinimumTimeBetweenDefrostAbnormalRunTimeInMinutes)
{
   Given ReadyToDefrostIs(SET);
   And TimeInMinutesUntilReadyToDefrostIs(MinTimeBetweenDefrostsInMinutes * 2);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds - 1);
   And FreshFoodDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds);
   And FreezerDoorAccelerationIs(1);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And ReadyToDefrostModuleIsInitialized();

   ReadyToDefrostShouldBe(CLEAR);

   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds + 1);
   ReadyToDefrostShouldBe(SET);
}

TEST(ReadyToDefrost, ShouldClearReadyToDefrostErdWhenFreshFoodDoorAccelerationChangesSuchThatSumOfCompressorOnTimeAndDoorAccelerationIsLessThanTimeWhenReadyToDefrost)
{
   Given ReadyToDefrostIs(CLEAR);
   And TimeInMinutesUntilReadyToDefrostIs(MinTimeBetweenDefrostsInMinutes * 2);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds);
   And FreezerDoorAccelerationIs(0);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And ReadyToDefrostModuleIsInitialized();

   ReadyToDefrostShouldBe(SET);

   And FreshFoodDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds - 1);
   ReadyToDefrostShouldBe(CLEAR);
}

TEST(ReadyToDefrost, ShouldClearReadyToDefrostErdWhenFreezerDoorAccelerationChangesSuchThatSumOfCompressorOnTimeAndDoorAccelerationIsLessThanTimeWhenReadyToDefrost)
{
   Given ReadyToDefrostIs(CLEAR);
   And TimeInMinutesUntilReadyToDefrostIs(MinTimeBetweenDefrostsInMinutes * 2);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(0);
   And FreezerDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And ReadyToDefrostModuleIsInitialized();

   ReadyToDefrostShouldBe(SET);

   And FreezerDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds - 1);
   ReadyToDefrostShouldBe(CLEAR);
}

TEST(ReadyToDefrost, ShouldClearReadyToDefrostErdWhenConvertibleCompartmentDoorAccelerationChangesSuchThatSumOfCompressorOnTimeAndDoorAccelerationIsLessThanTimeWhenReadyToDefrost)
{
   Given ReadyToDefrostIs(CLEAR);
   And TimeInMinutesUntilReadyToDefrostIs(MinTimeBetweenDefrostsInMinutes * 2);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(0);
   And FreezerDoorAccelerationIs(0);
   And ConvertibleCompartmentDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds);
   And ReadyToDefrostModuleIsInitialized();

   ReadyToDefrostShouldBe(SET);

   And ConvertibleCompartmentDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds - 1);
   ReadyToDefrostShouldBe(CLEAR);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdWhenFreshFoodDoorAccelerationChangesSuchThatSumOfCompressorOnTimeAndDoorAccelerationIsEqualToTimeWhenReadyToDefrost)
{
   Given ReadyToDefrostIs(SET);
   And TimeInMinutesUntilReadyToDefrostIs(MinTimeBetweenDefrostsInMinutes * 2);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds - 1);
   And FreezerDoorAccelerationIs(0);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And ReadyToDefrostModuleIsInitialized();

   ReadyToDefrostShouldBe(CLEAR);

   And FreshFoodDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds);
   ReadyToDefrostShouldBe(SET);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdWhenFreezerDoorAccelerationChangesSuchThatSumOfCompressorOnTimeAndDoorAccelerationIsEqualToTimeWhenReadyToDefrost)
{
   Given ReadyToDefrostIs(SET);
   And TimeInMinutesUntilReadyToDefrostIs(MinTimeBetweenDefrostsInMinutes * 2);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(0);
   And FreezerDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds - 1);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And ReadyToDefrostModuleIsInitialized();

   ReadyToDefrostShouldBe(CLEAR);

   And FreshFoodDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds);
   ReadyToDefrostShouldBe(SET);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdWhenConvertibleCompartmentDoorAccelerationChangesSuchThatSumOfCompressorOnTimeAndDoorAccelerationIsEqualToTimeWhenReadyToDefrost)
{
   Given ReadyToDefrostIs(SET);
   And TimeInMinutesUntilReadyToDefrostIs(MinTimeBetweenDefrostsInMinutes * 2);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(0);
   And FreezerDoorAccelerationIs(0);
   And ConvertibleCompartmentDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds - 1);
   And ReadyToDefrostModuleIsInitialized();

   ReadyToDefrostShouldBe(CLEAR);

   And ConvertibleCompartmentDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds);
   ReadyToDefrostShouldBe(SET);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdWhenFreshFoodDoorAccelerationChangesSuchThatSumOfCompressorOnTimeAndDoorAccelerationIsGreaterThanTimeWhenReadyToDefrost)
{
   Given ReadyToDefrostIs(SET);
   And TimeInMinutesUntilReadyToDefrostIs(MinTimeBetweenDefrostsInMinutes * 2);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds - 1);
   And FreezerDoorAccelerationIs(0);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And ReadyToDefrostModuleIsInitialized();

   ReadyToDefrostShouldBe(CLEAR);

   And FreshFoodDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds + 1);
   ReadyToDefrostShouldBe(SET);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdWhenFreezerDoorAccelerationChangesSuchThatSumOfCompressorOnTimeAndDoorAccelerationIsGreaterThanTimeWhenReadyToDefrost)
{
   Given ReadyToDefrostIs(SET);
   And TimeInMinutesUntilReadyToDefrostIs(MinTimeBetweenDefrostsInMinutes * 2);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(0);
   And FreezerDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds - 1);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And ReadyToDefrostModuleIsInitialized();

   ReadyToDefrostShouldBe(CLEAR);

   And FreshFoodDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds + 1);
   ReadyToDefrostShouldBe(SET);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdWhenConvertibleCompartmentDoorAccelerationChangesSuchThatSumOfCompressorOnTimeAndDoorAccelerationIsGreaterThanTimeWhenReadyToDefrost)
{
   Given ReadyToDefrostIs(SET);
   And TimeInMinutesUntilReadyToDefrostIs(MinTimeBetweenDefrostsInMinutes * 2);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(0);
   And FreezerDoorAccelerationIs(0);
   And ConvertibleCompartmentDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds - 1);
   And ReadyToDefrostModuleIsInitialized();

   ReadyToDefrostShouldBe(CLEAR);

   And ConvertibleCompartmentDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds + 1);
   ReadyToDefrostShouldBe(SET);
}

TEST(ReadyToDefrost, ShouldClearReadyToDefrostErdWhenTimeWhenReadyToDefrostChangesSoThatSumOfCompressorOnTimeAndDoorAccelerationAreNowLessThanIt)
{
   Given ReadyToDefrostIs(CLEAR);
   And TimeInMinutesUntilReadyToDefrostIs(MinTimeBetweenDefrostsInMinutes * 2);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(0);
   And FreezerDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And ReadyToDefrostModuleIsInitialized();

   ReadyToDefrostShouldBe(SET);

   And TimeInMinutesUntilReadyToDefrostIs(MinTimeBetweenDefrostsInMinutes * 2 + 1);
   ReadyToDefrostShouldBe(CLEAR);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdWhenTimeWhenReadyToDefrostChangesSoThatSumOfCompressorOnTimeAndDoorAccelerationAreNowEqualToIt)
{
   Given ReadyToDefrostIs(SET);
   And TimeInMinutesUntilReadyToDefrostIs(MinTimeBetweenDefrostsInMinutes * 2 + 1);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(0);
   And FreezerDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And ReadyToDefrostModuleIsInitialized();

   ReadyToDefrostShouldBe(CLEAR);

   And TimeInMinutesUntilReadyToDefrostIs(MinTimeBetweenDefrostsInMinutes * 2);
   ReadyToDefrostShouldBe(SET);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdWhenTimeWhenReadyToDefrostChangesSoThatSumOfCompressorOnTimeAndDoorAccelerationAreNowGreaterThanIt)
{
   Given ReadyToDefrostIs(SET);
   And TimeInMinutesUntilReadyToDefrostIs(MinTimeBetweenDefrostsInMinutes * 2 + 1);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(0);
   And FreezerDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And ReadyToDefrostModuleIsInitialized();

   ReadyToDefrostShouldBe(CLEAR);

   And TimeInMinutesUntilReadyToDefrostIs(MinTimeBetweenDefrostsInMinutes * 2 - 1);
   ReadyToDefrostShouldBe(SET);
}
