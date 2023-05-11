/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "ApplianceFeatureApiUpdater.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"

enum
{
   // Foundation = 0x00000001
   // FreshFoodCabinetSetpoint = 0x00000020
   // FreezerCabinetSetpoint = 0x00000040
   Features = 0x00000061
};

TEST_GROUP(ApplianceFeatureApiUpdater)
{
   ReferDataModel_TestDouble_t referDataModelTestDouble;
   I_DataModel_t *dataModel;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&referDataModelTestDouble);
      dataModel = referDataModelTestDouble.dataModel;
   }

   void GivenTheModuleIsInitialized()
   {
      ApplianceFeatureApiUpdater_Init(dataModel);
   }

   void ApplianceFeatureApiManifestShouldBe(
      ApplianceFeatureApiType_t expectedType,
      ApplianceFeatureApiVersion_t expectedVersion,
      ApplianceFeatureApiFeatureWord_t expectedFeatures)
   {
      ApplianceFeatureApiManifest_t actual;
      DataModel_Read(dataModel, Erd_ApplianceApiApplianceTypeManifest1, &actual);

      CHECK_EQUAL(expectedType, actual.type);
      CHECK_EQUAL(expectedVersion, actual.version);
      CHECK_EQUAL(expectedFeatures, actual.features);
   }
};

TEST(ApplianceFeatureApiUpdater, ShouldUpdateApplianceFeatureApiManifestOnInit)
{
   GivenTheModuleIsInitialized();
   ApplianceFeatureApiManifestShouldBe(
      ApplianceFeatureApiType_Refrigerator,
      2,
      Features);
}
