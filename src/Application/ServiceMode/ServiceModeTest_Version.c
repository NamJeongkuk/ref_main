/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ServiceModeTest_Version.h"
#include "utils.h"
#include "Version.h"

static void Start(I_ServiceTest_t *instance, void *context, ServiceTestResources_t *resources, ServiceTestCallback_t callback)
{
   REINTERPRET(versionTestInstance, instance, ServiceModeTest_Version_t *);
   Version_t version;
   DataModel_Read(resources->dataModel, versionTestInstance->_private.config->versionErd, &version);

   ServiceTestResultArgs_t testResultArgs;
   testResultArgs.status.testResponse = ServiceModeTestStatusResponse_Running;
   testResultArgs.status.dataFormat = ServiceModeTestStatusDataFormat_VersionInfo;
   testResultArgs.status.testNumber = instance->testNumber;
   testResultArgs.status.diagnosticData[0] = version.major;
   testResultArgs.status.diagnosticData[1] = version.minor;
   testResultArgs.status.diagnosticData[2] = 0;
   testResultArgs.status.diagnosticData[3] = 0;

   callback(context, &testResultArgs);
}

static void Stop(I_ServiceTest_t *instance, void *context, ServiceTestResources_t *resources, ServiceTestCallback_t callback)
{
   IGNORE(resources);

   ServiceTestResultArgs_t testResultArgs;
   testResultArgs.status.testResponse = ServiceModeTestStatusResponse_Stopped;
   testResultArgs.status.dataFormat = ServiceModeTestStatusDataFormat_Unused;
   testResultArgs.status.testNumber = instance->testNumber;
   testResultArgs.status.diagnosticData[0] = 0;
   testResultArgs.status.diagnosticData[1] = 0;
   testResultArgs.status.diagnosticData[2] = 0;
   testResultArgs.status.diagnosticData[3] = 0;

   callback(context, &testResultArgs);
}

static const I_ServiceTest_Api_t api = {
   .Start = Start,
   .Stop = Stop
};

void ServiceModeTest_Version_Init(
   ServiceModeTest_Version_t *instance,
   ServiceModeTestNumber_t testNumber,
   const ServiceModeTest_VersionConfig_t *config)
{
   instance->interface.api = &api;
   instance->interface.testNumber = testNumber;
   instance->_private.config = config;
}
