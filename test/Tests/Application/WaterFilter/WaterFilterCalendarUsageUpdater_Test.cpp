/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "WaterFilterCalendarUsageUpdater.h"
#include "DataModelErdPointerAccess.h"
#include "Signal.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "uassert_test.h"
#include "ReferDataModel_TestDouble.h"
#include "TimeSource_TestDouble.h"
#include "Constants_Time.h"
#include "Constants_Binary.h"

enum
{
   WaterFilterCalendarUsageUpdateIntervalInMinutes = 1,
   WaterFilterCalendarUsageUpdateIntervalInSeconds = (WaterFilterCalendarUsageUpdateIntervalInMinutes * SECONDS_PER_MINUTE),
   WaterFilterCalendarUsageUpdateIntervalInMsec = (WaterFilterCalendarUsageUpdateIntervalInMinutes * MSEC_PER_MIN),
   OneDayInMsec = MINUTES_PER_DAY * MSEC_PER_MIN,
   OneDayInSeconds = SECONDS_PER_DAY,
   OneMinuteInMsec = MSEC_PER_MIN
};

static const WaterFilterCalendarUsageUpdaterConfig_t config = {
   .waterFilterCalendarUsageInSecondsErd = Erd_WaterFilterCalendarUsageInSeconds,
   .lifetimeTotalWaterUsageInOuncesX100Erd = Erd_UnitLifetimeWaterVolumeUsageInOuncesX100,
   .serviceDiagnosticsRunTimeInMinutesErd = Erd_ServiceDiagnosticsRunTimeInMinutes
};

static const CommonFilterData_t commonFilterData = {
   .filterRatedVolumeInOuncesX100 = 50000,
   .filterRatedLifeInMinutes = 6000,
   .minimumVolumeNeededToStartFilterLifeTimerInOunces = 100,
   .waterFilterCalendarUsageDelayForLowUsageInMinutes = 129600
};

TEST_GROUP(WaterFilterCalendarUsageUpdater)
{
   WaterFilterCalendarUsageUpdater_t instance;

   ReferDataModel_TestDouble_t referDataModelTestDouble;
   I_DataModel_t *dataModel;

   TimerModule_TestDouble_t *timerModuleTestDouble;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&referDataModelTestDouble);
      dataModel = referDataModelTestDouble.dataModel;

      timerModuleTestDouble = (TimerModule_TestDouble_t *)DataModelErdPointerAccess_GetTimerModule(dataModel, Erd_TimerModule);
   }

   void GivenWaterFilterCalendarUsageUpdaterIsInitialized()
   {
      WaterFilterCalendarUsageUpdater_Init(
         &instance,
         dataModel,
         &config,
         &commonFilterData);
   }

   void GivenLifetimeTotalWaterVolumeUsageInOzX100ErdIs(VolumeInOuncesX100_t totalLifetimeWaterVolumeUsageInOzX100)
   {
      DataModel_Write(
         dataModel,
         Erd_UnitLifetimeWaterVolumeUsageInOuncesX100,
         &totalLifetimeWaterVolumeUsageInOzX100);
   }

   void GivenWaterFilterCalendarUsageInSecondsErdIs(CalendarUsageInSeconds_t waterFilterCalendarUsageInSeconds)
   {
      DataModel_Write(
         dataModel,
         Erd_WaterFilterCalendarUsageInSeconds,
         &waterFilterCalendarUsageInSeconds);
   }

   void GivenServiceDiagnosticsRunTimeInMinutesErdIs(ApplianceRunTimeMinutes_t serviceDiagnosticsRunTimeInMinutes)
   {
      DataModel_Write(
         dataModel,
         Erd_ServiceDiagnosticsRunTimeInMinutes,
         &serviceDiagnosticsRunTimeInMinutes);
   }

   void WhenLifetimeTotalWaterVolumeUsageInOzX100ErdIs(VolumeInOuncesX100_t totalLifetimeWaterVolumeUsageInOzX100)
   {
      DataModel_Write(
         dataModel,
         Erd_UnitLifetimeWaterVolumeUsageInOuncesX100,
         &totalLifetimeWaterVolumeUsageInOzX100);
   }

   void WhenServiceDiagnosticsRunTimeInMinutesErdIs(ApplianceRunTimeMinutes_t serviceDiagnosticsRunTimeInMinutes)
   {
      DataModel_Write(
         dataModel,
         Erd_ServiceDiagnosticsRunTimeInMinutes,
         &serviceDiagnosticsRunTimeInMinutes);
   }

   void WaterFilterCalendarUsageInSecondsErdShouldBe(CalendarUsageInSeconds_t expected)
   {
      CalendarUsageInSeconds_t actual;
      DataModel_Read(
         dataModel,
         Erd_WaterFilterCalendarUsageInSeconds,
         &actual);

      CHECK_EQUAL(expected, actual);
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks);
   }

   void AfterMaximumTimeBeforeCalendarUsageDelayHasPassed()
   {
      uint64_t waterFilterCalendarUsageDelayInMsec = (uint64_t)commonFilterData.waterFilterCalendarUsageDelayForLowUsageInMinutes * MSEC_PER_MIN;

      After(waterFilterCalendarUsageDelayInMsec / 2);
      After(waterFilterCalendarUsageDelayInMsec / 2);
   }
};

