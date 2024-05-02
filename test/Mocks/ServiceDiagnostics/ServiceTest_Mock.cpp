/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "utils.h"
#include "ServiceModeTestStatus.h"
}

#include "ServiceTest_Mock.h"
#include "CppUTestExt/MockSupport.h"

static void Start(I_ServiceTest_t *_instance, void *context, ServiceTestResources_t *resources, ServiceTestCallback_t callback)
{
   IGNORE(resources);
   REINTERPRET(instance, _instance, ServiceTest_Mock_t *);
   instance->_private.context = context;
   instance->_private.callback = callback;

   mock().actualCall("Start").onObject(instance);
}

static void Stop(I_ServiceTest_t *_instance, void *context, ServiceTestResources_t *resources, ServiceTestCallback_t callback)
{
   IGNORE(resources);
   REINTERPRET(instance, _instance, ServiceTest_Mock_t *);
   instance->_private.context = context;
   instance->_private.callback = callback;
   mock().actualCall("Stop").onObject(instance);
}

static const I_ServiceTest_Api_t api = {
   .Start = Start,
   .Stop = Stop
};

I_ServiceTest_t *ServiceTest_Mock_Init(ServiceTest_Mock_t *instance, const ServiceModeTest_TestNumbersMappingTable_t *testNumbersMappingTable)
{
   instance->interface.api = &api;
   instance->interface.testNumbersMappingTable = testNumbersMappingTable;

   return &instance->interface;
}

void ServiceTest_Mock_InvokeCallbackForActiveTestWith(ServiceTest_Mock_t *instance, ServiceModeTestStatus_t status)
{
   ServiceTestResultArgs_t args = { status };
   instance->_private.callback(instance->_private.context, &args);
}
