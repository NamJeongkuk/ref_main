/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "WaterFilterRemainingUsageUpdater.h"
#include "DataModelErdPointerAccess.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "uassert_test.h"
#include "ReferDataModel_TestDouble.h"
#include "Constants_Time.h"
#include "utils.h"

enum
{
   SomeTotalVolumeUsageInOuncesx100 = 1000,
   SomeCalendarUsageInSeconds = 2000,
   OtherTotalVolumeUsageInOuncesx100 = 3000,
   OtherCalendarUsageInSeconds = 4000,
   NotFullCalendarUsageInSeconds1 = 349200,
   NotFullCalendarUsageInSeconds2 = 352800,
   NotFullCalendarUsageInSeconds3 = 356459,
   FullCalendarUsageInSeconds = 360000,
   BiggerVolumeUsageThanFilterRatedVolume = 51000,
   BiggerCalendarUsageThanFilterRatedLife = 370000
};

static const WaterFilterRemainingUsageUpdaterConfig_t config = {
   .waterFilterVolumeUsageInOuncesX100Erd = Erd_WaterFilterVolumeUsageInOuncesX100,
   .waterFilterCalendarUsageInSecondsErd = Erd_WaterFilterCalendarUsageInSeconds,
   .waterFilterRemainingUsageErd = Erd_WaterFilterRemainingUsage
};

static const CommonFilterData_t commonFilterData = {
   .filterRatedVolumeInOuncesX100 = 50000,
   .filterRatedLifeInMinutes = 6000,
   .minimumVolumeNeededToStartFilterLifeTimerInOunces = 100,
   .waterFilterCalendarUsageDelayForLowUsageInMinutes = 129600
};

TEST_GROUP(WaterFilterRemainingUsageUpdater)
{
   WaterFilterRemainingUsageUpdater_t instance;

   ReferDataModel_TestDouble_t referDataModelTestDouble;
   I_DataModel_t *dataModel;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&referDataModelTestDouble);
      dataModel = referDataModelTestDouble.dataModel;
   }

   void GivenWaterFilterRemainingUsageUpdaterIsInitialized()
   {
      WaterFilterRemainingUsageUpdater_Init(
         &instance,
         dataModel,
         &config,
         &commonFilterData);
   }

   void WhenWaterFilterRemainingUsageUpdaterIsInitialized()
   {
      GivenWaterFilterRemainingUsageUpdaterIsInitialized();
   }

   void GivenWaterFilterVolumeUsageInOuncesX100ErdIs(VolumeInOuncesX100_t waterFilterVolumeUsageInOuncesx100)
   {
      DataModel_Write(
         dataModel,
         Erd_WaterFilterVolumeUsageInOuncesX100,
         &waterFilterVolumeUsageInOuncesx100);
   }

   void WhenWaterFilterVolumeUsageInOuncesX100ErdIs(VolumeInOuncesX100_t waterFilterVolumeUsageInOuncesx100)
   {
      GivenWaterFilterVolumeUsageInOuncesX100ErdIs(waterFilterVolumeUsageInOuncesx100);
   }

   void GivenWaterFilterCalendarUsageInSecondsErdIs(CalendarUsageInSeconds_t waterFilterCalendarUsageInSeconds)
   {
      DataModel_Write(
         dataModel,
         Erd_WaterFilterCalendarUsageInSeconds,
         &waterFilterCalendarUsageInSeconds);
   }

   void WhenWaterFilterCalendarUsageInSecondsErdIs(CalendarUsageInSeconds_t waterFilterCalendarUsageInSeconds)
   {
      GivenWaterFilterCalendarUsageInSecondsErdIs(waterFilterCalendarUsageInSeconds);
   }

   void WaterFilterPercentUsageRemainingShouldBe(uint8_t expectedPercentUsageRemaining)
   {
      WaterFilterRemainingUsage_t actual;
      DataModel_Read(
         dataModel,
         Erd_WaterFilterRemainingUsage,
         &actual);

      CHECK_EQUAL(expectedPercentUsageRemaining, actual.percentUsageRemaining);
   }

   void WaterFilterDaysUsageRemainingShouldBe(uint16_t expectedDaysUsageRemaining)
   {
      WaterFilterRemainingUsage_t actual;
      DataModel_Read(
         dataModel,
         Erd_WaterFilterRemainingUsage,
         &actual);

      CHECK_EQUAL(expectedDaysUsageRemaining, actual.daysUsageRemaining);
   }

   uint8_t TheLesserValueBetweenVolumeAndTimePercentUsageRemaining(VolumeInOuncesX100_t totalVolumeUsageInOuncesx100, CalendarUsageInSeconds_t calendarUsageInSeconds)
   {
      uint8_t volumePercentUsageRemaining = (commonFilterData.filterRatedVolumeInOuncesX100 - totalVolumeUsageInOuncesx100) * 100 / commonFilterData.filterRatedVolumeInOuncesX100;
      uint8_t timePercentUsageRemaining = (commonFilterData.filterRatedLifeInMinutes - (calendarUsageInSeconds / SECONDS_PER_MINUTE)) * 100 / commonFilterData.filterRatedLifeInMinutes;

      return MIN(volumePercentUsageRemaining, timePercentUsageRemaining);
   }

   uint16_t DividedWithCeilingWith(uint8_t percentUsageRemaining)
   {
      return UNSIGNED_DIVIDE_WITH_CEILING(((percentUsageRemaining * commonFilterData.filterRatedLifeInMinutes) / 100), MINUTES_PER_DAY);
   }
};

