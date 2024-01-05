/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "LastTwelveMonthsOfWaterUsageUpdater.h"
#include "SystemErds.h"
#include "DataModelErdPointerAccess.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "TimerModule_TestDouble.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"
#include "uassert_test.h"
#include "utils.h"

enum
{
   FilterMonthInMinutes = 43200,
   OuncesX100PerGallon = 128 * 100,
   FirstMonthWaterFilterVolumeUsageInOuncesX100 = 12874,
   SecondMonthWaterFilterVolumeUsageInOuncesX100 = 396090,
   GreaterThanU8MaxWaterFilterVolumeUsageInOuncesX100 = OuncesX100PerGallon * UINT8_MAX + (OuncesX100PerGallon * 5),
   FirstMonthWaterFilterVolumeUsageInGallons = FirstMonthWaterFilterVolumeUsageInOuncesX100 / OuncesX100PerGallon,
   SecondMonthWaterFilterVolumeUsageInGallons = (SecondMonthWaterFilterVolumeUsageInOuncesX100 - FirstMonthWaterFilterVolumeUsageInOuncesX100) / OuncesX100PerGallon
};

static const LastTwelveMonthsOfWaterUsageUpdaterConfig_t config = {
   .currentWaterFilterVolumeUsageInOuncesX100Erd = Erd_WaterFilterVolumeUsageInOuncesX100,
   .previousWaterFilterVolumeUsageInOuncesX100Erd = Erd_PreviousWaterFilterVolumeUsageInOuncesX100,
   .currentWaterFilterMonthTimeInMinutesErd = Erd_CurrentWaterFilterMonthTimeInMinutes,
   .lastTwelveMonthsOfWaterUsageInGallonsEepromErd = Erd_RfidFilterLastTwelveMonthsOfWaterUsageInGallons
};

static const RfidFilterData_t rfidFilterData = {
   .rfidFilterUpdateRateData = 0,
   .filterMonthInMinutes = 43200
};

TEST_GROUP(LastTwelveMonthsOfWaterUsageUpdater)
{
   LastTwelveMonthsOfWaterUsageUpdater_t instance;

   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;

      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);
   }

   void GivenTheModuleIsInitialized()
   {
      LastTwelveMonthsOfWaterUsageUpdater_Init(
         &instance,
         dataModel,
         &config,
         &rfidFilterData);
   }

   void GivenWaterFilterVolumeUsageInOuncesX100Is(VolumeInOuncesX100_t WaterFilterVolumeUsageInOuncesX100)
   {
      DataModel_Write(
         dataModel,
         Erd_WaterFilterVolumeUsageInOuncesX100,
         &WaterFilterVolumeUsageInOuncesX100);
   }

   void WhenWaterFilterVolumeUsageInOuncesX100Is(VolumeInOuncesX100_t WaterFilterVolumeUsageInOuncesX100)
   {
      GivenWaterFilterVolumeUsageInOuncesX100Is(WaterFilterVolumeUsageInOuncesX100);
   }

   void WaterFilterMonthTimeInMinutesShouldBe(uint16_t expected)
   {
      uint16_t actual;
      DataModel_Read(
         dataModel,
         Erd_CurrentWaterFilterMonthTimeInMinutes,
         &actual);

      CHECK_EQUAL(expected, actual);
   }

   void LastTwelveMonthsOfWaterUsageInGallonsShouldBe(RfidTwelveMonthsGallonUsage_t expected)
   {
      RfidTwelveMonthsGallonUsage_t actual;
      DataModel_Read(
         dataModel,
         Erd_RfidFilterLastTwelveMonthsOfWaterUsageInGallons,
         &actual);

      MEMCMP_EQUAL(&expected, &actual, sizeof(RfidTwelveMonthsGallonUsage_t));
   }

   VolumeInOuncesX100_t PreviousWaterFilterVolumeUsageInOuncesX100()
   {
      VolumeInOuncesX100_t previousWaterFilterVolumeUsageInOuncesX100;
      DataModel_Read(
         dataModel,
         Erd_PreviousWaterFilterVolumeUsageInOuncesX100,
         &previousWaterFilterVolumeUsageInOuncesX100);

      return previousWaterFilterVolumeUsageInOuncesX100;
   }

   void PreviousWaterFilterVolumeUsageInOuncesX100ShouldBe(VolumeInOuncesX100_t expected)
   {
      VolumeInOuncesX100_t actual = PreviousWaterFilterVolumeUsageInOuncesX100();
      CHECK_EQUAL(expected, actual);
   }

   void AfterTenMonths()
   {
      for(int i = 0; i < 10; i++)
      {
         After((uint32_t)FilterMonthInMinutes * MSEC_PER_MIN);
      }
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks);
   }
};

TEST(LastTwelveMonthsOfWaterUsageUpdater, ShouldUpdateWaterFilterMonthTimeInMinutesEveryMinute)
{
   GivenTheModuleIsInitialized();
   WaterFilterMonthTimeInMinutesShouldBe(0);

   After(MSEC_PER_MIN - 1);
   WaterFilterMonthTimeInMinutesShouldBe(0);

   After(1);
   WaterFilterMonthTimeInMinutesShouldBe(1);

   After(MSEC_PER_MIN - 1);
   WaterFilterMonthTimeInMinutesShouldBe(1);

   After(1);
   WaterFilterMonthTimeInMinutesShouldBe(2);
}

TEST(LastTwelveMonthsOfWaterUsageUpdater, ShouldResetWaterFilterMonthTimeInMinutesWhenReachingFilterMonthInMinutes)
{
   GivenTheModuleIsInitialized();

   After((uint32_t)FilterMonthInMinutes * MSEC_PER_MIN - 1);
   WaterFilterMonthTimeInMinutesShouldBe(FilterMonthInMinutes - 1);

   After(1);
   WaterFilterMonthTimeInMinutesShouldBe(0);
}

