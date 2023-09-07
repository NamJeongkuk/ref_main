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
#include "DoorIndex.h"
#include "I_DataModel.h"
#include "DispensingInhibitedReasonBitmap.h"

typedef struct
{
   Erd_t doorIsOpenErd; // bool
   DoorIndex_t doorIndex;
   size_t offsetInParametricForDoorInhibitsDispense;
} DoorInhibitDispensingResolverDoorConfig_t;

typedef struct
{
   Erd_t dispensingInhibitedErd;
   DispensingInhibitedReason_t dispensingInhibitedReason;
   const DoorInhibitDispensingResolverDoorConfig_t *doorsThatInhibitDispense;
   uint8_t numberOfDoors;
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
