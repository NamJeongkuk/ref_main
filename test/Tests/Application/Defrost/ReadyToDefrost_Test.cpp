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
#define MaxTimeBetweenDefrostInSeconds (defrostData->maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE)

static ReadyToDefrostConfiguration_t config = {
   .compressorOnTimeInSecondsErd = Erd_DefrostCompressorOnTimeInSeconds,
   .freshFoodScaledDoorAccelerationInSecondsErd = Erd_DefrostFreshFoodScaledDoorAccelerationInSeconds,
   .freezerScaledDoorAccelerationInSecondsErd = Erd_DefrostFreezerScaledDoorAccelerationInSeconds,
   .convertibleCompartmentScaledDoorAccelerationInSecondsErd = Erd_DefrostConvertibleCompartmentScaledDoorAccelerationInSeconds,
   .readyToDefrostErd = Erd_ReadyToDefrost,
   .timeInMinutesUntilReadyToDefrostErd = Erd_TimeInMinutesUntilReadyToDefrost,
   .defrostCompressorOnTimeCounterReadyErd = Erd_DefrostCompressorOnTimeCounterReady,
   .doorAccelerationCounterReadyErd = Erd_DoorAccelerationCounterReady,
   .freezerFilteredTemperatureWasTooWarmOnPowerUpErd = Erd_FreezerFilteredTemperatureTooWarmAtPowerUp,
   .freezerDefrostUseMinimumTimeErd = Erd_UseMinimumReadyToDefrostTime,
   .freezerEvapThermistorIsValidErd = Erd_FreezerEvapThermistor_IsValidResolved,
   .freshFoodDefrostWasAbnormalErd = Erd_FreshFoodDefrostWasAbnormal,
   .freezerDefrostWasAbnormalErd = Erd_FreezerDefrostWasAbnormal,
   .convertibleCompartmentDefrostWasAbnormalErd = Erd_ConvertibleCompartmentDefrostWasAbnormal,
   .hasConvertibleCompartment = Erd_HasConvertibleCompartment
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

   void FreezerFilteredTemperatureTooWarmAtPowerUpShouldBe(bool expectedState)
   {
      bool actualState;
      DataModel_Read(dataModel, Erd_FreezerFilteredTemperatureTooWarmAtPowerUp, &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void UseMinimumReadyToDefrostTimeShouldBe(bool expectedState)
   {
      bool actualState;
      DataModel_Read(dataModel, Erd_UseMinimumReadyToDefrostTime, &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void FreezerFilteredTemperatureTooWarmAtPowerUpIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreezerFilteredTemperatureTooWarmAtPowerUp, &state);
   }

   void UseMinimumReadyToDefrostTimeIs(bool state)
   {
      DataModel_Write(dataModel, Erd_UseMinimumReadyToDefrostTime, &state);
   }

   void FreezerEvapThermistorIs(bool valid)
   {
      DataModel_Write(dataModel, Erd_FreezerEvapThermistor_IsValidResolved, &valid);
   }

   void FreshFoodDefrostWasAbnormalIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreshFoodDefrostWasAbnormal, &state);
   }

   void FreezerDefrostWasAbnormalIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreezerDefrostWasAbnormal, &state);
   }

   void ConvertibleCompartmentDefrostWasAbnormalIs(bool state)
   {
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentDefrostWasAbnormal, &state);
   }

   void HasConvertibleCompartmentIs(bool state)
   {
      DataModel_Write(
         dataModel, Erd_HasConvertibleCompartment, &state);
   }

   void TimeBetweenDefrostsIsMinimumTime()
   {
      And FreezerFilteredTemperatureTooWarmAtPowerUpIs(SET);
      And UseMinimumReadyToDefrostTimeIs(SET);
      And FreezerEvapThermistorIs(CLEAR);
      And FreezerDefrostWasAbnormalIs(SET);
   }

   void TimeBetweenDefrostsIsNormalTime()
   {
      And FreezerFilteredTemperatureTooWarmAtPowerUpIs(CLEAR);
      And UseMinimumReadyToDefrostTimeIs(CLEAR);
      And FreezerEvapThermistorIs(SET);
      And FreezerDefrostWasAbnormalIs(CLEAR);
   }

   void PreviousDefrostsAreNormal()
   {
      FreshFoodDefrostWasAbnormalIs(CLEAR);
      FreezerDefrostWasAbnormalIs(CLEAR);
      ConvertibleCompartmentDefrostWasAbnormalIs(CLEAR);
   }

   void PreviousDefrostsAreNormalAndTimeWhenReadyToDefrostIsTimeBetweenDefrostsIsNormalTimesInMinutes()
   {
      Given PreviousDefrostsAreNormal();
      And TimeBetweenDefrostsIsNormalTime();

      And ReadyToDefrostModuleIsInitialized();
   }

   void TheTimeBetweenDefrostsIsNormalTimeAndReadyToDefrostIsClearAndTheReadyToDefrostModuleIsInitialized()
   {
      Given ReadyToDefrostIs(CLEAR);
      And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
      And TimeBetweenDefrostsIsNormalTime();

      And ReadyToDefrostModuleIsInitialized();
   }

   void TheTimeBetweenDefrostsIsNormalTimeAndReadyToDefrostIsSetAndTheReadyToDefrostModuleIsInitialized()
   {
      Given ReadyToDefrostIs(SET);
      And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
      And TimeBetweenDefrostsIsNormalTime();

      And ReadyToDefrostModuleIsInitialized();
   }
};