TEST(LastTwelveMonthsOfWaterUsageUpdater, ShouldStoreNewPreviousWaterFilterVolumeUsageInOuncesX100WhenReachingFilterMonthInMinutes)
{
   GivenWaterFilterVolumeUsageInOuncesX100Is(FirstMonthWaterFilterVolumeUsageInOuncesX100);
   GivenTheModuleIsInitialized();

   After((uint32_t)FilterMonthInMinutes * MSEC_PER_MIN);
   PreviousWaterFilterVolumeUsageInOuncesX100ShouldBe(FirstMonthWaterFilterVolumeUsageInOuncesX100);
}

TEST(LastTwelveMonthsOfWaterUsageUpdater, ShouldStoreNewMonthWaterUsageInGallonsWhenFilterMonthInMinutesIsReached)
{
   GivenTheModuleIsInitialized();

   RfidTwelveMonthsGallonUsage_t waterUsageInGallonsExpected = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

   WhenWaterFilterVolumeUsageInOuncesX100Is(FirstMonthWaterFilterVolumeUsageInOuncesX100);
   // Should not update the array until water usage run time reaches filter month in minutes
   After((uint32_t)FilterMonthInMinutes * MSEC_PER_MIN - 1);
   LastTwelveMonthsOfWaterUsageInGallonsShouldBe(waterUsageInGallonsExpected);

   waterUsageInGallonsExpected = { FirstMonthWaterFilterVolumeUsageInGallons, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

   After(1);
   LastTwelveMonthsOfWaterUsageInGallonsShouldBe(waterUsageInGallonsExpected);
}

TEST(LastTwelveMonthsOfWaterUsageUpdater, ShouldLimitTheGallonsUsedToU8MaxWhenVolumeUsageGoesAboveU8Max)
{
   GivenTheModuleIsInitialized();

   RfidTwelveMonthsGallonUsage_t waterUsageInGallonsExpected = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

   WhenWaterFilterVolumeUsageInOuncesX100Is(GreaterThanU8MaxWaterFilterVolumeUsageInOuncesX100);
   waterUsageInGallonsExpected = { UINT8_MAX, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

   After((uint32_t)FilterMonthInMinutes * MSEC_PER_MIN);
   LastTwelveMonthsOfWaterUsageInGallonsShouldBe(waterUsageInGallonsExpected);
}

TEST(LastTwelveMonthsOfWaterUsageUpdater, ShouldUpdateLastTwelveMonthsOfWaterUsageInGallonsWhenReachingFilterMonthsInMinutesAndWaterFilterVolumeUsageInOuncesX100IsMoreThanPreviousMonth)
{
   GivenTheModuleIsInitialized();

   RfidTwelveMonthsGallonUsage_t waterUsageInGallonsExpected = { 0 };

   WhenWaterFilterVolumeUsageInOuncesX100Is(FirstMonthWaterFilterVolumeUsageInOuncesX100);
   waterUsageInGallonsExpected = { FirstMonthWaterFilterVolumeUsageInGallons, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

   After((uint32_t)FilterMonthInMinutes * MSEC_PER_MIN);
   LastTwelveMonthsOfWaterUsageInGallonsShouldBe(waterUsageInGallonsExpected);

   WhenWaterFilterVolumeUsageInOuncesX100Is(SecondMonthWaterFilterVolumeUsageInOuncesX100);
   waterUsageInGallonsExpected = { SecondMonthWaterFilterVolumeUsageInGallons, FirstMonthWaterFilterVolumeUsageInGallons, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

   After((uint32_t)FilterMonthInMinutes * MSEC_PER_MIN);
   LastTwelveMonthsOfWaterUsageInGallonsShouldBe(waterUsageInGallonsExpected);
}

TEST(LastTwelveMonthsOfWaterUsageUpdater, ShouldDeleteTwelfthValueInLastTwelveMonthsOfWaterUsageEepromErdEveryMonthWhenReachingFilterMonthInMinutes)
{
   GivenTheModuleIsInitialized();

   RfidTwelveMonthsGallonUsage_t waterUsageInGallonsExpected = { 0 };

   WhenWaterFilterVolumeUsageInOuncesX100Is(FirstMonthWaterFilterVolumeUsageInOuncesX100);
   waterUsageInGallonsExpected = { FirstMonthWaterFilterVolumeUsageInGallons, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

   After((uint32_t)FilterMonthInMinutes * MSEC_PER_MIN);
   LastTwelveMonthsOfWaterUsageInGallonsShouldBe(waterUsageInGallonsExpected);

   WhenWaterFilterVolumeUsageInOuncesX100Is(SecondMonthWaterFilterVolumeUsageInOuncesX100);
   waterUsageInGallonsExpected = { SecondMonthWaterFilterVolumeUsageInGallons, FirstMonthWaterFilterVolumeUsageInGallons, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

   After((uint32_t)FilterMonthInMinutes * MSEC_PER_MIN);
   LastTwelveMonthsOfWaterUsageInGallonsShouldBe(waterUsageInGallonsExpected);

   waterUsageInGallonsExpected = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, SecondMonthWaterFilterVolumeUsageInGallons, FirstMonthWaterFilterVolumeUsageInGallons };

   AfterTenMonths();
   LastTwelveMonthsOfWaterUsageInGallonsShouldBe(waterUsageInGallonsExpected);

   waterUsageInGallonsExpected = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, SecondMonthWaterFilterVolumeUsageInGallons };

   After((uint32_t)FilterMonthInMinutes * MSEC_PER_MIN);
   LastTwelveMonthsOfWaterUsageInGallonsShouldBe(waterUsageInGallonsExpected);
}
