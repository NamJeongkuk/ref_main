/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SINGLEDOORFREEZERDOORPLUGIN_H
#define SINGLEDOORFREEZERDOORPLUGIN_H

#include "I_DataModel.h"
#include "SabbathInhibitDoors.h"
#include "OverrideArbiter.h"

typedef struct
{
   struct
   {
      SabbathInhibitDoors_t sabbathInhibitDoors;
      OverrideArbiter_t leftSideFreezerDoorOverrideArbiter;
   } _private;
} SingleDoorFreezerDoorPlugin_t;

void SingleDoorFreezerDoorPlugin_Init(SingleDoorFreezerDoorPlugin_t *instance, I_DataModel_t *dataModel);

#endif