TEST(ReadyToDefrost, ShouldInitialize)
{
   ReadyToDefrostModuleIsInitialized();
}

TEST(ReadyToDefrost, ShouldClearReadyToDefrostErdWhenCompressorOnTimeIsLessThanTimeBetweenDefrostsIsMinimumTimesAbnormalRunTimeOnInit)
{
   Given ReadyToDefrostIs(SET);
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds - 1);
   And ReadyToDefrostModuleIsInitialized();

   ReadyToDefrostShouldBe(CLEAR);
}

TEST(ReadyToDefrost, ShouldClearReadyToDefrostErdWhenCompressorOnTimeHasNotReachedTimeBetweenDefrostsIsMinimumTimeAbnormalRunTimeInMinutesAndSumOfCompressorOnTimeAndDoorAccelerationsAreEqualToTimeWhenReadyToDefrostOnInit)
{
   Given ReadyToDefrostIs(SET);
   And TimeBetweenDefrostsIsMinimumTime();
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds - 1);
   And FreshFoodDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds);
   And FreezerDoorAccelerationIs(1);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And ReadyToDefrostModuleIsInitialized();

   ReadyToDefrostShouldBe(CLEAR);
}

TEST(ReadyToDefrost, ShouldClearReadyToDefrostErdWhenCompressorOnTimeHasReachedTimeBetweenDefrostsIsMinimumTimeAbnormalRunTimeInMinutesAndSumOfCompressorOnTimeAndDoorAccelerationsAreLessThanTimeWhenReadyToDefrostOnInit)
{
   Given ReadyToDefrostIs(SET);
   And TimeBetweenDefrostsIsNormalTime();
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(0);
   And FreezerDoorAccelerationIs(0);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And ReadyToDefrostModuleIsInitialized();

   ReadyToDefrostShouldBe(CLEAR);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdWhenCompressorOnTimeHasReachedTimeBetweenDefrostsIsMinimumTimeAbnormalRunTimeInMinutesAndSumOfCompressorOnTimeAndDoorAccelerationsAreEqualToTimeWhenReadyToDefrostOnInit)
{
   Given ReadyToDefrostIs(CLEAR);
   And TimeBetweenDefrostsIsMinimumTime();
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(0);
   And FreezerDoorAccelerationIs(0);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And ReadyToDefrostModuleIsInitialized();

   ReadyToDefrostShouldBe(SET);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdWhenCompressorOnTimeHasReachedTimeBetweenDefrostsIsMinimumTimeAbnormalRunTimeInMinutesAndSumOfCompressorOnTimeAndDoorAccelerationsAreGreaterThanTimeWhenReadyToDefrostOnInit)
{
   Given ReadyToDefrostIs(CLEAR);
   And TimeBetweenDefrostsIsMinimumTime();
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(1);
   And FreezerDoorAccelerationIs(0);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And ReadyToDefrostModuleIsInitialized();

   ReadyToDefrostShouldBe(SET);
}

