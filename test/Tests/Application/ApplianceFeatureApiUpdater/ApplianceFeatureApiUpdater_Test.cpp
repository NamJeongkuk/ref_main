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
   // 0x00000001 Foundation
   // 0x00000010 Hydration Station
   // 0x00000020 Fresh FoodCabinet .Setpoint
   // 0x00000040 Freezer Cabinet Setpoint
   // 0x00000080 Dispenser Selection
   // 0x00000100 Cold Water Dispensable
   // 0x00000200 Crushed Ice Dispensable
   // 0x00000400 Cubed Ice Dispensable
   // 0x00000800 Cold Water Dispense Request
   // 0x00001000 Crushed Ice Dispense Request
   // 0x00002000 Cubed Ice Dispense Request
   // 0x00004000 Cooling On Off Request
   // 0x00008000 Left Side Freezer Door Status
   // 0x00010000 Right Side Fresh Food Door Status
   // 0x00020000 Water Filter Status
   // 0x00080000 Ice Maker 0
   // 0x00100000 Ice Maker 1
   // 0x00200000 Turbo Cool
   // 0x00400000 Turbo Freeze
   Features = 0x007BFFF1
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
