/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "BoardFeaturesToApplianceApiFeaturesUpdater.h"
#include "utils.h"
#include "SystemErds.h"

static void MatchManifestFeatures(BoardFeaturesToApplianceApiFeaturesUpdater_t *instance, const BoardFeatureApi_t *boardFeatures, uint8_t manifestNumber)
{
   ApplianceFeatureApiManifest_t currentApplianceManifest;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.featureMapConfig->featureMapData[manifestNumber].manifestErd,
      &currentApplianceManifest);

   for(uint8_t index = 0; index < instance->_private.featureMapConfig->featureMapData[manifestNumber].numberOfFeatureMaps; index++)
   {
      uint8_t boardBitPosition = instance->_private.featureMapConfig->featureMapData[manifestNumber].listOfFeatureMaps[index].remoteBoardFeatureBitPosition;
      uint8_t applianceBitPosition = instance->_private.featureMapConfig->featureMapData[manifestNumber].listOfFeatureMaps[index].applianceFeatureBitPosition;
      
      if(BIT_STATE(boardFeatures->featureBits, boardBitPosition))
      {
         BIT_SET(currentApplianceManifest.features, applianceBitPosition);
      }
      else
      {
         BIT_CLEAR(currentApplianceManifest.features, applianceBitPosition);
      }
   }

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.featureMapConfig->featureMapData[manifestNumber].manifestErd,
      &currentApplianceManifest);
}

static void BoardFeaturesChanged(void *context, const void *args)
{
   BoardFeaturesToApplianceApiFeaturesUpdater_t *instance = context;
   const BoardFeatureApi_t *boardFeatures = args;

   if(boardFeatures->featureApiVersion == instance->_private.featureMapConfig->currentVersion)
   {
      for(uint8_t manifestNumber = 0; manifestNumber < instance->_private.featureMapConfig->numberOfFeatureMapDatas; manifestNumber++)
      {
         MatchManifestFeatures(instance, boardFeatures, manifestNumber);
      }
   }
}

void BoardFeaturesToApplianceApiFeaturesUpdater_Init(
   BoardFeaturesToApplianceApiFeaturesUpdater_t *instance,
   I_DataModel_t *dataModel,
   const BoardFeaturesToApplianceApiFeaturesUpdaterConfig_t *featureMapConfig)
{
   instance->_private.dataModel = dataModel;
   instance->_private.featureMapConfig = featureMapConfig;

   EventSubscription_Init(
      &instance->_private.boardFeaturesChanged,
      instance,
      BoardFeaturesChanged);
   DataModel_Subscribe(
      dataModel,
      featureMapConfig->remoteBoardFeaturesErd,
      &instance->_private.boardFeaturesChanged);
}
