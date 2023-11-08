/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ServiceModePlugin.h"
#include "ServiceModeTestNumber.h"
#include "SystemErds.h"

static const ServiceModeTest_VersionConfig mainBoardApplicationVersionConfig = {
   .versionErd = Erd_ApplicationVersion
};

static const ServiceModeTest_VersionConfig mainBoardParametricVersionConfig = {
   .versionErd = Erd_ParametricVersion
};

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

   ServiceModeTest_Version_Init(
      &instance->_private.mainBoardApplicationVersion.test,
      ServiceModeTestNumber_MainBoardApplicationVersion,
      &mainBoardApplicationVersionConfig);
   ServiceModeRequestHandler_AddTest(
      &instance->_private.serviceModeRequestHandler,
      &instance->_private.mainBoardApplicationVersion.test.interface,
      &instance->_private.mainBoardApplicationVersion.component);

   ServiceModeTest_Version_Init(
      &instance->_private.mainBoardParametricVersion.test,
      ServiceModeTestNumber_MainBoardParametricVersion,
      &mainBoardParametricVersionConfig);
   ServiceModeRequestHandler_AddTest(
      &instance->_private.serviceModeRequestHandler,
      &instance->_private.mainBoardParametricVersion.test.interface,
      &instance->_private.mainBoardParametricVersion.component);
}