TEST(ReadyToDefrost, ShouldClearReadyToDefrostErdWhenCompressorOnTimeIsGreaterThanTimeBetweenDefrostsIsMinimumTimeAbnormalRunTimeInMinutesAndSumOfCompressorOnTimeAndDoorAccelerationsAreLessThanTimeWhenReadyToDefrostOnInit)
{
   Given ReadyToDefrostIs(SET);
   And TimeBetweenDefrostsIsNormalTime();
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds + 1);
   And FreshFoodDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds - 2);
   And FreezerDoorAccelerationIs(0);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And ReadyToDefrostModuleIsInitialized();

   ReadyToDefrostShouldBe(CLEAR);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdWhenCompressorOnTimeIsGreaterThanTimeBetweenDefrostsIsMinimumTimeAbnormalRunTimeInMinutesAndSumOfCompressorOnTimeAndDoorAccelerationsAreEqualToTimeWhenReadyToDefrostOnInit)
{
   Given ReadyToDefrostIs(CLEAR);
   And TimeBetweenDefrostsIsMinimumTime();
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds + 1);
   And FreshFoodDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds - 1);
   And FreezerDoorAccelerationIs(0);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And ReadyToDefrostModuleIsInitialized();

   ReadyToDefrostShouldBe(SET);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdWhenCompressorOnTimeIsGreaterThanTimeBetweenDefrostsIsMinimumTimeAbnormalRunTimeInMinutesAndSumOfCompressorOnTimeAndDoorAccelerationsAreGreaterThanTimeWhenReadyToDefrostOnInit)
{
   Given ReadyToDefrostIs(CLEAR);
   And TimeBetweenDefrostsIsMinimumTime();
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds + 1);
   And FreshFoodDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds);
   And FreezerDoorAccelerationIs(1);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And ReadyToDefrostModuleIsInitialized();

   ReadyToDefrostShouldBe(SET);
}

TEST(ReadyToDefrost, ShouldClearReadyToDefrostErdWhenCompressorOnTimeIsLessThanTimeBetweenDefrostsIsMinimumTimeAbnormalRunTimeInMinutes)
{
   Given ReadyToDefrostIs(CLEAR);
   And TimeBetweenDefrostsIsMinimumTime();
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds);
   And FreezerDoorAccelerationIs(1);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And ReadyToDefrostModuleIsInitialized();

   ReadyToDefrostShouldBe(SET);

   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds - 1);
   ReadyToDefrostShouldBe(CLEAR);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdWhenCompressorOnTimeIsEqualToTimeBetweenDefrostsIsMinimumTimeAbnormalRunTimeInMinutes)
{
   Given ReadyToDefrostIs(SET);
   And TimeBetweenDefrostsIsMinimumTime();
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds - 1);
   And FreshFoodDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds);
   And FreezerDoorAccelerationIs(1);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And ReadyToDefrostModuleIsInitialized();

   ReadyToDefrostShouldBe(CLEAR);

   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   ReadyToDefrostShouldBe(SET);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdWhenCompressorOnTimeIsGreaterThanTimeBetweenDefrostsIsMinimumTimeAbnormalRunTimeInMinutes)
{
   Given ReadyToDefrostIs(SET);
   And TimeBetweenDefrostsIsMinimumTime();
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
   And TimeBetweenDefrostsIsNormalTime();
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(MaxTimeBetweenDefrostInSeconds - MinTimeBetweenDefrostsInSeconds);
   And FreezerDoorAccelerationIs(0);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And ReadyToDefrostModuleIsInitialized();

   ReadyToDefrostShouldBe(SET);

   And FreshFoodDoorAccelerationIs(MaxTimeBetweenDefrostInSeconds - MinTimeBetweenDefrostsInSeconds - 1);
   ReadyToDefrostShouldBe(CLEAR);
}

