/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "WaterFilterLifeStatusUpdater.h"
#include "Constants_Time.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

enum
{
   ReplacementCalendarUsagePercent = 95,
   ReplacementVolumeUsagePercent = 95
};

static const CommonFilterData_t data = {
   .filterRatedVolumeInOuncesX100 = 50000,
   .filterRatedLifeInMinutes = 6000,
   .minimumVolumeNeededToStartFilterLifeTimerInOunces = 100,
   .waterFilterCalendarUsageDelayForLowUsageInMinutes = 129600
};

static const WaterFilterLifeStatusUpdaterConfig_t config = {
   .waterFilterVolumeUsageInOuncesX100Erd = Erd_WaterFilterVolumeUsageInOuncesX100,
   .filterCalendarUsageInSecondsErd = Erd_WaterFilterCalendarUsageInSeconds,
   .waterFilterLifeStatusErd = Erd_WaterFilterLifeStatus
};

TEST_GROUP(WaterFilterLifeStatusUpdater)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   WaterFilterLifeStatusUpdater_t instance;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
   }

   void GivenModuleIsInitialized()
   {
      WaterFilterLifeStatusUpdater_Init(
         &instance,
         dataModel,
         &config,
         &data);
   }

   void GivenWaterFilterVolumeUsageInOuncesX100Is(VolumeInOuncesX100_t ouncesX100)
   {
      DataModel_Write(dataModel, Erd_WaterFilterVolumeUsageInOuncesX100, &ouncesX100);
   }

   void WhenWaterFilterVolumeUsageInOuncesX100Is(VolumeInOuncesX100_t ouncesX100)
   {
      GivenWaterFilterVolumeUsageInOuncesX100Is(ouncesX100);
   }

   void GivenWaterFilterCalendarUsageInSecondsIs(uint32_t seconds)
   {
      DataModel_Write(dataModel, Erd_WaterFilterCalendarUsageInSeconds, &seconds);
   }

   void WhenWaterFilterCalendarUsageInSecondsIs(uint32_t seconds)
   {
      GivenWaterFilterCalendarUsageInSecondsIs(seconds);
   }

   void WaterFilterLifeStatusShouldBe(WaterFilterLifeStatus_t expected)
   {
      WaterFilterLifeStatus_t actual;
      DataModel_Read(dataModel, Erd_WaterFilterLifeStatus, &actual);

      CHECK_EQUAL(expected, actual);
   }
};

TEST(WaterFilterLifeStatusUpdater, ShouldSetWaterFilterLifeStatusToExpiredIfWaterFilterVolumeUsageExceedsParametricLimit)
{
   GivenWaterFilterVolumeUsageInOuncesX100Is(data.filterRatedVolumeInOuncesX100 + 1);
   GivenWaterFilterCalendarUsageInSecondsIs(0);
   GivenModuleIsInitialized();

   WaterFilterLifeStatusShouldBe(WaterFilterLifeStatus_Expired);
}

TEST(WaterFilterLifeStatusUpdater, ShouldSetWaterFilterLifeStatusToExpiredIfWaterFilterVolumeUsageMeetsParametricLimit)
{
   GivenWaterFilterVolumeUsageInOuncesX100Is(data.filterRatedVolumeInOuncesX100);
   GivenWaterFilterCalendarUsageInSecondsIs(0);
   GivenModuleIsInitialized();

   WaterFilterLifeStatusShouldBe(WaterFilterLifeStatus_Expired);
}

TEST(WaterFilterLifeStatusUpdater, ShouldSetWaterFilterLifeStatusToExpiredIfWaterFilterCalendarUsageExceedsParametricLimit)
{
   GivenWaterFilterVolumeUsageInOuncesX100Is(0);
   GivenWaterFilterCalendarUsageInSecondsIs((data.filterRatedLifeInMinutes * SECONDS_PER_MINUTE) + 1);
   GivenModuleIsInitialized();

   WaterFilterLifeStatusShouldBe(WaterFilterLifeStatus_Expired);
}

TEST(WaterFilterLifeStatusUpdater, ShouldSetWaterFilterLifeStatusToExpiredIfWaterFilterCalendarUsageMeetsParametricLimit)
{
   GivenWaterFilterVolumeUsageInOuncesX100Is(0);
   GivenWaterFilterCalendarUsageInSecondsIs((data.filterRatedLifeInMinutes * SECONDS_PER_MINUTE));
   GivenModuleIsInitialized();

   WaterFilterLifeStatusShouldBe(WaterFilterLifeStatus_Expired);
}