TEST(WaterFilterRemainingUsageUpdater, ShouldUpdateWaterFilterRemainingUsageWhenModuleIsInitialized)
{
   GivenWaterFilterVolumeUsageInOuncesX100ErdIs(SomeTotalVolumeUsageInOuncesx100);
   GivenWaterFilterCalendarUsageInSecondsErdIs(SomeCalendarUsageInSeconds);

   WhenWaterFilterRemainingUsageUpdaterIsInitialized();

   WaterFilterPercentUsageRemainingShouldBe(TheLesserValueBetweenVolumeAndTimePercentUsageRemaining(SomeTotalVolumeUsageInOuncesx100, SomeCalendarUsageInSeconds));
   WaterFilterDaysUsageRemainingShouldBe(DividedWithCeilingWith(TheLesserValueBetweenVolumeAndTimePercentUsageRemaining(SomeTotalVolumeUsageInOuncesx100, SomeCalendarUsageInSeconds)));
}

TEST(WaterFilterRemainingUsageUpdater, ShouldUpdateWaterFilterRemainingUsageWhenWaterFilterVolumeUsageIsChanged)
{
   GivenWaterFilterVolumeUsageInOuncesX100ErdIs(SomeTotalVolumeUsageInOuncesx100);
   GivenWaterFilterCalendarUsageInSecondsErdIs(SomeCalendarUsageInSeconds);
   GivenWaterFilterRemainingUsageUpdaterIsInitialized();

   WhenWaterFilterVolumeUsageInOuncesX100ErdIs(OtherTotalVolumeUsageInOuncesx100);
   WaterFilterPercentUsageRemainingShouldBe(TheLesserValueBetweenVolumeAndTimePercentUsageRemaining(OtherTotalVolumeUsageInOuncesx100, SomeCalendarUsageInSeconds));
   WaterFilterDaysUsageRemainingShouldBe(DividedWithCeilingWith(TheLesserValueBetweenVolumeAndTimePercentUsageRemaining(OtherTotalVolumeUsageInOuncesx100, SomeCalendarUsageInSeconds)));
}

