/*!
 * @file
 * @brief Update ERDs for inhibiting ice or water dispensing when door opens
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DOORINHIBITDISPENSINGRESOLVER_H
#define DOORINHIBITDISPENSINGRESOLVER_H

#include "EventSubscription.h"
#include "DispenserData.h"
#include "I_DataModel.h"

typedef struct
{
   Erd_t leftHandFreshFoodDoorIsOpenErd; // bool
   Erd_t rightHandFreshFoodDoorIsOpenErd; // bool
   Erd_t leftHandFreezerDoorIsOpenErd; // bool
   Erd_t rightHandFreezerDoorIsOpenErd; // bool
   Erd_t convertibleCompartmentDoorIsOpenErd; // bool
   Erd_t doorInDoorIsOpenErd; // bool
   Erd_t dispensingInhibitedByDoorErd; // bool
} DoorInhibitDispensingResolverConfig_t;

typedef struct
{
   struct
   {
      EventSubscription_t dataModelSubscription;
      I_DataModel_t *dataModel;
      const DoorInhibitDispensingResolverConfig_t *config;
      const DoorInhibitDispenseTable_t *doorInhibitDispenseTable;
      uint8_t doorInhibitStatus;
   } _private;
} DoorInhibitDispensingResolver_t;

void DoorInhibitDispensingResolver_Init(
   DoorInhibitDispensingResolver_t *instance,
   I_DataModel_t *dataModel,
   const DoorInhibitDispensingResolverConfig_t *config,
   const DoorInhibitDispenseTable_t *doorInhibitDispenseTable);

#endif
