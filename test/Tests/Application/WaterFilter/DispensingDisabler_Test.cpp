/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "DispensingDisabler.h"
#include "Constants_Binary.h"
}

#include "CppUTest/TestHarness.h"
#include "ReferDataModel_TestDouble.h"

static const DispensingDisablerConfig_t config = {
   .rfidFilterStateErd = Erd_WaterFilterState,
   .dispensingInhibitedReasonBitmapErd = Erd_DispensingInhibitedReason,
   .iceMakerFillInhibitedReasonBitmapErd = Erd_IceMakerFillInhibitedReason
};

TEST_GROUP(DispensingDisabler)
{
   DispensingDisabler_t instance;

   ReferDataModel_TestDouble_t referDataModelTestDouble;
   I_DataModel_t *dataModel;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&referDataModelTestDouble);
      dataModel = referDataModelTestDouble.dataModel;
   }

   void GivenTheModuleIsInitialized()
   {
      DispensingDisabler_Init(&instance, dataModel, &config);
   }

   void WhenTheModuleIsInitialized()
   {
      GivenTheModuleIsInitialized();
   }

   void WhenWaterFilterStateIs(WaterFilterState_t state)
   {
      DataModel_Write(dataModel, Erd_WaterFilterState, &state);
   }

   void GivenWaterFilterStateIs(WaterFilterState_t state)
   {
      WhenWaterFilterStateIs(state);
   }

   void DispensingInhibitedReasonByWaterDueToRfidFilterShouldBeSet()
   {
      DispensingInhibitedReasonBitmap_t dispensingInhibitedReasonBitmap;
      DataModel_Read(dataModel, Erd_DispensingInhibitedReason, &dispensingInhibitedReasonBitmap);

      bool inhibited = BITMAP_STATE(&dispensingInhibitedReasonBitmap, DispensingInhibitedReason_WaterDueToRfidFilter);
      CHECK_TRUE(inhibited);
   }

   void IceMakerFillInhibitedReasonByDueToRfidFilterShouldBeSet()
   {
      IceMakerFillInhibitedReasonBitmap_t iceMakerFillInhibitedReasonBitmap;
      DataModel_Read(dataModel, Erd_IceMakerFillInhibitedReason, &iceMakerFillInhibitedReasonBitmap);

      bool inhibited = BITMAP_STATE(&iceMakerFillInhibitedReasonBitmap, IceMakerFillInhibitedReason_DueToRfidFilter);
      CHECK_TRUE(inhibited);
   }

   void DispensingInhibitedReasonByWaterDueToRfidFilterShouldBeClear()
   {
      DispensingInhibitedReasonBitmap_t dispensingInhibitedReasonBitmap;
      DataModel_Read(dataModel, Erd_DispensingInhibitedReason, &dispensingInhibitedReasonBitmap);

      bool inhibited = BITMAP_STATE(&dispensingInhibitedReasonBitmap, DispensingInhibitedReason_WaterDueToRfidFilter);
      CHECK_FALSE(inhibited);
   }

   void IceMakerFillInhibitedReasonByDueToRfidFilterShouldBeClear()
   {
      IceMakerFillInhibitedReasonBitmap_t iceMakerFillInhibitedReasonBitmap;
      DataModel_Read(dataModel, Erd_IceMakerFillInhibitedReason, &iceMakerFillInhibitedReasonBitmap);

      bool inhibited = BITMAP_STATE(&iceMakerFillInhibitedReasonBitmap, IceMakerFillInhibitedReason_DueToRfidFilter);
      CHECK_FALSE(inhibited);
   }
};

TEST(DispensingDisabler, ShouldSetDispensingInhibitedReasonAndIceMakerFillInhibitedReasonWhenWaterFilterStateIsLeakBeforeInit)
{
   GivenWaterFilterStateIs(WaterFilterState_Leak);

   WhenTheModuleIsInitialized();
   DispensingInhibitedReasonByWaterDueToRfidFilterShouldBeSet();
   IceMakerFillInhibitedReasonByDueToRfidFilterShouldBeSet();
}

