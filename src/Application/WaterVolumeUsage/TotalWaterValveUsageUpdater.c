/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "TotalWaterValveUsageUpdater.h"
#include "RfidTypes.h"

static VolumeInOuncesX100_t CumulativeWaterVolumeUsageInOuncesX100(TotalWaterValveUsageUpdater_t *instance)
{
   VolumeInOuncesX100_t cumulativeWaterVolumeUsageInOuncesX100;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->cumulativeWaterVolumeUsageInOuncesX100Erd,
      &cumulativeWaterVolumeUsageInOuncesX100);

   return cumulativeWaterVolumeUsageInOuncesX100;
}

static void AddLastWaterVolumeUsageInOuncesX100ToCumulativeWaterVolumeUsageInOuncesX100(TotalWaterValveUsageUpdater_t *instance, VolumeInOuncesX100_t lastWaterVolumeUsageInOuncesX100)
{
   VolumeInOuncesX100_t accumulatedWaterVolumeUsageInOuncesX100 =
      CumulativeWaterVolumeUsageInOuncesX100(instance) + lastWaterVolumeUsageInOuncesX100;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->cumulativeWaterVolumeUsageInOuncesX100Erd,
      &accumulatedWaterVolumeUsageInOuncesX100);
}

static void DataModelChanged(void *context, const void *_args)
{
   TotalWaterValveUsageUpdater_t *instance = context;
   const DataModelOnDataChangeArgs_t *args = _args;

   for(uint8_t i = 0; i < instance->_private.config->waterVolumeUsageInOuncesX100ErdList.numberOfErds; i++)
   {
      if(args->erd == instance->_private.config->waterVolumeUsageInOuncesX100ErdList.erds[i])
      {
         const VolumeInOuncesX100_t *lastWaterVolumeUsageInOuncesX100 = args->data;
         if(*lastWaterVolumeUsageInOuncesX100 > 0)
         {
            AddLastWaterVolumeUsageInOuncesX100ToCumulativeWaterVolumeUsageInOuncesX100(instance, *lastWaterVolumeUsageInOuncesX100);
         }
         break;
      }
   }
}

void TotalWaterValveUsageUpdater_Init(
   TotalWaterValveUsageUpdater_t *instance,
   I_DataModel_t *dataModel,
   const TotalWaterValveUsageUpdaterConfig_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;

   EventSubscription_Init(
      &instance->_private.dataModelChangedSubscription,
      instance,
      DataModelChanged);

   DataModel_SubscribeAll(
      dataModel,
      &instance->_private.dataModelChangedSubscription);
}