TEST(ReadyToDefrost, ShouldClearReadyToDefrostErdWhenFreezerDoorAccelerationChangesSuchThatSumOfCompressorOnTimeAndDoorAccelerationIsLessThanTimeWhenReadyToDefrost)
{
   Given ReadyToDefrostIs(CLEAR);
   And TimeBetweenDefrostsIsNormalTime();
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(0);
   And FreezerDoorAccelerationIs(MaxTimeBetweenDefrostInSeconds - MinTimeBetweenDefrostsInSeconds);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And ReadyToDefrostModuleIsInitialized();

   ReadyToDefrostShouldBe(SET);

   And FreezerDoorAccelerationIs(MaxTimeBetweenDefrostInSeconds - MinTimeBetweenDefrostsInSeconds - 1);
   ReadyToDefrostShouldBe(CLEAR);
}

TEST(ReadyToDefrost, ShouldClearReadyToDefrostErdWhenConvertibleCompartmentDoorAccelerationChangesSuchThatSumOfCompressorOnTimeAndDoorAccelerationIsLessThanTimeWhenReadyToDefrost)
{
   Given ReadyToDefrostIs(CLEAR);
   And TimeBetweenDefrostsIsNormalTime();
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(0);
   And FreezerDoorAccelerationIs(0);
   And ConvertibleCompartmentDoorAccelerationIs(MaxTimeBetweenDefrostInSeconds - MinTimeBetweenDefrostsInSeconds);
   And ReadyToDefrostModuleIsInitialized();

   ReadyToDefrostShouldBe(SET);

   And ConvertibleCompartmentDoorAccelerationIs(MaxTimeBetweenDefrostInSeconds - MinTimeBetweenDefrostsInSeconds - 1);
   ReadyToDefrostShouldBe(CLEAR);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdWhenFreshFoodDoorAccelerationChangesSuchThatSumOfCompressorOnTimeAndDoorAccelerationIsEqualToTimeWhenReadyToDefrost)
{
   Given ReadyToDefrostIs(SET);
   And TimeBetweenDefrostsIsNormalTime();
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(MaxTimeBetweenDefrostInSeconds - MinTimeBetweenDefrostsInSeconds - 1);
   And FreezerDoorAccelerationIs(0);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And ReadyToDefrostModuleIsInitialized();

   ReadyToDefrostShouldBe(CLEAR);

   And FreshFoodDoorAccelerationIs(MaxTimeBetweenDefrostInSeconds - MinTimeBetweenDefrostsInSeconds);
   ReadyToDefrostShouldBe(SET);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdWhenFreezerDoorAccelerationChangesSuchThatSumOfCompressorOnTimeAndDoorAccelerationIsEqualToTimeWhenReadyToDefrost)
{
   Given ReadyToDefrostIs(SET);
   And TimeBetweenDefrostsIsNormalTime();
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(0);
   And FreezerDoorAccelerationIs(MaxTimeBetweenDefrostInSeconds - MinTimeBetweenDefrostsInSeconds - 1);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And ReadyToDefrostModuleIsInitialized();

   ReadyToDefrostShouldBe(CLEAR);

   And FreshFoodDoorAccelerationIs(MaxTimeBetweenDefrostInSeconds - MinTimeBetweenDefrostsInSeconds);
   ReadyToDefrostShouldBe(SET);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdWhenConvertibleCompartmentDoorAccelerationChangesSuchThatSumOfCompressorOnTimeAndDoorAccelerationIsEqualToTimeWhenReadyToDefrost)
{
   Given ReadyToDefrostIs(SET);
   And TimeBetweenDefrostsIsNormalTime();
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(0);
   And FreezerDoorAccelerationIs(0);
   And ConvertibleCompartmentDoorAccelerationIs(MaxTimeBetweenDefrostInSeconds - MinTimeBetweenDefrostsInSeconds - 1);
   And ReadyToDefrostModuleIsInitialized();

   ReadyToDefrostShouldBe(CLEAR);

   And ConvertibleCompartmentDoorAccelerationIs(MaxTimeBetweenDefrostInSeconds - MinTimeBetweenDefrostsInSeconds);
   ReadyToDefrostShouldBe(SET);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdWhenFreshFoodDoorAccelerationChangesSuchThatSumOfCompressorOnTimeAndDoorAccelerationIsGreaterThanTimeWhenReadyToDefrost)
{
   Given ReadyToDefrostIs(SET);
   And TimeBetweenDefrostsIsNormalTime();
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(MaxTimeBetweenDefrostInSeconds - MinTimeBetweenDefrostsInSeconds - 1);
   And FreezerDoorAccelerationIs(0);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And ReadyToDefrostModuleIsInitialized();

   ReadyToDefrostShouldBe(CLEAR);

   And FreshFoodDoorAccelerationIs(MaxTimeBetweenDefrostInSeconds - MinTimeBetweenDefrostsInSeconds + 1);
   ReadyToDefrostShouldBe(SET);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdWhenFreezerDoorAccelerationChangesSuchThatSumOfCompressorOnTimeAndDoorAccelerationIsGreaterThanTimeWhenReadyToDefrost)
{
   Given ReadyToDefrostIs(SET);
   And TimeBetweenDefrostsIsNormalTime();
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(0);
   And FreezerDoorAccelerationIs(MaxTimeBetweenDefrostInSeconds - MinTimeBetweenDefrostsInSeconds - 1);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And ReadyToDefrostModuleIsInitialized();

   ReadyToDefrostShouldBe(CLEAR);

   And FreshFoodDoorAccelerationIs(MaxTimeBetweenDefrostInSeconds - MinTimeBetweenDefrostsInSeconds + 1);
   ReadyToDefrostShouldBe(SET);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdWhenConvertibleCompartmentDoorAccelerationChangesSuchThatSumOfCompressorOnTimeAndDoorAccelerationIsGreaterThanTimeWhenReadyToDefrost)
{
   Given ReadyToDefrostIs(SET);
   And TimeBetweenDefrostsIsNormalTime();
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(0);
   And FreezerDoorAccelerationIs(0);
   And ConvertibleCompartmentDoorAccelerationIs(MaxTimeBetweenDefrostInSeconds - MinTimeBetweenDefrostsInSeconds - 1);
   And ReadyToDefrostModuleIsInitialized();

   ReadyToDefrostShouldBe(CLEAR);

   And ConvertibleCompartmentDoorAccelerationIs(MaxTimeBetweenDefrostInSeconds - MinTimeBetweenDefrostsInSeconds + 1);
   ReadyToDefrostShouldBe(SET);
}

