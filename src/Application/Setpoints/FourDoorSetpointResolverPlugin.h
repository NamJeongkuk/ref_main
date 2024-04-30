/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FOURDOORSETPOINTRESOLVERPLUGIN_H
#define FOURDOORSETPOINTRESOLVERPLUGIN_H

#include "I_DataModel.h"
#include "ErdResolver.h"
#include "ColdestOfSetpointVotes.h"

typedef struct
{
   struct
   {
      ColdestOfSetpointVotes_t coldestOfFreshFoodSetpoints;
      ColdestOfSetpointVotes_t coldestOfFreezerSetpoints;
      ErdResolver_t freshFoodSetpointResolver;
      ErdResolver_t freezerSetpointResolver;
      ErdResolver_t featurePanSetpointResolver;
   } _private;
} FourDoorSetpointResolverPlugin_t;

void FourDoorSetpointResolverPlugin_Init(FourDoorSetpointResolverPlugin_t *instance, I_DataModel_t *dataModel);

#endif
