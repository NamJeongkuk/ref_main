/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DOORPLUGIN_H
#define DOORPLUGIN_H

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
      OverrideArbiter_t leftSideFreezerDoorOverrideArbiter;
      OverrideArbiter_t rightSideFreshFoodDoorOverrideArbiter;
   } _private;
} SideBySideDoorPlugin_t;

/*!
 *
 * @param instance
 * @param dataModel
 */
void SideBySideDoorPlugin_Init(SideBySideDoorPlugin_t *instance, I_DataModel_t *dataModel);

#endif