TEST(WaterFilterCalendarUsageUpdater, ShouldUpdateWaterFilterCalendarUsageInSecondsErdEveryWaterFilterCalendarUsageUpdateIntervalAfterMinimumVolumeConditionHasBeenMet)
{
   GivenWaterFilterCalendarUsageUpdaterIsInitialized();

   WhenLifetimeTotalWaterVolumeUsageInOzX100ErdIs((commonFilterData.minimumVolumeNeededToStartFilterLifeTimerInOunces * 100) + 1);
   After(WaterFilterCalendarUsageUpdateIntervalInMsec - 1);
   WaterFilterCalendarUsageInSecondsErdShouldBe(0);

   After(1);
   WaterFilterCalendarUsageInSecondsErdShouldBe(WaterFilterCalendarUsageUpdateIntervalInSeconds);
}

TEST(WaterFilterCalendarUsageUpdater, ShouldUpdateWaterFilterCalendarUsageInSecondsErdEveryWaterFilterCalendarUsageUpdateIntervalAfterLowCalendarUsageDelayHasPassed)
{
   GivenWaterFilterCalendarUsageUpdaterIsInitialized();

   WhenServiceDiagnosticsRunTimeInMinutesErdIs(commonFilterData.waterFilterCalendarUsageDelayForLowUsageInMinutes + 1);
   After(WaterFilterCalendarUsageUpdateIntervalInMsec - 1);
   WaterFilterCalendarUsageInSecondsErdShouldBe(0);

   After(1);
   WaterFilterCalendarUsageInSecondsErdShouldBe(WaterFilterCalendarUsageUpdateIntervalInSeconds);
}

TEST(WaterFilterCalendarUsageUpdater, ShouldNotUpdateWaterFilterCalendarUsageInSecondsErdEveryWaterFilterCalendarUsageUpdateIntervalUntilMinimumVolumeConditionHasBeenMet)
{
   GivenWaterFilterCalendarUsageUpdaterIsInitialized();

   WhenLifetimeTotalWaterVolumeUsageInOzX100ErdIs(commonFilterData.minimumVolumeNeededToStartFilterLifeTimerInOunces * 100);
   AfterMaximumTimeBeforeCalendarUsageDelayHasPassed();
   WaterFilterCalendarUsageInSecondsErdShouldBe(0);

   WhenLifetimeTotalWaterVolumeUsageInOzX100ErdIs((commonFilterData.minimumVolumeNeededToStartFilterLifeTimerInOunces * 100) + 1);
   After(WaterFilterCalendarUsageUpdateIntervalInMsec - 1);
   WaterFilterCalendarUsageInSecondsErdShouldBe(0);

   After(1);
   WaterFilterCalendarUsageInSecondsErdShouldBe(WaterFilterCalendarUsageUpdateIntervalInSeconds);
}

TEST(WaterFilterCalendarUsageUpdater, ShouldNotUpdateWaterFilterCalendarUsageInSecondsErdEveryWaterFilterCalendarUsageUpdateIntervalUntilLowCalendarUsageDelayHasPassed)
{
   GivenWaterFilterCalendarUsageUpdaterIsInitialized();

   AfterMaximumTimeBeforeCalendarUsageDelayHasPassed();
   WaterFilterCalendarUsageInSecondsErdShouldBe(0);

   After(OneMinuteInMsec);
   WaterFilterCalendarUsageInSecondsErdShouldBe(0);

   After(WaterFilterCalendarUsageUpdateIntervalInMsec - 1);
   WaterFilterCalendarUsageInSecondsErdShouldBe(0);

   After(1);
   WaterFilterCalendarUsageInSecondsErdShouldBe(WaterFilterCalendarUsageUpdateIntervalInSeconds);
}

TEST(WaterFilterCalendarUsageUpdater, ShouldUpdateWaterFilterCalendarUsageInSecondsErdEveryWaterFilterCalendarUsageUpdateIntervalOnInitAfterMinimumVolumeConditionHasBeenMetOnInit)
{
   WhenLifetimeTotalWaterVolumeUsageInOzX100ErdIs((commonFilterData.minimumVolumeNeededToStartFilterLifeTimerInOunces * 100) + 1);

   GivenWaterFilterCalendarUsageUpdaterIsInitialized();
   After(WaterFilterCalendarUsageUpdateIntervalInMsec - 1);
   WaterFilterCalendarUsageInSecondsErdShouldBe(0);

   After(1);
   WaterFilterCalendarUsageInSecondsErdShouldBe(WaterFilterCalendarUsageUpdateIntervalInSeconds);
}

