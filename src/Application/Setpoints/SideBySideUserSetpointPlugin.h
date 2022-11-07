/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SIDEBYSIDEUSERSETPOINTPLUGIN_H
#define SIDEBYSIDEUSERSETPOINTPLUGIN_H

#include "I_DataModel.h"
#include "UserSetpointRequestHandler.h"

typedef struct
{
   struct
   {
      UserSetpointRequestHandler_t freshFoodHandler;
      UserSetpointRequestHandler_t freezerHandler;
   } _private;
} SideBySideUserSetpointPlugin_t;

/*!
 * @param instance
 * @param dataModel
 */
void SideBySideUserSetpointPlugin_Init(
   SideBySideUserSetpointPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
