/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SINGLEDOORFREEZERUSERSETPOINTPLUGIN_H
#define SINGLEDOORFREEZERUSERSETPOINTPLUGIN_H

#include "I_DataModel.h"
#include "UserSetpointRequestHandler.h"

typedef struct
{
   struct
   {
      UserSetpointRequestHandler_t freezerHandler;
   } _private;
} SingleDoorFreezerUserSetpointPlugin_t;

/*!
 * @param instance
 * @param dataModel
 */
void SingleDoorFreezerUserSetpointPlugin_Init(
   SingleDoorFreezerUserSetpointPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