TEST(WaterFilterRemainingUsageUpdater, ShouldUpdateWaterFilterRemainingUsageWhenCalendarUsageIsChanged)
{
   GivenWaterFilterVolumeUsageInOuncesX100ErdIs(SomeTotalVolumeUsageInOuncesx100);
   GivenWaterFilterCalendarUsageInSecondsErdIs(SomeCalendarUsageInSeconds);
   GivenWaterFilterRemainingUsageUpdaterIsInitialized();

   WhenWaterFilterCalendarUsageInSecondsErdIs(OtherCalendarUsageInSeconds);
   WaterFilterPercentUsageRemainingShouldBe(TheLesserValueBetweenVolumeAndTimePercentUsageRemaining(SomeTotalVolumeUsageInOuncesx100, OtherCalendarUsageInSeconds));
   WaterFilterDaysUsageRemainingShouldBe(DividedWithCeilingWith(TheLesserValueBetweenVolumeAndTimePercentUsageRemaining(SomeTotalVolumeUsageInOuncesx100, OtherCalendarUsageInSeconds)));
}

TEST(WaterFilterRemainingUsageUpdater, ShouldChangeWaterFilterDaysUsageRemainingToZeroWhenFilterPercentUsageRemainingIsFully0)
{
   GivenWaterFilterVolumeUsageInOuncesX100ErdIs(SomeTotalVolumeUsageInOuncesx100);
   GivenWaterFilterCalendarUsageInSecondsErdIs(NotFullCalendarUsageInSeconds1);
   GivenWaterFilterRemainingUsageUpdaterIsInitialized();
   WaterFilterPercentUsageRemainingShouldBe(3);
   WaterFilterDaysUsageRemainingShouldBe(1);

   WhenWaterFilterCalendarUsageInSecondsErdIs(NotFullCalendarUsageInSeconds2);
   WaterFilterPercentUsageRemainingShouldBe(2);
   WaterFilterDaysUsageRemainingShouldBe(1);

   WhenWaterFilterCalendarUsageInSecondsErdIs(NotFullCalendarUsageInSeconds3);
   WaterFilterPercentUsageRemainingShouldBe(1);
   WaterFilterDaysUsageRemainingShouldBe(1);

   WhenWaterFilterCalendarUsageInSecondsErdIs(FullCalendarUsageInSeconds);
   WaterFilterPercentUsageRemainingShouldBe(0);
   WaterFilterDaysUsageRemainingShouldBe(0);
}

TEST(WaterFilterRemainingUsageUpdater, ShouldUpdateWaterFilterRemainingUsageToZeroWhenWaterFilterVolumeUsageIsChangedButBiggerThanFilterRatedVolume)
{
   GivenWaterFilterVolumeUsageInOuncesX100ErdIs(SomeTotalVolumeUsageInOuncesx100);
   GivenWaterFilterCalendarUsageInSecondsErdIs(SomeCalendarUsageInSeconds);
   GivenWaterFilterRemainingUsageUpdaterIsInitialized();

   WhenWaterFilterVolumeUsageInOuncesX100ErdIs(BiggerVolumeUsageThanFilterRatedVolume);
   WaterFilterPercentUsageRemainingShouldBe(0);
   WaterFilterDaysUsageRemainingShouldBe(0);
}

TEST(WaterFilterRemainingUsageUpdater, ShouldUpdateWaterFilterRemainingUsageToZeroWhenCalendarUsageIsChangedButBiggerThanFilterRatedLife)
{
   GivenWaterFilterVolumeUsageInOuncesX100ErdIs(SomeTotalVolumeUsageInOuncesx100);
   GivenWaterFilterCalendarUsageInSecondsErdIs(SomeCalendarUsageInSeconds);
   GivenWaterFilterRemainingUsageUpdaterIsInitialized();

   WhenWaterFilterCalendarUsageInSecondsErdIs(BiggerCalendarUsageThanFilterRatedLife);
   WaterFilterPercentUsageRemainingShouldBe(0);
   WaterFilterDaysUsageRemainingShouldBe(0);
}
