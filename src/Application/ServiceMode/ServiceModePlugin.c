/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ServiceModePlugin.h"
#include "Gea2Addresses.h"
#include "ServiceModeTestNumber.h"
#include "SystemErds.h"

static const ServiceModeTest_VersionConfig_t mainBoardApplicationVersionConfig = {
   .versionErd = Erd_ApplicationVersion
};

static const ServiceModeTest_VersionConfig_t mainBoardParametricVersionConfig = {
   .versionErd = Erd_ParametricVersion
};

static const ServiceModeTest_BoardVersionConfig_t dispenserBoardVersionConfig = {
   .destinationAddress = Gea2Address_Dispenser
};

static const ServiceModeTest_BoardVersionConfig_t internalTempUiBoardVersionConfig = {
   .destinationAddress = Gea2Address_InternalTemperatureUi
};

static const ServiceModeTest_BoardVersionConfig_t wifiBoardVersionConfig = {
   .destinationAddress = Gea2Address_EmbeddedWiFi
};

static const ServiceModeTest_BoardVersionConfig_t rfidBoardVersionConfig = {
   .destinationAddress = Gea2Address_RfidBoard
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

   ServiceModeTest_BoardVersion_Init(
      &instance->_private.dispenserBoardVersion.test,
      ServiceModeTestNumber_DispenserBoardVersion,
      &dispenserBoardVersionConfig);
   ServiceModeRequestHandler_AddTest(
      &instance->_private.serviceModeRequestHandler,
      &instance->_private.dispenserBoardVersion.test.interface,
      &instance->_private.dispenserBoardVersion.component);

   ServiceModeTest_BoardVersion_Init(
      &instance->_private.internalTempBoardUiVersion.test,
      ServiceModeTestNumber_InternalTempBoardUiVersion,
      &internalTempUiBoardVersionConfig);
   ServiceModeRequestHandler_AddTest(
      &instance->_private.serviceModeRequestHandler,
      &instance->_private.internalTempBoardUiVersion.test.interface,
      &instance->_private.internalTempBoardUiVersion.component);

   ServiceModeTest_BoardVersion_Init(
      &instance->_private.wifiBoardVersion.test,
      ServiceModeTestNumber_WifiBoardVersion,
      &wifiBoardVersionConfig);
   ServiceModeRequestHandler_AddTest(
      &instance->_private.serviceModeRequestHandler,
      &instance->_private.wifiBoardVersion.test.interface,
      &instance->_private.wifiBoardVersion.component);

   ServiceModeTest_BoardVersion_Init(
      &instance->_private.rfidBoardVersion.test,
      ServiceModeTestNumber_RfidBoardVersion,
      &rfidBoardVersionConfig);
   ServiceModeRequestHandler_AddTest(
      &instance->_private.serviceModeRequestHandler,
      &instance->_private.rfidBoardVersion.test.interface,
      &instance->_private.rfidBoardVersion.component);
}