TEST(ReadyToDefrost, ShouldClearReadyToDefrostErdWhenTimeWhenReadyToDefrostChangesSoThatSumOfCompressorOnTimeAndDoorAccelerationAreNowLessThanIt)
{
   Given ReadyToDefrostIs(CLEAR);
   And TimeBetweenDefrostsIsMinimumTime();
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(0);
   And FreezerDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And ReadyToDefrostModuleIsInitialized();

   ReadyToDefrostShouldBe(SET);

   And TimeBetweenDefrostsIsNormalTime();
   ReadyToDefrostShouldBe(CLEAR);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdWhenTimeWhenReadyToDefrostChangesSoThatSumOfCompressorOnTimeAndDoorAccelerationAreNowEqualToIt)
{
   Given ReadyToDefrostIs(SET);
   And TimeBetweenDefrostsIsNormalTime();
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(0);
   And FreezerDoorAccelerationIs(0);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And ReadyToDefrostModuleIsInitialized();

   ReadyToDefrostShouldBe(CLEAR);

   And TimeBetweenDefrostsIsMinimumTime();
   ReadyToDefrostShouldBe(SET);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdWhenTimeWhenReadyToDefrostChangesSoThatSumOfCompressorOnTimeAndDoorAccelerationAreNowGreaterThanIt)
{
   Given ReadyToDefrostIs(SET);
   And TimeBetweenDefrostsIsNormalTime();
   And CompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   And FreshFoodDoorAccelerationIs(0);
   And FreezerDoorAccelerationIs(MinTimeBetweenDefrostsInSeconds);
   And ConvertibleCompartmentDoorAccelerationIs(0);
   And ReadyToDefrostModuleIsInitialized();

   ReadyToDefrostShouldBe(CLEAR);

   And TimeBetweenDefrostsIsMinimumTime();
   ReadyToDefrostShouldBe(SET);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdWhenDefrostUseMinimumTimeErdIsSetThenClearReadyToDefrostErdWhenUseMinimumTimeErdIsCleared)
{
   Given TheTimeBetweenDefrostsIsNormalTimeAndReadyToDefrostIsClearAndTheReadyToDefrostModuleIsInitialized();
   And UseMinimumReadyToDefrostTimeIs(SET);

   ReadyToDefrostShouldBe(SET);

   And UseMinimumReadyToDefrostTimeIs(CLEAR);
   ReadyToDefrostShouldBe(CLEAR);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdWhenFreezerEvapThermistorIsInvalidThenClearReadyToDefrostErdWhenFreezerEvapThermistorIsValid)
{
   Given TheTimeBetweenDefrostsIsNormalTimeAndReadyToDefrostIsClearAndTheReadyToDefrostModuleIsInitialized();
   And FreezerEvapThermistorIs(CLEAR);

   ReadyToDefrostShouldBe(SET);

   And FreezerEvapThermistorIs(SET);
   ReadyToDefrostShouldBe(CLEAR);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdWhenFreshFoodDefrostWasAbnormalIsSetThenClearReadyToDefrostErdWhenFreshFoodDefrostWasAbnormalIsCleared)
{
   Given TheTimeBetweenDefrostsIsNormalTimeAndReadyToDefrostIsClearAndTheReadyToDefrostModuleIsInitialized();
   And FreshFoodDefrostWasAbnormalIs(SET);

   ReadyToDefrostShouldBe(SET);

   And FreshFoodDefrostWasAbnormalIs(CLEAR);
   ReadyToDefrostShouldBe(CLEAR);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdWhenFreezerDefrostWasAbnormalIsSetThenClearReadyToDefrostErdWhenFreezerDefrostWasAbnormalIsCleared)
{
   Given TheTimeBetweenDefrostsIsNormalTimeAndReadyToDefrostIsClearAndTheReadyToDefrostModuleIsInitialized();
   And FreezerDefrostWasAbnormalIs(SET);

   ReadyToDefrostShouldBe(SET);

   And FreezerDefrostWasAbnormalIs(CLEAR);
   ReadyToDefrostShouldBe(CLEAR);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdWhenConvertibleCompartmentDefrostWasAbnormalIsSetThenClearReadyToDefrostErdWhenConvertibleCompartmentDefrostWasAbnormalIsCleared)
{
   Given TheTimeBetweenDefrostsIsNormalTimeAndReadyToDefrostIsClearAndTheReadyToDefrostModuleIsInitialized();
   And HasConvertibleCompartmentIs(SET);
   And ConvertibleCompartmentDefrostWasAbnormalIs(SET);

   ReadyToDefrostShouldBe(SET);

   And ConvertibleCompartmentDefrostWasAbnormalIs(CLEAR);
   ReadyToDefrostShouldBe(CLEAR);
}

TEST(ReadyToDefrost, ShouldClearReadyToDefrostErdWhenThereIsNoConvertibleCompartment)
{
   Given TheTimeBetweenDefrostsIsNormalTimeAndReadyToDefrostIsSetAndTheReadyToDefrostModuleIsInitialized();
   And HasConvertibleCompartmentIs(CLEAR);
   And ConvertibleCompartmentDefrostWasAbnormalIs(SET);

   ReadyToDefrostShouldBe(CLEAR);
}

TEST(ReadyToDefrost, ShouldClearUseMinimumReadyToDefrostTimeErdWhenReadyToDefrostIsSet)
{
   Given TheTimeBetweenDefrostsIsNormalTimeAndReadyToDefrostIsClearAndTheReadyToDefrostModuleIsInitialized();
   And FreezerEvapThermistorIs(CLEAR);
   UseMinimumReadyToDefrostTimeIs(SET);

   ReadyToDefrostShouldBe(SET);
   UseMinimumReadyToDefrostTimeIs(CLEAR);
}
