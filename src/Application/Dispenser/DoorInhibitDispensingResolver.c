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

      bool dispensingInhibited = instance->_private.doorInhibitStatus > 0;
      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->dispensingInhibitedByDoorErd,
         &dispensingInhibited);
   }
}

static void OnDataModelChange(void *context, const void *_args)
{
   DoorInhibitDispensingResolver_t *instance = context;
   const DataModelOnDataChangeArgs_t *args = _args;

   if(args->erd == instance->_private.config->leftHandFreshFoodDoorIsOpenErd)
   {
      UpdateDispensingInhibitedByDoor(
         instance,
         instance->_private.doorInhibitDispenseTable->leftHandFreshFood,
         DoorIndex_LeftHandFreshFood,
         args->erd);
   }
   else if(args->erd == instance->_private.config->rightHandFreshFoodDoorIsOpenErd)
   {
      UpdateDispensingInhibitedByDoor(
         instance,
         instance->_private.doorInhibitDispenseTable->rightHandFreshFood,
         DoorIndex_RightHandFreshFood,
         args->erd);
   }
   else if(args->erd == instance->_private.config->leftHandFreezerDoorIsOpenErd)
   {
      UpdateDispensingInhibitedByDoor(
         instance,
         instance->_private.doorInhibitDispenseTable->leftHandFreezer,
         DoorIndex_LeftHandFreezer,
         args->erd);
   }
   else if(args->erd == instance->_private.config->rightHandFreezerDoorIsOpenErd)
   {
      UpdateDispensingInhibitedByDoor(
         instance,
         instance->_private.doorInhibitDispenseTable->rightHandFreezer,
         DoorIndex_RightHandFreezer,
         args->erd);
   }
   else if(args->erd == instance->_private.config->convertibleCompartmentDoorIsOpenErd)
   {
      UpdateDispensingInhibitedByDoor(
         instance,
         instance->_private.doorInhibitDispenseTable->convertibleCompartment,
         DoorIndex_ConvertibleCompartment,
         args->erd);
   }
   else if(args->erd == instance->_private.config->doorInDoorIsOpenErd)
   {
      UpdateDispensingInhibitedByDoor(
         instance,
         instance->_private.doorInhibitDispenseTable->doorInDoor,
         DoorIndex_DoorInDoor,
         args->erd);
   }
}

static void UpdateDispensingInhibitedByDoorOnInit(DoorInhibitDispensingResolver_t *instance)
{
   UpdateDispensingInhibitedByDoor(
      instance,
      instance->_private.doorInhibitDispenseTable->leftHandFreshFood,
      DoorIndex_LeftHandFreshFood,
      instance->_private.config->leftHandFreshFoodDoorIsOpenErd);

   UpdateDispensingInhibitedByDoor(
      instance,
      instance->_private.doorInhibitDispenseTable->rightHandFreshFood,
      DoorIndex_RightHandFreshFood,
      instance->_private.config->rightHandFreshFoodDoorIsOpenErd);

   UpdateDispensingInhibitedByDoor(
      instance,
      instance->_private.doorInhibitDispenseTable->leftHandFreezer,
      DoorIndex_LeftHandFreezer,
      instance->_private.config->leftHandFreezerDoorIsOpenErd);

   UpdateDispensingInhibitedByDoor(
      instance,
      instance->_private.doorInhibitDispenseTable->rightHandFreezer,
      DoorIndex_RightHandFreezer,
      instance->_private.config->rightHandFreezerDoorIsOpenErd);

   UpdateDispensingInhibitedByDoor(
      instance,
      instance->_private.doorInhibitDispenseTable->convertibleCompartment,
      DoorIndex_ConvertibleCompartment,
      instance->_private.config->convertibleCompartmentDoorIsOpenErd);

   UpdateDispensingInhibitedByDoor(
      instance,
      instance->_private.doorInhibitDispenseTable->doorInDoor,
      DoorIndex_DoorInDoor,
      instance->_private.config->doorInDoorIsOpenErd);
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