TEST(WaterFilterCalendarUsageUpdater, ShouldUpdateWaterFilterCalendarUsageInSecondsErdEveryWaterFilterCalendarUsageUpdateIntervalOnInitAfterLowCalendarUsageDelayHasPassedOnInit)
{
   GivenWaterFilterCalendarUsageInSecondsErdIs(0);

   WhenServiceDiagnosticsRunTimeInMinutesErdIs(commonFilterData.waterFilterCalendarUsageDelayForLowUsageInMinutes + 1);

   GivenWaterFilterCalendarUsageUpdaterIsInitialized();
   After(WaterFilterCalendarUsageUpdateIntervalInMsec - 1);
   WaterFilterCalendarUsageInSecondsErdShouldBe(0);

   After(1);
   WaterFilterCalendarUsageInSecondsErdShouldBe(WaterFilterCalendarUsageUpdateIntervalInSeconds);
}

TEST(WaterFilterCalendarUsageUpdater, ShouldUnsubscribeAllAndNotRestartWaterFilterCalendarUsageTimerAfterMinimumVolumeConditionHasBeenMetOnce)
{
   GivenWaterFilterCalendarUsageUpdaterIsInitialized();

   WhenLifetimeTotalWaterVolumeUsageInOzX100ErdIs((commonFilterData.minimumVolumeNeededToStartFilterLifeTimerInOunces * 100) + 1);
   After(WaterFilterCalendarUsageUpdateIntervalInMsec - 1);
   WaterFilterCalendarUsageInSecondsErdShouldBe(0);

   WhenLifetimeTotalWaterVolumeUsageInOzX100ErdIs((commonFilterData.minimumVolumeNeededToStartFilterLifeTimerInOunces * 100) + 2);
   WaterFilterCalendarUsageInSecondsErdShouldBe(0);

   After(1);
   WaterFilterCalendarUsageInSecondsErdShouldBe(WaterFilterCalendarUsageUpdateIntervalInSeconds);
}

TEST(WaterFilterCalendarUsageUpdater, ShouldUnsubscribeAllAndNotRestartWaterFilterCalendarUsageTimerAfterLowCalendarUsageDelayHasPassedOnce)
{
   GivenWaterFilterCalendarUsageUpdaterIsInitialized();

   WhenServiceDiagnosticsRunTimeInMinutesErdIs(commonFilterData.waterFilterCalendarUsageDelayForLowUsageInMinutes + 1);
   After(WaterFilterCalendarUsageUpdateIntervalInMsec - 1);
   WaterFilterCalendarUsageInSecondsErdShouldBe(0);

   WhenServiceDiagnosticsRunTimeInMinutesErdIs(commonFilterData.waterFilterCalendarUsageDelayForLowUsageInMinutes + 2);
   WaterFilterCalendarUsageInSecondsErdShouldBe(0);

   After(1);
   WaterFilterCalendarUsageInSecondsErdShouldBe(WaterFilterCalendarUsageUpdateIntervalInSeconds);
}

TEST(WaterFilterCalendarUsageUpdater, ShouldIncrementWaterFilterCalendarUsageTimerWaterFilterCalendarUsageUpdateIntervalForADayAfterMinimumVolumeConditionHasBeenMet)
{
   GivenWaterFilterCalendarUsageUpdaterIsInitialized();

   WhenLifetimeTotalWaterVolumeUsageInOzX100ErdIs((commonFilterData.minimumVolumeNeededToStartFilterLifeTimerInOunces * 100) + 1);
   WaterFilterCalendarUsageInSecondsErdShouldBe(0);

   After(OneDayInMsec);
   WaterFilterCalendarUsageInSecondsErdShouldBe(OneDayInSeconds);
}

TEST(WaterFilterCalendarUsageUpdater, ShouldIncrementWaterFilterCalendarUsageTimerEveryWaterFilterCalendarUsageUpdateIntervalForADayAfterLowCalendarUsageDelayHasPassed)
{
   GivenWaterFilterCalendarUsageUpdaterIsInitialized();

   WhenServiceDiagnosticsRunTimeInMinutesErdIs(commonFilterData.waterFilterCalendarUsageDelayForLowUsageInMinutes + 1);
   WaterFilterCalendarUsageInSecondsErdShouldBe(0);

   After(OneDayInMsec);
   WaterFilterCalendarUsageInSecondsErdShouldBe(OneDayInSeconds);
}
