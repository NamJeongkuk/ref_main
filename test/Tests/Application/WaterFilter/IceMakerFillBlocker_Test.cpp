/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "IceMakerFillBlocker.h"
#include "PersonalityParametricData.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

enum
{
   NonZeroVolume = 100,
   NewFilterDispenserPurgeVolumeInOuncesX100 = 1000,
   NewFilterFillStateDelayInMinutes = 50
};

const IceMakerFillBlockerConfig_t config = {
   .unitLifetimeDispensedWaterInOuncesX100Erd = Erd_UnitLifetimeDispensedWaterInOuncesX100,
   .waterFilterVolumeUsageInOuncesX100Erd = Erd_WaterFilterVolumeUsageInOuncesX100,
   .waterFilterCalendarUsageInSecondsErd = Erd_WaterFilterCalendarUsageInSeconds,
   .iceMakerFillInhibitedReasonErd = Erd_IceMakerFillInhibitedReason
};

const IceMakerFillBlockerData_t iceMakerFillBlockerData = {
   .newFilterDispenserPurgeVolumeInOuncesX100 = NewFilterDispenserPurgeVolumeInOuncesX100,
   .newFilterFillStateDelayInMinutes = NewFilterFillStateDelayInMinutes
};

TEST_GROUP(IceMakerFillBlocker)
{
   IceMakerFillBlocker_t instance;
   ReferDataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelTestDouble);
      dataModel = dataModelTestDouble.dataModel;
   }

   void GivenInitialization()
   {
      IceMakerFillBlocker_Init(
         &instance,
         dataModel,
         &config,
         &iceMakerFillBlockerData);
   }

   void WhenUnitLifetimeDispensedWaterInOuncesX100Is(VolumeInOuncesX100_t volume)
   {
      DataModel_Write(dataModel, Erd_UnitLifetimeDispensedWaterInOuncesX100, &volume);
   }

   void WhenWaterFilterVolumeUsageInOuncesX100Is(VolumeInOuncesX100_t waterVolumeUsageInOuncesX100)
   {
      DataModel_Write(dataModel, Erd_WaterFilterVolumeUsageInOuncesX100, &waterVolumeUsageInOuncesX100);
   }

   void WhenWaterFilterCalendarUsageInSecondsIs(CalendarUsageInSeconds_t calendarUsage)
   {
      DataModel_Write(dataModel, Erd_WaterFilterCalendarUsageInSeconds, &calendarUsage);
   }

   void GivenUnitLifetimeDispensedWaterInOuncesX100Is(VolumeInOuncesX100_t volume)
   {
      WhenUnitLifetimeDispensedWaterInOuncesX100Is(volume);
   }

   void GivenWaterFilterVolumeUsageInOuncesX100Is(VolumeInOuncesX100_t waterVolumeUsageInOuncesX100)
   {
      WhenWaterFilterVolumeUsageInOuncesX100Is(waterVolumeUsageInOuncesX100);
   }

   void GivenWaterFilterCalendarUsageInSecondsIs(CalendarUsageInSeconds_t calendarUsage)
   {
      WhenWaterFilterCalendarUsageInSecondsIs(calendarUsage);
   }

   void IceMakerInhibitedDueToNewFilterShouldBe(bool expected)
   {
      IceMakerFillInhibitedReasonBitmap_t iceMakerFillInhibitedReasonBitmap;
      DataModel_Read(dataModel, Erd_IceMakerFillInhibitedReason, &iceMakerFillInhibitedReasonBitmap);

      CHECK_EQUAL(expected, BITMAP_STATE(&iceMakerFillInhibitedReasonBitmap, IceMakerFillInhibitedReason_DueToNewFilter));
   }

   void GivenClearConditionsAreMetWithNonZeroDispensing()
   {
      GivenUnitLifetimeDispensedWaterInOuncesX100Is(NonZeroVolume);
      GivenWaterFilterVolumeUsageInOuncesX100Is(NewFilterDispenserPurgeVolumeInOuncesX100 + 1);
      GivenWaterFilterCalendarUsageInSecondsIs((NewFilterFillStateDelayInMinutes + 1) * SECONDS_PER_MINUTE);
      IceMakerInhibitedDueToNewFilterShouldBe(CLEAR);
   }
};

