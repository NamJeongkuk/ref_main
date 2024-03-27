/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "AllFreshFoodDoorStatus.h"
#include "SystemErds.h"

static void UpdateAllFreshFoodDoorStatus(AllFreshFoodDoorStatus_t *instance)
{
   bool allFreshFoodDoorsAreClosed = true;

   for(uint8_t i = 0; i < instance->_private.config->doorStatusErds->numDoorStatusErds; i++)
   {
      bool doorIsOpen;
      DataModel_Read(
         instance->_private.dataModel,
         instance->_private.config->doorStatusErds->doorStatusErdList[i],
         &doorIsOpen);

      if(doorIsOpen)
      {
         allFreshFoodDoorsAreClosed = false;
         break;
      }
   }

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->allFreshFoodDoorsAreClosedErd,
      &allFreshFoodDoorsAreClosed);
}

static void DataModelChanged(void *context, const void *_args)
{
   AllFreshFoodDoorStatus_t *instance = context;
   const DataModelOnDataChangeArgs_t *onChangeData = _args;

   for(uint8_t i = 0; i < instance->_private.config->doorStatusErds->numDoorStatusErds; i++)
   {
      if(onChangeData->erd == instance->_private.config->doorStatusErds->doorStatusErdList[i])
      {
         UpdateAllFreshFoodDoorStatus(instance);
         break;
      }
   }
}

void AllFreshFoodDoorStatus_Init(
   AllFreshFoodDoorStatus_t *instance,
   I_DataModel_t *dataModel,
   const AllFreshFoodDoorStatusConfiguration_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;

   UpdateAllFreshFoodDoorStatus(instance);

   EventSubscription_Init(
      &instance->_private.dataModelChangedSubscription,
      instance,
      DataModelChanged);

   Event_Subscribe(
      instance->_private.dataModel->OnDataChange,
      &instance->_private.dataModelChangedSubscription);
}