TEST(DispensingDisabler, ShouldSetDispensingInhibitedReasonAndIceMakerFillInhibitedReasonWhenWaterFilterStateIsErrorBeforeInit)
{
   GivenWaterFilterStateIs(WaterFilterState_Error);

   WhenTheModuleIsInitialized();
   DispensingInhibitedReasonByWaterDueToRfidFilterShouldBeSet();
   IceMakerFillInhibitedReasonByDueToRfidFilterShouldBeSet();
}

TEST(DispensingDisabler, ShouldSetDispensingInhibitedReasonAndIceMakerFillInhibitedReasonWhenWaterFilterStateIsLeak)
{
   GivenWaterFilterStateIs(WaterFilterState_Good);
   GivenTheModuleIsInitialized();
   DispensingInhibitedReasonByWaterDueToRfidFilterShouldBeClear();
   IceMakerFillInhibitedReasonByDueToRfidFilterShouldBeClear();

   WhenWaterFilterStateIs(WaterFilterState_Leak);
   DispensingInhibitedReasonByWaterDueToRfidFilterShouldBeSet();
   IceMakerFillInhibitedReasonByDueToRfidFilterShouldBeSet();
}

TEST(DispensingDisabler, ShouldSetDispensingInhibitedReasonAndIceMakerFillInhibitedReasonWhenWaterFilterStateIsError)
{
   GivenWaterFilterStateIs(WaterFilterState_Good);
   GivenTheModuleIsInitialized();
   DispensingInhibitedReasonByWaterDueToRfidFilterShouldBeClear();
   IceMakerFillInhibitedReasonByDueToRfidFilterShouldBeClear();

   WhenWaterFilterStateIs(WaterFilterState_Error);
   DispensingInhibitedReasonByWaterDueToRfidFilterShouldBeSet();
   IceMakerFillInhibitedReasonByDueToRfidFilterShouldBeSet();
}

TEST(DispensingDisabler, ShouldClearDispensingInhibitedReasonAndIceMakerFillInhibitedReasonWhenWaterFilterStateChangesFromLeakToGood)
{
   GivenWaterFilterStateIs(WaterFilterState_Leak);
   GivenTheModuleIsInitialized();
   DispensingInhibitedReasonByWaterDueToRfidFilterShouldBeSet();
   IceMakerFillInhibitedReasonByDueToRfidFilterShouldBeSet();

   WhenWaterFilterStateIs(WaterFilterState_Good);
   DispensingInhibitedReasonByWaterDueToRfidFilterShouldBeClear();
   IceMakerFillInhibitedReasonByDueToRfidFilterShouldBeClear();
}

TEST(DispensingDisabler, ShouldClearDispensingInhibitedReasonAndIceMakerFillInhibitedReasonWhenWaterFilterStateChangesFromErrorToGood)
{
   GivenWaterFilterStateIs(WaterFilterState_Error);
   GivenTheModuleIsInitialized();
   DispensingInhibitedReasonByWaterDueToRfidFilterShouldBeSet();
   IceMakerFillInhibitedReasonByDueToRfidFilterShouldBeSet();

   WhenWaterFilterStateIs(WaterFilterState_Good);
   DispensingInhibitedReasonByWaterDueToRfidFilterShouldBeClear();
   IceMakerFillInhibitedReasonByDueToRfidFilterShouldBeClear();
}

TEST(DispensingDisabler, ShouldClearDispensingInhibitedReasonAndIceMakerFillInhibitedReasonWhenWaterFilterStateChangesFromLeakToReplace)
{
   GivenWaterFilterStateIs(WaterFilterState_Leak);
   GivenTheModuleIsInitialized();
   DispensingInhibitedReasonByWaterDueToRfidFilterShouldBeSet();
   IceMakerFillInhibitedReasonByDueToRfidFilterShouldBeSet();

   WhenWaterFilterStateIs(WaterFilterState_Replace);
   DispensingInhibitedReasonByWaterDueToRfidFilterShouldBeClear();
   IceMakerFillInhibitedReasonByDueToRfidFilterShouldBeClear();
}