TEST(IceMakerFillBlocker, ShouldNotInhibitIceMakerFillOnInitWhenDispensedWaterVolumeIsZeroAndWaterFilterVolumeIsLessThanPurgeVolumeAndFilterCalendarUsageIsLessThanNewFilterFillStateDelay)
{
   GivenUnitLifetimeDispensedWaterInOuncesX100Is(0);
   GivenWaterFilterVolumeUsageInOuncesX100Is(NewFilterDispenserPurgeVolumeInOuncesX100 - 1);
   GivenWaterFilterCalendarUsageInSecondsIs((NewFilterFillStateDelayInMinutes - 1) * SECONDS_PER_MINUTE);
   GivenInitialization();
   IceMakerInhibitedDueToNewFilterShouldBe(CLEAR);
}

TEST(IceMakerFillBlocker, ShouldInhibitIceMakerFillOnInitWhileDispensedWaterVolumeIsNonZeroAndFilterWaterFilterVolumeIsLessThanPurgeVolumeAndCalendarUsageIsLessThanNewFilterFillStateDelay)
{
   GivenUnitLifetimeDispensedWaterInOuncesX100Is(NonZeroVolume);
   GivenWaterFilterVolumeUsageInOuncesX100Is(NewFilterDispenserPurgeVolumeInOuncesX100 - 1);
   GivenWaterFilterCalendarUsageInSecondsIs((NewFilterFillStateDelayInMinutes - 1) * SECONDS_PER_MINUTE);
   GivenInitialization();
   IceMakerInhibitedDueToNewFilterShouldBe(SET);
}

TEST(IceMakerFillBlocker, ShouldNotInhibitIceMakerFillOnInitWhileDispensedWaterVolumeIsNonZeroAndWaterFilterVolumeIsLessThanPurgeVolumeAndCalendarUsageIsGreaterThanNewFilterFillStateDelay)
{
   GivenUnitLifetimeDispensedWaterInOuncesX100Is(NonZeroVolume);
   GivenWaterFilterVolumeUsageInOuncesX100Is(NewFilterDispenserPurgeVolumeInOuncesX100 - 1);
   GivenWaterFilterCalendarUsageInSecondsIs((NewFilterFillStateDelayInMinutes + 1) * SECONDS_PER_MINUTE);
   GivenInitialization();
   IceMakerInhibitedDueToNewFilterShouldBe(CLEAR);
}

TEST(IceMakerFillBlocker, ShouldNotInhibitIceMakerFillOnInitWhileDispensedWaterVolumeIsNonZeroWaterFilterVolumeUsageIsGreaterThanPurgeVolumeAndCalendarUsageIsLessThanNewFilterFillStateDelay)
{
   GivenUnitLifetimeDispensedWaterInOuncesX100Is(NonZeroVolume);
   GivenWaterFilterVolumeUsageInOuncesX100Is(NewFilterDispenserPurgeVolumeInOuncesX100 + 1);
   GivenWaterFilterCalendarUsageInSecondsIs((NewFilterFillStateDelayInMinutes - 1) * SECONDS_PER_MINUTE);
   GivenInitialization();
   IceMakerInhibitedDueToNewFilterShouldBe(CLEAR);
}

TEST(IceMakerFillBlocker, ShouldNotInhibitIceMakerFillWhenDispensedWaterVolumeBecomesZero)
{
   GivenClearConditionsAreMetWithNonZeroDispensing();
   GivenInitialization();
   IceMakerInhibitedDueToNewFilterShouldBe(CLEAR);

   WhenUnitLifetimeDispensedWaterInOuncesX100Is(0);
   IceMakerInhibitedDueToNewFilterShouldBe(CLEAR);
}

