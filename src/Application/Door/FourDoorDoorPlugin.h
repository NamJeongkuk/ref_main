/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FOURDOORDOORPLUGIN_H
#define FOURDOORDOORPLUGIN_H

#include "AllFreshFoodDoorStatus.h"
#include "I_DataModel.h"
#include "SabbathInhibitDoors.h"
#include "OverrideArbiter.h"
#include "ConvertibleCompartmentStatePlugin.h"

typedef struct
{
   struct
   {
      AllFreshFoodDoorStatus_t allFreshFoodDoorStatus;
      SabbathInhibitDoors_t sabbathInhibitDoors;
      OverrideArbiter_t leftSideFreshFoodDoorOverrideArbiter;
      OverrideArbiter_t rightSideFreshFoodDoorOverrideArbiter;
      OverrideArbiter_t convertibleCompartmentDoorOverrideArbiter;
      OverrideArbiter_t bottomFreezerDrawerOverrideArbiter;
   } _private;
} FourDoorDoorPlugin_t;

void FourDoorDoorPlugin_Init(FourDoorDoorPlugin_t *instance, I_DataModel_t *dataModel);

#endif
