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
   bool rightFreshFoodDoorIsOpen;
   bool leftFreshFoodDoorIsOpen;
   bool allFreshFoodDoorsAreClosed;

   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->rightFreshDoorStatusErd,
      &rightFreshFoodDoorIsOpen);

   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->leftFreshDoorStatusErd,
      &leftFreshFoodDoorIsOpen);

   allFreshFoodDoorsAreClosed = (!rightFreshFoodDoorIsOpen) && (!leftFreshFoodDoorIsOpen);

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->allFreshFoodDoorsAreClosedErd,
      &allFreshFoodDoorsAreClosed);
}

static void DataModelChanged(void *context, const void *_args)
{
   REINTERPRET(instance, context, AllFreshFoodDoorStatus_t *);
   REINTERPRET(onChangeData, _args, const DataModelOnDataChangeArgs_t *);
   REINTERPRET(erd, onChangeData->erd, Erd_t);

   if(erd == instance->_private.config->rightFreshDoorStatusErd ||
      erd == instance->_private.config->leftFreshDoorStatusErd)
   {
      UpdateAllFreshFoodDoorStatus(instance);
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
