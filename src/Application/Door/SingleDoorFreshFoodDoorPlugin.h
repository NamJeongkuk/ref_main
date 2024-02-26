/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SINGLEDOORFRESHFOODDOORPLUGIN_H
#define SINGLEDOORFRESHFOODDOORPLUGIN_H

#include "AllFreshFoodDoorStatus.h"
#include "I_DataModel.h"
#include "SabbathInhibitDoors.h"
#include "OverrideArbiter.h"

typedef struct
{
   struct
   {
      AllFreshFoodDoorStatus_t allFreshFoodDoorStatus;
      SabbathInhibitDoors_t sabbathInhibitDoors;
      OverrideArbiter_t rightSideFreshFoodDoorOverrideArbiter;
   } _private;
} SingleDoorFreshFoodDoorPlugin_t;

void SingleDoorFreshFoodDoorPlugin_Init(SingleDoorFreshFoodDoorPlugin_t *instance, I_DataModel_t *dataModel);

#endif
