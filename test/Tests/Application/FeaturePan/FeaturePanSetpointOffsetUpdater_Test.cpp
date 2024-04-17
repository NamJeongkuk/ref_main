/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "FeaturePanSetpointOffsetUpdater.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"

static const FeaturePanSetpointOffsetUpdaterConfiguration_t config = {
   .featurePanCurrentModeErd = Erd_FeaturePanCurrentMode,
   .setpointOffsetErd = Erd_FeaturePan_SetpointOffsetInDegFx100,
};

TEST_GROUP(FeaturePanSetpointOffsetUpdater)
{
   ReferDataModel_TestDouble_t referDataModelTestDouble;
   I_DataModel_t *dataModel;

   FeaturePanSetpointOffsetUpdater_t instance;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&referDataModelTestDouble);
      dataModel = referDataModelTestDouble.dataModel;
   }

   void TheModuleIsInitialized()
   {
      FeaturePanSetpointOffsetUpdater_Init(&instance, dataModel, &config);
   }

   void WhenTheFeaturePanCurrentModeErdChangesTo(FeaturePanCurrentMode_t mode)
   {
      DataModel_Write(dataModel, Erd_FeaturePanCurrentMode, &mode);
   }

   void TheSetpointOffsetErdShouldBe(TemperatureDegFx100_t expected)
   {
      TemperatureDegFx100_t actual;

      DataModel_Read(
         dataModel,
         Erd_FeaturePan_SetpointOffsetInDegFx100,
         &actual);

      CHECK_EQUAL(expected, actual);
   }
};

TEST(FeaturePanSetpointOffsetUpdater, ShouldUpdateTheSetpointOffsetBasedOnTheCurrentFeaturePanModeWhenCurrentModeChanges)
{
   WhenTheFeaturePanCurrentModeErdChangesTo(FeaturePanCurrentMode_Mode5);
   TheModuleIsInitialized();
   TheSetpointOffsetErdShouldBe(10);

   WhenTheFeaturePanCurrentModeErdChangesTo(FeaturePanCurrentMode_Mode1);
   TheSetpointOffsetErdShouldBe(-30);

   WhenTheFeaturePanCurrentModeErdChangesTo(FeaturePanCurrentMode_Mode2);
   TheSetpointOffsetErdShouldBe(-20);

   WhenTheFeaturePanCurrentModeErdChangesTo(FeaturePanCurrentMode_Mode3);
   TheSetpointOffsetErdShouldBe(-10);

   WhenTheFeaturePanCurrentModeErdChangesTo(FeaturePanCurrentMode_Mode4);
   TheSetpointOffsetErdShouldBe(0);

   WhenTheFeaturePanCurrentModeErdChangesTo(FeaturePanCurrentMode_Mode5);
   TheSetpointOffsetErdShouldBe(10);

   WhenTheFeaturePanCurrentModeErdChangesTo(FeaturePanCurrentMode_Mode6);
   TheSetpointOffsetErdShouldBe(20);

   WhenTheFeaturePanCurrentModeErdChangesTo(FeaturePanCurrentMode_Mode7);
   TheSetpointOffsetErdShouldBe(30);

   WhenTheFeaturePanCurrentModeErdChangesTo(FeaturePanCurrentMode_Off);
   TheSetpointOffsetErdShouldBe(0);

   WhenTheFeaturePanCurrentModeErdChangesTo(FeaturePanCurrentMode_NotControllable);
   TheSetpointOffsetErdShouldBe(0);
}
