/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "BooleanRequestStatusUpdater.h"

static void ResetRequestErdFromIndex(BooleanRequestStatusUpdater_t *instance, uint8_t erdPairIndex)
{
   uint8_t resetRequest = BooleanRequestStatus_ResetValue;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->requestStatusErdPairs[erdPairIndex].requestErd,
      &resetRequest);
}

static bool ARequestErdChanged(BooleanRequestStatusUpdater_t *instance, Erd_t erd, uint8_t *foundIndex)
{
   for(uint8_t index = 0; index < instance->_private.config->numberOfRequestStatusErdPairs; index++)
   {
      if(erd == instance->_private.config->requestStatusErdPairs[index].requestErd)
      {
         *foundIndex = index;
         return true;
      }
   }
   return false;
}

static void SetStatusErdFromIndex(BooleanRequestStatusUpdater_t *instance, bool status, uint8_t erdPairIndex)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->requestStatusErdPairs[erdPairIndex].statusErd,
      &status);
}

static void DataModelChanged(void *context, const void *_args)
{
   BooleanRequestStatusUpdater_t *instance = context;
   const DataModelOnDataChangeArgs_t *args = _args;

   uint8_t foundErdPairIndex;
   if(ARequestErdChanged(instance, args->erd, &foundErdPairIndex))
   {
      const uint8_t *request = args->data;
      if(*request != BooleanRequestStatus_ResetValue)
      {
         ResetRequestErdFromIndex(instance, foundErdPairIndex);
         SetStatusErdFromIndex(instance, *request, foundErdPairIndex);
      }
   }
}

static void ResetAllRequestErds(BooleanRequestStatusUpdater_t *instance)
{
   for(uint8_t i = 0; i < instance->_private.config->numberOfRequestStatusErdPairs; i++)
   {
      ResetRequestErdFromIndex(instance, i);
   }
}

void BooleanRequestStatusUpdater_Init(
   BooleanRequestStatusUpdater_t *instance,
   const BooleanRequestStatusUpdaterConfig_t *config,
   I_DataModel_t *dataModel)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;

   ResetAllRequestErds(instance);

   EventSubscription_Init(
      &instance->_private.dataModelChangedSubscription,
      instance,
      DataModelChanged);
   DataModel_SubscribeAll(
      dataModel,
      &instance->_private.dataModelChangedSubscription);
}
