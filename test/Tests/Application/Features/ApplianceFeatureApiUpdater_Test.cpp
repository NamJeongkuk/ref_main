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
   NoFeatureBits = 0
};

static const uint32_t featureBits[] = {
   ApplianceFeatureApi_Refrigerator_Version2_IceMaker0,
   ApplianceFeatureApi_Refrigerator_Version2_IceMaker1,
   ApplianceFeatureApi_Refrigerator_Version3_IceMaker2,
};

static const uint8_t apiVersions[] = { 2, 2, 3 };

static const Erd_t boolFeatureErds[] = {
   Erd_IceMaker0_MoldThermistorIsValid,
   Erd_IceMaker1_MoldThermistorIsValid,
   Erd_IceMaker2_MoldThermistorIsValid
};

static const ApplianceFeatureApiUpdaterTable_t boolToFeatureMap = {
   .featureBits = featureBits,
   .apiVersions = apiVersions,
   .boolFeatureErds = boolFeatureErds,
   .numberOfElements = NUM_ELEMENTS(boolFeatureErds)
};

static const ApplianceFeatureApiUpdaterConfig_t testConfig = {
   .boolToFeatureMap = boolToFeatureMap
};

TEST_GROUP(ApplianceFeatureApiUpdater)
{
   ReferDataModel_TestDouble_t referDataModelTestDouble;
   I_DataModel_t *dataModel;

   ApplianceFeatureApiUpdater_t instance;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&referDataModelTestDouble);
      dataModel = referDataModelTestDouble.dataModel;
   }

   void GivenTheModuleIsInitialized()
   {
      ApplianceFeatureApiUpdater_Init(&instance, dataModel, &testConfig);
   }

   void ApplianceFeatureApiManifestShouldBe(
      Erd_t erd,
      ApplianceFeatureApiType_t expectedType,
      ApplianceFeatureApiVersion_t expectedVersion,
      ApplianceFeatureApiFeatureWord_t expectedFeatures)
   {
      ApplianceFeatureApiManifest_t actual;
      DataModel_Read(dataModel, erd, &actual);

      CHECK_EQUAL(expectedType, actual.type);
      CHECK_EQUAL(expectedVersion, actual.version);
      CHECK_EQUAL(expectedFeatures, actual.features);
   }

   void GivenIceMaker0_MoldThermistorIsValidIs(bool state)
   {
      DataModel_Write(dataModel, Erd_IceMaker0_MoldThermistorIsValid, &state);
   }
   void GivenIceMaker1_MoldThermistorIsValidIs(bool state)
   {
      DataModel_Write(dataModel, Erd_IceMaker1_MoldThermistorIsValid, &state);
   }
   void GivenIceMaker2_MoldThermistorIsValidIs(bool state)
   {
      DataModel_Write(dataModel, Erd_IceMaker2_MoldThermistorIsValid, &state);
   }

   void WhenIceMaker0_MoldThermistorIsValidIs(bool state)
   {
      DataModel_Write(dataModel, Erd_IceMaker0_MoldThermistorIsValid, &state);
   }
   void WhenIceMaker1_MoldThermistorIsValidIs(bool state)
   {
      DataModel_Write(dataModel, Erd_IceMaker1_MoldThermistorIsValid, &state);
   }
   void WhenIceMaker2_MoldThermistorIsValidIs(bool state)
   {
      DataModel_Write(dataModel, Erd_IceMaker2_MoldThermistorIsValid, &state);
   }

   void GivenApplianceManifestIsType(Erd_t manifestErd, ApplianceFeatureApiType_t type)
   {
      ApplianceFeatureApiManifest_t manifest;
      DataModel_Read(dataModel, manifestErd, &manifest);
      manifest.type = type;
      DataModel_Write(dataModel, manifestErd, &manifest);
   }

   void GivenApplianceManifestIsVersion(Erd_t manifestErd, ApplianceFeatureApiVersion_t version)
   {
      ApplianceFeatureApiManifest_t manifest;
      DataModel_Read(dataModel, manifestErd, &manifest);
      manifest.version = version;
      DataModel_Write(dataModel, manifestErd, &manifest);
   }
};

TEST(ApplianceFeatureApiUpdater, ShouldInitializeApplianceFeatureApiManifestWithBoolErdStates)
{
   GivenApplianceManifestIsType(Erd_ApplianceApiApplianceTypeManifest1, ApplianceFeatureApiType_Refrigerator);
   GivenApplianceManifestIsVersion(Erd_ApplianceApiApplianceTypeManifest1, 2);

   GivenApplianceManifestIsType(Erd_ApplianceApiApplianceTypeManifest2, ApplianceFeatureApiType_Refrigerator);
   GivenApplianceManifestIsVersion(Erd_ApplianceApiApplianceTypeManifest2, 3);

   GivenIceMaker0_MoldThermistorIsValidIs(true);
   GivenIceMaker1_MoldThermistorIsValidIs(false);
   GivenIceMaker2_MoldThermistorIsValidIs(true);

   GivenTheModuleIsInitialized();
   ApplianceFeatureApiManifestShouldBe(
      Erd_ApplianceApiApplianceTypeManifest1,
      ApplianceFeatureApiType_Refrigerator,
      2,
      ApplianceFeatureApi_Refrigerator_Version2_IceMaker0);
   ApplianceFeatureApiManifestShouldBe(
      Erd_ApplianceApiApplianceTypeManifest2,
      ApplianceFeatureApiType_Refrigerator,
      3,
      ApplianceFeatureApi_Refrigerator_Version3_IceMaker2);
}

TEST(ApplianceFeatureApiUpdater, ShouldUpdateApplianceFeatureApiManifestOnChange)
{
   GivenApplianceManifestIsType(Erd_ApplianceApiApplianceTypeManifest1, ApplianceFeatureApiType_Refrigerator);
   GivenApplianceManifestIsVersion(Erd_ApplianceApiApplianceTypeManifest1, 2);

   GivenApplianceManifestIsType(Erd_ApplianceApiApplianceTypeManifest2, ApplianceFeatureApiType_Refrigerator);
   GivenApplianceManifestIsVersion(Erd_ApplianceApiApplianceTypeManifest2, 3);

   GivenTheModuleIsInitialized();

   WhenIceMaker0_MoldThermistorIsValidIs(true);
   WhenIceMaker1_MoldThermistorIsValidIs(true);
   ApplianceFeatureApiManifestShouldBe(
      Erd_ApplianceApiApplianceTypeManifest1,
      ApplianceFeatureApiType_Refrigerator,
      2,
      ApplianceFeatureApi_Refrigerator_Version2_IceMaker0 | ApplianceFeatureApi_Refrigerator_Version2_IceMaker1);

   WhenIceMaker2_MoldThermistorIsValidIs(true);
   ApplianceFeatureApiManifestShouldBe(
      Erd_ApplianceApiApplianceTypeManifest2,
      ApplianceFeatureApiType_Refrigerator,
      3,
      ApplianceFeatureApi_Refrigerator_Version3_IceMaker2);

   WhenIceMaker0_MoldThermistorIsValidIs(false);
   ApplianceFeatureApiManifestShouldBe(
      Erd_ApplianceApiApplianceTypeManifest1,
      ApplianceFeatureApiType_Refrigerator,
      2,
      ApplianceFeatureApi_Refrigerator_Version2_IceMaker1);

   WhenIceMaker2_MoldThermistorIsValidIs(false);
   ApplianceFeatureApiManifestShouldBe(
      Erd_ApplianceApiApplianceTypeManifest2,
      ApplianceFeatureApiType_Refrigerator,
      3,
      NoFeatureBits);
}
