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
   SomeTotalWaterVolumeUsageInOuncesx100 = FilterRatedVolumeInOuncesx100 + 1000,
   IncreasedTotalWaterVolumeUsageInOuncesx100 = FilterRatedVolumeInOuncesx100 + 2000,
   NewTotalWaterVolumeUsageInOuncesx100 = 1000
};

static const WaterFilterUsageSinceExpirationUpdaterConfig_t config = {
   .waterFilterLifeStatusErd = Erd_WaterFilterLifeStatus,
   .waterFilterUsageSinceExpirationErd = Erd_WaterFilterUsageSinceExpiration,
   .calendarUsageInSecondsErd = Erd_WaterFilterCalendarUsageInSeconds,
   .totalWaterVolumeUsageInOuncesx100Erd = Erd_TotalWaterVolumeUsageInOuncesX100,
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

   void GivenTotalWaterVolumeUsageInOuncesx100Is(VolumeInOuncesX100_t volumeInOuncesx100)
   {
      DataModel_Write(dataModel, Erd_TotalWaterVolumeUsageInOuncesX100, &volumeInOuncesx100);
   }

   void WhenTotalWaterVolumeUsageInOuncesx100Is(VolumeInOuncesX100_t volumeInOuncesx100)
   {
      GivenTotalWaterVolumeUsageInOuncesx100Is(volumeInOuncesx100);
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
   GivenTotalWaterVolumeUsageInOuncesx100Is(SomeTotalWaterVolumeUsageInOuncesx100);
   GivenWaterFilterUsageSinceExpirationUpdaterIsInitialized();

   WhenWaterFilterLifeStatusIs(WaterFilterLifeStatus_Replace);
   WaterFilterUsageSinceExpirationShouldBeReset();
}

TEST(WaterFilterUsageSinceExpirationUpdater, ShouldResetWaterFilterUsageSinceExpirationWhenWaterFilterLifeStatusIsChangedFromExpiredToNoOrderNecessary)
{
   GivenWaterFilterLifeStatusIs(WaterFilterLifeStatus_Expired);
   GivenCalendarUsageInSecondsIs(SomeCalendarUsageSinceExpirationInSeconds);
   GivenTotalWaterVolumeUsageInOuncesx100Is(SomeTotalWaterVolumeUsageInOuncesx100);
   GivenWaterFilterUsageSinceExpirationUpdaterIsInitialized();

   WhenWaterFilterLifeStatusIs(WaterFilterLifeStatus_NoOrderNecessary);
   WaterFilterUsageSinceExpirationShouldBeReset();
}

TEST(WaterFilterUsageSinceExpirationUpdater, ShouldResetWaterFilterUsageSinceExpirationWhenWaterFilterLifeStatusIsChangedFromExpiredToNoOrderNecessaryAfterTotalWaterVolumeUsageOrCalendarUsageIsChangedToZero)
{
   GivenWaterFilterLifeStatusIs(WaterFilterLifeStatus_Expired);
   GivenCalendarUsageInSecondsIs(SomeCalendarUsageSinceExpirationInSeconds);
   GivenTotalWaterVolumeUsageInOuncesx100Is(SomeTotalWaterVolumeUsageInOuncesx100);
   GivenDispensesSinceFilterExpiredIs(10);
   GivenWaterFilterUsageSinceExpirationUpdaterIsInitialized();

   WhenTotalWaterVolumeUsageInOuncesx100Is(0);
   TheDispensesSinceFilterExpiredShouldBe(11);

   WhenCalendarUsageInSecondsIs(0);
   TheDaysSinceFilterExpiredShouldBe(0);

   WhenWaterFilterLifeStatusIs(WaterFilterLifeStatus_NoOrderNecessary);
   WaterFilterUsageSinceExpirationShouldBeReset();
}

TEST(WaterFilterUsageSinceExpirationUpdater, ShouldIncreaseDispensesSinceFilterExpiredWhenTotalWaterVolumeUsageIsChangedWhileWaterFilterLifeStatusIsExpired)
{
   GivenWaterFilterLifeStatusIs(WaterFilterLifeStatus_Expired);
   GivenTotalWaterVolumeUsageInOuncesx100Is(SomeTotalWaterVolumeUsageInOuncesx100);
   GivenDispensesSinceFilterExpiredIs(10);
   GivenWaterFilterUsageSinceExpirationUpdaterIsInitialized();

   WhenTotalWaterVolumeUsageInOuncesx100Is(IncreasedTotalWaterVolumeUsageInOuncesx100);
   TheDispensesSinceFilterExpiredShouldBe(11);
}

TEST(WaterFilterUsageSinceExpirationUpdater, ShouldNotIncreaseDispensesSinceFilterExpiredWhenTotalWaterVolumeUsageIsChangedWhileWaterFilterUsageSinceExpirationHasBeenReset)
{
   GivenWaterFilterLifeStatusIs(WaterFilterLifeStatus_Expired);
   GivenCalendarUsageInSecondsIs(SomeCalendarUsageSinceExpirationInSeconds);
   GivenTotalWaterVolumeUsageInOuncesx100Is(SomeTotalWaterVolumeUsageInOuncesx100);
   GivenWaterFilterUsageSinceExpirationUpdaterIsInitialized();
   GivenWaterFilterLifeStatusIs(WaterFilterLifeStatus_NoOrderNecessary);
   TheDispensesSinceFilterExpiredShouldBe(0);

   WhenTotalWaterVolumeUsageInOuncesx100Is(NewTotalWaterVolumeUsageInOuncesx100);
   TheDispensesSinceFilterExpiredShouldBe(0);
}
