/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SERVICEMODEPLUGIN_H
#define SERVICEMODEPLUGIN_H

#include "I_DataModel.h"
#include "ServiceModeRequestHandler.h"
#include "ServiceModeTest_ExitServiceMode.h"
#include "I_ServiceTest.h"

typedef struct
{
   struct
   {
      ServiceModeRequestHandler_t serviceModeRequestHandler;

      struct
      {
         ServiceModeTest_ExitServiceMode_t test;
         ServiceModeTestComponent_t component;
      } exitServiceMode;
   } _private;
} ServiceModePlugin_t;

void ServiceModePlugin_Init(ServiceModePlugin_t *instance, I_DataModel_t *dataModel);

#endif
