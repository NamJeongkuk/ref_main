/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "FeaturePanMode.h"
#include "FeaturePanModeRequestHandler.h"
#include "utils.h"
#include "SystemErds.h"

static void WriteCurrentModeStatusToDefaultIfUninitialized(I_DataModel_t *dataModel)
{
   const PersonalityParametricData_t *personalityData = PersonalityParametricData_Get(dataModel);

   FeaturePanCurrentMode_t currentMode;

   DataModel_Read(
      dataModel,
      Erd_FeaturePanCurrentModeStatus,
      &currentMode);

   if(currentMode == FeaturePanCurrentMode_NotControllable)
   {
      DataModel_Write(
         dataModel,
         Erd_FeaturePanCurrentModeStatus,
         &personalityData->featurePanData->featurePanDefaultMode);

      const FeaturePanCurrentMode_t resetRequest = FeaturePanCurrentMode_NotControllable;
      DataModel_Write(
         dataModel,
         Erd_FeaturePanCurrentModeRequest,
         &resetRequest);
   }
}

static void ResolveRequestBasedOnAllowedModes(FeaturePanModeRequestHandler_t *instance, FeaturePanCurrentMode_t request)
{
   if(IN_RANGE(instance->_private.featurePanData->featurePanMinAllowedMode, request, instance->_private.featurePanData->featurePanMaxAllowedMode))
   {
      DataModel_Write(
         instance->_private.dataModel,
         Erd_FeaturePanCurrentModeStatus,
         &request);

      const FeaturePanCurrentMode_t resetRequest = FeaturePanCurrentMode_NotControllable;
      DataModel_Write(
         instance->_private.dataModel,
         Erd_FeaturePanCurrentModeRequest,
         &resetRequest);
   }
}

static void OnDataModelChanged(void *context, const void *args)
{
   FeaturePanModeRequestHandler_t *instance = context;
   const DataModelOnDataChangeArgs_t *onChangeData = args;
   const Erd_t erd = onChangeData->erd;

   if(Erd_FeaturePanCurrentModeRequest == erd)
   {
      const FeaturePanCurrentMode_t *requestedMode = onChangeData->data;

      if(FeaturePanCurrentMode_NotControllable != *requestedMode)
      {
         ResolveRequestBasedOnAllowedModes(instance, *requestedMode);
      }
   }
}

void FeaturePanModeRequestHandler_Init(
   FeaturePanModeRequestHandler_t *instance,
   I_DataModel_t *dataModel,
   const FeaturePanData_t *featurePanData)
{
   instance->_private.dataModel = dataModel;
   instance->_private.featurePanData = featurePanData;

   WriteCurrentModeStatusToDefaultIfUninitialized(dataModel);

   EventSubscription_Init(
      &instance->_private.subscription,
      instance,
      OnDataModelChanged);

   DataModel_SubscribeAll(dataModel, &instance->_private.subscription);
}
