/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ServiceModeTest_ExitServiceMode.h"
#include "SystemErds.h"
#include "Signal.h"
#include "utils.h"

static void Start(I_ServiceTest_t *instance, void *context, ServiceTestResources_t *resources, ServiceTestCallback_t callback)
{
   IGNORE(instance);
   IGNORE(context);
   IGNORE(callback);
   Signal_SendViaErd(DataModel_AsDataSource(resources->dataModel), Erd_BroadcastResetRequestSignal);
}

static void Stop(I_ServiceTest_t *instance, void *context, ServiceTestResources_t *resources, ServiceTestCallback_t callback)
{
   IGNORE(instance);
   IGNORE(context);
   IGNORE(resources);
   IGNORE(callback);
}

static const I_ServiceTest_Api_t api = {
   .Start = Start,
   .Stop = Stop
};

void ServiceModeTest_ExitServiceMode_Init(
   ServiceModeTest_ExitServiceMode_t *instance,
   ServiceModeTestNumber_t testNumber)
{
   instance->interface.api = &api;
   instance->interface.testNumber = testNumber;
}
