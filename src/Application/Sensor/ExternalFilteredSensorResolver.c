/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ExternalFilteredSensorResolver.h"
#include "Constants_Binary.c"
#include "PersonalityParametricData.h"
#include "RelativeHumidityPercentx100.h"
#include "utils.h"

static void UpdateResolvedSensorErdsWithExternalSensorData(ExternalFilteredSensorResolver_t *instance)
{
   if(instance->_private.config->sensorType == ExternalFilteredSensorResolver_SensorType_Temperature)
   {
      TemperatureDegFx100_t externalSensorDataInDegFx100;

      DataModel_Read(
         instance->_private.dataModel,
         instance->_private.config->externalFilteredSensorErd,
         &externalSensorDataInDegFx100);

      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->resolvedFilteredSensorErd,
         &externalSensorDataInDegFx100);
   }
   else
   {
      RelativeHumidityPercentx100_t externalSensorDataInPercentx100;

      DataModel_Read(
         instance->_private.dataModel,
         instance->_private.config->externalFilteredSensorErd,
         &externalSensorDataInPercentx100);

      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->resolvedFilteredSensorErd,
         &externalSensorDataInPercentx100);
   }
   bool externalSensorIsValid;

   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->externalFilteredSensorIsValidErd,
      &externalSensorIsValid);

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->resolvedFilteredSensorIsValidErd,
      &externalSensorIsValid);
}

static void UpdateResolvedSensorErdsWithInternalSensorData(ExternalFilteredSensorResolver_t *instance)
{
   if(instance->_private.config->sensorType == ExternalFilteredSensorResolver_SensorType_Temperature)
   {
      TemperatureDegFx100_t internalSensorDataInDegFx100;

      DataModel_Read(
         instance->_private.dataModel,
         instance->_private.config->internalFilteredSensorErd,
         &internalSensorDataInDegFx100);

      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->resolvedFilteredSensorErd,
         &internalSensorDataInDegFx100);
   }
   else
   {
      RelativeHumidityPercentx100_t internalSensorDataInPercentx100;

      DataModel_Read(
         instance->_private.dataModel,
         instance->_private.config->internalFilteredSensorErd,
         &internalSensorDataInPercentx100);

      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->resolvedFilteredSensorErd,
         &internalSensorDataInPercentx100);
   }

   bool internalSensorIsValid;

   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->internalFilteredSensorIsValidErd,
      &internalSensorIsValid);

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->resolvedFilteredSensorIsValidErd,
      &internalSensorIsValid);
}

static void UpdateResolvedSensorErds(ExternalFilteredSensorResolver_t *instance)
{
   bool internalSensorIsValid;
   bool externalSensorIsValid;

   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->internalFilteredSensorIsValidErd,
      &internalSensorIsValid);

   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->externalFilteredSensorIsValidErd,
      &externalSensorIsValid);

   if(instance->_private.ambientSensorData->enableExternalSensorCheck && !internalSensorIsValid && externalSensorIsValid)
   {
      UpdateResolvedSensorErdsWithExternalSensorData(instance);
   }
   else
   {
      UpdateResolvedSensorErdsWithInternalSensorData(instance);
   }
}

static void OnDataModelChange(void *context, const void *_args)
{
   ExternalFilteredSensorResolver_t *instance = context;
   const DataModelOnDataChangeArgs_t *args = _args;

   if(args->erd == instance->_private.config->internalFilteredSensorErd ||
      args->erd == instance->_private.config->externalFilteredSensorErd ||
      args->erd == instance->_private.config->internalFilteredSensorIsValidErd ||
      args->erd == instance->_private.config->externalFilteredSensorIsValidErd)
   {
      UpdateResolvedSensorErds(instance);
   }
}

void ExternalFilteredSensorResolver_Init(
   ExternalFilteredSensorResolver_t *instance,
   I_DataModel_t *dataModel,
   const ExternalFilteredSensorResolverConfig_t *config,
   const SensorDataSensorType_t *ambientSensorData)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;
   instance->_private.ambientSensorData = ambientSensorData;

   UpdateResolvedSensorErds(instance);

   EventSubscription_Init(
      &instance->_private.onDataModelChangeSubscription,
      instance,
      OnDataModelChange);

   DataModel_SubscribeAll(
      instance->_private.dataModel,
      &instance->_private.onDataModelChangeSubscription);
}
