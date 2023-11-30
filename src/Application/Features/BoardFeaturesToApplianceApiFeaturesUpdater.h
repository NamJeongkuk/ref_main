/*!
 * @file
 * @brief This module is responsible for mapping peripheral board features to appliances features
 * Each board shall send its features in the form of an erd of type BoardFeatureApi_t
 * When this ERD changes, this module will compare these features to the map provided
 * This will then update the Appliance API based on mapping
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef BOARDFEATURESTOAPPLIANCEAPIFEATURESUPDATER_H
#define BOARDFEATURESTOAPPLIANCEAPIFEATURESUPDATER_H

#include "I_DataModel.h"
#include "EventSubscription.h"
#include "BoardFeatureApi.h"
#include "ErdList.h"

typedef struct
{
   uint8_t remoteBoardFeatureBitPosition;
   uint8_t applianceFeatureBitPosition;
} FeatureMap_t;

typedef struct
{
   const FeatureMap_t *listOfFeatureMaps;
   Erd_t manifestErd; // ApplianceFeatureApiManifest_t
   uint8_t numberOfFeatureMaps;
} FeatureMapData_t;

typedef struct
{
   const FeatureMapData_t *featureMapData;
   uint8_t numberOfFeatureMapDatas;
   BoardFeatureApiVersion_t currentVersion;
   Erd_t remoteBoardFeaturesErd; // BoardFeatureApi_t
} BoardFeaturesToApplianceApiFeaturesUpdaterConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      EventSubscription_t boardFeaturesChanged;
      const BoardFeaturesToApplianceApiFeaturesUpdaterConfig_t *featureMapConfig;
   } _private;
} BoardFeaturesToApplianceApiFeaturesUpdater_t;

/*!
 * @param instance
 * @param dataModel
 * @param featureMapConfig
 */
void BoardFeaturesToApplianceApiFeaturesUpdater_Init(
   BoardFeaturesToApplianceApiFeaturesUpdater_t *instance,
   I_DataModel_t *dataModel,
   const BoardFeaturesToApplianceApiFeaturesUpdaterConfig_t *featureMapConfig);

#endif
