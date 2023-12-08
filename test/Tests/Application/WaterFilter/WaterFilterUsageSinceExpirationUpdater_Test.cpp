/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "WaterFilterUsageSinceExpirationUpdater.h"
#include "DataModelErdPointerAccess.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "uassert_test.h"
#include "ReferDataModel_TestDouble.h"
#include "Constants_Time.h"

enum
{
   FilterRatedLifeInMinutes = 6000,
   FilterRatedVolumeInOuncesx100 = 50000,
   SomeCalendarUsageSinceExpirationInSeconds = (FilterRatedLifeInMinutes * SECONDS_PER_MINUTE) + 1000,
   IncreasedCalendarUsageSinceExpirationInSeconds = (FilterRatedLifeInMinutes * SECONDS_PER_MINUTE) + 2000,
   SomeWaterFilterVolumeUsageInOuncesx100 = FilterRatedVolumeInOuncesx100 + 1000,
   IncreasedWaterFilterVolumeUsageInOuncesx100 = FilterRatedVolumeInOuncesx100 + 2000,
   NewWaterFilterVolumeUsageInOuncesx100 = 1000
};

static const WaterFilterUsageSinceExpirationUpdaterConfig_t config = {
   .waterFilterLifeStatusErd = Erd_WaterFilterLifeStatus,
   .waterFilterUsageSinceExpirationErd = Erd_WaterFilterUsageSinceExpiration,
   .calendarUsageInSecondsErd = Erd_WaterFilterCalendarUsageInSeconds,
   .waterFilterVolumeUsageInOuncesX100Erd = Erd_WaterFilterVolumeUsageInOuncesX100,
};

static const CommonFilterData_t commonFilterData = {
   .filterRatedVolumeInOuncesX100 = FilterRatedVolumeInOuncesx100,
   .filterRatedLifeInMinutes = FilterRatedLifeInMinutes,
   .minimumVolumeNeededToStartFilterLifeTimerInOunces = 100,
   .waterFilterCalendarUsageDelayForLowUsageInMinutes = 129600
};

TEST_GROUP(WaterFilterUsageSinceExpirationUpdater)
{
   WaterFilterUsageSinceExpirationUpdater_t instance;

   ReferDataModel_TestDouble_t referDataModelTestDouble;
   I_DataModel_t *dataModel;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&referDataModelTestDouble);
      dataModel = referDataModelTestDouble.dataModel;
   }

   void GivenWaterFilterLifeStatusIs(WaterFilterLifeStatus_t waterFilterLifeStatus)
   {
      DataModel_Write(dataModel, Erd_WaterFilterLifeStatus, &waterFilterLifeStatus);
   }

   void WhenWaterFilterLifeStatusIs(WaterFilterLifeStatus_t waterFilterLifeStatus)
   {
      GivenWaterFilterLifeStatusIs(waterFilterLifeStatus);
   }

   void GivenWaterFilterUsageSinceExpirationUpdaterIsInitialized()
   {
      WaterFilterUsageSinceExpirationUpdater_Init(
         &instance,
         dataModel,
         &config,
         &commonFilterData);
   }

   void WhenWaterFilterUsageSinceExpirationUpdaterIsInitialized()
   {
      GivenWaterFilterUsageSinceExpirationUpdaterIsInitialized();
   }

   void GivenCalendarUsageInSecondsIs(CalendarUsageInSeconds_t calendarUsageInSeconds)
   {
      DataModel_Write(dataModel, Erd_WaterFilterCalendarUsageInSeconds, &calendarUsageInSeconds);
   }

   void WhenCalendarUsageInSecondsIs(CalendarUsageInSeconds_t calendarUsageInSeconds)
   {
      GivenCalendarUsageInSecondsIs(calendarUsageInSeconds);
   }

   void GivenWaterFilterVolumeUsageInOuncesx100Is(VolumeInOuncesX100_t volumeInOuncesx100)
   {
      DataModel_Write(dataModel, Erd_WaterFilterVolumeUsageInOuncesX100, &volumeInOuncesx100);
   }

   void WhenWaterFilterVolumeUsageInOuncesx100Is(VolumeInOuncesX100_t volumeInOuncesx100)
   {
      GivenWaterFilterVolumeUsageInOuncesx100Is(volumeInOuncesx100);
   }

   void TheDaysSinceFilterExpiredShouldBe(uint16_t expectedDaysSinceFilterExpired)
   {
      WaterFilterUsageSinceExpiration_t actual;
      DataModel_Read(dataModel, Erd_WaterFilterUsageSinceExpiration, &actual);

      CHECK_EQUAL(expectedDaysSinceFilterExpired, actual.daysSinceFilterExpired);
   }

   void TheDispensesSinceFilterExpiredShouldBe(uint16_t expectedDispensesSinceFilterExpired)
   {
      WaterFilterUsageSinceExpiration_t actual;
      DataModel_Read(dataModel, Erd_WaterFilterUsageSinceExpiration, &actual);

      CHECK_EQUAL(expectedDispensesSinceFilterExpired, actual.dispensesSinceFilterExpired);
   }

   void WaterFilterUsageSinceExpirationShouldBeReset(void)
   {
      TheDaysSinceFilterExpiredShouldBe(0);
      TheDispensesSinceFilterExpiredShouldBe(0);
   }

   void GivenDispensesSinceFilterExpiredIs(uint16_t theNumberOfDispenses)
   {
      WaterFilterUsageSinceExpiration_t waterFilterUsageSinceExpiration;
      DataModel_Read(dataModel, Erd_WaterFilterUsageSinceExpiration, &waterFilterUsageSinceExpiration);

      waterFilterUsageSinceExpiration.dispensesSinceFilterExpired = theNumberOfDispenses;
      DataModel_Write(dataModel, Erd_WaterFilterUsageSinceExpiration, &waterFilterUsageSinceExpiration);
   }
};

