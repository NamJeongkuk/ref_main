/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FOURDOORUSERSETPOINTPLUGIN_H
#define FOURDOORUSERSETPOINTPLUGIN_H

#include "I_DataModel.h"
#include "UserSetpointRequestHandler.h"

typedef struct
{
   struct
   {
      UserSetpointRequestHandler_t freshFoodHandler;
      UserSetpointRequestHandler_t freezerHandler;
   } _private;
} FourDoorUserSetpointPlugin_t;

void FourDoorUserSetpointPlugin_Init(FourDoorUserSetpointPlugin_t *instance, I_DataModel_t *dataModel);

#endif