TEST(WaterFilterLifeStatusUpdater, ShouldSetWaterFilterLifeStatusToReplaceIfWaterFilterVolumeUsageIsBetweenParametricLimitAndNinetyFivePercentOfParametricLimit)
{
   GivenWaterFilterVolumeUsageInOuncesX100Is((data.filterRatedVolumeInOuncesX100 * ReplacementVolumeUsagePercent) / 100 + 1);
   GivenWaterFilterCalendarUsageInSecondsIs(0);
   GivenModuleIsInitialized();

   WaterFilterLifeStatusShouldBe(WaterFilterLifeStatus_Replace);
}

TEST(WaterFilterLifeStatusUpdater, ShouldSetWaterFilterLifeStatusToReplaceIfWaterFilterCalendarUsageIsBetweenParametricLimitAndNinetyFivePercentOfParametricLimit)
{
   GivenWaterFilterVolumeUsageInOuncesX100Is(0);
   GivenWaterFilterCalendarUsageInSecondsIs((ReplacementCalendarUsagePercent * data.filterRatedLifeInMinutes * SECONDS_PER_MINUTE) / 100 + 1);
   GivenModuleIsInitialized();

   WaterFilterLifeStatusShouldBe(WaterFilterLifeStatus_Replace);
}

TEST(WaterFilterLifeStatusUpdater, ShouldSetWaterFilterLifeStatusToNoOrderNecessaryIfBothWaterFilterVolumeUsageAndWaterFilterCalendarUsageAreBelowNinetyFivePercentOfTheirParametricLimits)
{
   GivenWaterFilterVolumeUsageInOuncesX100Is((data.filterRatedVolumeInOuncesX100 * ReplacementVolumeUsagePercent) / 100 - 1);
   GivenWaterFilterCalendarUsageInSecondsIs((ReplacementCalendarUsagePercent * data.filterRatedLifeInMinutes * SECONDS_PER_MINUTE) / 100 - 1);
   GivenModuleIsInitialized();

   WaterFilterLifeStatusShouldBe(WaterFilterLifeStatus_NoOrderNecessary);
}

TEST(WaterFilterLifeStatusUpdater, ShouldSetWaterFilterLifeStatusToExpiredWhenWaterFilterVolumeUsageExceedsParametricLimit)
{
   GivenWaterFilterVolumeUsageInOuncesX100Is((data.filterRatedVolumeInOuncesX100 * ReplacementVolumeUsagePercent) / 100 - 1);
   GivenWaterFilterCalendarUsageInSecondsIs(0);
   GivenModuleIsInitialized();
   WaterFilterLifeStatusShouldBe(WaterFilterLifeStatus_NoOrderNecessary);

   WhenWaterFilterVolumeUsageInOuncesX100Is(data.filterRatedVolumeInOuncesX100 + 1);
   WaterFilterLifeStatusShouldBe(WaterFilterLifeStatus_Expired);
}

TEST(WaterFilterLifeStatusUpdater, ShouldSetWaterFilterLifeStatusToExpiredWhenWaterFilterVolumeUsageMeetsParametricLimit)
{
   GivenWaterFilterVolumeUsageInOuncesX100Is((data.filterRatedVolumeInOuncesX100 * ReplacementVolumeUsagePercent) / 100 - 1);
   GivenWaterFilterCalendarUsageInSecondsIs(0);
   GivenModuleIsInitialized();
   WaterFilterLifeStatusShouldBe(WaterFilterLifeStatus_NoOrderNecessary);

   WhenWaterFilterVolumeUsageInOuncesX100Is(data.filterRatedVolumeInOuncesX100);
   WaterFilterLifeStatusShouldBe(WaterFilterLifeStatus_Expired);
}

TEST(WaterFilterLifeStatusUpdater, ShouldSetWaterFilterLifeStatusToExpiredWhenWaterFilterCalendarUsageInSecondsExceedsParametricLimit)
{
   GivenWaterFilterVolumeUsageInOuncesX100Is(0);
   GivenWaterFilterCalendarUsageInSecondsIs((ReplacementCalendarUsagePercent * data.filterRatedLifeInMinutes * SECONDS_PER_MINUTE) / 100 - 1);
   GivenModuleIsInitialized();
   WaterFilterLifeStatusShouldBe(WaterFilterLifeStatus_NoOrderNecessary);

   WhenWaterFilterCalendarUsageInSecondsIs((data.filterRatedLifeInMinutes * SECONDS_PER_MINUTE) + 1);
   WaterFilterLifeStatusShouldBe(WaterFilterLifeStatus_Expired);
}

