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
   .rfidBoardLeakDetectedFaultErd = Erd_RfidBoardLeakDetectedFault,
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

   void WhenLeakDetectedIsSet()
   {
      DataModel_Write(dataModel, Erd_RfidBoardLeakDetectedFault, set);
   }

   void GivenLeakDetectedIsSet()
   {
      WhenLeakDetectedIsSet();
   }

   void WhenLeakDetectedIsClear()
   {
      DataModel_Write(dataModel, Erd_RfidBoardLeakDetectedFault, clear);
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

TEST(DispensingDisabler, ShouldSetDispensingInhibitedReasonAndIceMakerFillInhibitedReasonWhenLeakIsDetectedBeforeInit)
{
   GivenLeakDetectedIsSet();

   WhenTheModuleIsInitialized();
   DispensingInhibitedReasonByWaterDueToRfidFilterShouldBeSet();
   IceMakerFillInhibitedReasonByDueToRfidFilterShouldBeSet();
}

TEST(DispensingDisabler, ShouldSetDispensingInhibitedReasonAndIceMakerFillInhibitedReasonWhenLeakIsDetected)
{
   GivenTheModuleIsInitialized();
   DispensingInhibitedReasonByWaterDueToRfidFilterShouldBeClear();
   IceMakerFillInhibitedReasonByDueToRfidFilterShouldBeClear();

   WhenLeakDetectedIsSet();
   DispensingInhibitedReasonByWaterDueToRfidFilterShouldBeSet();
   IceMakerFillInhibitedReasonByDueToRfidFilterShouldBeSet();
}

TEST(DispensingDisabler, ShouldClearDispensingInhibitedReasonAndIceMakerFillInhibitedReasonWhenLeakIsNotDetected)
{
   GivenLeakDetectedIsSet();
   GivenTheModuleIsInitialized();
   DispensingInhibitedReasonByWaterDueToRfidFilterShouldBeSet();
   IceMakerFillInhibitedReasonByDueToRfidFilterShouldBeSet();

   WhenLeakDetectedIsClear();
   DispensingInhibitedReasonByWaterDueToRfidFilterShouldBeClear();
   IceMakerFillInhibitedReasonByDueToRfidFilterShouldBeClear();
}
