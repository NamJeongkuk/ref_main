/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ConvertibleCompartmentModeSetpointResolver.h"
#include "FeaturePanMode.h"
#include "utils.h"

enum
{
   ResetValue = INT16_MAX
};

static void ResolveSetpointToDefaultTemperatureForEntry(
   ConvertibleCompartmentModeSetpointResolver_t *instance,
   const ConvertibleCompartmentModeSetpointResolverConfigurationEntry_t *entry)
{
   FeaturePanModeRange_t range;
   DataModel_Read(
      instance->_private.dataModel,
      entry->setpointRangeErd,
      &range);

   TemperatureDegFx100_t currentStatus;
   DataModel_Read(
      instance->_private.dataModel,
      entry->setpointStatusErd,
      &currentStatus);

   if(currentStatus == ResetValue)
   {
      DataModel_Write(
         instance->_private.dataModel,
         entry->setpointStatusErd,
         &range.defaultTemperature);
   }
};

static void ResolveSetpointForEntry(
   ConvertibleCompartmentModeSetpointResolver_t *instance,
   const ConvertibleCompartmentModeSetpointResolverConfigurationEntry_t *entry)
{
   FeaturePanModeRange_t rangeData;
   TemperatureDegFx100_t requestedSetpoint;

   DataModel_Read(
      instance->_private.dataModel,
      entry->setpointRangeErd,
      &rangeData);

   DataModel_Read(
      instance->_private.dataModel,
      entry->setpointRequestErd,
      &requestedSetpoint);

   TemperatureDegFx100_t resolvedSetpointTemperature = CLAMP(
      requestedSetpoint,
      rangeData.minTemperature,
      rangeData.maxTemperature);

   DataModel_Write(
      instance->_private.dataModel,
      entry->setpointStatusErd,
      &resolvedSetpointTemperature);
};

static void OnDataModelChanged(void *context, const void *args)
{
   ConvertibleCompartmentModeSetpointResolver_t *instance = context;
   const DataModelOnDataChangeArgs_t *onChangeData = args;
   const Erd_t erd = onChangeData->erd;

   for(uint8_t i = 0; i < instance->_private.config->numberOfEntries; i++)
   {
      if(erd == instance->_private.config->entries[i].setpointRequestErd)
      {
         ResolveSetpointForEntry(instance, &instance->_private.config->entries[i]);
         break;
      }
   }
}

void ConvertibleCompartmentModeSetpointResolver_Init(
   ConvertibleCompartmentModeSetpointResolver_t *instance,
   I_DataModel_t *dataModel,
   const ConvertibleCompartmentModeSetpointResolverConfiguration_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;

   for(uint8_t i = 0; i < instance->_private.config->numberOfEntries; i++)
   {
      ResolveSetpointToDefaultTemperatureForEntry(instance, &instance->_private.config->entries[i]);
   }

   EventSubscription_Init(
      &instance->_private.subscription,
      instance,
      OnDataModelChanged);

   DataModel_SubscribeAll(dataModel, &instance->_private.subscription);
}
