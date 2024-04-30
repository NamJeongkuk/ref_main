/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "FeaturePanSetpointOffsetUpdater.h"
#include "PersonalityParametricData.h"
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
   const FeaturePanSetpointOffsetData_t *setpointOffsetData = PersonalityParametricData_Get(dataModel)->setpointData->adjustedSetpointData->featurePanAdjustedSetpointData->setpointOffsetData;

   WhenTheFeaturePanCurrentModeErdChangesTo(FeaturePanCurrentMode_Mode5);
   TheModuleIsInitialized();
   TheSetpointOffsetErdShouldBe(setpointOffsetData->mode5OffsetInDegFx100);

   WhenTheFeaturePanCurrentModeErdChangesTo(FeaturePanCurrentMode_Mode1);
   TheSetpointOffsetErdShouldBe(setpointOffsetData->mode1OffsetInDegFx100);

   WhenTheFeaturePanCurrentModeErdChangesTo(FeaturePanCurrentMode_Mode2);
   TheSetpointOffsetErdShouldBe(setpointOffsetData->mode2OffsetInDegFx100);

   WhenTheFeaturePanCurrentModeErdChangesTo(FeaturePanCurrentMode_Mode3);
   TheSetpointOffsetErdShouldBe(setpointOffsetData->mode3OffsetInDegFx100);

   WhenTheFeaturePanCurrentModeErdChangesTo(FeaturePanCurrentMode_Mode4);
   TheSetpointOffsetErdShouldBe(setpointOffsetData->mode4OffsetInDegFx100);

   WhenTheFeaturePanCurrentModeErdChangesTo(FeaturePanCurrentMode_Mode5);
   TheSetpointOffsetErdShouldBe(setpointOffsetData->mode5OffsetInDegFx100);

   WhenTheFeaturePanCurrentModeErdChangesTo(FeaturePanCurrentMode_Mode6);
   TheSetpointOffsetErdShouldBe(setpointOffsetData->mode6OffsetInDegFx100);

   WhenTheFeaturePanCurrentModeErdChangesTo(FeaturePanCurrentMode_Mode7);
   TheSetpointOffsetErdShouldBe(setpointOffsetData->mode7OffsetInDegFx100);

   WhenTheFeaturePanCurrentModeErdChangesTo(FeaturePanCurrentMode_Off);
   TheSetpointOffsetErdShouldBe(0);

   WhenTheFeaturePanCurrentModeErdChangesTo(FeaturePanCurrentMode_NotControllable);
   TheSetpointOffsetErdShouldBe(0);
}
