/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SINGLEDOORFREEZERSETPOINTRESOLVERPLUGIN_H
#define SINGLEDOORFREEZERSETPOINTRESOLVERPLUGIN_H

#include "ColdestOfSetpointVotes.h"
#include "ErdResolver.h"
#include "I_DataModel.h"

typedef struct
{
   struct
   {
      ColdestOfSetpointVotes_t coldestOfFreezerSetpoints;
      ErdResolver_t freezerSetpointResolver;
   } _private;
} SingleDoorFreezerSetpointResolverPlugin_t;

void SingleDoorFreezerSetpointResolverPlugin_Init(
   SingleDoorFreezerSetpointResolverPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
