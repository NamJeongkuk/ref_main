/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "UserAllowableInteriorLightingResolver.h"
#include "UserAllowableInteriorLightState.h"
#include "utils.h"

static void SetLightState(
   UserAllowableInteriorLightingResolver_t *instance,
   const DoorLightingData_t *doorLightingData,
   PercentageDutyCycle_t userAllowablePercentageDutyCycle,
   Erd_t lightStateErd)
{
   UserAllowableInteriorLightState_t lightState;
   lightState.userAllowable = true;
   lightState.percentageDutyCycle =
      (uint8_t)((uint16_t)(doorLightingData->normalOperationRampingPwmDutyCycle.maxPwmDutyCyclePercentage * userAllowablePercentageDutyCycle) / 100);
   DataModel_Write(
      instance->_private.dataModel,
      lightStateErd,
      &lightState);
}

static PercentageDutyCycle_t UserAllowablePercentageDutyCycleForConfig(
   UserAllowableInteriorLightingResolver_t *instance,
   uint8_t configIndex)
{
   PercentageDutyCycle_t userAllowablePercentageDutyCycle;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->userAllowableLightConfigs[configIndex].statusErd,
      &userAllowablePercentageDutyCycle);

   return userAllowablePercentageDutyCycle;
}

static void SetUserAllowableLightStateErdsForConfig(UserAllowableInteriorLightingResolver_t *instance, uint8_t configIndex)
{
   UserAllowableInteriorLightingBitmap_t lightingBitmap;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->userAllowableLightConfigs[configIndex].configErd,
      &lightingBitmap);

   for(uint8_t lightIndex = 0; lightIndex < sizeof(instance->_private.config->lights) / sizeof(Erd_t); lightIndex++)
   {
      if(BITMAP_STATE(&lightingBitmap, lightIndex))
      {
         SetLightState(
            instance,
            *(&instance->_private.lightingData->doorLightingDataSet.freshFoodBackWallDoorLightingData + lightIndex),
            UserAllowablePercentageDutyCycleForConfig(instance, configIndex),
            ((const Erd_t *)&instance->_private.config->lights)[lightIndex]);
      }
   }
}

static void DataModelChanged(void *context, const void *args)
{
   UserAllowableInteriorLightingResolver_t *instance = context;
   const DataModelOnDataChangeArgs_t *onChangeData = args;
   Erd_t erd = onChangeData->erd;

   for(uint8_t configIndex = 0; configIndex < instance->_private.config->numberOfUserAllowableLightConfigs; configIndex++)
   {
      if(erd == instance->_private.config->userAllowableLightConfigs[configIndex].statusErd)
      {
         SetUserAllowableLightStateErdsForConfig(instance, configIndex);
      }
   }
}

void UserAllowableInteriorLightingResolver_Init(
   UserAllowableInteriorLightingResolver_t *instance,
   I_DataModel_t *dataModel,
   const LightingData_t *lightingData,
   const UserAllowableInteriorLightingResolverConfiguration_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;
   instance->_private.lightingData = lightingData;

   for(uint8_t configIndex = 0; configIndex < instance->_private.config->numberOfUserAllowableLightConfigs; configIndex++)
   {
      SetUserAllowableLightStateErdsForConfig(instance, configIndex);
   }

   EventSubscription_Init(
      &instance->_private.dataModelSubscription,
      instance,
      DataModelChanged);
   Event_Subscribe(
      dataModel->OnDataChange,
      &instance->_private.dataModelSubscription);
}
