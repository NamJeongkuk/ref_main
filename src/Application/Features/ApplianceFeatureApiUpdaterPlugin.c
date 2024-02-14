/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SystemErds.h"
#include "ApplianceFeatureApi.h"
#include "ApplianceFeatureApiUpdater.h"
#include "ApplianceFeatureApiUpdaterPlugin.h"
#include "utils.h"

// clang-format off
//       Appliance API Feature Bit                            Version  Associated Bool ERD
#define MAP(ENTRY) \
   ENTRY(ApplianceFeatureApi_Refrigerator_Version2_IceMaker0, 2,       Erd_IceMaker0_MoldThermistorIsValid) \
   ENTRY(ApplianceFeatureApi_Refrigerator_Version2_IceMaker1, 2,       Erd_IceMaker1_MoldThermistorIsValid) \
   ENTRY(ApplianceFeatureApi_Refrigerator_Version3_IceMaker2, 3,       Erd_IceMaker2_MoldThermistorIsValid)
// clang-format on

#define EXPAND_AS_FEATURE_BITS(_apiBit, _apiVersion, _boolErd) _apiBit,
static const uint32_t featuresBits[] = { MAP(EXPAND_AS_FEATURE_BITS) };

#define EXPAND_AS_API_VERSIONS(_apiBit, _apiVersion, _boolErd) _apiVersion,
static const uint8_t apiVersions[] = { MAP(EXPAND_AS_API_VERSIONS) };

#define EXPAND_AS_BOOL_ERDS(_apiBit, _apiVersion, _boolErd) _boolErd,
static const Erd_t boolFeatureErds[] = { MAP(EXPAND_AS_BOOL_ERDS) };

static const ApplianceFeatureApiUpdaterConfig_t config = {
   .boolToFeatureMap.featureBits = featuresBits,
   .boolToFeatureMap.apiVersions = apiVersions,
   .boolToFeatureMap.boolFeatureErds = boolFeatureErds,
   .boolToFeatureMap.numberOfElements = NUM_ELEMENTS(boolFeatureErds)
};

static void WriteConstantApplianceFeatureApiManifestBits(I_DataModel_t *dataModel)
{
   ApplianceFeatureApiManifest_t applianceFeatureApiManifest;
   applianceFeatureApiManifest.type = ApplianceFeatureApiType_Refrigerator;
   applianceFeatureApiManifest.version = 2;
   applianceFeatureApiManifest.features =
      ApplianceFeatureApi_Refrigerator_Version2_Foundation |
      ApplianceFeatureApi_Refrigerator_Version2_FreshFoodCabinetSetpoint |
      ApplianceFeatureApi_Refrigerator_Version2_FreezerCabinetSetpoint |
      ApplianceFeatureApi_Refrigerator_Version2_DispenserSelection |
      ApplianceFeatureApi_Refrigerator_Version2_ColdWaterDispensable |
      ApplianceFeatureApi_Refrigerator_Version2_CrushedIceDispensable |
      ApplianceFeatureApi_Refrigerator_Version2_CubedIceDispensable |
      ApplianceFeatureApi_Refrigerator_Version2_ColdWaterDispenseRequest |
      ApplianceFeatureApi_Refrigerator_Version2_CrushedIceDispenseRequest |
      ApplianceFeatureApi_Refrigerator_Version2_CubedIceDispenseRequest |
      ApplianceFeatureApi_Refrigerator_Version2_CoolingOnOffRequest |
      ApplianceFeatureApi_Refrigerator_Version2_LeftSideFreezerDoorStatus |
      ApplianceFeatureApi_Refrigerator_Version2_RightSideFreshFoodDoorStatus |
      ApplianceFeatureApi_Refrigerator_Version2_WaterFilterStatus |
      ApplianceFeatureApi_Refrigerator_Version2_TurboCool |
      ApplianceFeatureApi_Refrigerator_Version2_TurboFreeze |
      ApplianceFeatureApi_Refrigerator_Version2_DimmableLight0PercentLevel |
      ApplianceFeatureApi_Refrigerator_Version2_NightSnackModeLighting |
      ApplianceFeatureApi_Refrigerator_Version2_V2RefrigerationNotifications;
   DataModel_Write(dataModel, Erd_ApplianceApiApplianceTypeManifest1, &applianceFeatureApiManifest);

   applianceFeatureApiManifest.version = 3;
   applianceFeatureApiManifest.features = 0;
   DataModel_Write(dataModel, Erd_ApplianceApiApplianceTypeManifest2, &applianceFeatureApiManifest);
}

void ApplianceFeatureApiUpdaterPlugin_Init(ApplianceFeatureApiUpdaterPlugin_t *instance, I_DataModel_t *dataModel)
{
   WriteConstantApplianceFeatureApiManifestBits(dataModel);

   ApplianceFeatureApiUpdater_Init(
      &instance->_private.applianceFeatureApiUpdater,
      dataModel,
      &config);
}
