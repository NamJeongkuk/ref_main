/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "FeaturePanMode.h"
#include "FeaturePanSetpointOffsetUpdater.h"
#include "PersonalityParametricData.h"
#include "TemperatureDegFx100.h"

static FeaturePanCurrentMode_t FeaturePanCurrentMode(FeaturePanSetpointOffsetUpdater_t *instance)
{
   FeaturePanCurrentMode_t currentMode;

   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->featurePanCurrentModeErd,
      &currentMode);

   return currentMode;
}

static void UpdateSetpointOffsetBasedOnCurrentFeaturePanMode(FeaturePanSetpointOffsetUpdater_t *instance)
{
   const FeaturePanSetpointOffsetData_t *setpointOffsetData = PersonalityParametricData_Get(instance->_private.dataModel)->featurePanThermalOffsetData->setpointOffsetData;
   FeaturePanCurrentMode_t currentMode = FeaturePanCurrentMode(instance);
   TemperatureDegFx100_t setpointOffset = 0;

   switch(currentMode)
   {
      case FeaturePanCurrentMode_Mode1:
         setpointOffset = setpointOffsetData->mode1OffsetInDegFx100;
         break;

      case FeaturePanCurrentMode_Mode2:
         setpointOffset = setpointOffsetData->mode2OffsetInDegFx100;
         break;

      case FeaturePanCurrentMode_Mode3:
         setpointOffset = setpointOffsetData->mode3OffsetInDegFx100;
         break;

      case FeaturePanCurrentMode_Mode4:
         setpointOffset = setpointOffsetData->mode4OffsetInDegFx100;
         break;

      case FeaturePanCurrentMode_Mode5:
         setpointOffset = setpointOffsetData->mode5OffsetInDegFx100;
         break;

      case FeaturePanCurrentMode_Mode6:
         setpointOffset = setpointOffsetData->mode6OffsetInDegFx100;
         break;

      case FeaturePanCurrentMode_Mode7:
         setpointOffset = setpointOffsetData->mode7OffsetInDegFx100;
         break;
   }

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->setpointOffsetErd,
      &setpointOffset);
}

static void OnDataModelChanged(void *context, const void *args)
{
   FeaturePanSetpointOffsetUpdater_t *instance = context;
   const DataModelOnDataChangeArgs_t *onChangeData = args;
   const Erd_t erd = onChangeData->erd;

   if(erd == instance->_private.config->featurePanCurrentModeErd)
   {
      UpdateSetpointOffsetBasedOnCurrentFeaturePanMode(instance);
   }
}

void FeaturePanSetpointOffsetUpdater_Init(
   FeaturePanSetpointOffsetUpdater_t *instance,
   I_DataModel_t *dataModel,
   const FeaturePanSetpointOffsetUpdaterConfiguration_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;

   UpdateSetpointOffsetBasedOnCurrentFeaturePanMode(instance);

   EventSubscription_Init(
      &instance->_private.subscription,
      instance,
      OnDataModelChanged);

   DataModel_SubscribeAll(dataModel, &instance->_private.subscription);
}