TEST(WaterFilterUsageSinceExpirationUpdater, ShouldUpdateDaysSinceFilterExpiredIfWaterFilterLifeStatusIsExpiredOnInit)
{
   GivenWaterFilterLifeStatusIs(WaterFilterLifeStatus_Expired);
   GivenCalendarUsageInSecondsIs(SomeCalendarUsageSinceExpirationInSeconds);

   WhenWaterFilterUsageSinceExpirationUpdaterIsInitialized();
   TheDaysSinceFilterExpiredShouldBe((SomeCalendarUsageSinceExpirationInSeconds - (commonFilterData.filterRatedLifeInMinutes * SECONDS_PER_MINUTE)) / SECONDS_PER_DAY);
}

TEST(WaterFilterUsageSinceExpirationUpdater, ShouldUpdateDaysSinceFilterExpiredWhenCalendarUsageIsChangedWhileWaterFilterLifeStatusIsExpired)
{
   GivenWaterFilterLifeStatusIs(WaterFilterLifeStatus_Expired);
   GivenCalendarUsageInSecondsIs(SomeCalendarUsageSinceExpirationInSeconds);
   GivenWaterFilterUsageSinceExpirationUpdaterIsInitialized();
   TheDaysSinceFilterExpiredShouldBe((SomeCalendarUsageSinceExpirationInSeconds - (commonFilterData.filterRatedLifeInMinutes * SECONDS_PER_MINUTE)) / SECONDS_PER_DAY);

   WhenCalendarUsageInSecondsIs(IncreasedCalendarUsageSinceExpirationInSeconds);
   TheDaysSinceFilterExpiredShouldBe((IncreasedCalendarUsageSinceExpirationInSeconds - (commonFilterData.filterRatedLifeInMinutes * SECONDS_PER_MINUTE)) / SECONDS_PER_DAY);
}

TEST(WaterFilterUsageSinceExpirationUpdater, ShouldResetWaterFilterUsageSinceExpirationWhenWaterFilterLifeStatusIsChangedToReplace)
{
   GivenWaterFilterLifeStatusIs(WaterFilterLifeStatus_NoOrderNecessary);
   GivenCalendarUsageInSecondsIs(SomeCalendarUsageSinceExpirationInSeconds);
   GivenWaterFilterVolumeUsageInOuncesx100Is(SomeWaterFilterVolumeUsageInOuncesx100);
   GivenWaterFilterUsageSinceExpirationUpdaterIsInitialized();

   WhenWaterFilterLifeStatusIs(WaterFilterLifeStatus_Replace);
   WaterFilterUsageSinceExpirationShouldBeReset();
}

