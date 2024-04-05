/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DOORPLUGIN_H
#define DOORPLUGIN_H

#include "I_DataModel.h"
#include "SabbathInhibitDoors.h"
#include "OverrideArbiter.h"
#include "ErdLogicService.h"

typedef struct
{
   struct
   {
      SabbathInhibitDoors_t sabbathInhibitDoors;
      OverrideArbiter_t leftSideFreezerDoorOverrideArbiter;
      OverrideArbiter_t rightSideFreshFoodDoorOverrideArbiter;
      ErdLogicService_t freshFoodDoorsErdLogicService;
   } _private;
} SideBySideDoorPlugin_t;

/*!
 *
 * @param instance
 * @param dataModel
 */
void SideBySideDoorPlugin_Init(SideBySideDoorPlugin_t *instance, I_DataModel_t *dataModel);

#endif
