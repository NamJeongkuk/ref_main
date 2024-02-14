/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SystemErds.h"
#include "ApplianceFeatureApiUpdater.h"
#include "uassert.h"
#include "utils.h"

enum
{
   ApplianceManifestApiVersion2 = 2,
   ApplianceManifestApiVersion3
};

static void SetBits(
   ApplianceFeatureApiUpdater_t *instance,
   const uint8_t apiVersion,
   const uint32_t featureBit,
   const bool bitState)
{
   Erd_t manifestErd;

   if(ApplianceManifestApiVersion2 == apiVersion)
   {
      manifestErd = Erd_ApplianceApiApplianceTypeManifest1;
   }
   else if(ApplianceManifestApiVersion3 == apiVersion)
   {
      manifestErd = Erd_ApplianceApiApplianceTypeManifest2;
   }
   else
   {
      uassert(!"ApiVersionNotSupported");
      return;
   }

   ApplianceFeatureApiManifest_t manifest;
   DataModel_Read(
      instance->_private.dataModel,
      manifestErd,
      &manifest);

   if(bitState)
   {
      manifest.features |= featureBit;
   }
   else
   {
      manifest.features &= ~featureBit;
   }

   DataModel_Write(
      instance->_private.dataModel,
      manifestErd,
      &manifest);
}

static void OnDataModelChange(void *context, const void *_args)
{
   ApplianceFeatureApiUpdater_t *instance = context;
   const DataModelOnDataChangeArgs_t *args = _args;

   ApplianceFeatureApiUpdaterTable_t boolToFeatureMap = instance->_private.config->boolToFeatureMap;

   for(uint8_t i = 0; i < boolToFeatureMap.numberOfElements; i++)
   {
      if(args->erd == boolToFeatureMap.boolFeatureErds[i])
      {
         const bool *state = args->data;
         SetBits(
            instance,
            boolToFeatureMap.apiVersions[i],
            boolToFeatureMap.featureBits[i],
            *state);
         return;
      }
   }
}

static void SetInitialBits(ApplianceFeatureApiUpdater_t *instance, I_DataModel_t *dataModel)
{
   ApplianceFeatureApiUpdaterTable_t boolToFeatureMap = instance->_private.config->boolToFeatureMap;

   for(uint8_t i = 0; i < boolToFeatureMap.numberOfElements; i++)
   {
      bool state;
      DataModel_Read(
         dataModel,
         boolToFeatureMap.boolFeatureErds[i],
         &state);
      SetBits(instance, boolToFeatureMap.apiVersions[i], boolToFeatureMap.featureBits[i], state);
   }
}

void ApplianceFeatureApiUpdater_Init(
   ApplianceFeatureApiUpdater_t *instance,
   I_DataModel_t *dataModel,
   const ApplianceFeatureApiUpdaterConfig_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;

   SetInitialBits(instance, dataModel);

   EventSubscription_Init(
      &instance->_private.dataModelSubscription,
      instance,
      OnDataModelChange);

   DataModel_SubscribeAll(
      instance->_private.dataModel,
      &instance->_private.dataModelSubscription);
}
