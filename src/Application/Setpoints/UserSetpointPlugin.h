/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef USERSETPOINTPLUGIN_H
#define USERSETPOINTPLUGIN_H

#include "I_DataModel.h"
#include "UserSetpointRequestHandler.h"

typedef struct
{
   struct
   {
      UserSetpointRequestHandler_t freshFoodHandler;
      UserSetpointRequestHandler_t freezerHandler;
   } _private;
} UserSetpointPlugin_t;

/*!
 * @param instance
 * @param dataModel
 */
void UserSetpointPlugin_Init(
   UserSetpointPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
