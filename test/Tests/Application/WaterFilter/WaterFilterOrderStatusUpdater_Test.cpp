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
   .totalWaterVolumeUsageInOuncesX100Erd = Erd_TotalWaterVolumeUsageInOuncesX100,
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

   void GivenTotalWaterVolumeUsageInOuncesX100Is(uint32_t ouncesX100)
   {
      DataModel_Write(dataModel, Erd_TotalWaterVolumeUsageInOuncesX100, &ouncesX100);
   }

   void WhenTotalWaterVolumeUsageInOuncesX100Is(uint32_t ouncesX100)
   {
      GivenTotalWaterVolumeUsageInOuncesX100Is(ouncesX100);
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

TEST(WaterFilterLifeStatusUpdater, ShouldSetWaterFilterLifeStatusToExpiredIfTotalWaterVolumeUsageExceedsParametricLimit)
{
   GivenTotalWaterVolumeUsageInOuncesX100Is(data.filterRatedVolumeInOuncesX100 + 1);
   GivenWaterFilterCalendarUsageInSecondsIs(0);
   GivenModuleIsInitialized();

   WaterFilterLifeStatusShouldBe(WaterFilterLifeStatus_Expired);
}

TEST(WaterFilterLifeStatusUpdater, ShouldSetWaterFilterLifeStatusToExpiredIfTotalWaterVolumeUsageMeetsParametricLimit)
{
   GivenTotalWaterVolumeUsageInOuncesX100Is(data.filterRatedVolumeInOuncesX100);
   GivenWaterFilterCalendarUsageInSecondsIs(0);
   GivenModuleIsInitialized();

   WaterFilterLifeStatusShouldBe(WaterFilterLifeStatus_Expired);
}

TEST(WaterFilterLifeStatusUpdater, ShouldSetWaterFilterLifeStatusToExpiredIfWaterFilterCalendarUsageExceedsParametricLimit)
{
   GivenTotalWaterVolumeUsageInOuncesX100Is(0);
   GivenWaterFilterCalendarUsageInSecondsIs((data.filterRatedLifeInMinutes * SECONDS_PER_MINUTE) + 1);
   GivenModuleIsInitialized();

   WaterFilterLifeStatusShouldBe(WaterFilterLifeStatus_Expired);
}

TEST(WaterFilterLifeStatusUpdater, ShouldSetWaterFilterLifeStatusToExpiredIfWaterFilterCalendarUsageMeetsParametricLimit)
{
   GivenTotalWaterVolumeUsageInOuncesX100Is(0);
   GivenWaterFilterCalendarUsageInSecondsIs((data.filterRatedLifeInMinutes * SECONDS_PER_MINUTE));
   GivenModuleIsInitialized();

   WaterFilterLifeStatusShouldBe(WaterFilterLifeStatus_Expired);
}

TEST(WaterFilterLifeStatusUpdater, ShouldSetWaterFilterLifeStatusToReplaceIfTotalWaterVolumeUsageIsBetweenParametricLimitAndNinetyFivePercentOfParametricLimit)
{
   GivenTotalWaterVolumeUsageInOuncesX100Is((data.filterRatedVolumeInOuncesX100 * ReplacementVolumeUsagePercent) / 100 + 1);
   GivenWaterFilterCalendarUsageInSecondsIs(0);
   GivenModuleIsInitialized();

   WaterFilterLifeStatusShouldBe(WaterFilterLifeStatus_Replace);
}

TEST(WaterFilterLifeStatusUpdater, ShouldSetWaterFilterLifeStatusToReplaceIfWaterFilterCalendarUsageIsBetweenParametricLimitAndNinetyFivePercentOfParametricLimit)
{
   GivenTotalWaterVolumeUsageInOuncesX100Is(0);
   GivenWaterFilterCalendarUsageInSecondsIs((ReplacementCalendarUsagePercent * data.filterRatedLifeInMinutes * SECONDS_PER_MINUTE) / 100 + 1);
   GivenModuleIsInitialized();

   WaterFilterLifeStatusShouldBe(WaterFilterLifeStatus_Replace);
}

TEST(WaterFilterLifeStatusUpdater, ShouldSetWaterFilterLifeStatusToNoOrderNecessaryIfBothTotalWaterVolumeUsageAndWaterFilterCalendarUsageAreBelowNinetyFivePercentOfTheirParametricLimits)
{
   GivenTotalWaterVolumeUsageInOuncesX100Is((data.filterRatedVolumeInOuncesX100 * ReplacementVolumeUsagePercent) / 100 - 1);
   GivenWaterFilterCalendarUsageInSecondsIs((ReplacementCalendarUsagePercent * data.filterRatedLifeInMinutes * SECONDS_PER_MINUTE) / 100 - 1);
   GivenModuleIsInitialized();

   WaterFilterLifeStatusShouldBe(WaterFilterLifeStatus_NoOrderNecessary);
}

TEST(WaterFilterLifeStatusUpdater, ShouldSetWaterFilterLifeStatusToExpiredWhenTotalWaterVolumeUsageExceedsParametricLimit)
{
   GivenTotalWaterVolumeUsageInOuncesX100Is((data.filterRatedVolumeInOuncesX100 * ReplacementVolumeUsagePercent) / 100 - 1);
   GivenWaterFilterCalendarUsageInSecondsIs(0);
   GivenModuleIsInitialized();
   WaterFilterLifeStatusShouldBe(WaterFilterLifeStatus_NoOrderNecessary);

   WhenTotalWaterVolumeUsageInOuncesX100Is(data.filterRatedVolumeInOuncesX100 + 1);
   WaterFilterLifeStatusShouldBe(WaterFilterLifeStatus_Expired);
}

TEST(WaterFilterLifeStatusUpdater, ShouldSetWaterFilterLifeStatusToExpiredWhenTotalWaterVolumeUsageMeetsParametricLimit)
{
   GivenTotalWaterVolumeUsageInOuncesX100Is((data.filterRatedVolumeInOuncesX100 * ReplacementVolumeUsagePercent) / 100 - 1);
   GivenWaterFilterCalendarUsageInSecondsIs(0);
   GivenModuleIsInitialized();
   WaterFilterLifeStatusShouldBe(WaterFilterLifeStatus_NoOrderNecessary);

   WhenTotalWaterVolumeUsageInOuncesX100Is(data.filterRatedVolumeInOuncesX100);
   WaterFilterLifeStatusShouldBe(WaterFilterLifeStatus_Expired);
}

TEST(WaterFilterLifeStatusUpdater, ShouldSetWaterFilterLifeStatusToExpiredWhenWaterFilterCalendarUsageInSecondsExceedsParametricLimit)
{
   GivenTotalWaterVolumeUsageInOuncesX100Is(0);
   GivenWaterFilterCalendarUsageInSecondsIs((ReplacementCalendarUsagePercent * data.filterRatedLifeInMinutes * SECONDS_PER_MINUTE) / 100 - 1);
   GivenModuleIsInitialized();
   WaterFilterLifeStatusShouldBe(WaterFilterLifeStatus_NoOrderNecessary);

   WhenWaterFilterCalendarUsageInSecondsIs((data.filterRatedLifeInMinutes * SECONDS_PER_MINUTE) + 1);
   WaterFilterLifeStatusShouldBe(WaterFilterLifeStatus_Expired);
}

TEST(WaterFilterLifeStatusUpdater, ShouldSetWaterFilterLifeStatusToExpiredWhenWaterFilterCalendarUsageInSecondsMeetsParametricLimit)
{
   GivenTotalWaterVolumeUsageInOuncesX100Is(0);
   GivenWaterFilterCalendarUsageInSecondsIs((ReplacementCalendarUsagePercent * data.filterRatedLifeInMinutes * SECONDS_PER_MINUTE) / 100 - 1);
   GivenModuleIsInitialized();
   WaterFilterLifeStatusShouldBe(WaterFilterLifeStatus_NoOrderNecessary);

   WhenWaterFilterCalendarUsageInSecondsIs((data.filterRatedLifeInMinutes * SECONDS_PER_MINUTE));
   WaterFilterLifeStatusShouldBe(WaterFilterLifeStatus_Expired);
}

TEST(WaterFilterLifeStatusUpdater, ShouldSetWaterFilterLifeStatusToReplaceWhenTotalWaterVolumeUsageChangesToBetweenParametricLimitAndNinetyFivePercentOfParametricLimit)
{
   GivenTotalWaterVolumeUsageInOuncesX100Is((data.filterRatedVolumeInOuncesX100 * ReplacementVolumeUsagePercent) / 100 - 1);
   GivenWaterFilterCalendarUsageInSecondsIs(0);
   GivenModuleIsInitialized();
   WaterFilterLifeStatusShouldBe(WaterFilterLifeStatus_NoOrderNecessary);

   WhenTotalWaterVolumeUsageInOuncesX100Is((data.filterRatedVolumeInOuncesX100 * ReplacementVolumeUsagePercent) / 100 + 1);
   WaterFilterLifeStatusShouldBe(WaterFilterLifeStatus_Replace);
}

TEST(WaterFilterLifeStatusUpdater, ShouldSetWaterFilterLifeStatusToReplaceWhenWaterFilterCalendarUsageInSecondsChangesToBetweenParametricLimitAndNinetyFivePercentOfParametricLimit)
{
   GivenTotalWaterVolumeUsageInOuncesX100Is(0);
   GivenWaterFilterCalendarUsageInSecondsIs((ReplacementCalendarUsagePercent * data.filterRatedLifeInMinutes * SECONDS_PER_MINUTE) / 100 - 1);
   GivenModuleIsInitialized();
   WaterFilterLifeStatusShouldBe(WaterFilterLifeStatus_NoOrderNecessary);

   WhenWaterFilterCalendarUsageInSecondsIs((ReplacementCalendarUsagePercent * data.filterRatedLifeInMinutes * SECONDS_PER_MINUTE) / 100 + 1);
   WaterFilterLifeStatusShouldBe(WaterFilterLifeStatus_Replace);
}

TEST(WaterFilterLifeStatusUpdater, ShouldSetWaterFilterLifeStatusToNoOrderNecessaryWhenTotalWaterVolumeUsageDecreasesBelowNinetyFivePercentOfParametricLimit)
{
   GivenTotalWaterVolumeUsageInOuncesX100Is(data.filterRatedVolumeInOuncesX100 + 1);
   GivenWaterFilterCalendarUsageInSecondsIs(0);
   GivenModuleIsInitialized();
   WaterFilterLifeStatusShouldBe(WaterFilterLifeStatus_Expired);

   WhenTotalWaterVolumeUsageInOuncesX100Is((data.filterRatedVolumeInOuncesX100 * ReplacementVolumeUsagePercent) / 100 - 1);
   WaterFilterLifeStatusShouldBe(WaterFilterLifeStatus_NoOrderNecessary);
}

TEST(WaterFilterLifeStatusUpdater, ShouldSetWaterFilterLifeStatusToNoOrderNecessaryWhenWaterFilterCalendarUsageInSecondsDecreasesBelowNinetyFivePercentOfParametricLimit)
{
   GivenTotalWaterVolumeUsageInOuncesX100Is(0);
   GivenWaterFilterCalendarUsageInSecondsIs(data.filterRatedLifeInMinutes * SECONDS_PER_MINUTE);
   GivenModuleIsInitialized();
   WaterFilterLifeStatusShouldBe(WaterFilterLifeStatus_Expired);

   WhenWaterFilterCalendarUsageInSecondsIs((ReplacementCalendarUsagePercent * data.filterRatedLifeInMinutes * SECONDS_PER_MINUTE) / 100 - 1);
   WaterFilterLifeStatusShouldBe(WaterFilterLifeStatus_NoOrderNecessary);
}
