/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SINGLEDOORFRESHFOODUSERSETPOINTPLUGIN_H
#define SINGLEDOORFRESHFOODUSERSETPOINTPLUGIN_H

#include "UserSetpointRequestHandler.h"

typedef struct
{
   struct
   {
      UserSetpointRequestHandler_t freshFoodHandler;
   } _private;
} SingleDoorFreshFoodUserSetpointPlugin_t;

void SingleDoorFreshFoodUserSetpointPlugin_Init(
   SingleDoorFreshFoodUserSetpointPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
