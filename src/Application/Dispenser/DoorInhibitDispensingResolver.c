/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "DoorInhibitDispensingResolver.h"
#include "Constants_Binary.h"
#include "DoorIndex.h"
#include "utils.h"

static bool DoorIsOpen(DoorInhibitDispensingResolver_t *instance, Erd_t doorErd)
{
   bool doorIsOpen;
   DataModel_Read(instance->_private.dataModel, doorErd, &doorIsOpen);

   return doorIsOpen;
}

static void UpdateDispensingInhibitedByDoor(
   DoorInhibitDispensingResolver_t *instance,
   bool inhibitCheckIsEnabledForTheDoor,
   DoorIndex_t doorIndex,
   Erd_t doorErd)
{
   if(inhibitCheckIsEnabledForTheDoor && (doorErd != Erd_Invalid))
   {
      bool doorIsOpen = DoorIsOpen(instance, doorErd);
      if(doorIsOpen)
      {
         BIT_SET(instance->_private.doorInhibitStatus, doorIndex);
      }
      else
      {
         BIT_CLEAR(instance->_private.doorInhibitStatus, doorIndex);
      }

      DispensingInhibitedBitmap_t dispensingInhibitedBitmap;
      DataModel_Read(
         instance->_private.dataModel,
         instance->_private.config->dispensingInhibitedErd,
         &dispensingInhibitedBitmap);

      if(instance->_private.doorInhibitStatus > 0)
      {
         BIT_SET(dispensingInhibitedBitmap, instance->_private.config->dispensingInhibitedBitmapIndex);
      }
      else
      {
         BIT_CLEAR(dispensingInhibitedBitmap, instance->_private.config->dispensingInhibitedBitmapIndex);
      }

      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->dispensingInhibitedErd,
         &dispensingInhibitedBitmap);
   }
}

static bool GetDoorInhibitingStatus(DoorInhibitDispensingResolver_t *instance, uint8_t doorIndex)
{
   bool doorInhibitingStatus;
   memcpy(
      &doorInhibitingStatus,
      ((const uint8_t *)instance->_private.doorInhibitDispenseTable) + instance->_private.config->doorsThatInhibitDispense[doorIndex].offsetInParametricForDoorInhibitsDispense,
      sizeof(doorInhibitingStatus));

   return doorInhibitingStatus;
}

static void OnDataModelChange(void *context, const void *_args)
{
   DoorInhibitDispensingResolver_t *instance = context;
   const DataModelOnDataChangeArgs_t *args = _args;

   for(uint8_t i = 0; i < instance->_private.config->numberOfDoors; i++)
   {
      if(args->erd == instance->_private.config->doorsThatInhibitDispense[i].doorIsOpenErd)
      {
         UpdateDispensingInhibitedByDoor(
            instance,
            GetDoorInhibitingStatus(instance, i),
            instance->_private.config->doorsThatInhibitDispense[i].doorIndex,
            instance->_private.config->doorsThatInhibitDispense[i].doorIsOpenErd);
      }
   }
}

static void UpdateDispensingInhibitedByDoorOnInit(DoorInhibitDispensingResolver_t *instance)
{
   for(uint8_t i = 0; i < instance->_private.config->numberOfDoors; i++)
   {
      UpdateDispensingInhibitedByDoor(
         instance,
         GetDoorInhibitingStatus(instance, i),
         instance->_private.config->doorsThatInhibitDispense[i].doorIndex,
         instance->_private.config->doorsThatInhibitDispense[i].doorIsOpenErd);
   }
}

void DoorInhibitDispensingResolver_Init(
   DoorInhibitDispensingResolver_t *instance,
   I_DataModel_t *dataModel,
   const DoorInhibitDispensingResolverConfig_t *config,
   const DoorInhibitDispenseTable_t *doorInhibitDispenseTable)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;
   instance->_private.doorInhibitDispenseTable = doorInhibitDispenseTable;

   UpdateDispensingInhibitedByDoorOnInit(instance);

   EventSubscription_Init(
      &instance->_private.dataModelSubscription,
      instance,
      OnDataModelChange);
   DataModel_SubscribeAll(
      dataModel,
      &instance->_private.dataModelSubscription);
}