TEST(DispensingDisabler, ShouldClearDispensingInhibitedReasonAndIceMakerFillInhibitedReasonWhenWaterFilterStateChangesFromErrorToReplace)
{
   GivenWaterFilterStateIs(WaterFilterState_Error);
   GivenTheModuleIsInitialized();
   DispensingInhibitedReasonByWaterDueToRfidFilterShouldBeSet();
   IceMakerFillInhibitedReasonByDueToRfidFilterShouldBeSet();

   WhenWaterFilterStateIs(WaterFilterState_Replace);
   DispensingInhibitedReasonByWaterDueToRfidFilterShouldBeClear();
   IceMakerFillInhibitedReasonByDueToRfidFilterShouldBeClear();
}

TEST(DispensingDisabler, ShouldClearDispensingInhibitedReasonAndIceMakerFillInhibitedReasonWhenWaterFilterStateChangesFromLeakToExpired)
{
   GivenWaterFilterStateIs(WaterFilterState_Leak);
   GivenTheModuleIsInitialized();
   DispensingInhibitedReasonByWaterDueToRfidFilterShouldBeSet();
   IceMakerFillInhibitedReasonByDueToRfidFilterShouldBeSet();

   WhenWaterFilterStateIs(WaterFilterState_Expired);
   DispensingInhibitedReasonByWaterDueToRfidFilterShouldBeClear();
   IceMakerFillInhibitedReasonByDueToRfidFilterShouldBeClear();
}

TEST(DispensingDisabler, ShouldClearDispensingInhibitedReasonAndIceMakerFillInhibitedReasonWhenWaterFilterStateChangesFromErrorToExpired)
{
   GivenWaterFilterStateIs(WaterFilterState_Error);
   GivenTheModuleIsInitialized();
   DispensingInhibitedReasonByWaterDueToRfidFilterShouldBeSet();
   IceMakerFillInhibitedReasonByDueToRfidFilterShouldBeSet();

   WhenWaterFilterStateIs(WaterFilterState_Expired);
   DispensingInhibitedReasonByWaterDueToRfidFilterShouldBeClear();
   IceMakerFillInhibitedReasonByDueToRfidFilterShouldBeClear();
}

TEST(DispensingDisabler, ShouldClearDispensingInhibitedReasonAndIceMakerFillInhibitedReasonWhenWaterFilterStateChangesFromLeakToBypass)
{
   GivenWaterFilterStateIs(WaterFilterState_Leak);
   GivenTheModuleIsInitialized();
   DispensingInhibitedReasonByWaterDueToRfidFilterShouldBeSet();
   IceMakerFillInhibitedReasonByDueToRfidFilterShouldBeSet();

   WhenWaterFilterStateIs(WaterFilterState_Bypass);
   DispensingInhibitedReasonByWaterDueToRfidFilterShouldBeClear();
   IceMakerFillInhibitedReasonByDueToRfidFilterShouldBeClear();
}

TEST(DispensingDisabler, ShouldClearDispensingInhibitedReasonAndIceMakerFillInhibitedReasonWhenWaterFilterStateChangesFromErrorToBypass)
{
   GivenWaterFilterStateIs(WaterFilterState_Error);
   GivenTheModuleIsInitialized();
   DispensingInhibitedReasonByWaterDueToRfidFilterShouldBeSet();
   IceMakerFillInhibitedReasonByDueToRfidFilterShouldBeSet();

   WhenWaterFilterStateIs(WaterFilterState_Bypass);
   DispensingInhibitedReasonByWaterDueToRfidFilterShouldBeClear();
   IceMakerFillInhibitedReasonByDueToRfidFilterShouldBeClear();
}
