/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SINGLEDOORFRESHFOODSETPOINTRESOLVERPLUGIN_H
#define SINGLEDOORFRESHFOODSETPOINTRESOLVERPLUGIN_H

#include "I_DataModel.h"
#include "ErdResolver.h"
#include "ColdestOfSetpointVotes.h"

typedef struct
{
   struct
   {
      ColdestOfSetpointVotes_t coldestOfFreshFoodSetpoints;
      ErdResolver_t freshFoodSetpointResolver;
   } _private;
} SingleDoorFreshFoodSetpointResolverPlugin_t;

void SingleDoorFreshFoodSetpointResolverPlugin_Init(
   SingleDoorFreshFoodSetpointResolverPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