TEST(WaterFilterLifeStatusUpdater, ShouldSetWaterFilterLifeStatusToExpiredWhenWaterFilterCalendarUsageInSecondsMeetsParametricLimit)
{
   GivenWaterFilterVolumeUsageInOuncesX100Is(0);
   GivenWaterFilterCalendarUsageInSecondsIs((ReplacementCalendarUsagePercent * data.filterRatedLifeInMinutes * SECONDS_PER_MINUTE) / 100 - 1);
   GivenModuleIsInitialized();
   WaterFilterLifeStatusShouldBe(WaterFilterLifeStatus_NoOrderNecessary);

   WhenWaterFilterCalendarUsageInSecondsIs((data.filterRatedLifeInMinutes * SECONDS_PER_MINUTE));
   WaterFilterLifeStatusShouldBe(WaterFilterLifeStatus_Expired);
}

TEST(WaterFilterLifeStatusUpdater, ShouldSetWaterFilterLifeStatusToReplaceWhenWaterFilterVolumeUsageChangesToBetweenParametricLimitAndNinetyFivePercentOfParametricLimit)
{
   GivenWaterFilterVolumeUsageInOuncesX100Is((data.filterRatedVolumeInOuncesX100 * ReplacementVolumeUsagePercent) / 100 - 1);
   GivenWaterFilterCalendarUsageInSecondsIs(0);
   GivenModuleIsInitialized();
   WaterFilterLifeStatusShouldBe(WaterFilterLifeStatus_NoOrderNecessary);

   WhenWaterFilterVolumeUsageInOuncesX100Is((data.filterRatedVolumeInOuncesX100 * ReplacementVolumeUsagePercent) / 100 + 1);
   WaterFilterLifeStatusShouldBe(WaterFilterLifeStatus_Replace);
}

TEST(WaterFilterLifeStatusUpdater, ShouldSetWaterFilterLifeStatusToReplaceWhenWaterFilterCalendarUsageInSecondsChangesToBetweenParametricLimitAndNinetyFivePercentOfParametricLimit)
{
   GivenWaterFilterVolumeUsageInOuncesX100Is(0);
   GivenWaterFilterCalendarUsageInSecondsIs((ReplacementCalendarUsagePercent * data.filterRatedLifeInMinutes * SECONDS_PER_MINUTE) / 100 - 1);
   GivenModuleIsInitialized();
   WaterFilterLifeStatusShouldBe(WaterFilterLifeStatus_NoOrderNecessary);

   WhenWaterFilterCalendarUsageInSecondsIs((ReplacementCalendarUsagePercent * data.filterRatedLifeInMinutes * SECONDS_PER_MINUTE) / 100 + 1);
   WaterFilterLifeStatusShouldBe(WaterFilterLifeStatus_Replace);
}

TEST(WaterFilterLifeStatusUpdater, ShouldSetWaterFilterLifeStatusToNoOrderNecessaryWhenWaterFilterVolumeUsageDecreasesBelowNinetyFivePercentOfParametricLimit)
{
   GivenWaterFilterVolumeUsageInOuncesX100Is(data.filterRatedVolumeInOuncesX100 + 1);
   GivenWaterFilterCalendarUsageInSecondsIs(0);
   GivenModuleIsInitialized();
   WaterFilterLifeStatusShouldBe(WaterFilterLifeStatus_Expired);

   WhenWaterFilterVolumeUsageInOuncesX100Is((data.filterRatedVolumeInOuncesX100 * ReplacementVolumeUsagePercent) / 100 - 1);
   WaterFilterLifeStatusShouldBe(WaterFilterLifeStatus_NoOrderNecessary);
}

TEST(WaterFilterLifeStatusUpdater, ShouldSetWaterFilterLifeStatusToNoOrderNecessaryWhenWaterFilterCalendarUsageInSecondsDecreasesBelowNinetyFivePercentOfParametricLimit)
{
   GivenWaterFilterVolumeUsageInOuncesX100Is(0);
   GivenWaterFilterCalendarUsageInSecondsIs(data.filterRatedLifeInMinutes * SECONDS_PER_MINUTE);
   GivenModuleIsInitialized();
   WaterFilterLifeStatusShouldBe(WaterFilterLifeStatus_Expired);

   WhenWaterFilterCalendarUsageInSecondsIs((ReplacementCalendarUsagePercent * data.filterRatedLifeInMinutes * SECONDS_PER_MINUTE) / 100 - 1);
   WaterFilterLifeStatusShouldBe(WaterFilterLifeStatus_NoOrderNecessary);
}