TEST(IceMakerFillBlocker, ShouldInhibitIceMakerFillWhenWaterFilterVolumeUsageBecomesLessThanPurgeVolumeWhileCalendarUsageIsLessThanDelayInMinutes)
{
   GivenUnitLifetimeDispensedWaterInOuncesX100Is(NonZeroVolume);
   GivenWaterFilterVolumeUsageInOuncesX100Is(NewFilterDispenserPurgeVolumeInOuncesX100 + 1);
   GivenWaterFilterCalendarUsageInSecondsIs((NewFilterFillStateDelayInMinutes - 1) * SECONDS_PER_MINUTE);
   GivenInitialization();
   IceMakerInhibitedDueToNewFilterShouldBe(CLEAR);

   WhenWaterFilterVolumeUsageInOuncesX100Is(NewFilterDispenserPurgeVolumeInOuncesX100 - 1);
   IceMakerInhibitedDueToNewFilterShouldBe(SET);
}

TEST(IceMakerFillBlocker, ShouldInhibitIceMakerFillWhenFilterCalendarUsageBecomesLessThanNewFilterFillStateDelayWhileWaterFilterVolumeIsLessThanPurgeVolume)
{
   GivenUnitLifetimeDispensedWaterInOuncesX100Is(NonZeroVolume);
   GivenWaterFilterVolumeUsageInOuncesX100Is(NewFilterDispenserPurgeVolumeInOuncesX100 - 1);
   GivenWaterFilterCalendarUsageInSecondsIs((NewFilterFillStateDelayInMinutes + 1) * SECONDS_PER_MINUTE);
   GivenInitialization();
   IceMakerInhibitedDueToNewFilterShouldBe(CLEAR);

   WhenWaterFilterCalendarUsageInSecondsIs((NewFilterFillStateDelayInMinutes - 1) * SECONDS_PER_MINUTE);
   IceMakerInhibitedDueToNewFilterShouldBe(SET);
}

TEST(IceMakerFillBlocker, ShouldUninhibitIceMakerFillWhenDispensedWaterVolumeBecomesZeroWhileWaterFilterVolumeIsLessThanPurgeVolumeAndFilterCalendarUsageIsLessThanNewFilterFillStateDelay)
{
   GivenUnitLifetimeDispensedWaterInOuncesX100Is(NonZeroVolume);
   GivenWaterFilterVolumeUsageInOuncesX100Is(NewFilterDispenserPurgeVolumeInOuncesX100 - 1);
   GivenWaterFilterCalendarUsageInSecondsIs((NewFilterFillStateDelayInMinutes - 1) * SECONDS_PER_MINUTE);
   GivenInitialization();
   IceMakerInhibitedDueToNewFilterShouldBe(SET);

   WhenUnitLifetimeDispensedWaterInOuncesX100Is(0);
   IceMakerInhibitedDueToNewFilterShouldBe(CLEAR);
}

TEST(IceMakerFillBlocker, ShouldUninhibitIceMakerFillWhenWaterVolumeUsageBecomesGreaterThanPurgeVolumeWhileCalendarUsageIsLessThanNewFilterFillStateDelayTime)
{
   GivenUnitLifetimeDispensedWaterInOuncesX100Is(NonZeroVolume);
   GivenWaterFilterVolumeUsageInOuncesX100Is(NewFilterDispenserPurgeVolumeInOuncesX100 - 1);
   GivenWaterFilterCalendarUsageInSecondsIs((NewFilterFillStateDelayInMinutes - 1) * SECONDS_PER_MINUTE);
   GivenInitialization();
   IceMakerInhibitedDueToNewFilterShouldBe(SET);

   WhenWaterFilterVolumeUsageInOuncesX100Is(NewFilterDispenserPurgeVolumeInOuncesX100 + 1);
   IceMakerInhibitedDueToNewFilterShouldBe(CLEAR);
}