TEST(WaterFilterUsageSinceExpirationUpdater, ShouldResetWaterFilterUsageSinceExpirationWhenWaterFilterLifeStatusIsChangedFromExpiredToNoOrderNecessary)
{
   GivenWaterFilterLifeStatusIs(WaterFilterLifeStatus_Expired);
   GivenCalendarUsageInSecondsIs(SomeCalendarUsageSinceExpirationInSeconds);
   GivenWaterFilterVolumeUsageInOuncesx100Is(SomeWaterFilterVolumeUsageInOuncesx100);
   GivenWaterFilterUsageSinceExpirationUpdaterIsInitialized();

   WhenWaterFilterLifeStatusIs(WaterFilterLifeStatus_NoOrderNecessary);
   WaterFilterUsageSinceExpirationShouldBeReset();
}

TEST(WaterFilterUsageSinceExpirationUpdater, ShouldResetWaterFilterUsageSinceExpirationWhenWaterFilterLifeStatusIsChangedFromExpiredToNoOrderNecessaryAfterWaterFilterVolumeUsageOrCalendarUsageIsChangedToZero)
{
   GivenWaterFilterLifeStatusIs(WaterFilterLifeStatus_Expired);
   GivenCalendarUsageInSecondsIs(SomeCalendarUsageSinceExpirationInSeconds);
   GivenWaterFilterVolumeUsageInOuncesx100Is(SomeWaterFilterVolumeUsageInOuncesx100);
   GivenDispensesSinceFilterExpiredIs(10);
   GivenWaterFilterUsageSinceExpirationUpdaterIsInitialized();

   WhenWaterFilterVolumeUsageInOuncesx100Is(0);
   TheDispensesSinceFilterExpiredShouldBe(11);

   WhenCalendarUsageInSecondsIs(0);
   TheDaysSinceFilterExpiredShouldBe(0);

   WhenWaterFilterLifeStatusIs(WaterFilterLifeStatus_NoOrderNecessary);
   WaterFilterUsageSinceExpirationShouldBeReset();
}

TEST(WaterFilterUsageSinceExpirationUpdater, ShouldIncreaseDispensesSinceFilterExpiredWhenWaterFilterVolumeUsageIsChangedWhileWaterFilterLifeStatusIsExpired)
{
   GivenWaterFilterLifeStatusIs(WaterFilterLifeStatus_Expired);
   GivenWaterFilterVolumeUsageInOuncesx100Is(SomeWaterFilterVolumeUsageInOuncesx100);
   GivenDispensesSinceFilterExpiredIs(10);
   GivenWaterFilterUsageSinceExpirationUpdaterIsInitialized();

   WhenWaterFilterVolumeUsageInOuncesx100Is(IncreasedWaterFilterVolumeUsageInOuncesx100);
   TheDispensesSinceFilterExpiredShouldBe(11);
}

TEST(WaterFilterUsageSinceExpirationUpdater, ShouldNotIncreaseDispensesSinceFilterExpiredWhenWaterFilterVolumeUsageIsChangedWhileWaterFilterUsageSinceExpirationHasBeenReset)
{
   GivenWaterFilterLifeStatusIs(WaterFilterLifeStatus_Expired);
   GivenCalendarUsageInSecondsIs(SomeCalendarUsageSinceExpirationInSeconds);
   GivenWaterFilterVolumeUsageInOuncesx100Is(SomeWaterFilterVolumeUsageInOuncesx100);
   GivenWaterFilterUsageSinceExpirationUpdaterIsInitialized();
   GivenWaterFilterLifeStatusIs(WaterFilterLifeStatus_NoOrderNecessary);
   TheDispensesSinceFilterExpiredShouldBe(0);

   WhenWaterFilterVolumeUsageInOuncesx100Is(NewWaterFilterVolumeUsageInOuncesx100);
   TheDispensesSinceFilterExpiredShouldBe(0);
}
