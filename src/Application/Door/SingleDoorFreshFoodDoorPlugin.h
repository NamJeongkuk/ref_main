/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SINGLEDOORFRESHFOODDOORPLUGIN_H
#define SINGLEDOORFRESHFOODDOORPLUGIN_H

#include "I_DataModel.h"
#include "SabbathInhibitDoors.h"
#include "OverrideArbiter.h"
#include "ErdLogicService.h"

typedef struct
{
   struct
   {
      SabbathInhibitDoors_t sabbathInhibitDoors;
      OverrideArbiter_t rightSideFreshFoodDoorOverrideArbiter;
      ErdLogicService_t freshFoodDoorsErdLogicService;
   } _private;
} SingleDoorFreshFoodDoorPlugin_t;

void SingleDoorFreshFoodDoorPlugin_Init(SingleDoorFreshFoodDoorPlugin_t *instance, I_DataModel_t *dataModel);

#endif
