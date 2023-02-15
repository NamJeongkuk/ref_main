/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ResetNvErdImmediatelyWhenRamErdChangesToZero.h"

static struct
{
   EventSubscription_t dataModelSubscription;
   const ResetNvErdImmediatelyWhenRamErdChangesToZeroConfiguration_t *config;
} instance;

static void DataModelChanged(void *context, const void *args)
{
   I_DataModel_t *dataModel = context;
   const DataModelOnDataChangeArgs_t *onChangeData = args;
   Erd_t erd = onChangeData->erd;

   for(uint8_t i = 0; i < instance.config->entryCount; i++)
   {
      if(erd == instance.config->entries[i].sourceErd)
      {
         const uint8_t *data = onChangeData->data;

         uint8_t size = DataModel_SizeOf(dataModel, erd);
         bool allZeroes = true;

         for(uint8_t i = 0; i < size; i++)
         {
            if(data[i] != 0)
            {
               allZeroes = false;
            }
         }

         if(allZeroes)
         {
            DataModel_Write(
               dataModel,
               instance.config->entries[i].destinationErd,
               data);
         }
      }
   }
}

void ResetNvErdImmediatelyWhenRamErdChangesToZero_Init(
   I_DataModel_t *dataModel,
   const ResetNvErdImmediatelyWhenRamErdChangesToZeroConfiguration_t *config)
{
   instance.config = config;

   EventSubscription_Init(
      &instance.dataModelSubscription,
      dataModel,
      DataModelChanged);
   Event_Subscribe(
      dataModel->OnDataChange,
      &instance.dataModelSubscription);
}
