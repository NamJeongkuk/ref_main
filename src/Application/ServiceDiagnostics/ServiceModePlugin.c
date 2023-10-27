/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ServiceModePlugin.h"
#include "ServiceModeTestNumber.h"

void ServiceModePlugin_Init(ServiceModePlugin_t *instance, I_DataModel_t *dataModel)
{
   ServiceModeRequestHandler_Init(
      &instance->_private.serviceModeRequestHandler,
      dataModel,
      ServiceModeTestNumber_Total - 1);

   ServiceModeTest_ExitServiceMode_Init(
      &instance->_private.exitServiceMode.test,
      ServiceModeTestNumber_ExitServiceMode);
   ServiceModeRequestHandler_AddTest(
      &instance->_private.serviceModeRequestHandler,
      &instance->_private.exitServiceMode.test.interface,
      &instance->_private.exitServiceMode.component);
}