TEST(IceMakerFillBlocker, ShouldUninhibitIceMakerFillWhenCalendarUsageBecomesGreaterThanNewFilterFillStateDelayTimeWhileWaterVolumeIsLessThanPurgeVolume)
{
   GivenUnitLifetimeDispensedWaterInOuncesX100Is(NonZeroVolume);
   GivenWaterFilterVolumeUsageInOuncesX100Is(NewFilterDispenserPurgeVolumeInOuncesX100 - 1);
   GivenWaterFilterCalendarUsageInSecondsIs((NewFilterFillStateDelayInMinutes - 1) * SECONDS_PER_MINUTE);
   GivenInitialization();
   IceMakerInhibitedDueToNewFilterShouldBe(SET);

   WhenWaterFilterCalendarUsageInSecondsIs((NewFilterFillStateDelayInMinutes + 1) * SECONDS_PER_MINUTE);
   IceMakerInhibitedDueToNewFilterShouldBe(CLEAR);
}

TEST(IceMakerFillBlocker, ShouldUninhibitIceMakerFillWhenWaterVolumeUsageBecomesEqualToPurgeVolumeWhileCalendarUsageIsLessThanNewFilterFillStateDelayTime)
{
   GivenUnitLifetimeDispensedWaterInOuncesX100Is(NonZeroVolume);
   GivenWaterFilterVolumeUsageInOuncesX100Is(NewFilterDispenserPurgeVolumeInOuncesX100 - 1);
   GivenWaterFilterCalendarUsageInSecondsIs((NewFilterFillStateDelayInMinutes - 1) * SECONDS_PER_MINUTE);
   GivenInitialization();
   IceMakerInhibitedDueToNewFilterShouldBe(SET);

   WhenWaterFilterVolumeUsageInOuncesX100Is(NewFilterDispenserPurgeVolumeInOuncesX100);
   IceMakerInhibitedDueToNewFilterShouldBe(CLEAR);
}

TEST(IceMakerFillBlocker, ShouldUninhibitIceMakerFillWhenCalendarUsageBecomesEqualToNewFilterFillStateDelayWhileWaterVolumeIsLessThanPurgeVolume)
{
   GivenUnitLifetimeDispensedWaterInOuncesX100Is(NonZeroVolume);
   GivenWaterFilterVolumeUsageInOuncesX100Is(NewFilterDispenserPurgeVolumeInOuncesX100 - 1);
   GivenWaterFilterCalendarUsageInSecondsIs((NewFilterFillStateDelayInMinutes - 1) * SECONDS_PER_MINUTE);
   GivenInitialization();
   IceMakerInhibitedDueToNewFilterShouldBe(SET);

   WhenWaterFilterCalendarUsageInSecondsIs(NewFilterFillStateDelayInMinutes * SECONDS_PER_MINUTE);
   IceMakerInhibitedDueToNewFilterShouldBe(CLEAR);
}

TEST(IceMakerFillBlocker, ShouldInhibitIceMakerFillWhenUnitLifetimeDispensedWaterBecomesNonZeroWhileWaterVolumeIsLessThanPurgeVolumeAndCalendarUsageIsLessThanNewFilterFillStateDelay)
{
   GivenUnitLifetimeDispensedWaterInOuncesX100Is(0);
   GivenWaterFilterVolumeUsageInOuncesX100Is(NewFilterDispenserPurgeVolumeInOuncesX100 - 1);
   GivenWaterFilterCalendarUsageInSecondsIs((NewFilterFillStateDelayInMinutes - 1) * SECONDS_PER_MINUTE);
   GivenInitialization();
   IceMakerInhibitedDueToNewFilterShouldBe(CLEAR);

   WhenUnitLifetimeDispensedWaterInOuncesX100Is(1);
   IceMakerInhibitedDueToNewFilterShouldBe(SET);
}
