/*!
 * @file
 * @brief Implementation of I16 ERD Adder
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "I16ErdAdder.h"
#include "utils.h"

static void AddErds(I16ErdAdder_t *instance)
{
   int16_t resultValue = 0;

   for(uint8_t index = 0; index < instance->_private.config->i16ErdsToBeAdded.numberOfErds; index++)
   {
      int16_t readValue;
      DataModel_Read(
         instance->_private.dataModel,
         instance->_private.config->i16ErdsToBeAdded.erds[index],
         &readValue);

      resultValue = CLAMP(resultValue + readValue, INT16_MIN, INT16_MAX);
   }
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->resultErd,
      &resultValue);
}

static bool OnChangeErdIsIncludedInErdsToBeAdded(I16ErdAdder_t *instance, Erd_t erd)
{
   for(uint8_t index = 0; index < instance->_private.config->i16ErdsToBeAdded.numberOfErds; index++)
   {
      if(erd == instance->_private.config->i16ErdsToBeAdded.erds[index])
      {
         return true;
      }
   }
   return false;
}

static void DataModelChanged(void *context, const void *args)
{
   I16ErdAdder_t *instance = context;
   const DataModelOnDataChangeArgs_t *onChangeData = args;
   Erd_t erd = onChangeData->erd;

   if(OnChangeErdIsIncludedInErdsToBeAdded(instance, erd))
   {
      AddErds(instance);
   }
}

void I16ErdAdder_Init(
   I16ErdAdder_t *instance,
   I_DataModel_t *dataModel,
   const I16ErdAdderConfiguration_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;

   AddErds(instance);

   EventSubscription_Init(
      &instance->_private.dataModelSubscription,
      instance,
      DataModelChanged);

   Event_Subscribe(
      instance->_private.dataModel->OnDataChange,
      &instance->_private.dataModelSubscription);
}
