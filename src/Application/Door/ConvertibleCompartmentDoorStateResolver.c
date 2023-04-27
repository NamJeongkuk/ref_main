/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ConvertibleCompartmentDoorStateResolver.h"
#include "ConvertibleCompartmentStateType.h"
#include "Constants_Binary.h"
#include "utils.h"

enum
{
   Closed,
   Open
};

static ConvertibleCompartmentStateType_t ConvertibleCompartmentStateType(ConvertibleCompartmentDoorStateResolver_t *instance)
{
   ConvertibleCompartmentStateType_t state;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->convertibleCompartmentStateErd,
      &state);

   return state;
}

static bool ConvertibleCompartmentDoorIsOpen(ConvertibleCompartmentDoorStateResolver_t *instance)
{
   bool doorIsOpen;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->convertibleCompartmentDoorIsOpenErd,
      &doorIsOpen);

   return doorIsOpen;
}

static void SetConvertibleCompartmentAsFreshFoodDoorTo(ConvertibleCompartmentDoorStateResolver_t *instance, bool state)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->convertibleCompartmentAsFreshFoodDoorIsOpenErd,
      &state);
}

static void SetConvertibleCompartmentAsFreezerDoorTo(ConvertibleCompartmentDoorStateResolver_t *instance, bool state)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->convertibleCompartmentAsFreezerDoorIsOpenErd,
      &state);
}

static void DataModelChanged(void *context, const void *args)
{
   ConvertibleCompartmentDoorStateResolver_t *instance = context;
   const DataModelOnDataChangeArgs_t *onChangeData = args;
   Erd_t erd = onChangeData->erd;

   if(erd == instance->_private.config->convertibleCompartmentDoorIsOpenErd)
   {
      const bool *doorIsOpen = onChangeData->data;
      ConvertibleCompartmentStateType_t state = ConvertibleCompartmentStateType(instance);

      if(*doorIsOpen)
      {
         if(state == ConvertibleCompartmentStateType_FreshFood)
         {
            SetConvertibleCompartmentAsFreshFoodDoorTo(instance, Open);
         }
         else
         {
            SetConvertibleCompartmentAsFreezerDoorTo(instance, Open);
         }
      }
      else
      {
         if(state == ConvertibleCompartmentStateType_FreshFood)
         {
            SetConvertibleCompartmentAsFreshFoodDoorTo(instance, Closed);
         }
         else
         {
            SetConvertibleCompartmentAsFreezerDoorTo(instance, Closed);
         }
      }
   }
   else if(erd == instance->_private.config->convertibleCompartmentStateErd)
   {
      const ConvertibleCompartmentStateType_t *state = onChangeData->data;
      {
         if(ConvertibleCompartmentDoorIsOpen(instance))
         {
            if(*state == ConvertibleCompartmentStateType_FreshFood)
            {
               SetConvertibleCompartmentAsFreshFoodDoorTo(instance, Open);
               SetConvertibleCompartmentAsFreezerDoorTo(instance, Closed);
            }
            else
            {
               SetConvertibleCompartmentAsFreshFoodDoorTo(instance, Closed);
               SetConvertibleCompartmentAsFreezerDoorTo(instance, Open);
            }
         }
         else
         {
            if(*state == ConvertibleCompartmentStateType_FreshFood)
            {
               SetConvertibleCompartmentAsFreshFoodDoorTo(instance, Closed);
            }
            else
            {
               SetConvertibleCompartmentAsFreezerDoorTo(instance, Closed);
            }
         }
      }
   }
}

void ConvertibleCompartmentDoorStateResolver_Init(
   ConvertibleCompartmentDoorStateResolver_t *instance,
   I_DataModel_t *dataModel,
   const ConvertibleCompartmentDoorResolverConfiguration_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;

   if(ConvertibleCompartmentDoorIsOpen(instance))
   {
      if(ConvertibleCompartmentStateType(instance) == ConvertibleCompartmentStateType_FreshFood)
      {
         SetConvertibleCompartmentAsFreshFoodDoorTo(instance, Open);
      }
      else
      {
         SetConvertibleCompartmentAsFreezerDoorTo(instance, Open);
      }
   }

   EventSubscription_Init(
      &instance->_private.dataModelSubscription,
      instance,
      DataModelChanged);

   DataModel_SubscribeAll(
      instance->_private.dataModel,
      &instance->_private.dataModelSubscription);
}
