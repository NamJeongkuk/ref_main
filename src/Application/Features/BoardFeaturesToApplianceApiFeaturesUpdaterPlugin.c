/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "BoardFeaturesToApplianceApiFeaturesUpdaterPlugin.h"
#include "ApplianceFeatureApi.h"
#include "DispenserUiFeatures.h"
#include "SystemErds.h"

#define BIT_POSITION(value) __builtin_ctzl(value)

static const FeatureMap_t dispenserUiManifestOneMap[] = {
   {
      .remoteBoardFeatureBitPosition = BIT_POSITION(DispenserUiBoardFeatureBits_PresenceSensingCapable),
      .applianceFeatureBitPosition = BIT_POSITION(ApplianceFeatureApi_Refrigerator_Version2_PresenceSensingEnabled),
   },
   {
      .remoteBoardFeatureBitPosition = BIT_POSITION(DispenserUiBoardFeatureBits_PresenceSensingCapable),
      .applianceFeatureBitPosition = BIT_POSITION(ApplianceFeatureApi_Refrigerator_Version2_PresenceSensedActivatesRecessLight),
   },
};

static const FeatureMapData_t dispenserUiManifestOneMapData = {
   .listOfFeatureMaps = dispenserUiManifestOneMap,
   .manifestErd = Erd_ApplianceApiApplianceTypeManifest1,
   .numberOfFeatureMaps = NUM_ELEMENTS(dispenserUiManifestOneMap)
};

static const FeatureMapData_t listOfDispenserFeatureMaps[] = {
   dispenserUiManifestOneMapData
};

static const BoardFeaturesToApplianceApiFeaturesUpdaterConfig_t dispenserUiFeatureMapConfig = {
   .featureMapData = listOfDispenserFeatureMaps,
   .numberOfFeatureMapDatas = NUM_ELEMENTS(listOfDispenserFeatureMaps),
   .currentVersion = BoardFeatureApiVersion_DispenserUi,
   .remoteBoardFeaturesErd = Erd_DispenserUiReportedFeatures,
};

void BoardFeaturesToApplianceApiFeaturesUpdaterPlugin_Init(
   BoardFeaturesToApplianceApiFeaturesUpdaterPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   BoardFeaturesToApplianceApiFeaturesUpdater_Init(
      &instance->_private.dispenserFeatureUpdater,
      dataModel,
      &dispenserUiFeatureMapConfig);
}
